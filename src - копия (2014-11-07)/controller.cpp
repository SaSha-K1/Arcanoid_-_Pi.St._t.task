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
//#include <array>					//для std::array 	
#include <vector>					//для std::vector
#include <time.h>					//для SleepMy()     ///@@@ метод `SleepMy()` может уберу. Тогда и этот инклуд может оказаться ненужным. Chck!
//#include <fstream>    //ск.вс. нужен только в CMyExcptn.h
#include "myExcptn.h"
#include "globals.h"
#include "ballsManager.h"
#include "bricksManager.h"
//#include "brick.h"  //@@@ #WARNING: закомментил пока не глядя. Но по-идее тут должны быть ссылки только на менеджеры, но не на вложенные в них CBall и CBrick
#include "guiPrint.h"
#include "dbgPrint.h"
//#include "lockableAccessVec.h"  //в `ProcessWinLostState()` создаю auto_ptr<_LAV_OFBRICKS_HNDLR>. этот момент убрал. Возможно, больше и не нужно
#include "random.h" ///@@@ нужно только д/ dbgPrint'ов в `OnMouseKeyDown()`. Убрать!


using guiPrint::Print;
using dbgPrnt::DbgPrint;



float dbgTmp0, dbgTmp2, dbgTmp3, dbgTmp6;   ///@@@убрать это?

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


//// InitBricksOnLvlsCoords() ////
void CArkanoidController::InitBricksOnLvlsCoords()
{   ///@@@ Рассчитать координаты исходя из р-ров кирпича
    m_vpvBricksOnLvlsCoords.push_back(new std::vector<vector2>);    //Создаю век-р координат бриков д/ lvl №1
    
    //Задаю координаты кирпичей на уровне №1:
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

    //#DEVELOP: для 2го и след.уровней сделать тут .push_back() нового пустого ..
    //.. вектора д/ каждого доп.уровня свыше 1го и наполнить его координатами.
}


//// InitNewGame() ////                             ///@@ Stoped_here 2014/09/06
void CArkanoidController::InitNewGame(CScene* sc)   //Метод вызывается впервые в OnShow() и далее после каждого WIN'а
{
#if DONT_DELETE_ANI_BRICKS==1
    if (m_GameState==NEW)
#else
    if (m_GameState!=LOST)
#endif
    {
//#OUTDATED (вынес в отдельный метод Брик_менеджера):
//        m_pBricksMngr->AddBrick(sc->FindAniObject(ANI_SC01_BRICK), (*m_vpvBricksOnLvlsCoords[0])[0], sc);
//        //#OUTDATED: переделал цикл под итераторы
//        //for (u32 ibr=1; ibr < m_uNUM_OF_BRICKS_ON_LVL1; ++ibr)	//frm "1", т.к. 0й кирпич внесли отдельно - он базовый
//        //{
//        //    addBrick(m_vpBricks[0]->getBrickAniObj(), m_aBricksCoordsLvl1[ibr], sc);
//        //}
//        for (auto it = m_vpvBricksOnLvlsCoords[0]->begin()+1;  it != m_vpvBricksOnLvlsCoords[0]->end(); ++it)	//frm "1", т.к. 0й кирпич внесли отдельно - он базовый  //ITERATOR using
//        {   ///@@@ Тут^^^ маг.число [0]. Избавиться!
//            m_pBricksMngr->AddBrick(/*m_pBricksMngr->m_vpBricks[0]->GetBrickAniObj()*/sc->FindAniObject(ANI_SC01_BRICK),  *it,  sc);    ///@@@ #TODO: разобраться, какого ругается на "ptr на неполный тип" КОМ-р, с 1ым пар-ром `m_pBricksMngr->...`
//        }
//        /*после отладки дописать ещё кубики до общ.кол-ва 12 и исправить соотв. константу*/
//        //вообще наверное тут лучше научиться работать с xml и загружать из него всю переменную информацию по уровню.
        m_pBricksMngr->AddBricksOnStart(sc, m_vpvBricksOnLvlsCoords[0]);  //Заполняю вектор кирпичей и размещаю их в сцене. (применяется на старте игры и (только в случае варианта реализации с удалением ани-биков) также после выигрыша)

#if DONT_DELETE_ANI_BRICKS==1
        m_pBricksMngr->InitOfBricksAniObjects();
#endif
    }
#if DONT_DELETE_ANI_BRICKS==1
    else if (m_GameState==WON)
    {
        m_pBricksMngr->RetrieveBrick(m_vpvBricksOnLvlsCoords[0]);  ///@@@ #TODO: Магическое число [0] to get rid of
    }
#endif
    //далее код универсален и для "LOST" и для "WON" и для 1го запуска
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
    if (m_GameState==RUN)   ///@@@ это метод, вызываемый каждый такт. Возможно вынести условие проверки "А делать ли что-то вообще?" (ну т.е., вроде как то, что в этой строке написано) дык вот - подумать, может вынести его во внешн.код, чтоб экономить на вызове этой ф-ции
        return;

    //GameOver
    if (m_GameState==LOST)
    {
        Print("G A M E   O V E R", (m_fSceneHeight-15)/2, (m_fSceneWidth-100)/2, 1,250,250, 100);
        Print("Click to continue", (m_fSceneHeight+40)/2,m_fSceneWidth*2/5+40, 40,40,40, 100);
		
        //ожидание клика (подбробное описание см.в месте определения переменной)
        if (m_WaitForClick==0)
            //m_WaitForClick=1;   //Was: error C2440: '=' : cannot convert from 'int' to 'CArkanoidController::WFC_STATE' 149
            m_WaitForClick = static_cast<WFC_STATE>(1);   

        if (m_WaitForClick==1)
            return;

        SleepMy(5);///@@@@ надпись не успевает почему-то показаться. Попробовать писать тут в файл, а в OnRender() проверять флаг, и печатать из файла. Пока обошёлся m_WaitForClick()'ом
        Print("", (m_fSceneHeight-10)/2, (m_fSceneWidth-30)/2, 10,220,220, 100);    ///@@@??: вроде затираю сообщение о Lost, но вот только не пойму, как именно, т.к. координаты не совпадают
        //ballsPictObjectsUsage.fill(0);	//это лишнее
        InitNewGame(sc);

        //m_WaitForClick = 0;   //Was: error C2440: '=' : cannot convert from 'int' to 'CArkanoidController::WFC_STATE'	158
        m_WaitForClick = WFC_OFF;
    }
	
    //Victory
    else if (m_GameState==WON)
    {
        Print("C O N G R A T U L A T I O N S!   Y O U   W O N!  SCORE IS: ", m_uGameScore, (m_fSceneHeight-15)/2,m_fSceneWidth/4, 220,10,220, 100);
        Print("Click to continue", (m_fSceneHeight+40)/2,m_fSceneWidth*2/5+40, 40,40,40, 100);
        //удалить все мячики в цикле (проследить, чтобы граф.объекты их тоже уехали за поле
#if DEBUG==1
//#OUTDATED:
//        {   //Проверяю, что вектор кирпичей пуст.   ///@@@ Тут vvv Не пойму, что не нра КОМП-ру.. Вроде всё верно! (( так "m_pBricksMngr->GetLavBricksPtr()" передаётся нормально ptr на `CLockableAccessVec<_BRICK*>` в методе `OnRender()` (см.тут 356стр.); Так "std::auto_ptr<_LAV_OFBRICKS_HNDLR> pLavHndlr = m_LavBricks.CreateAccessHandler(3);" создаю ауто_птр, обращаясь к `.CreateAccessHandler()` через точку напр. в bricksManager.cpp (стр.28). Почему тут не работает доступ к єтому методу ч/з `->` из ptr'а на `m_LavBricks` НЕ ПОЙМУ!??   Должно быть какая-то накопленная ошибка даёт знать о себе тут. Надеюсь, что когда допилю всё, тут всё станет само собой гуд.
//                                                    ///@@@.. можно просто заменить эти строки методом IsBricksEmpty(), определив его в Брик_менеджере. Тогда проблему эту можно бу обойти вроде..
//            std::auto_ptr<_LAV_OFBRICKS_HNDLR> pLavHndlr = m_pBricksMngr->GetLavBricksPtr()->CreateAccessHandler(6); //m_LavBricks.CreateAccessHandler(6);
//            if ( !(*pLavHndlr)->empty() ) throw CMyExcptn(10);
//        }
        m_pBricksMngr->IsBricksEmptyCheck();
#endif

        m_pBallsMngr->CleanBalls(m_GameState);

		//ожидание клика (подбробное описание см.в месте определения переменной)
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
void CArkanoidController::SleepMy(int seconds)  ///@@@ #TODO: заменить на флаг "ничего не делать в OnShow()"
{
    clock_t time_end;
    time_end = clock() + seconds * CLOCKS_PER_SEC;
    while (clock() < time_end)  ///@@@ #TODO: заменить хотя бы на:  #if OS==WIN  sleep(..);
    {
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                                            Унаследованные методы + C-tor, D-tor                                            ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//// С-tor() ////
CArkanoidController::CArkanoidController(u32 sceneId)
    :ISceneController(sceneId), 
    m_fBOTTOM_GAP(35),
    m_fSceneHeight(600),                            //Это dflt value, переопределяю в OnShow()
    m_fSceneWidth(800),                             //Это dflt value, переопределяю в OnShow()
    m_fPaddleY1(m_fSceneHeight - m_fBOTTOM_GAP),    //Это dflt value, переопределяю в OnShow()  ///@@@#WARNING: арифметика в списке иниц-ции может не сработать - не знаю =(
    m_fCurrPaddleY(m_fPaddleY1),                    //Это dflt value, переопределяю в OnShow()
    ///@@@ #TODO: когда будут окончательно очерчены классы CBallsManager и CBricksManager, либо инициализ-ть тут, либо решить, где, ну и собсно сделать.
    m_pBallsMngr(nullptr),
    m_pBricksMngr(nullptr), 
    //m_pBallsMngr(new CBallsManager()),     //ptr на singleton-экземпляр менеджера шариков 
    //m_pBricksMngr(new CBricksManager(m_uNUM_OF_BRICKS_ON_LEVEL1)), 

//#OUTDATED: (прим.см. controller.h, str.31)
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
    ///@@! выстроил в порядке декларации в заголовке класса
{
  InitBricksOnLvlsCoords();
//#OUTDATED:
//    m_fPaddleY1 = m_fSceneHeight - m_fBOTTOM_GAP;	//Это dflt value, переопределяю в OnShow()
//    m_fCurrPaddleY = m_fPaddleY1;                       //Это dflt value, переопределяю в OnShow()
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
    CScene *sc = m_scene;                                   //@@ ?: д/ более простой изменяемости?? (а надо ли? - может просто `m_scene` использовать)
    m_pPad = sc->FindPictureObject(PIC_SC01_PADDLE);
//#OUTDATED (Видимо не нужен):    g_pBall = sc->FindPictureObject(PIC_SC01_BALL);     ///@@@ Видимо не нужен

    //Готовлю array-входн.пар-р для C-tor'а CBallsManager.  //Вынес этот код сюда, чтобы уйти от зависимости класса CBallsManager ..
    {                                                       //.. от кол-ва видов шариков и конкретных их объектов.
        std::vector<CPictureObject*> vpBallPicObjs;
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL));
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL2));
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL3));
        vpBallPicObjs.push_back(sc->FindPictureObject(PIC_SC01_BALL4));

    //Создаю менеджер шариков:
        m_pBallsMngr = new CBallsManager(vpBallPicObjs);    //conditional delete is in D-tor.
    }
    //Создаю менеджер кирпичей:
    m_pBricksMngr = new /*_BRICK_MNGR*/CBricksManager(/*12*/NUMBER_OF_BRICKS);  ///@@ Передаю маг.числом, НО не держу лишнюю глоб.переменную. Верно? //Это что? Типа БРИКС_ОН_ЛВЛ ? (вроде да, therefore заменил `12` на NUMBER_OF_BRICKS)
    
    //#OUTDATED (метод уже перекочевал в CBall и вызывается в C-tor'e):
    //InitBallHitChckPntsArr();

    //#OUTDATED: InitOfBallsPictObjects(sc);	//инициализирую массив CPict.Object'ов шариков
//#OUTDATED:    CPictureObject* pOriBall = sc->FindPictureObject(PIC_SC01_BALL);  ///@@@ может вынести в глоб. vars (тем более, если его, как и oriBrick не задейтсвовать в самой игре
    m_fPaddleY1 = m_fSceneHeight - m_fBOTTOM_GAP - (u32)m_pPad->GetSize().y;  //#COMPILER(Warning) `(u32)`
    m_fCurrPaddleY = m_fPaddleY1;
//#OUTDATED:    g_fMaxDltYWithPad = m_pPad->GetCenter().y - pOriBall->GetCenter().y;   //if(ball[i]..->GetPos().y > g_fPaddleY+g_fMaxDltYWithPad) ..
//                                                                            //.. то больше не просчитывать IsHit() с Pad'ом.
    std::srand((int)time(NULL));		//для того, чтобы rand() выдавал не одно и то же
//#OUTDATED:    _BALL::InitFewPixelsShift();	//OLD: Инициализирую таблицу 1-(2-3..)-пиксельных смещений шарика при контакте.     //@@ перенёс вызов в C-tor шарика (в условный блок, исполняемый только при cre. 1го шарика)
	
    InitNewGame(sc);
}
EXCPTN_CATCH



//// OnUpdate() ////
void CArkanoidController::OnUpdate(u32 _dt) ///@@@ Warning	C4100: '_dt' : unreferenced formal parameter 302    //Poss-но тут нужно исп-ть dt, чтобы на компах с разной произвотид-тью (где м/у `OnUpdate()`-ами может проходить различное время), игра шла с одной скоростью.
try {
    CScene *sc = m_scene;                                   //@@ ?: д/ более простой изменяемости?? (а надо ли? - может просто `m_scene` использовать)
    //Костыли: перенёс сюда из OnRender()
    //Вызываю метод, отыгрывающий анимацию уничтожения в 1й проход и удаляющий объект во второй
    m_pBricksMngr->CleanBricks(sc, this); 	//Костыли: переносил это в OnUpdate() - не помогло

    //Удалить объекты класса CBall вылетевших за пределы поля шариков
    m_pBallsMngr->CleanBalls(m_fSceneHeight, m_fSceneWidth, m_GameState);

    ////Провека коллизий и пересчёт траекторий
    //_DIRECTION dir;	//Костыли: перенёс отсюда в OnRender() - не помогло
    //for (auto itb=m_vBalls.begin(); itb!= m_vBalls.end(); ++itb)	///@@@ Использование ITERATOR'а 
    //{
    //	dir=(*itb)->IsHit();
    //	if (dir != -1)
    //	{
    //		(*itb)->GetBallPicObj()->MoveToPolarRo(FLT_MAX,  (*itb)->getNewMoveCenter(dir),  (*itb)->GetVel());
    //		//сдвигаю шарик на 2-3пикселz, чтобы выйти из тек.коллизии. Пояснчения причин и альтернативы см. в прим.к описанию метода initOf1PixelShift()
    //		(*itb)->GetBallPicObj()->SetPos( vec3To2((*itb)->GetBallPicObj()->GetPos()) + mFewPixelsShift[dir] );
    //	}
    //}
}
EXCPTN_CATCH



//// OnRender() ////
void CArkanoidController::OnRender(vector2 _scroll)     ///@@@ Быть может нужно как-то исп-ть этот пар-р:  Warning	C4100: '_scroll' : unreferenced formal parameter 330
try {
    LOG_INFO("CArkanoidController::OnRender");
    vector2 pos = CCursorController::GetInst().GetCursorPos();

    CScene *sc = m_scene;   ///@@@ /*+см.в кажд.методе эту же строку*/ зачем каждый рендер создавать этот ptr заново? Может его передавать пар-ром, или сделать static'ом? Хотя ск.вс., т.к. m_scene - это, должно быть, верхняя (активная) (хотя написано в доках, что это материнская сцена). Ну короче м/у рендерами может она видимо измениься. Поэтому обновляем. ??

    ProcessWinLostState(sc);    ///@@@ #TODO: кажд.рендер прогоняем эти проверки. Быть может заменить проверкой единств-го флага "Что-то изменилось с game_state'ом". И только if `true`, то гонять этот метод?

    Print("SCORE: ", m_uGameScore, 15,m_fSceneWidth-100, 240,10,240,100);

#if DEBUG==1																										
    CPictureObject* ball1 = sc->FindPictureObject(PIC_SC01_BALL);
    //dbgPrint("rndmAng1 = ", dbgTmp0, 10,10, 255,10,255);
    if (!m_pBallsMngr->IsBallsEmpty())
        DbgPrint("currBallFi = ", m_pBallsMngr->GetBallFi(0)/*m_vBalls[0]->GetFi()*/, 10,200, 255,10,255);  ///@@: Тут подчёркивание - /*возможно*/НЕ ошибка КОМП-ра, /*т.к.*/то, что я объявил френдом метод, ДАЁТ ДОСТУП К ЗАКРЫТЫМ ДАННЫМ _ВНУТРИ_ ф-ции, НО НЕ В СПИСКЕ ЕЁ ПАР-РОВ!
    DbgPrint("currMouse X : Y = ", pos.x, pos.y, " : ", 25,10, 10,10,10);
	
    DbgPrint("ball1->GetPos().x : .y    ", ball1->GetPos().x, ball1->GetPos().y, " : ", 300);
    //dbgPrint("rand()%... :   ", dbgTmp6, 355,10, 50,50,1);
#endif						

    //Костыли: перенёс сюда из OnUpdate()
    //Провека коллизий и пересчёт траекторий    ///@@ Stoped_here 2014/09/16
    m_pBallsMngr->ProcessIsHit(m_pPad, m_pBricksMngr->GetLavBricksPtr(), m_fSceneWidth, m_GameState);

}
EXCPTN_CATCH



//// OnMouseMove() ////
void CArkanoidController::OnMouseMove(u32 _x, u32 _y)
try {	///@@@ привязать курсор мыши к центру paddle'а и добавить ограничение, чтобы не вываливалась и влево тоже. А также откорректировать эту проверку. И вынести весь этот код в отдельную ф-цию.
    //#OUTDATED (уже есть член класса): CPictureObject* pad = m_scene->FindPictureObject(PIC_SC01_PADDLE);
	
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
}   ///@@@ Не понятно, вроде ж в люб.случ. отрабатывает return (правда wrn указывает на строку с EXCPTN_CATCH): Warning	C4715: 'CArkanoidController::OnMouseKeyDown' : not all control paths return a value 402
EXCPTN_CATCH