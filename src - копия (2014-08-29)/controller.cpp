////////////////////////////////////////////////////////////////
//
//  controller.cpp: a part of 2DTemlate application
//  authors: Yulia Malyarevich
//  ©2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#include "controller.h"
#include "cursor.h"
#include "obj.h"
#include <sstream>					//для std::ostringstream
#include <time.h>					//для std::srand(time(NULL))
//#include <random>					//для MyRandom::GetInst()()  НО не работает
#include <array>					//для std::array	
#include <vector>					//для std::vector
#include <time.h>					//для sleepMy()
#include <fstream>
#include "myExcptn.h"
#include "globals.h"
#include "ballsManager.h"
#include "bricksManager.h"
#include "brick.h"
#include "guiPrint.h"


using guiPrint::Print;


float dbgTmp0, dbgTmp2, dbgTmp3, dbgTmp6;

////Структура и вектор для организации вывода сообщений об исключениях	//Пока не использую
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



///@@@ #TODO: заменить на флаг "ничего не делать в OnShow()"
//Delay для gameOver()
void sleepMy(int seconds)
{
    clock_t time_end;
    time_end = clock() + seconds * CLOCKS_PER_SEC;
    while (clock() < time_end)
    {
    }
}



//Метод вызывается впервые в OnShow() и далее после каждого WIN'а
void CArkanoidController::InitNewGame(CScene *sc)
{
#if DONT_DELETE_ANI_BRICKS==1
    if (m_GameState==NEW)
#else
    if (m_GameState!=LOST)
#endif
    {
        m_pBricksMngr->AddBrick(sc->FindAniObject(ANI_SC01_BRICK), (*m_vpvBricksOnLvlsCoords[0])[0], sc);
        //#OUTDATED: переделал цикл под итераторы
        //for (u32 ibr=1; ibr < m_uNUM_OF_BRICKS_ON_LVL1; ++ibr)	//frm "1", т.к. 0й кирпич внесли отдельно - он базовый
        //{
        //    addBrick(m_vpBricks[0]->getBrickAniObj(), m_aBricksCoordsLvl1[ibr], sc);
        //}
        for (auto it = m_vpvBricksOnLvlsCoords[0]->begin()+1;  it != m_vpvBricksOnLvlsCoords[0]->end(); ++it)	//frm "1", т.к. 0й кирпич внесли отдельно - он базовый
        {
            m_pBricksMngr->AddBrick(/*m_pBricksMngr->m_vpBricks[0]->GetBrickAniObj()*/sc->FindAniObject(ANI_SC01_BRICK),  *it,  sc);    ///@@@ #TODO: разобраться, какого рукается на "ptr на неполный тип" КОМ-р, с 1ым пар-ром `m_pBricksMngr->...`
        }
        /*после отладки дописать ещё кубики до общ.кол-ва 12 и исправить соотв. константу*/
        //вообще наверное тут лучше научиться работать с xml и загружать из него всю переменную информацию по уровню.
#if DONT_DELETE_ANI_BRICKS==1
        m_pBricksMngr->InitOfBricksAniObjects(m_vpBricks);
#endif
    }
#if DONT_DELETE_ANI_BRICKS==1
    else if (m_GameState==WON)
    {
        RetrieveBrick(apBricksAniObjects, m_aBricksCoordsLvl1);
    }
#endif
    //далее код универсален и для "LOST" и для "WON" и для 1го запуска
    m_pBallsMngr->AddBall(aBallHitChckPnts, true);
    m_uGameScore = 0;
    m_GameState = RUN;
}



//// ProcessWinLostState() ////
void CArkanoidController::ProcessWinLostState(CScene *sc)
{
    if (m_GameState==RUN)
        return;

    //GameOver
    if (m_GameState==LOST)
    {
        Print("G A M E   O V E R", (g_fSceneHeight-15)/2, (g_fSceneWidth-100)/2, 1,250,250, 100);
        Print("Click to continue", (g_fSceneHeight+40)/2,g_fSceneWidth*2/5+40, 40,40,40, 100);
		
        //ожидание клика (подбробное описание см.в месте определения переменной)
        if (m_WaitForClick==0)
            m_WaitForClick=1;
        if (m_WaitForClick==1)
            return;

        sleepMy(5);///@@@@ надпись не успевает почему-то показаться. Попробовать писать тут в файл, а в OnRender() проверять флаг, и печатать из файла. Пока обошёлся m_WaitForClick()'ом
        Print("", (g_fSceneHeight-10)/2, (g_fSceneWidth-30)/2, 10,220,220, 100);
        //ballsPictObjectsUsage.fill(0);	//это лишнее
        InitNewGame(sc);

        m_WaitForClick=0;
    }
	
    //Victory
    else if (m_GameState==WON)
    {
        Print("C O N G R A T U L A T I O N S!   Y O U   W O N!  SCORE IS: ", m_uGameScore, (g_fSceneHeight-15)/2,g_fSceneWidth/4, 220,10,220, 100);
        Print("Click to continue", (g_fSceneHeight+40)/2,g_fSceneWidth*2/5+40, 40,40,40, 100);
        //удалить все мячики в цикле (проследить, чтобы граф.объекты их тоже уехали за поле
#if DEBUG==1
        if (!m_vpBricks.empty()) throw CMyExcptn(10);
#endif

        if (!g_vBalls.empty())
            for (char i=g_vBalls.size()-1;  i>=0; --i)
                delBall(i);

		//ожидание клика (подбробное описание см.в месте определения переменной)
        if (m_WaitForClick==0)
            m_WaitForClick=1;
        if (m_WaitForClick==1)
            return;
		
        InitNewGame(sc);
        Print("", (g_fSceneHeight-10)/2, (g_fSceneWidth-30)/2, 220,10,220, 100);

        m_WaitForClick=0;
    }
}																											


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//// С-tor() ////
CArkanoidController::CArkanoidController(u32 sceneId)
    :ISceneController(sceneId), 
    m_uGameScore(0), 
    m_fBOTTOM_GAP(35),
    
//#OUTDATED: (прим.см. controller.h, str.31)
//#if NUMBER_OF_BRICKS == 12
//    m_uNUM_OF_BRICKS_ON_LEVEL1(12),
//#elif NUMBER_OF_BRICKS == 2
//    m_uNUM_OF_BRICKS_ON_LEVEL1(2),
//#endif
    
    ///@@@ #TODO: когда будут окончательно очерчены классы CBallsManager и CBricksManager, либо инициализ-ть тут, либо решить, где, ну и собсно сделать.
    m_pBallsMngr(nullptr),
    m_pBricksMngr(nullptr), 
    //m_pBallsMngr(new CBallsManager()),     //ptr на singleton-экземпляр менеджера шариков 
    //m_pBricksMngr(new CBricksManager(m_uNUM_OF_BRICKS_ON_LEVEL1)), 
    m_WaitForClick(WFC_OFF)
    

{
#if DONT_DELETE_ANI_BRICKS 1
    m_GameState = NEW;
#else
    m_GameState = RUN;
#endif

    m_fSceneHeight = 600;                           //Это dflt value, переопределяю в OnShow()				
    m_fSceneWidth = 800;                            //Это dflt value, переопределяю в OnShow()				
    m_fPaddleY1 = m_fSceneHeight - m_fBOTTOM_GAP;	//Это dflt value, переопределяю в OnShow()
    m_fCurrPaddleY = m_fPaddleY1;                       //Это dflt value, переопределяю в OnShow()

}



//// D-tor() ////
CArkanoidController::~CArkanoidController()
{
    if (nullptr != m_pBallsMngr)
        delete m_pBallsMngr;
}



//// OnShow() ////
void CArkanoidController::OnShow()
try {
    g_fSceneHeight = CRender::GetInst().GetHeight()/*768*/;
    g_fSceneWidth = CRender::GetInst().GetWidth()/*1024*/;
    CScene *sc = m_scene;
    g_pPad = sc->FindPictureObject(PIC_SC01_PADDLE);
    g_pBall = sc->FindPictureObject(PIC_SC01_BALL);

    //Готовлю array-входн.пар-р для C-tor'а CBallsManager.  //Вынес этот код сюда, чтобы уйти от зависимости ..
                                                            //.. класса CBallsManager от кол-ва шариков.
    std::array<CPictureObject*,0U> aBallPicObjs;
    aBallPicObjs[0] = sc->FindPictureObject(PIC_SC01_BALL);
    aBallPicObjs[1] = sc->FindPictureObject(PIC_SC01_BALL2);
    aBallPicObjs[2] = sc->FindPictureObject(PIC_SC01_BALL3);
    aBallPicObjs[3] = sc->FindPictureObject(PIC_SC01_BALL4);

    m_pBallsMngr = new CBallsManager(g_pBall, aBallPicObjs);    //conditional delete is in D-tor.
    InitBallHitChckPntsArr();
    InitOfBallsPictObjects(sc);	//инициализирую массив CPict.Object'ов шариков
//#OUTDATED:    CPictureObject* pOriBall = sc->FindPictureObject(PIC_SC01_BALL);  ///@@@ может вынести в глоб. vars (тем более, если его, как и oriBrick не задейтсвовать в самой игре
    g_fPaddleY1 = g_fSceneHeight - BOTTOM_GAP - (u32)g_pPad->GetSize().y;  //#COMPILER(Warning) `(u32)`
    g_fPaddleY = g_fPaddleY1;
//#OUTDATED:    g_fMaxDltYWithPad = g_pPad->GetCenter().y - pOriBall->GetCenter().y;   //if(ball[i]..->GetPos().y > g_fPaddleY+g_fMaxDltYWithPad) ..
//                                                                            //.. то больше не просчитывать isHit() с Pad'ом.
    std::srand((int)time(NULL));		//для того, чтобы rand() выдавал не одно и то же
    InitFewPixelsShift();	//инициализирую таблицу 1-пиксельных смещений шарика при контакте
	
    InitNewGame(sc);
}
EXCPTN_CATCH



//// OnUpdate() ////
void CArkanoidController::OnUpdate(u32 _dt)
try {

    //Костыли: перенёс сюда из OnRender()
    //Вызываю метод, отыгрывающий анимацию уничтожения в 1й проход и удаляющий объект во второй
    for (char i=m_vpBricks.size()-1; i>=0; --i)	//Костыли: переносил это в OnUpdate() - не помогло
    {
	    if (m_vpBricks[i]->m_state == CBrick::DSTR_ST || m_vpBricks[i]->m_state == CBrick::DSTR_EN)
		    delBrick(i, /*sc*/m_scene);
    }

    //Удалить объекты класса CBall вылетевших за пределы поля шариков
    if (!g_vBalls.empty())	//tmp: using ITERATOR 2
    {
        for (char i=g_vBalls.size()-1;  i >= 0;  --i) //Не итератором, т.к. теряют валидность при .erase()	//(auto it=g_vBalls.begin();  it != g_vBalls.end();  ++it)
        {
            CPictureObject *bo = g_vBalls[i]->getBallPicObj();
            if (bo->GetPos().x < -bo->GetSize().x -5  ||  bo->GetPos().x > g_fSceneWidth +5  ||  
                bo->GetPos().y < -bo->GetSize().y -5  ||  bo->GetPos().y > g_fSceneHeight +5)
            {
                delBall(i);	//условие проигрыша проверяю в методе delBall()
                //Не использую тут итераторы, а перебираю в обратном порядке, потому что внутри delBall() удаляется it и, ==> происходит ресайз вектора, наружный цикл более неработоспособен, т.к. итераторы уже недействительны.
            }
        }
    }

    ////Провека коллизий и пересчёт траекторий
    //DIRECTION dir;	//Костыли: перенёс отсюда в OnRender() - не помогло
    //for (auto itb=g_vBalls.begin(); itb!= g_vBalls.end(); ++itb)	///@@@ Использование ITERATOR'а 
    //{
    //	dir=(*itb)->isHit();
    //	if (dir != -1)
    //	{
    //		(*itb)->getBallPicObj()->MoveToPolarRo(FLT_MAX,  (*itb)->getNewMoveCenter(dir),  (*itb)->getVel());
    //		//сдвигаю шарик на 2-3пикселz, чтобы выйти из тек.коллизии. Пояснчения причин и альтернативы см. в прим.к описанию метода initOf1PixelShift()
    //		(*itb)->getBallPicObj()->SetPos( vec3To2((*itb)->getBallPicObj()->GetPos()) + mFewPixelsShift[dir] );
    //	}
    //}
}
EXCPTN_CATCH



//// OnRender() ////
void CArkanoidController::OnRender(vector2 _scroll)
try {
    LOG_INFO("CArkanoidController::OnRender");
    vector2 pos = CCursorController::GetInst().GetCursorPos();

    CScene *sc = m_scene;

    ////Вызываю метод, отыгрывающий анимацию уничтожения в 1й проход и удаляющий объект во второй
    //for (char i=m_vpBricks.size()-1; i>=0; --i)	//Костыли: переносил это в OnUpdate() - не помогло
    //{
    //	if (m_vpBricks[i]->m_state == CBrick::DSTR_ST || m_vpBricks[i]->m_state == CBrick::DSTR_EN)
    //		delBrick(i, sc);
    //}

    processWinLostState(sc);

    Print("SCORE: ", m_uGameScore, 15,g_fSceneWidth-100, 240,10,240,100);

#if DEBUG==1																										
    CPictureObject* ball1 = sc->FindPictureObject(PIC_SC01_BALL);
    //dbgPrint("rndmAng1 = ", dbgTmp0, 10,10, 255,10,255);
    if (!g_vBalls.empty())
	    dbgPrint("currBallFi = ", g_vBalls[0]->getFi(), 10,200, 255,10,255);
    dbgPrint("currMouse X : Y = ", pos.x, pos.y, " : ", 25,10, 10,10,10);
	
    dbgPrint("ball1->GetPos().x : .y    ", ball1->GetPos().x, ball1->GetPos().y, " : ", 300);
    //dbgPrint("rand()%... :   ", dbgTmp6, 355,10, 50,50,1);
#endif						

    //Костыли: перенёс сюда из OnUpdate()
    //Провека коллизий и пересчёт траекторий
    DIRECTION dir;
    for (auto itb=g_vBalls.begin(); itb!= g_vBalls.end(); ++itb)	///@@@ Использование ITERATOR'а 
    {
        dir=(*itb)->isHit();
        if (dir != -1)
        {
            (*itb)->getBallPicObj()->MoveToPolarRo(FLT_MAX,  (*itb)->getNewMoveCenter(dir),  (*itb)->getVel());
           //сдвигаю шарик на 2-3пикселz, чтобы выйти из тек.коллизии. Пояснчения причин и альтернативы см. в прим.к описанию метода initOf1PixelShift()
           (*itb)->getBallPicObj()->SetPos( vec3To2((*itb)->getBallPicObj()->GetPos()) + mFewPixelsShift[dir] );
        }
    }

}
EXCPTN_CATCH



//// OnMouseMove() ////
void CArkanoidController::OnMouseMove(u32 _x, u32 _y)
try {	///@@@ привязать курсор мыши к центру paddle'а и добавить ограничение, чтобы не вываливалась и влево тоже. А также откорректировать эту проверку. И вынести весь этот код в отдельную ф-цию.
    CPictureObject* pad = m_scene->FindPictureObject(PIC_SC01_PADDLE);
	
    //! Move Pad
    if (_x > g_fSceneWidth - pad->GetSize().x)
        pad->SetPos((float)g_fSceneWidth - pad->GetSize().x, (float)g_fPaddleY);									//w(tc)
    else
        pad->SetPos((float)_x, (float)g_fPaddleY);															//S$-c

    //! Move Ball with Pad
    if (CBall::getIsAttachedToPaddle())	//этим условием экономлю время на проход по циклу на кажд.обработки оборачивающей ф-ции, т.к. часто оно не будет выполняться 
    {
        CPictureObject *bo = CBall::getIsAttachedToPaddle()->getBallPicObj();
        bo->SetPos(pad->GetCenter().x - bo->GetSize().x/2,   g_fPaddleY - bo->GetSize().y-1 );
    }
}
EXCPTN_CATCH



//// OnMouseKeyDown() ////
bool CArkanoidController::OnMouseKeyDown(u32 _key, u32 _x, u32 _y)
try {
    if (0 == _key)
    {
        if (m_WaitForClick==1)
	        m_WaitForClick=2;

        /**///m_scene->FindAniObject(ANI_SC01_BRICK)->StartMovement(MV_SC01_BRICK);

        if (CBall::getIsAttachedToPaddle()) 
        {
            CBall *b = CBall::getIsAttachedToPaddle();
            CPictureObject* bo = b->getBallPicObj();
            bo->MoveToPolarRo(FLT_MAX, b->getNewMoveCenter(S), b->getVel());	
            b->tryToInvertIsAttachedToPad();
        }
#if DEBUG==1
        dbgTmp2 = static_cast<float>( CRndGen::GetInst().Generate(/*0U, */static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180);
        dbgTmp3 = static_cast<float>( CRndGen::GetInst().Generate(/*0U, */static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180);
        dbgTmp6 = static_cast<float>( std::rand()%static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180;
#endif
    }
    return false;
}
EXCPTN_CATCH