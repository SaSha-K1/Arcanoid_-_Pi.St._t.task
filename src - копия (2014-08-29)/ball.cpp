
#include "ball.h"
#include "myExcptn.h"


//Static members:
//#OUTDATED (�� �����, ����� ..FiPlus ����� static'��):     float   CArkanoidController::CBallsManager::CBall::m_sfFiPlus = 0;
CArkanoidController::CBallsManager::CBall*  CArkanoidController::CBallsManager::CBall::m_spIsAttachedToPad = nullptr;
const u32                                   CArkanoidController::CBallsManager::CBall::m_suNUM_PIXELS_4_SHIFT = 3;
const float                                 CArkanoidController::CBallsManager::CBall::m_fDEFAULT_BALL_VEL = 200;
const u32                                   CArkanoidController::CBallsManager::CBall::m_suNUM_OF_HIT_CHCK_PNTS = 8;    //������ ==8 : (N(north),E(east),S,W + 4 �������������). !� ������ ���������, ������� � ����� ����-�� enum DIRECTION � (��������) ������ m_vfBallHitChckPntsAngles.
std::vector<float>                          CArkanoidController::CBallsManager::CBall::m_vfBallHitChckPntsAngles;
std::vector<vector2>                        CArkanoidController::CBallsManager::CBall::m_vHitChckPnts;

CArkanoidController::CBallsManager::CBall::CBall(
        CPictureObject* const pBall, 
        //#OUTDATED (����� static const'���� � ����� ����):     const u32 numOfHitChckPnts,
        //#OUTDATED: std::array<vector2,/*uNumOfHitChckPnts*/0U>&  aBallHitChckPnts,     ///#WARNING: ��������� �� "0U" ..?
        const bool              bAttachToPad
    ) 
    : m_pBallPicObj(pBall), 
    m_fVelocity(m_fDEFAULT_BALL_VEL), 
    m_fFi(0)
    //#OUTDATED (����� static const'���� � ����� ����):     m_uNumOfHitChckPnts(numOfHitChckPnts),
    //#DEL_IT?: m_fBallR(pBallPicObj->GetSize().y/2.0)
{ 
    if (m_vHitChckPnts.empty())
    {
        InitBallHitChckPntsAnglesVec();
        InitHitChckPntsVec();
    }
    if (bAttachToPad)
    {
        m_spIsAttachedToPad = this; 
        LocateAttachedToPadBall();	//����, ���� ������� Pad, �� ����� ��������������� � ��� �����, �� ��� ���-��� 
    }
}

//////////////////////\ PREPARE_ZONE \\\\\\\\\\\\\\\\\\\\\/
void CArkanoidController::CBallsManager::CBall::InitBallHitChckPntsAnglesVec()
{
    float newChckPnt;
    m_vfBallHitChckPntsAngles.push_back(0.0);
    for (u32 i=1;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)  //
    {
        newChckPnt = 2*PI*i/m_suNUM_OF_HIT_CHCK_PNTS;
        if (PI < newChckPnt)    //����������� ���� �� ��������� [0; 2*PI) � �������� (-PI; PI].     //��� ����� ���� ������������ � NormalizeFi(), ���� ���������� �, ����� ��������� ��������, �� �������� �������, � � NormalizeFi() ������ �������� � ��������. �����, ��� ��� ������ ����������
            newChckPnt -= 2*PI;
        m_vfBallHitChckPntsAngles.push_back(newChckPnt);
    }
    //#OUTDATED: array ��������������� ���:  
    //      { 0,          //N
    //        PI/4,       //NE
    //        HALFPI,     //E
    //        PI*3/4,     //SE
    //        PI,         //S
    //        -PI*3/4,    //SW
    //        -HALFPI,    //W
    //        -PI/4 };    //NW
}


void CArkanoidController::CBallsManager::CBall::InitHitChckPntsVec()
{
    float ballR = m_pBallPicObj->GetSize().y/2.0;
    float currAng;
    //#OUTDATED:    for (u32 i=0;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)    // aBallHitChckPnts.size(); ++i)
    for (auto& it = m_vfBallHitChckPntsAngles.begin();  m_vfBallHitChckPntsAngles.end() != it;  ++it)
    {
        //#OUTDATED:    currAng = m_vfBallHitChckPntsAngles[i]; //��� ������������. ������, ��� �����, �.�. ���� ����� ������������ ������ �������.

        //��������� ���������� ������� ����������� ��������� x � y �� ��������� ���������� ������:
        //��������� �������� ��������� (1 ��� 0) �����: �.�. ��� ����� ���� � ��������� ������������, ��, ..
        //.. ���� ��� ����� ������ ������� ��� ������� ������, �� �, ������ ������ ��������� ����� ����� ..
        //.. _�_ ����������� ��� ���������, ���������� 1-��; � �������� ����������� - ��������.

        //#OUTDATED ():
        //aBallHitChckPnts[i] = vector2(
        ///@@ STOPED HERE (28/08/14)
        m_vHitChckPnts.push_back(
            vector2(
                (ballR-1) + ballR * sin(*it) 
                    +((*it > 0   &&  *it < PI) ? 1 : 0),    ///@@@ /*myround(*/ ���� ��?
                (ballR-1) - ballR * cos(*it) 
                    +(( (*it > -PI  &&  *it < -HALFPI)  ||  (*it > HALFPI  /*&&  currAng <= PI*/) ) ? 1 : 0)
                    //��� ������������������ ����� ������� �� �����, ���� ���� �� ����� ���� ������ PI (��� ������)
            )
        );
    }
}
//X//X//X//X//X//X//X\ PREPARE_ZONE \\X\\X\\X\\X\\X\\X\\X/


void CArkanoidController::CBallsManager::CBall::ComputeNewFi(const DIRECTION dir)
{
    if (N==dir || S==dir)	//��� �������� ���������, �.�. ������� �������� fi ��������� ��� ���� �������
    {
        m_fFi = -m_fFi + PI +m_sfFiPlus;	//������������� ���� � ����������� �� ����� ����� �� Pad'� ������ ��� S �������.
        m_sfFiPlus=0;	//������������ ����� � ��������.
    }
    else if (NE==dir || SW==dir)
        m_fFi = -m_fFi -PI/2.0;
    else if (E==dir || W==dir)
        m_fFi = -m_fFi;
    else /*if (SE==dir || NW==dir)*/	//��������� ������� ����� � �� ���������
#if DEBUG==1
        if (SE==dir || NW==dir)
#endif
            m_fFi = -m_fFi + PI/2;
#if DEBUG==1
        else throw CMyExcptn(2);
#endif

    NormalizeFi();
}


void CArkanoidController::CBallsManager::CBall::NormalizeFi()
{
#if DEBUG==1
    if (2*PI <= m_fFi  ||  -2*PI >= m_fFi) throw CMyExcptn(3);
#endif
    if (PI < m_fFi)
        m_fFi=m_fFi-2*PI*(int( (m_fFi+PI)/(2*PI) ));		//����.��������� - ������� ������� ��� �� 2��, ����� ..
                                                            //.. ���-��� == (-PI ; PI].     //(m_fFi+PI) - ��� +PI, ����� ��������� ..
                                                            //.. ������� �������� ����� ����� PI � ������� m_fFi ��� ������� ������ ..
                                                            //.. �� 2*PI ������ +1. ��� �����,����� ������� m_fFi � �������� (-PI;PI].
    else if (-PI >= m_fFi)
        m_fFi=m_fFi+2*PI*(int( (m_fFi-PI)/(-2*PI) ));
}


vector2 CArkanoidController::CBallsManager::CBall::ComputeNewMoveCenter(u32 moveCCoordScale/* = pow(10.0, 3) */) const  //���������������� �������� by def., ..
                                                                                   //.. ������������� � ���������� ������.
                                                                                  //���-�� `moveCCoordScale` - const ��������� ..
                                                                                 //.. ���������� ������ �������� (��-���� ����� ..
                                                                                //.. ���� ������ ������, �������, ����� ��� ����. ..
                                                                               //.. ���� �� �������). ����� ����� �������� ..
                                                                              //.. ����������� �������� ������ ����������. ..
{
    vector2 mc;
    //#OUTDATED: const u32 k = MOVE_C_COORD_SCALE;

    if (-PI/4 <= m_fFi  &&  PI/4 >= m_fFi) 	// ^ up
                                        mc.y = m_pBallPicObj->GetPos().y + moveCCoordScale,
                                        mc.x = m_pBallPicObj->GetPos().x - moveCCoordScale * tan(m_fFi);
    else if (( m_fFi > -PI  &&  m_fFi <= -3/4*PI )  ||  ( m_fFi >= 3/4*PI  &&  m_fFi <= PI )) 	// v down
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale,
                                        mc.x = m_pBallPicObj->GetPos().x + moveCCoordScale * tan(PI+m_fFi);
    else if (m_fFi > PI/4  &&  m_fFi < 3/4*PI) 	// > right
                                        mc.x = m_pBallPicObj->GetPos().x - moveCCoordScale,
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(PI-m_fFi);	//��� � � ����.����� ..
                                                                                                //.. ���� ctg, �.�. �/� tg ������� ..
                                                                                                //.. ������ � ���� �������� �����.
    else if (m_fFi > -3/4*PI  &&  m_fFi < -PI/4) 	// < left
                                        mc.x = m_pBallPicObj->GetPos().x + moveCCoordScale,
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(m_fFi);
#if DEBUG==1
    else throw CMyExcptn(1);
#endif
    return mc;
}


void CArkanoidController::CBallsManager::CBall::ComputeStartFi(
        const float rndStartAng,/* = 10*(float)PI/180, */   //���������������� �������� by def., ������������� � ���������� ������.
        const u32 multiplForGen/* = pow(10.0, 4) */         //���������������� �������� by def., ������������� � ���������� ������.
     )    
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad)
        throw CMyExcptn(6);
    else
#endif
    {
        m_fFi = (float)CRndGen::GetInst().Generate(static_cast<u32>(2 * rndStartAng * multiplForGen)) 
            / multiplForGen - rndStartAng;
    }
}


vector2 CArkanoidController::CBallsManager::CBall::GetNewMoveCenter(const DIRECTION dir)
{
#if DEBUG==1
    if (NO==dir) throw CMyExcptn(5);
#endif

    if (this != m_spIsAttachedToPad)		
    {
        m_pBallPicObj->StopMoveTo();	///@@@ ��.��. �� �����
        computeNewFi(dir);
    }
    else /*if (this == m_spIsAttachedToPad)*/ computeStartFi();
    return computeNewMoveCenter();
}


bool CArkanoidController::CBallsManager::CBall::TryToInvertIsAttachedToPad() 
{ 
    if (this == m_spIsAttachedToPad)	        { m_spIsAttachedToPad = nullptr; return 1; }
    else if (m_spIsAttachedToPad == nullptr)    { m_spIsAttachedToPad = this; return 1; }
    else /*if other ball is attached*/	return 0; 
}


void CArkanoidController::CBallsManager::CBall::LocateAttachedToPadBall()
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad) throw CMyExcptn(9);
#endif
    m_pBallPicObj->SetPos( g_pPad->GetCenter().x - m_pBallPicObj->GetSize().x / 2,   
                           g_fPaddleY - m_pBallPicObj->GetSize().y - 1 );
}

//������� �������� (m_sfFiPlus) � ���� ��������� (m_fFi) � ����������� �� �������� ����� �������� � Pad'�� �� �������� ����������
void CArkanoidController::CBallsManager::CBall::ComputeFiPlus()
{
    float dlt = m_pBallPicObj->GetCenter().x - g_pPad->GetCenter().x;	//������� ���������� ������ �� ������ Pad'� ��� �����
    float weightedDlt = dlt/g_pPad->GetSize().x/2; //delta ���������� �� ��������� � 1/2 ����� Pad'� (�.�. ���������� � �������� [0, 1]
    m_sfFiPlus= (PI/2/*4*/)*weightedDlt*((fabs(m_fFi)-PI/2)/PI/2);	//������������ ������������� � ���� == PI/4, �������������� ..
                                                                   //.. ���������� dlt'�� � �������� m_fFi � PI/2, ���������� � ..
                                                                  //.. ����� PI/2 (�.�. ���� [0, 1]). ��� m_fFi ==����� PI/2 ..
                                                                 //.. (3,1417/2) ���� ����� �� ����� ���������� < PI/2. �.�. ..
                                                                //.. ������������� ���������� - ����� �� ������� ���� ��-�� ��.
}


void CArkanoidController::CBallsManager::CBall::IsHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (0  >=  m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y)     vTrggrdHitChckPnts.push_back(N);
    if (0  >=  m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x)     vTrggrdHitChckPnts.push_back(W);
    else if (m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  >=  g_fSceneWidth)    vTrggrdHitChckPnts.push_back(E);
}


void CArkanoidController::CBallsManager::CBall::IsHitWithPad(
        std::vector<DIRECTION>& vTrggrdHitChckPnts,  
        bool& needToCheckBricks, 
        const float fMaxDltYWithPad,  
        const float& fDltPxlsToChckHit
     )
{
    //#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //��� ���� ������, ������ ����� ��������� �������, ������, �.�. �������� ��������� ����� ..
    //                                        //.. �� ��������� �������� ����� ���������� �������� �������������� (��. "#OPT") ��������.

    //����������� (����� #OPT): ���� ����� ���� ����������� Pad'�, ��� ���� ������ ��� ���� (����������� Pad'a +10), ..
    //.. �� �������� � Pad'�� ������ �� ����� - �������� ������� ���.�������� ������� �� 2�� (Pad ������ 1��)
    if ( m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y  <  g_fPaddleY - fDltPxlsToChckHit  ||	
         m_pBallPicObj->GetPos().y  >  g_fPaddleY + fMaxDltYWithPad  ||   //if ����� ������ ���� �������� Pad'�,�� ��� game over.
         m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  <  g_pPad->GetPos().x - fDltPxlsToChckHit  ||
         m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x  >  g_pPad->GetPos().x + g_pPad->GetSize().x + fDltPxlsToChckHit )
         /*�� ������. �������, �� ����� ���������, ����� - ���������*/;
    else
    {
        for (int d=E/*2*/; d<=W/*6*/; ++d)	// �������� [2, 6] == [E, W] (��������� ����� ��������� ��� Pad'� �� �����)
        {                                   //#WARNING(possibly). �������� �������� implicit �������������� ����� �����.
            if ( g_pPad->Hit(m_aHitChckPnts[d] + Vec3To2(m_pBallPicObj->GetPos())/*,0*/) )
            {
                vTrggrdHitChckPnts.push_back(static_cast<DIRECTION>(d));
                needToCheckBricks=0;
                //������� �������� (m_sfFiPlus) � ���� (m_fFi) � ����������� �� �������� ����� �������� � Pad'�� �� �������� Pad'�:
                if (S == vTrggrdHitChckPnts.back())	//�� ����������� ����, ���� ������� �� ���� ��� �����.�����������
                    ComputeFiPlus();
            }
        }
    }
}


void CArkanoidController::CBallsManager::CBall::IsHitWithBricks(std::vector<DIRECTION>& vTrggrdHitChckPnts,  const float& fDltPxlsToChckHit)
{
#if LOCKS==1
        while (CBrick::BricksLockOn(1) != 1)	///In isHit()
        {	/*������� ����� �� ��������� � m_vpBricks. ���� ��� ������� ���-�� ������, ���*/
#if LOW_LVL_LOCKS==1		
            //�������: �������� �������� ��� ��������
            if (CBrick::BricksLockOn(1) != 1)
                return NO;
            else
                break;
#endif//LOW_LVL_LOCKS
        }
#endif//LOCKS

//#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //��� ���� ������, ������ ����� ��������� �������, ������, �.�. �������� ��������� ����� ..
//                                        //.. �� ��������� �������� ����� ���������� �������� �������������� (��. "#OPT") ��������.

        //for (auto it = m_vpBricks.begin(); it!=m_vpBricks.end(); ++it)	//���� �������� � ���������� ���������� ��-�� ���������������
        //for (auto it = m_vpBricks.rbegin(); it!=m_vpBricks.rend(); ++it)	///@@@ ��������� ITERAOR N
        //for (char ib = m_vpBricks.size()-1; ib>=0; --ib)
        for (char ib = 0; ib < m_vpBricks.size(); ++ib)	//�������
        {		
            /*�������*/ CAniObject ao = m_vpBricks[ib]->getBrickAniObj();	//�������: ������ ��������� ���� ����� ������� - �� ��������
            //CAniObject *ao = (*it)->getBrickAniObj();	//��� ������������
            //CAniObject ao (m_vpBricks[ib]->getBrickAniObj());	//��� ������������
                //if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao->GetPos().y -dlt  ||	//Opt.: �������� ����������� ����������� ���������������
                //m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao->GetPos().y + ao->GetSize().y +dlt ||	//opt
                //m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao->GetPos().x -dlt  ||					//opt
                //m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao->GetPos().x + ao->GetSize().x +dlt)	//opt
            //#OPT: �������� ����������� ����������� ���������������:
            if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao.GetPos().y -fDltPxlsToChckHit  ||	
                m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao.GetPos().y + ao.GetSize().y +fDltPxlsToChckHit || 
                m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao.GetPos().x -fDltPxlsToChckHit  || 
                m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao.GetPos().x + ao.GetSize().x +fDltPxlsToChckHit) 
                continue;
	
            for (u32 i=0; i<uNumOfHitChckPnts; ++i)
            {
                if ( /*ao->*/ao.Hit(m_aHitChckPnts[i] + vec3To2(m_pBallPicObj->GetPos())/*,0*/) )
                {
                    vTrggrdHitChckPnts.push_back(static_cast<DIRECTION>(i));
                    if (/*(*it)*/ m_vpBricks[ib]/**/->m_state == CBrick::ING)
                        /*(*it)*/ m_vpBricks[ib]/**/->m_state=CBrick::DSTR_ST;
                }
            }
        }
#if LOCKS==1
#if DEBUG==1
        if (CBrick::BricksLockOff(1)==0) throw CMyExcptn(16); /*�������� ����� �� ��������� � m_vpBricks. ���� ��� ������� ���-�� ������, ���*/
#else
        CBrick::BricksLockOff(1)
#endif//DEBUG
#endif//LOCKS
}


//������������� � ������������� �����������
//#UGLY: ����������� ���� �� ������ �������� ��������� � ���� DIRECTION ����� �/� typedef - ����� � ���� nested classes, ��� typedef ����� ������ ���� ����� � �����. �� � �����.����. ����������� ������������ �������-��������� ("auto .. )-> ...")
CArkanoidController::CBallsManager::CBall::DIRECTION CArkanoidController::CBallsManager::CBall::InterpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (vTrggrdHitChckPnts.empty()) return NO;	//��� ��������

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	
        // �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1� �� 2� �����.������� ������.��������
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2� �� 2� �����.������� ������.��������	//��� ������ ����� ������� ���� ����������, �� �������� ��� ��������� ������� ����
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//�.�. if �������.������ ��������
    else throw CMyExcptn(11);	//�� ����� - �������� �� .empty() ���� � ������

    return HitReview(dir);
}


//#UGLY: ����������� ���� �� ������ �������� ��������� � ���� DIRECTION ����� �/� typedef - ����� � ���� nested classes, ��� typedef ����� ������ ���� ����� � �����. �� � �����.����. ����������� ������������ �������-��������� ("auto .. )-> ...")
CArkanoidController::CBallsManager::CBall::DIRECTION CArkanoidController::CBallsManager::CBall::IsHit(const float fDltPxlsToChckHit/* = 3*/)   //���������������� ����-� by def., ������������� � ���������� ������.
{																		///@@@ !!! �������������� !!!
    if (this == m_spIsAttachedToPad  ||  RUN != m_GameState) return NO;
    bool needToCheckBricks=1;	//��� ��� ����� ��� ��������� �������� ��������: �.�. ������� ����������� ����������, ��, ..
                                //.. ���� �������� ������������ � Pad'��, �� �������� �������� ������������.
    std::vector<DIRECTION> vTrggrdHitChckPnts;	///@@@����� ����� ������� ������� ��� ����� ��������� (��.��. 2 ���������� ��) - ��� - �� �����������.

//����� ������������ � <^> ������ ������
    IsHitWithScreenBorders(vTrggrdHitChckPnts);

//����� ������������ � Pad'��
    IsHitWithPad(vTrggrdHitChckPnts, needToCheckBricks, g_pPad->GetCenter().y - g_pBall->GetCenter().y, fDltPxlsToChckHit);

//����� ������������ � ���������
    if (needToCheckBricks)	//��.��������� ���� - � �������� ���� ����������
        IsHitWithBricks(vTrggrdHitChckPnts, fDltPxlsToChckHit);

//������������� � ������������� �����������
    InterpretIsHitResults(vTrggrdHitChckPnts);
    if (vTrggrdHitChckPnts.empty()) return NO;	//��� ��������

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	// �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1� �� 2� �����.������� ������.��������
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2� �� 2� �����.������� ������.��������	//��� ������ ����� ������� ���� ����������, �� �������� ��� ��������� ������� ����
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//�.�. if �������.������ ��������
    else throw CMyExcptn(11);	//�� ����� - �������� �� .empty() ���� � ������

    return HitReview(dir);
}

//�������� �� ��������� ����������� ��� ������������� ��������, �������� ���������� �� �������� ��� ��� ���������
//#UGLY: ����������� ���� �� ������ �������� ��������� � ���� DIRECTION ����� �/� typedef - ����� � ���� nested classes, ��� typedef ����� ������ ���� ����� � �����. �� � �����.����. ����������� ������������ �������-��������� ("auto .. )-> ...")
CArkanoidController::CBallsManager::CBall::DIRECTION CArkanoidController::CBallsManager::CBall::HitReview(DIRECTION dir) const
{
#if DEBUG==1
    if (m_fFi>PI || m_fFi<= -PI) throw CMyExcptn(12);
#endif
    switch (dir)
    {
        case N:  if (m_fFi>= PI/2 || m_fFi<=-PI/2) dir=NO; break;
        case NE: if (m_fFi>= PI*3/4 || m_fFi<=-PI/4) dir=NO; break;
        case E:  if (m_fFi == PI || m_fFi<= 0) dir=NO; break;
        case SE: if (m_fFi>= -PI*3/4 && m_fFi<= PI/4) dir=NO; break;
        case S:  if (m_fFi>= -PI/2 && m_fFi<= PI/2) dir=NO; break;
        case SW: if (m_fFi>= -PI/4 && m_fFi<= PI*3/4) dir=NO; break;
        case W:  if (m_fFi >= 0) dir=NO; break;
        case NW: if (m_fFi>= PI/4 || m_fFi<= -PI*3/4) dir=NO; break;
        default: throw CMyExcptn(13);
    }
    return dir;
}
    //xxx END_of class BALL xxx// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx







void CArkanoidController::CBallsManager::CBall::InitFewPixelsShift()
{
    float currAng;
    for (u32 i=0; i < aBallHitChckPnts.size(); ++i)
    {
        currAng = aBallHitChckPntsAngles[i]; //��� ������������. ������, ��� �����, �.�. ���� ����� ������������ ������ �������.
        mFewPixelsShift[static_cast<DIRECTION>(i)] = vector2(   //#COMPILER(type_cast)
            NUM_PIXELS_4_SHIFT *-sin(currAng), 
            NUM_PIXELS_4_SHIFT * cos(currAng)
            ); ///@@@ /*myround(*/ ���� ��?
    }

    //#OUTDATED:
    //mFewPixelsShift [N]  = vector2( 0, 3);      // ( 0, 3)
    //mFewPixelsShift [NE] = vector2(-2, 2);      // (-2, 2)
    //mFewPixelsShift [E]  = vector2(-3, 0);      // (-3, 0)
    //mFewPixelsShift [SE] = vector2(-2,-2);      // (-2,-2)
    //mFewPixelsShift [S]  = vector2( 0,-3);      // ( 0,-3)
    //mFewPixelsShift [SW] = vector2( 2,-2);      // ( 2,-2)
    //mFewPixelsShift [W]  = vector2( 3, 0);      // ( 3, 0)
    //mFewPixelsShift [NW] = vector2( 2, 2);      // ( 2, 2)
}
