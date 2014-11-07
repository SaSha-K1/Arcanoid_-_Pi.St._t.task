////////////////////////////////////////////////////////////////
//
//  controller.cpp: a part of 2DTemlate application
//  authors: Yulia Malyarevich
//  �2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#include "controller.h"
#include "cursor.h"
#include "obj.h"
#include <sstream>					//��� std::ostringstream
#include <time.h>					//��� std::srand(time(NULL))
//#include <random>					//��� MyRandom::GetInst()()  �� �� ��������
//#include <array>					//��� std::array 	
#include <vector>					//��� std::vector
#include <time.h>					//��� SleepMy()     ///@@@ ����� `SleepMy()` ����� �����. ����� � ���� ������ ����� ��������� ��������. Chck!
//#include <fstream>    //��.��. ����� ������ � CMyExcptn.h
#include "myExcptn.h"
#include "globals.h"
#include "ballsManager.h"
#include "bricksManager.h"
//#include "brick.h"  //@@@ #WARNING: ����������� ���� �� �����. �� ��-���� ��� ������ ���� ������ ������ �� ���������, �� �� �� ��������� � ��� CBall � CBrick
#include "guiPrint.h"
#include "dbgPrint.h"
//#include "lockableAccessVec.h"  //� `ProcessWinLostState()` ������ auto_ptr<_LAV_OFBRICKS_HNDLR>. ���� ������ �����. ��������, ������ � �� �����
#include "random.h" ///@@@ ����� ������ �/ dbgPrint'�� � `OnMouseKeyDown()`. ������!


using guiPrint::Print;
using dbgPrnt::DbgPrint;



float dbgTmp0, dbgTmp2, dbgTmp3, dbgTmp6;   ///@@@������ ���?

////��������� � ������ ��� ����������� ������ ��������� �� �����������	//���� �� ���������
//struct printContent	
//{
//  const std::string txt;
//  const u32 y;
//  const u32 x;
//  const u32 r;
//  const u32 g;
//  const u32 b;
//  printContent(std::string &txtIn, u32 yIn, u32 xIn, u32 rIn, u32 gIn, u32 bIn) 
//      : txt(txtIn), y(yIn), x(xIn), r(rIn), g(gIn), b(bIn)
//  {
//  }
//};

//std::vector<printContent> exceptPrintVec;


//// InitBricksOnLvlsCoords() ////
void CArkanoidController::InitBricksOnLvlsCoords()
{   ///@@@ ���������� ���������� ������ �� �-��� �������
    m_vpvBricksOnLvlsCoords.push_back(new std::vector<vector2>);    //������ ���-� ��������� ������ �/ lvl �1
    
    //����� ���������� �������� �� ������ �1:
#if NUMBER_OF_BRICKS == 2
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2( 59, 140));	//0
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(210, 140));	//1
#elif NUMBER_OF_BRICKS == 12
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2( 59, 140));	//0
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(210, 140));	//1
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(361, 140));	//2
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(512, 140));	//3
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(663, 140));	//4
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(814, 140));	//5
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2( 59,  89));	//6(3)
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(361,  89));	//7(3)
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(663,  89));	//8(3)
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(210, 191));	//9(1)
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(512, 191));	//10(1)
    m_vpvBricksOnLvlsCoords[0]->push_back(vector2(814, 191));	//11(1)
#endif 

    //#DEVELOP: ��� 2�� � ����.������� ������� ��� .push_back() ������ ������� ..
    //.. ������� �/ ������� ���.������ ����� 1�� � ��������� ��� ������������.
}


//// InitNewGame() ////                             ///@@ Stoped_here 2014/09/06
void CArkanoidController::InitNewGame(CScene* sc)   //����� ���������� ������� � OnShow() � ����� ����� ������� WIN'�
{
#if DONT_DELETE_ANI_BRICKS==1
    if (m_GameState==NEW)
#else
    if (m_GameState!=LOST)
#endif
    {
//#OUTDATED (����� � ��������� ����� ����_���������):
//        m_pBricksMngr->AddBrick(sc->FindAniObject(ANI_SC01_BRICK), (*m_vpvBricksOnLvlsCoords[0])[0], sc);
//        //#OUTDATED: ��������� ���� ��� ���������
//        //for (u32 ibr=1; ibr < m_uNUM_OF_BRICKS_ON_LVL1; ++ibr)	//frm "1", �.�. 0� ������ ������ �������� - �� �������
//        //{
//        //    addBrick(m_vpBricks[0]->getBrickAniObj(), m_aBricksCoordsLvl1[ibr], sc);
//        //}
//        for (auto it = m_vpvBricksOnLvlsCoords[0]->begin()+1;  it != m_vpvBricksOnLvlsCoords[0]->end(); ++it)	//frm "1", �.�. 0� ������ ������ �������� - �� �������  //ITERATOR using
//        {   ///@@@ ���^^^ ���.����� [0]. ����������!
//            m_pBricksMngr->AddBrick(/*m_pBricksMngr->m_vpBricks[0]->GetBrickAniObj()*/sc->FindAniObject(ANI_SC01_BRICK),  *it,  sc);    ///@@@ #TODO: �����������, ������ �������� �� "ptr �� �������� ���" ���-�, � 1�� ���-��� `m_pBricksMngr->...`
//        }
//        /*����� ������� �������� ��� ������ �� ���.���-�� 12 � ��������� �����. ���������*/
//        //������ �������� ��� ����� ��������� �������� � xml � ��������� �� ���� ��� ���������� ���������� �� ������.
        m_pBricksMngr->AddBricksOnStart(sc, m_vpvBricksOnLvlsCoords[0]);  //�������� ������ �������� � �������� �� � �����. (����������� �� ������ ���� � (������ � ������ �������� ���������� � ��������� ���-�����) ����� ����� ��������)

#if DONT_DELETE_ANI_BRICKS==1
        m_pBricksMngr->InitOfBricksAniObjects();
#endif
    }
#if DONT_DELETE_ANI_BRICKS==1
    else if (m_GameState==WON)
    {
        m_pBricksMngr->RetrieveBrick(m_vpvBricksOnLvlsCoords[0]);  ///@@@ #TODO: ���������� ����� [0] to get rid of
    }
#endif
    //����� ��� ����������� � ��� "LOST" � ��� "WON" � ��� 1�� �������
    m_pBallsMngr->AddBall(true, m_pPad);
    m_uGameScore = 0;
    m_GameState = RUN;
}


//// MovePad() ////
void CArkanoidController::MovePad(u32 _x, u32 _y)
{
    if (_x > m_fSceneWidth - m_pPad->GetSize().x)
        m_pPad->SetPos((float)m_fSceneWidth - m_pPad->GetSize().x, (float)m_fCurrPaddleY);									//w(tc)
    else
        m_pPad->SetPos((float)_x, (float)m_fCurrPaddleY);															//S$-c
}


//// ProcessWinLostState() ////
void CArkanoidController::ProcessWinLostState(CScene* sc)
{
    if (m_GameState==RUN)   ///@@@ ��� �����, ���������� ������ ����. �������� ������� ������� �������� "� ������ �� ���-�� ������?" (�� �.�., ����� ��� ��, ��� � ���� ������ ��������) ��� ��� - ��������, ����� ������� ��� �� �����.���, ���� ��������� �� ������ ���� �-���
        return;

    //GameOver
    if (m_GameState==LOST)
    {
        Print("G A M E   O V E R", (m_fSceneHeight-15)/2, (m_fSceneWidth-100)/2, 1,250,250, 100);
        Print("Click to continue", (m_fSceneHeight+40)/2,m_fSceneWidth*2/5+40, 40,40,40, 100);
		
        //�������� ����� (���������� �������� ��.� ����� ����������� ����������)
        if (m_WaitForClick==0)
            //m_WaitForClick=1;   //Was: error C2440: '=' : cannot convert from 'int' to 'CArkanoidController::WFC_STATE' 149
            m_WaitForClick = static_cast<WFC_STATE>(1);   

        if (m_WaitForClick==1)
            return;

        SleepMy(5);///@@@@ ������� �� �������� ������-�� ����������. ����������� ������ ��� � ����, � � OnRender() ��������� ����, � �������� �� �����. ���� �������� m_WaitForClick()'��
        Print("", (m_fSceneHeight-10)/2, (m_fSceneWidth-30)/2, 10,220,220, 100);    ///@@@??: ����� ������� ��������� � Lost, �� ��� ������ �� �����, ��� ������, �.�. ���������� �� ���������
        //ballsPictObjectsUsage.fill(0);	//��� ������
        InitNewGame(sc);

        //m_WaitForClick = 0;   //Was: error C2440: '=' : cannot convert from 'int' to 'CArkanoidController::WFC_STATE'	158
        m_WaitForClick = WFC_OFF;
    }
	
    //Victory
    else if (m_GameState==WON)
    {
        Print("C O N G R A T U L A T I O N S!   Y O U   W O N!  SCORE IS: ", m_uGameScore, (m_fSceneHeight-15)/2,m_fSceneWidth/4, 220,10,220, 100);
        Print("Click to continue", (m_fSceneHeight+40)/2,m_fSceneWidth*2/5+40, 40,40,40, 100);
        //������� ��� ������ � ����� (����������, ����� ����.������� �� ���� ������ �� ����
#if DEBUG==1
//#OUTDATED:
//        {   //��������, ��� ������ �������� ����.   ///@@@ ��� vvv �� �����, ��� �� ��� ����-��.. ����� �� �����! (( ��� "m_pBricksMngr->GetLavBricksPtr()" ��������� ��������� ptr �� `CLockableAccessVec<_BRICK*>` � ������ `OnRender()` (��.��� 356���.); ��� "std::auto_ptr<_LAV_OFBRICKS_HNDLR> pLavHndlr = m_LavBricks.CreateAccessHandler(3);" ������ ����_���, ��������� � `.CreateAccessHandler()` ����� ����� ����. � bricksManager.cpp (���.28). ������ ��� �� �������� ������ � ����� ������ �/� `->` �� ptr'� �� `m_LavBricks` �� �����!??   ������ ���� �����-�� ����������� ������ ��� ����� � ���� ���. �������, ��� ����� ������ ��, ��� �� ������ ���� ����� ���.
//                                                    ///@@@.. ����� ������ �������� ��� ������ ������� IsBricksEmpty(), ��������� ��� � ����_���������. ����� �������� ��� ����� �� ������ �����..
//            std::auto_ptr<_LAV_OFBRICKS_HNDLR> pLavHndlr = m_pBricksMngr->GetLavBricksPtr()->CreateAccessHandler(6); //m_LavBricks.CreateAccessHandler(6);
//            if ( !(*pLavHndlr)->empty() ) throw CMyExcptn(10);
//        }
        m_pBricksMngr->IsBricksEmptyCheck();
#endif

        m_pBallsMngr->CleanBalls(m_GameState);

		//�������� ����� (���������� �������� ��.� ����� ����������� ����������)
        if (m_WaitForClick==0)
            m_WaitForClick = static_cast<WFC_STATE>(1);
        if (m_WaitForClick==1)
            return;
		
        InitNewGame(sc);
        Print("", (m_fSceneHeight-10)/2, (m_fSceneWidth-30)/2, 220,10,220, 100);

        m_WaitForClick = WFC_OFF/*0*/;
    }
}									


//// SleepMy() ////
void CArkanoidController::SleepMy(int seconds)  ///@@@ #TODO: �������� �� ���� "������ �� ������ � OnShow()"
{
    clock_t time_end;
    time_end = clock() + seconds * CLOCKS_PER_SEC;
    while (clock() < time_end)  ///@@@ #TODO: �������� ���� �� ��:  #if OS==WIN  sleep(..);
    {
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                            �������������� ������ + C-tor, D-tor                                            ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//// �-tor() ////
CArkanoidController::CArkanoidController(u32 sceneId)
    :ISceneController(sceneId), 
    m_fBOTTOM_GAP(35),
    m_fSceneHeight(600),                            //��� dflt value, ������������� � OnShow()
    m_fSceneWidth(800),                             //��� dflt value, ������������� � OnShow()
    m_fPaddleY1(m_fSceneHeight - m_fBOTTOM_GAP),    //��� dflt value, ������������� � OnShow()  ///@@@#WARNING: ���������� � ������ ����-��� ����� �� ��������� - �� ���� =(
    m_fCurrPaddleY(m_fPaddleY1),                    //��� dflt value, ������������� � OnShow()
    ///@@@ #TODO: ����� ����� ������������ �������� ������ CBallsManager � CBricksManager, ���� ���������-�� ���, ���� ������, ���, �� � ������ �������.
    m_pBallsMngr(nullptr),
    m_pBricksMngr(nullptr), 
    //m_pBallsMngr(new CBallsManager()),     //ptr �� singleton-��������� ��������� ������� 
    //m_pBricksMngr(new CBricksManager(m_uNUM_OF_BRICKS_ON_LEVEL1)), 

//#OUTDATED: (����.��. controller.h, str.31)
//#if NUMBER_OF_BRICKS == 12
//    m_uNUM_OF_BRICKS_ON_LEVEL1(12),
//#elif NUMBER_OF_BRICKS == 2
//    m_uNUM_OF_BRICKS_ON_LEVEL1(2),
//#endif

    m_WaitForClick(WFC_OFF),
#if DONT_DELETE_ANI_BRICKS == 1
    m_GameState(NEW),
#else
    m_GameState(RUN),
#endif
    m_uGameScore(0)
    ///@@! �������� � ������� ���������� � ��������� ������
{
  InitBricksOnLvlsCoords();
//#OUTDATED:
//    m_fPaddleY1 = m_fSceneHeight - m_fBOTTOM_GAP;	//��� dflt value, ������������� � OnShow()
//    m_fCurrPaddleY = m_fPaddleY1;                       //��� dflt value, ������������� � OnShow()
}



//// D-tor() ////
CArkanoidController::~CArkanoidController()
{
    if (nullptr != m_pBallsMngr)
        delete m_pBallsMngr;
    if (nullptr != m_pBricksMngr)
        delete m_pBricksMngr;
}



//// OnShow() ////
void CArkanoidController::OnShow()
try {
#if DEBUG==1
    time_t startT = time(NULL);
    exceptPrintFile << asctime(localtime(&startT))<< "   < << <<< <<<< <<<<< <<<<<< GAME START {i.e. `OnShow()` started working} >>>>>> >>>>> >>>> >>> >> >" << std::endl;
#endif

    m_fSceneHeight = CRender::GetInst().GetHeight()/*768*/;
    m_fSceneWidth = CRender::GetInst().GetWidth()/*1024*/;
    CScene *sc = m_scene;                                   //@@ ?: �/ ����� ������� ������������?? (� ���� ��? - ����� ������ `m_scene` ������������)
    m_pPad = sc->FindPictureObject(PIC_SC01_PADDLE);
//#OUTDATED (������ �� �����):    g_pBall = sc->FindPictureObject(PIC_SC01_BALL);     ///@@@ ������ �� �����

    //������� array-�����.���-� ��� C-tor'� CBallsManager.  //����� ���� ��� ����, ����� ���� �� ����������� ������ CBallsManager ..
    {                                                       //.. �� ���-�� ����� ������� � ���������� �� ��������.
        std::vector<CPictureObject*> vpBallPicObjs;
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL));
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL2));
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL3));
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL4));

    //������ �������� �������:
        m_pBallsMngr = new CBallsManager(vpBallPicObjs);    //conditional delete is in D-tor.
    }
    //������ �������� ��������:
    m_pBricksMngr = new /*_BRICK_MNGR*/CBricksManager(/*12*/NUMBER_OF_BRICKS);  ///@@ ������� ���.������, �� �� ����� ������ ����.����������. �����? //��� ���? ���� �����_��_��� ? (����� ��, therefore ������� `12` �� NUMBER_OF_BRICKS)
    
    //#OUTDATED (����� ��� ����������� � CBall � ���������� � C-tor'e):
    //InitBallHitChckPntsArr();

    //#OUTDATED: InitOfBallsPictObjects(sc);	//������������� ������ CPict.Object'�� �������
//#OUTDATED:    CPictureObject* pOriBall = sc->FindPictureObject(PIC_SC01_BALL);  ///@@@ ����� ������� � ����. vars (��� �����, ���� ���, ��� � oriBrick �� ������������� � ����� ����
    m_fPaddleY1 = m_fSceneHeight - m_fBOTTOM_GAP - (u32)m_pPad->GetSize().y;  //#COMPILER(Warning) `(u32)`
    m_fCurrPaddleY = m_fPaddleY1;
//#OUTDATED:    g_fMaxDltYWithPad = m_pPad->GetCenter().y - pOriBall->GetCenter().y;   //if(ball[i]..->GetPos().y > g_fPaddleY+g_fMaxDltYWithPad) ..
//                                                                            //.. �� ������ �� ������������ IsHit() � Pad'��.
    std::srand((int)time(NULL));		//��� ����, ����� rand() ������� �� ���� � �� ��
//#OUTDATED:    _BALL::InitFewPixelsShift();	//OLD: ������������� ������� 1-(2-3..)-���������� �������� ������ ��� ��������.     //@@ ������ ����� � C-tor ������ (� �������� ����, ����������� ������ ��� cre. 1�� ������)
	
    InitNewGame(sc);
}
EXCPTN_CATCH



//// OnUpdate() ////
void CArkanoidController::OnUpdate(u32 _dt) ///@@@ Warning	C4100: '_dt' : unreferenced formal parameter 302    //Poss-�� ��� ����� ���-�� dt, ����� �� ������ � ������ ����������-��� (��� �/� `OnUpdate()`-��� ����� ��������� ��������� �����), ���� ��� � ����� ���������.
try {
    CScene *sc = m_scene;                                   //@@ ?: �/ ����� ������� ������������?? (� ���� ��? - ����� ������ `m_scene` ������������)
    //�������: ������ ���� �� OnRender()
    //������� �����, ������������ �������� ����������� � 1� ������ � ��������� ������ �� ������
    m_pBricksMngr->CleanBricks(sc, this); 	//�������: ��������� ��� � OnUpdate() - �� �������

    //������� ������� ������ CBall ���������� �� ������� ���� �������
    m_pBallsMngr->CleanBalls(m_fSceneHeight, m_fSceneWidth, m_GameState);

    ////������� �������� � �������� ����������
    //_DIRECTION dir;	//�������: ������ ������ � OnRender() - �� �������
    //for (auto itb=m_vBalls.begin(); itb!= m_vBalls.end(); ++itb)	///@@@ ������������� ITERATOR'� 
    //{
    //	dir=(*itb)->IsHit();
    //	if (dir != -1)
    //	{
    //		(*itb)->GetBallPicObj()->MoveToPolarRo(FLT_MAX,  (*itb)->getNewMoveCenter(dir),  (*itb)->GetVel());
    //		//������� ����� �� 2-3������z, ����� ����� �� ���.��������. ���������� ������ � ������������ ��. � ����.� �������� ������ initOf1PixelShift()
    //		(*itb)->GetBallPicObj()->SetPos( vec3To2((*itb)->GetBallPicObj()->GetPos()) + mFewPixelsShift[dir] );
    //	}
    //}
}
EXCPTN_CATCH



//// OnRender() ////
void CArkanoidController::OnRender(vector2 _scroll)     ///@@@ ���� ����� ����� ���-�� ���-�� ���� ���-�:  Warning	C4100: '_scroll' : unreferenced formal parameter 330
try {
    LOG_INFO("CArkanoidController::OnRender");
    vector2 pos = CCursorController::GetInst().GetCursorPos();

    CScene *sc = m_scene;   ///@@@ /*+��.� ����.������ ��� �� ������*/ ����� ������ ������ ��������� ���� ptr ������? ����� ��� ���������� ���-���, ��� ������� static'��? ���� ��.��., �.�. m_scene - ���, ������ ����, ������� (��������) (���� �������� � �����, ��� ��� ����������� �����). �� ������ �/� ��������� ����� ��� ������ ���������. ������� ���������. ??

    ProcessWinLostState(sc);    ///@@@ #TODO: ����.������ ��������� ��� ��������. ���� ����� �������� ��������� �������-�� ����� "���-�� ���������� � game_state'��". � ������ if `true`, �� ������ ���� �����?

    Print("SCORE: ", m_uGameScore, 15,m_fSceneWidth-100, 240,10,240,100);

#if DEBUG==1																										
    CPictureObject* ball1 = sc->FindPictureObject(PIC_SC01_BALL);
    //dbgPrint("rndmAng1 = ", dbgTmp0, 10,10, 255,10,255);
    if (!m_pBallsMngr->IsBallsEmpty())
        DbgPrint("currBallFi = ", m_pBallsMngr->GetBallFi(0)/*m_vBalls[0]->GetFi()*/, 10,200, 255,10,255);  ///@@: ��� ������������� - /*��������*/�� ������ ����-��, /*�.�.*/��, ��� � ������� ������� �����, ���� ������ � �������� ������ _������_ �-���, �� �� � ������ Ũ ���-���!
    DbgPrint("currMouse X : Y = ", pos.x, pos.y, " : ", 25,10, 10,10,10);
	
    DbgPrint("ball1->GetPos().x : .y    ", ball1->GetPos().x, ball1->GetPos().y, " : ", 300);
    //dbgPrint("rand()%... :   ", dbgTmp6, 355,10, 50,50,1);
#endif						

    //�������: ������ ���� �� OnUpdate()
    //������� �������� � �������� ����������    ///@@ Stoped_here 2014/09/16
    m_pBallsMngr->ProcessIsHit(m_pPad, m_pBricksMngr->GetLavBricksPtr(), m_fSceneWidth, m_GameState);

}
EXCPTN_CATCH



//// OnMouseMove() ////
void CArkanoidController::OnMouseMove(u32 _x, u32 _y)
try {	///@@@ ��������� ������ ���� � ������ paddle'� � �������� �����������, ����� �� ������������ � ����� ����. � ����� ���������������� ��� ��������. � ������� ���� ���� ��� � ��������� �-���.
    //#OUTDATED (��� ���� ���� ������): CPictureObject* pad = m_scene->FindPictureObject(PIC_SC01_PADDLE);
	
    //! Move Pad
    MovePad(_x, _y);

    //! Move Ball with Pad
    m_pBallsMngr->MoveBallWithPad(m_pPad, m_fCurrPaddleY);
}
EXCPTN_CATCH



//// OnMouseKeyDown() ////
bool CArkanoidController::OnMouseKeyDown(u32 _key, u32 _x, u32 _y)
try {
    if (0 == _key)
    {
        if (m_WaitForClick == WFC_ON) //==1
	        m_WaitForClick = WCF_DONE; //=2

        /**///m_scene->FindAniObject(ANI_SC01_BRICK)->StartMovement(MV_SC01_BRICK);

        m_pBallsMngr->BallThrowIn();
#if DEBUG==1
        dbgTmp2 = static_cast<float>( CRndGen::GetInst().Generate(/*0U, */static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180);
        dbgTmp3 = static_cast<float>( CRndGen::GetInst().Generate(/*0U, */static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180);
        dbgTmp6 = static_cast<float>( std::rand()%static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180;
#endif
    }
    return false;
}   ///@@@ �� �������, ����� � � ���.����. ������������ return (������ wrn ��������� �� ������ � EXCPTN_CATCH): Warning	C4715: 'CArkanoidController::OnMouseKeyDown' : not all control paths return a value 402
EXCPTN_CATCH