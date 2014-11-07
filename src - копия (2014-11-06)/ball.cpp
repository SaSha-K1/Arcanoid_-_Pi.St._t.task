
#include "pictureobj.h"     //��� pPad->GetCenter()
#include "random.h"         //��� CRndGen::GetInst()
#include "ball.h"
#include "myExcptn.h"
#include "brick.h"  ///@@@ ��� �����. �����, �.�. ��������� � ->GetBrickAniObj(), ->m_state, � BRICK_STATE. �� ��� � ������ IsHitWithBrick().   #TODO: ���������� ���������� �� ����� ����������� ���� 2� ������� (��������, ������� ������������ �� ������������� ������ CAbstractBrick, �� �������� ������������ ��� CBrick).


//// TYPES:
typedef /*typename/**/ CLockableAccessVec<_BRICK*>::CLavHandler _LAV_OFBRICKS_HNDLR;  // `typename` may be not allowed out of `template`  ///: err C2899: typename cannot be used outside a template declaration	10



//// Static data-members:
_BALL*                  _BALL::m_spIsAttachedToPad      = nullptr;
const u32               _BALL::m_suNUM_PIXELS_4_SHIFT   = 3;
const float             _BALL::m_fDEFAULT_BALL_VEL      = 200;
const u32               _BALL::m_suNUM_OF_HIT_CHCK_PNTS = 8;    //������ ==8 : (N(north),E(east),S,W + 4 �������������). ..
                                                                //.. !� ������ ���������, ������� � ����� ����-��: enum _DIRECTION � ..
                                                                //.. (��������) ������ vfBallHitChckPntsAngles (� ������ ..
                                                                //.. InitBallHitChckPnts(). ����� ������� �� ������ ����� IsHitWithPad().
std::vector<float>                  _BALL::m_svfBallHitChckPntsAngles;
std::vector<vector2>                _BALL::m_svHitChckPnts;
std::map<_BALL::_DIRECTION,vector2>  _BALL::m_smFewPixelsShift;
///@@ Stoped_here 2014/09/14 - ��������� ������ �� controller .h / .cpp 


//// C-tor ////
_BALL::CBall(
        CPictureObject*            const  pBall, 
        const bool                        bAttachToPad,
        /*const*/ CPictureObject*  const  pPad
    ) 
    : m_pBallPicObj(pBall), 
    m_fVelocity(m_fDEFAULT_BALL_VEL), 
    m_fFi(0),
    m_fFiPlus(0)
{ 
    if (m_svHitChckPnts.empty())    // `true` -> ������, ������ 1� �����
    {
        InitHitChckPntsVec();
        InitFewPixelsShift();
    }
    if (bAttachToPad)
    {
        m_spIsAttachedToPad = this; 
        LocateAttachedToPadBall(pPad);	//����, ���� ������� Pad, �� ����� ��������������� � ��� �����, �� ��� ���-��� 
    }
    InitFewPixelsShift();
}


//////////////////////\ PREPARE_ZONE \\\\\\\\\\\\\\\\\\\\\/
    ///@@������ ��������� ���� ����� ���� � ���� �� �����

//#OUDATED ( �� �� �������� ������� �� 2 ������: Init..Angles() � Init..Pnts(), �.�. ������ ����� ����� � InitFewPxlsShift() ):
//// InitHitChckPntsVec() ////
//void _BALL::InitHitChckPntsVec()
//{
//    std::vector<float> vfBallHitChckPntsAngles;   ///@@@ ? ��������� �� ��� ����?: //#IMPORTANT: � ����.������������� ��������� ���-�� ��-���, ������ ������ �� ������ ���-�� >=2. ������� � ����� ����-�� enum _DIRECTION � class CBall static member m_suNUM_OF_HIT_CHCK_PNTS.
//    // 1of2) Initializing of vector of ball hit check points ANGLES:
//    float newChckPnt;
//    vfBallHitChckPntsAngles.push_back(0.0);
//    for (u32 i=1;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)  //������� � 1, �.�. 0-��� ��-� ������������� ����� ������
//    {
//        newChckPnt = 2 * PI * i / m_suNUM_OF_HIT_CHCK_PNTS;
//        if (PI < newChckPnt)    //����������� ���� �� ��������� [0; 2*PI) � �������� (-PI; PI].     
//                                //.. ��� ����� ���� ������������ � NormalizeFi(), ���� ���������� �, ����� ��������� ��������, �� ..
//                                //.. �������� �������, � � NormalizeFi() ������ �������� � ��������. �����, ��� ��� ������ ����������.
//            newChckPnt -= 2 * PI;
//        vfBallHitChckPntsAngles.push_back(newChckPnt);
//    }
//        //#OUTDATED: array ��������������� ���:  
//        //      { 0,          //N
//        //        PI/4,       //NE
//        //        HALFPI,     //E
//        //        PI*3/4,     //SE
//        //        PI,         //S
//        //        -PI*3/4,    //SW
//        //        -HALFPI,    //W
//        //        -PI/4 };    //NW
//
//    // 2of2) Initializing of vector of hit check points:
//    float ballR = m_pBallPicObj->GetSize().y / 2.0;
//    float currAng;
//    for (auto& it = vfBallHitChckPntsAngles.begin();  vfBallHitChckPntsAngles.end() != it;  ++it)
//    {
//        //��������� ���������� ������� ����������� ��������� x � y �� ��������� ���������� ������:
//        //��������� �������� ��������� (1 ��� 0) �����: �.�. ��� ����� ���� � ��������� ������������, ��, ..
//        //.. ���� ��� ����� ������ ������� ��� ������� ������, �� �, ������ ������ ��������� ����� ����� ..
//        //.. _�_ ����������� ��� ���������, ���������� 1-��; � �������� ����������� - ��������.
//        m_svHitChckPnts.push_back(
//            vector2(
//                (ballR-1) + ballR * sin(*it) 
//                    +((*it > 0   &&  *it < PI) ? 1 : 0),    ///@@@ /*myround(*/ ���� ��?
//                (ballR-1) - ballR * cos(*it) 
//                    +(( (*it > -PI  &&  *it < -HALFPI)  ||  (*it > HALFPI  /*&&  currAng <= PI*/) ) ? 1 : 0)
//                    //��� ������������������ ����� ������� �� �����, ���� ���� �� ����� ���� ������ PI (��� ������)
//            )
//        );
//    }
//}


//// InitBallHitChckPntsAnglesVec() ////
///@@ Stoped here 2014/09/12 - � ���������� � ������.
void _BALL::InitBallHitChckPntsAnglesVec()  //Initializing of vector of ball hit check points ANGLES
{
    float newChckPnt;
    m_svfBallHitChckPntsAngles.push_back(0.0);
    for (u32 i=1;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)  //������� � 1, �.�. 0-��� ��-� ������������� ����� ������
    {
        newChckPnt = 2 * PI * i / m_suNUM_OF_HIT_CHCK_PNTS;
        if (PI < newChckPnt)    //����������� ���� �� ��������� [0; 2*PI) � �������� (-PI; PI].     
                                //.. ��� ����� ���� ������������ � NormalizeFi(), ���� ���������� �, ����� ��������� ��������, �� ..
                                //.. �������� �������, � � NormalizeFi() ������ �������� � ��������. �����, ��� ��� ������ ����������.
            newChckPnt -= 2 * PI;
        m_svfBallHitChckPntsAngles.push_back(newChckPnt);
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


//// InitHitChckPntsVec() ////
void _BALL::InitHitChckPntsVec()
{
//#if DEBUG==1
//    if (m_suNUM_OF_HIT_CHCK_PNTS != m_svfBallHitChckPntsAngles.size()) throw CMyExcptn(23);
//#endif //DEBUG

    // 1of2) Initializing of vector of ball hit check points ANGLES:
    InitBallHitChckPntsAnglesVec();

#if DEBUG==1
    if (m_suNUM_OF_HIT_CHCK_PNTS != m_svfBallHitChckPntsAngles.size()) throw CMyExcptn(23);
#endif //DEBUG


    // 2of2) Initializing of vector of hit check points:
    float ballR = m_pBallPicObj->GetSize().y / 2.0;
    float currAng;  ///@@@? WRN C4101: 'currAng' : unreferenced local variable 141
    for (auto/*&*/ it = m_svfBallHitChckPntsAngles.begin();  m_svfBallHitChckPntsAngles.end() != it;  ++it)
    {
        //��������� ���������� ������� ����������� ��������� x � y �� ��������� ���������� ������:
        //��������� �������� ��������� (1 ��� 0) �����: �.�. ��� ����� ���� � ��������� ������������, ��, ..
        //.. ���� ��� ����� ������ ������� ��� ������� ������, �� �, ������ ������ ��������� ����� ����� ..
        //.. _�_ ����������� ��� ���������, ���������� 1-��; � �������� ����������� - ��������.
        m_svHitChckPnts.push_back(
            vector2(
                (ballR-1) + ballR * sin(*it) 
                    +(
                        (*it > 0   &&  *it < PI) 
                        ? 1 
                        : 0
                    ),    ///@@@ /*myround(*/ ���� ��?
                (ballR-1) - ballR * cos(*it) 
                    +(
                        ( (*it > -PI  &&  *it < -HALFPI)  ||  (*it > HALFPI  /*&&  currAng <= PI*/) ) 
                        ? 1 
                        : 0
                    )
                    //��� ������������������ ����� ������� �� �����, ���� ���� �� ����� ���� ������ PI (��� ������)
            )
        );
    }
}


//X//X//X//X//X//X//X\ PREPARE_ZONE \\X\\X\\X\\X\\X\\X\\X/


//// ComputeNewFi() ////
void _BALL::ComputeNewFi(const _DIRECTION dir)
{
    if (N==dir || S==dir)	//��� �������� ���������, �.�. ������� �������� Fi ��������� ��� ���� �������
    {
        m_fFi = -m_fFi + PI +m_fFiPlus;	//������������� ���� � ����������� �� ����� ����� �� Pad'� ������ ��� S �������.
        m_fFiPlus=0;	//������������ ����� � ��������.
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


//// NormalizeFi() ////
void _BALL::NormalizeFi()
{
#if DEBUG==1
    if (2*PI <= m_fFi  ||  -2*PI >= m_fFi) throw CMyExcptn(3);
#endif
    if (PI < m_fFi)
        m_fFi = m_fFi - 2 * PI * (int( (m_fFi+PI) / (2*PI) ));		//����.��������� - ������� ������� ��� �� 2��, ����� ..
                                                                    //.. ���-��� == (-PI ; PI].     //(m_fFi+PI) - ��� +PI, ����� ..
                                                                    //.. ��������� ������� �������� ����� ����� PI � ������� m_fFi ��� ..
                                                                    //.. ������� ������ �� 2*PI ������ +1. ��� �����,����� ������� m_fFi ..
                                                                    //.. � �������� (-PI;PI].
    else if (-PI >= m_fFi)
        m_fFi = m_fFi + 2 * PI * (int( (m_fFi-PI) / (-2*PI) ));
}


//// ComputeNewMoveCenter() ////
vector2 _BALL::ComputeNewMoveCenter(const u32 moveCCoordScale/* = pow(10.0, 3) */) const
//������ /**/ - ����-� by def., ������������� � ������-�� ������. ��� ���������.
//���-�� `moveCCoordScale` - const ��������� ���������� ������ �������� (��-���� ����� ���� ������ ������, ..
//.. �������, ����� ��� ����. ���� �� �������). ����� ����� �������� ����������� �������� ������ ����������.
{
    vector2 mc;
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


//// ComputeStartFi() ////
void _BALL::ComputeStartFi(
        const float rndStartAng,/* = 10*(float)PI/180, */   //������ /**/ - ����-� by def., ������������� � ������-�� ������. ��� ���������
        const u32   multiplForGen/* = pow(10.0, 4) */       //������ /**/ - ����-� by def., ������������� � ������-�� ������. ��� ���������
     )    
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad)
        throw CMyExcptn(6);
    else
#endif
    {
        m_fFi = (float)CRndGen::GetInst().Generate( static_cast<u32>(2 * rndStartAng * multiplForGen) ) / multiplForGen - rndStartAng;
    }
}


//// GetNewMoveCenter() ////
vector2 _BALL::GetNewMoveCenter(const _DIRECTION dir)
{
#if DEBUG==1
    if (NO==dir) throw CMyExcptn(5);
#endif

    if (this != m_spIsAttachedToPad)		
    {
        m_pBallPicObj->StopMoveTo();	///@@@ ����� ��� ���� �������. �����, ��������� ������������ ������.
        ComputeNewFi(dir);
    }
    else ComputeStartFi();  //�� ���� ��� �����  "if (this == m_spIsAttachedToPad)"
    return ComputeNewMoveCenter();
}


//// TryToInvertIsAttachedToPad() ////
bool _BALL::TryToInvertIsAttachedToPad()    ///@@@ 1) �������� �������� �� 2 ������, �.�. ��� ����� ����������� ��� ���������;
                                        ///@@@ 2) ����� �� ������������ ��������?
                                    ///@@@ 3) ��������� �� ����, ����� �� �������� `if` � ������. ���� ���, �� � �� �������� ������ "TryTo".
{ 
    if (this == m_spIsAttachedToPad)	        { m_spIsAttachedToPad = nullptr;    return 1; }     ///@@@ �������� �������� ���-� velocity ������ �� default, ����� ������� � ��������� ���������. ����� ����� �������� �� ������������ � Pad'�, ������� �� ���, ���� �������� ���� (�� ���� == ��������� �������� �������) ����������� � ������� ������������ ������
    else if (m_spIsAttachedToPad == nullptr)    { m_spIsAttachedToPad = this;       return 1; }
    else return 0; //�� ���� ��� �����  "if other ball is attached"
}


//// LocateAttachedToPadBall() ////
void _BALL::LocateAttachedToPadBall(/*const*/ CPictureObject* const pPad)
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad) throw CMyExcptn(9);
#endif
    m_pBallPicObj->SetPos(
            pPad->GetCenter().x - m_pBallPicObj->GetSize().x / 2,   
            pPad->GetPos().y - m_pBallPicObj->GetSize().y - 1   //#OUTDATED (����� �� ���������� ��� 1 ���-�, ���� ������ ���������� Pad'�): g_fPaddleY - m_pBallPicObj->GetSize().y - 1
        );
}


//// ComputeFiPlus() ////
void _BALL::ComputeFiPlus(CPictureObject* pPad)
//��� ������� �������� (m_fFiPlus) � ���� ��������� (m_fFi) � ����������� �� �������� ����� �������� � Pad'�� �� �������� ����������.
{
    float dlt = m_pBallPicObj->GetCenter().x - pPad->GetCenter().x;	//������� ���������� ������ �� ������ Pad'� ��� �����
    float weightedDlt = dlt / pPad->GetSize().x / 2; //delta ���������� �� ��������� � 1/2 ����� Pad'� (�.�. ���������� � �������� [0, 1]
    m_fFiPlus = (PI/2) * weightedDlt * ( (fabs(m_fFi)-PI/2) / PI / 2 );	//������������ ������������� � ���� == PI/4, �������������� ..
                                                                           //.. ���������� dlt'�� � �������� m_fFi � PI/2, ���������� � ..
                                                                          //.. ����� PI/2 (�.�. ���� [0, 1]). ��� m_fFi ==����� PI/2 ..
                                                                         //.. (3,1417/2) ���� ����� �� ����� ���������� < PI/2. �.�. ..
                                                                        //.. ������������� ���������� - ����� �� ������� ���� ��-�� ��.
}


//// IsHitWithScreenBorders() ////
void _BALL::IsHitWithScreenBorders(
        const float             fSceneW,
        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts  //�������������� vector Triggered Hit Check Points - ����������� ������� ��������
    ) const
{
    if (0  >=  m_svHitChckPnts[N].y + m_pBallPicObj->GetPos().y)             rsltTrggrdHitChckPnts.push_back(N);
    if (0  >=  m_svHitChckPnts[W].x + m_pBallPicObj->GetPos().x)             rsltTrggrdHitChckPnts.push_back(W);
    else if (m_svHitChckPnts[E].x + m_pBallPicObj->GetPos().x  >=  fSceneW)  rsltTrggrdHitChckPnts.push_back(E);
}


//// IsHitWithPad() ////
void _BALL::IsHitWithPad(
        //Inp.:    
        CPictureObject* const   pPad,
        const float             fMaxDltYWithPad,  
        const float             fDltPxlsToChckHit,   //��� ���� ������, ������ ����� ��������� �������, ������, �.�. �������� ..
                                                    //.. ��������� ����� �� ��������� �������� ����� ���������� �������� ..
                                                    //.. �������������� (��. "#OPT") ��������.
        //Outp.:
        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts,  
        bool&                   rsltNeedToCheckBricks
     )
{
    //����������� (����� #OPT): ���� ����� ���� ����������� Pad'�, ��� ���� ������ ��� ���� (����������� Pad'a +10), ..
    //.. �� �������� � Pad'�� ������ �� ����� - �������� ������� ���.�������� ������� �� 2�� (Pad ������ 1��)
    if ( m_svHitChckPnts[S].y + m_pBallPicObj->GetPos().y   <   pPad->GetPos().y - fDltPxlsToChckHit  ||	
         m_pBallPicObj->GetPos().y   >   pPad->GetPos().y + fMaxDltYWithPad  ||   //if ����� ������ ���� �������� Pad'�,�� ��� game over.
         m_svHitChckPnts[E].x + m_pBallPicObj->GetPos().x   <   pPad->GetPos().x - fDltPxlsToChckHit  ||
         m_svHitChckPnts[W].x + m_pBallPicObj->GetPos().x   >   pPad->GetPos().x + pPad->GetSize().x + fDltPxlsToChckHit )
         /* , �� - ������. � ������� ����� - ��������� (��� ����� ��������) */;
    else
    {
        //����������� �������� �������� HitChckPnts, ���-� ����� ��������� ��� ������ ������������ ..
        //.. � Pad'�� - �.�. ������� ������ ���� �������������� ������ ������� ����� ������:
        u32 begin = int (m_suNUM_OF_HIT_CHCK_PNTS/4.0 + 0.5);   // N/4 + .5
        u32 end = m_suNUM_OF_HIT_CHCK_PNTS - begin;             // N-(..)
        
        for (u32 d=begin;  d<=end;  ++d)  //��� 8 �������� �������� [2, 6] == [E, W]
        {                               //#OUTDATED (���� ��������� �/ ��������� ����������� � _DIRECTION): #WARNING(possibly). �������� �������� implicit �������������� ����� �����.
            if ( pPad->Hit(m_svHitChckPnts[d] + Vec3To2(m_pBallPicObj->GetPos())/*,0*/) )   /// ��� WRN: "C4239: nonstandard extension used : 'argument' : conversion from 'vector2' to 'vector2 &' "  - ��� ����-� "+" ������������� �/ `vector2` ��������� 2�� ���-� �� ������. ������, �� ��.

            {
                rsltTrggrdHitChckPnts.push_back(static_cast<_DIRECTION>(d));
                rsltNeedToCheckBricks = 0;
                //������� �������� (m_fFiPlus) � ���� (m_fFi) � ����������� �� �������� ����� �������� � Pad'�� �� �������� Pad'�:
                if (S == rsltTrggrdHitChckPnts.back())	//�� ����������� ����, ���� ������� �� ���� ��� �����.�����������
                    ComputeFiPlus(pPad);
            }
        }
    }
}


//// IsHitWithBricks() ////
void _BALL::IsHitWithBricks(
        const float                         fDltPxlsToChckHit,      // :Inp.    //����� - �� ������� �������� �� �������� ������ �������� ..
                                                                    //.. �������� ��������� rect'�� �� ��������� ��������.
        /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,
        std::vector<_DIRECTION>&             rsltTrggrdHitChckPnts   // :Outp.
    )
{
//#OUTDATED:
//#if LOCKS==1    ///@@ Stoped here (2014-08-29) - ������ � ��������� - ��� �� ��������� ������������ - � ����� scope ���������?
//        while (_BRICK::BricksLockOn(1) != 1)	///In IsHit()
//        {	/*������� ����� �� ��������� � m_vpBricks {{������ ������ ��: `m_LavBricks`}}. ���� ��� ������� ���-�� ������, ���*/

            std::auto_ptr<_LAV_OFBRICKS_HNDLR> pLavHndlr = lavBricks.CreateAccessHandler(2);
#if DEBUG==1
            if (nullptr == pLavHndlr.get()) throw CMyExcptn(22);
#endif //DEBUG

//#TODO: ���� �� ����� �������� ��� �����, ������������ ���� ���:
//#if LOW_LVL_LOCKS==1	
//            //�������: �������� �������� ��� ��������
//            if (_BRICK::BricksLockOn(1) != 1)
//                return NO;
//            else
//                break;
//#endif //LOW_LVL_LOCKS

//#OUTDATED:
//        }
//#endif //LOCKS

        //! ���������� �������:
        ///@@@ ������ ������:
        //for (auto it = m_vpBricks.begin(); it!=m_vpBricks.end(); ++it)	//���� �������� � ���������� ���������� ��-�� ���������������
        //for (auto it = m_vpBricks.rbegin(); it!=m_vpBricks.rend(); ++it)	///@@@ ��������� ITERAOR N
        //for (char ib = m_vpBricks.size()-1; ib>=0; --ib)
        //#OUTDATED: for (char ib = 0; ib < m_vpBricks.size(); ++ib)	//�������
        for (u32 ib = 0;  ib < (*pLavHndlr)->size();  ++ib)	//�������
        {		
            /*�������*/ CAniObject ao = /*m_vpBricks*/(*(*pLavHndlr))[ib]->GetBrickAniObj();	//�������: ������ ��������� ���� ����� ������� - �� ��������
            //CAniObject *ao = (*it)->getBrickAniObj();	//��� ������������
            //CAniObject ao (m_vpBricks[ib]->getBrickAniObj());	//��� ������������
                //if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao->GetPos().y -dlt  ||	//Opt.: �������� ����������� ����������� ���������������
                //m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao->GetPos().y + ao->GetSize().y +dlt ||	//opt
                //m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao->GetPos().x -dlt  ||					//opt
                //m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao->GetPos().x + ao->GetSize().x +dlt)	//opt
            //#OPT: �������� ����������� ����������� ���������������:
            if (m_svHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao.GetPos().y + ao.GetSize().y +fDltPxlsToChckHit || 
                m_svHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao.GetPos().x -fDltPxlsToChckHit  || 
                m_svHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao.GetPos().x + ao.GetSize().x +fDltPxlsToChckHit ||
                m_svHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao.GetPos().y -fDltPxlsToChckHit)
                continue;   //! �.�., ���� ��� �������� ����������� ��������������� (�� ������� ������ "- fDltPxlsToChckHit"), �� � ����. �������.
	///@@@ #OPT: ���� � �� ���� ������������ �� ��������������� �������� �� ������, �� ��� ����� ������ �������� ������ ��� 3 ����� (����. ��� 5 - �.�. �� 1/2 ������)
    ///.. �/ ����� ����������� �������.�������, �������� �������� �� � ��������� ������. ... ���� ... �.�.�. ����������, ��� ��������� ����� ����� �� 4 �������, � �2, �.�. ����� ����� �� ���� ����������� �����, � ��������� � ��������, ����� ��� 4 ������� ����� ����������� ������������.
    
    ///@@@ � ��.�������, ����� ���.���.�����, ���. ����� ���������, ������� �� �������� ��. �������� ��� ����!
            //! ��������� �������� ��� �� ���-���-������� (�.�. �� ������ - �������� ������������)
            //! ���������� ������� � ���� ���� ������������, ���������� � ������ ..
            //!.. `rsltTrggrdHitChckPnts` ��� ������������ ������� (�� enum _DIRECTION).
            //!.. ����� ��� ������������ ������ �������� �������� ������� m_state �� "ING" (in game) �� "DSTR_ST" (destruction start).
            for (u32 i=0;  i < m_suNUM_OF_HIT_CHCK_PNTS;  ++i)
            {
                if ( /*ao->*/ao.Hit(m_svHitChckPnts[i] + Vec3To2(m_pBallPicObj->GetPos())/*,0*/) )  //! `m_svHitChckPnts[i]` - ������������� ..
                {                                                                                   //!.. ���������� ������� ���-�� ��������� ..
                                                                                                    //!.. ������ (�.�. �����.������ ��� ����).
                    rsltTrggrdHitChckPnts.push_back(static_cast<_DIRECTION>(i));                     
                    if (/*(*it)*/ (*(*pLavHndlr))[ib]/**/->m_state == _BRICK::ING)      // "==" comparison
                        /*(*it)*/ (*(*pLavHndlr))[ib]/**/->m_state = _BRICK::DSTR_ST;   // "="  assignment
                }
            }
        }
//#OUTDATED: #if LOCKS==1
#if DEBUG==1
            //if (pLavHndlr->GetReservedKey() == pLavHndlr->GetCurrValOfLockCode()) throw CMyExcptn(16); /*�������� ����� �� ��������� � m_vpBricks.*/ ///@@@ ����-� �� ��
        if (pLavHndlr->GetReservedKey() != pLavHndlr->GetCurrValOfLockCode()) throw CMyExcptn(16);
        
        if (0 != lavBricks.ReleaseAccessHandler(pLavHndlr))     //! �������� ����� �� ��������� � lavBricks.
            throw CMyExcptn(26);

//#OUTDATED:
//#else
//        CBrick::BricksLockOff(1)
#elif
        lavBricks.ReleaseAccessHandler(pLavHndlr);

#endif //DEBUG
//#OUTDATED: #endif //LOCKS
}


//������������� � ������������� �����������
//#UGLY: ����������� ���� �� ������ �������� ��������� � ���� _DIRECTION ����� �/� typedef - ����� � ���� nested classes, ��� typedef ����� ������ ���� ����� � �����. �� � �����.����. ����������� ������������ �������-��������� ("auto .. )-> ...")
_BALL::_DIRECTION  _BALL::InterpretIsHitResults(std::vector<_DIRECTION>& vTrggrdHitChckPnts) const
{
    if (vTrggrdHitChckPnts.empty()) return NO;	//��� ��������

    _DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        ///#WRONG? : ��� � ����������� ����, ��� ��������� ������� ����� � ����� ������������������, � ���  " vTrggrdHitChckPnts[0]+1 "  - ��� ���� � ������ "+" � �� "-" 1 ??!
        ///if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);	// �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
        
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	// �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
        {
            if (vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0] == 2)
                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);
            else //i.e. (vTrggrdHitChckPnts[0]-vTrggrdHitChckPnts[1] == 2)
                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[1]+1);
        }
        // �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1� �� 2� �����.������� ������.��������
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2� �� 2� �����.������� ������.��������	//��� ������ ����� ������� ���� ����������, �� �������� ��� ��������� ������� ����
#if DEBUG==1
        else 
        {
  #if HARD_DEBUG==1
            time_t currT = time(NULL);
            exceptPrintFile << asctime(localtime(&currT))<< "!!!: vTrggrdHitChckPnts.size()==2, but combination is unexpected:"
                "  [0]-"<< vTrggrdHitChckPnts[0]<< 
                ", [1]-"<< vTrggrdHitChckPnts[1]<< std::endl;
  #endif //HARD_DEBUG==1
            throw CMyExcptn(7);
        }
#endif //DEBUG==1
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2)
    {
  #if HARD_DEBUG==1
        time_t currT = time(NULL);
        exceptPrintFile << asctime(localtime(&currT))<< "!!!: vTrggrdHitChckPnts.size() > 2. Values combination is:"    ///@@@ #WRN: `asctime()` & `localtime()` are unsafe - use `asctime_s()` & `localtime_s()` instead (��.����� use of this func. in other places in the PRO)
            "  [0]-"<< vTrggrdHitChckPnts[0]<< 
            ", [1]-"<< vTrggrdHitChckPnts[1]<< 
            ", [2]-"<< vTrggrdHitChckPnts[2]<< 
            ", [3]-"<< vTrggrdHitChckPnts[3]<< 
            ", [4]-"<< vTrggrdHitChckPnts[4]<< 
            ", [5]-"<< vTrggrdHitChckPnts[5]<< 
            ", [6]-"<< vTrggrdHitChckPnts[6]<< 
            ", [7]-"<< vTrggrdHitChckPnts[7]<< std::endl;
  #endif //HARD_DEBUG==1
        throw CMyExcptn(4);		//assert(vHP.size()<=2);
    }
#endif //DEBUG==1

    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//�.�. if �������.������ ��������
    else throw CMyExcptn(11);	//�� ����� - �������� �� .empty() ���� � ������

    return HitReview(dir);
}


//#UGLY: ����������� ���� �� ������ �������� ��������� � ���� _DIRECTION ����� �/� typedef - ����� � ���� nested classes, ��� typedef ����� ������ ���� ����� � �����. �� � �����.����. ����������� ������������ �������-��������� ("auto .. )-> ...")
_BALL::_DIRECTION  _BALL::IsHit(
        //#XI: CPictureObject* const               pCurrBall,
        CPictureObject* const               pPad,
        /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,      // :Inp.
        const float                         fSceneW,
        CArkanoidController::GAME_STATE     gameState,
        const float                         fDltPxlsToChckHit /*=3*/  //���������������� ����-� by def., ������������� � ���������� ������.
    )   
{																		///@@@ !!! �������������� !!!
    if (this == m_spIsAttachedToPad  ||  RUN != gameState) return NO;
    bool needToCheckBricks = 1;	//��� ��� ����� ��� ��������� �������� ��������: �.�. ������� ����������� ����������, ��, ..
                                //.. ���� �������� ������������ � Pad'��, �� �������� �������� ������������.
    std::vector<_DIRECTION> vTrggrdHitChckPnts;	//vec. of triggered ball hit check points 
                                                ///@@@����� ����� ������� ������� ��� ����� ��������� (��.��. 2 ���������� ��) - ��� - �� �����������.

//����� ������������ � <^> ������ ������
    IsHitWithScreenBorders(fSceneW, vTrggrdHitChckPnts);

//����� ������������ � Pad'��
    //#XI: IsHitWithPad(pPad, pPad->GetCenter().y - pCurrBall->GetCenter().y, fDltPxlsToChckHit, vTrggrdHitChckPnts, needToCheckBricks);
    IsHitWithPad(pPad, pPad->GetCenter().y - GetBallPicObj()->GetCenter().y, fDltPxlsToChckHit, vTrggrdHitChckPnts, needToCheckBricks);

//����� ������������ � ���������
    if (needToCheckBricks)	//��.��������� ���� - � �������� ���� ����������
        IsHitWithBricks(fDltPxlsToChckHit, lavBricks, vTrggrdHitChckPnts);

//������������� � ������������� �����������
    return InterpretIsHitResults(vTrggrdHitChckPnts);

//#OLD (������ � `InterpretIsHitResults()`):
//    if (vTrggrdHitChckPnts.empty()) return NO;	//��� ��������
//
//    _DIRECTION dir;
//    if (vTrggrdHitChckPnts.size()==2)
//    {
//        ///#WRONG? : ��� � ����������� ����, ��� ��������� ������� ����� � ����� ������������������, � ���  " vTrggrdHitChckPnts[0]+1 "  - ��� ���� � ������ "+" � �� "-" 1 ??!
//        ///if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);	// �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
//        
//        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	// �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
//        {
//            if (vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0] == 2)
//                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[0]+1);
//            else //i.e. (vTrggrdHitChckPnts[0]-vTrggrdHitChckPnts[1] == 2)
//                dir= static_cast<_DIRECTION>(vTrggrdHitChckPnts[1]+1);
//        }
//        else if ( (vTrggrdHitChckPnts[0]==N  && vTrggrdHitChckPnts[1]==W ) || (vTrggrdHitChckPnts[0]==W  && vTrggrdHitChckPnts[1]==N ) )  dir= NW;  //1� �� 2� �����.������� ������.��������
//        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) || (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) )  dir= N;   //2� �� 2� �����.������� ������.��������	//��� ������ ����� ������� ���� ����������, �� �������� ��� ��������� ������� ����
//#if DEBUG==1
//        else throw CMyExcptn(7);
//#endif
//    }
//
//#if DEBUG==1
//    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
//#endif
//
//    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//�.�. if �������.������ ��������
//
//#if DEBUG==1
//    else throw CMyExcptn(11);	//�� ����� - �������� �� .empty() ���� � ������
//#endif
//
//    return HitReview(dir);
}


//�������� �� ��������� ����������� ��� ������������� ��������, �������� ���������� �� �������� ��� ��� ���������
//#UGLY: ����������� ���� �� ������ �������� ��������� � ���� _DIRECTION ����� �/� typedef - ����� � ���� nested classes, ��� typedef ����� ������ ���� ����� � �����. �� � �����.����. ����������� ������������ �������-��������� ("auto .. )-> ...")
_BALL::_DIRECTION  _BALL::HitReview(_DIRECTION dir) const
{
#if DEBUG==1
    if (m_fFi>PI || m_fFi<= -PI) throw CMyExcptn(12);
#endif
    //! ����� ������� ����� ��������, ����� � ������� ����������� �������� ������ ��� ������������, ..
    //!.. ������������ � ������� ������������ �������. �.�. ��� ���� ������ ������������ �������, ..
    //!.. ����� ����� ��� �������� ����� ��� ������������� ��������, �� �� ����� ��� �������� ..
    //!.. ���������� ������, ����� ���������������(-��) ������(-�) ����� �� ���� ������������. ..
    //!.. ���� ��� ������������ ������� ��� �������� - ����� ������� ������, ����� ����� ����� ����� ..
    //!.. ����������� ����������� � ����������� ����� ����������, ���������� ����� ����� ����� ..
    //!.. ������� (�.�. ���-�� ����� "�����" �� ��� �������� - ����������� �������� ���������� ���� ..
    //!.. �� ������).
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


//// InitFewPixelsShift() ////
void _BALL::InitFewPixelsShift()
{
    float currAng;
    for (u32 i=0; i < m_svHitChckPnts.size(); ++i)
    {
        currAng = m_svfBallHitChckPntsAngles[i]; //��� ������������. ������, ��� �����, �.�. ���� ����� ������������ ������ �������.
        m_smFewPixelsShift[static_cast<_DIRECTION>(i)] = vector2(   //#COMPILER(type_cast)
                                                            m_suNUM_PIXELS_4_SHIFT *-sin(currAng), 
                                                            m_suNUM_PIXELS_4_SHIFT * cos(currAng)
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
