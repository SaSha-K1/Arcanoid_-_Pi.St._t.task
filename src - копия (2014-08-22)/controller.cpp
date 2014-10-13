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


#define EXCPTN_CATCH catch (const CMyExcptn myEx)\
    {\
        time_t t = time(NULL);\
        switch (myEx.m_code)\
        {\
            case 1: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"1.Unexpected last `else` in CBall::computeNewMoveCenter().\"" << std::endl; break;\
            case 2: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"2.Unexpected last `else` in CBall::computeNewFi().\"\n" << std::endl; break;\
            case 3: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"3.Condition (m_fFi>=2*PI || m_fFi<=-2*PI) in CBall::normalizeFi() didn't satisfied.\"" << std::endl;  break;\
            case 4: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"4.vHP.size() is >2! in isHit().\"" << std::endl; break;\
            case 5: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"5.CBall::getNewMoveCenterIfHit() got dir==NO.\"" << std::endl; break;\
            case 6: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"6.CBall::computeStartFi() was called by ball, that isn't attached to paddle.\"" << std::endl; break;\
            case 7: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"7.In isHit()  vHP.size()==2 but values combination is unexpected.\"" << std::endl; break;\
            case 9: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"9.The locateAttachedToPadBall() was called for ball, that isn't attached to Pad.\"" << std::endl; break;\
            case 10: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"10.The delBall() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 11: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"11.Unexpected last `else` in CBall::isHit().\"\n" << std::endl; break;\
            case 12: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"12.The m_fFi>PI || <=-PI in CBall::isHit() before `switch(dir)`.\"\n" << std::endl; break;\
            case 13: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"13.The `default` condition-branch in `switch(dir)` in CBall::isHit().\"\n" << std::endl; break;\
            case 14: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"14.The delBrick() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 15: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"15.g_vBricks.empty() condition isn't complete, but \"WON\" state is set.\"" << std::endl; break;\
            case 16: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"16.CBrick::bricksLockOff() returned \"0\" in isHit().\"" << std::endl; break;\
            case 17: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"17.CBrick::bricksLockOff() returned \"0\" in delBrick().\"" << std::endl; break;\
            default: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"Unexpected exception!\"" << std::endl; break;\
        }\
    }


//#OUTDATED: const u32   MULTIPL_FOR_GEN = pow(10.0, 4);      //Это чтобы float перевести в u32, которые кушает Generate()
//#OUTDATED: const u32   MOVE_C_COORD_SCALE = pow(10.0, 3);	//Const множитель координаты центра движения (по-сути может быть другим числом, 
                                        //.. главное, чтобы был дост.далёк от единицы)
const char  MAX_BALL_NUM = 5;
const u32   NUMBER_OF_LEVELS=1;	//Кол-во уровней в игре
const u32   BRICKS_ON_LEVEL [NUMBER_OF_LEVELS] = {/*2*/12};	//Кол-во кирпичей на уровне - для размерности вектора ActObjects.objects
const float HALFPI = PI/2;	//экономлю время работы CTG()
const float DEFAULT_BALL_VEL = 200;     //Скорость мячика by def.

//#OUTDATED: u32 gameScore = 0;  ///@@@ m!b. членом класса Arcanoid

//#OUTDATED: const float RAND_START_ANGLE=10*(float)PI/180;	//Модуль отклонения от нуля случайного угла старт.запуска шарика, радиан

//#OUTDATED:
//const float DLT_PXLS_TO_CHCK_HIT = 3/*5*/;  //Зазор в пикселях м/у объектами, когда в проверке isHit() перехожу от поиска ..
//                                            //.. коллизии вмещающих rect'ов к детальной проверке по контрольным точкам. ..
//                                            //.. Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. успевает ..
//                                            //.. пролететь сразу на несколько пикселей между итерациями просчёта ..
//                                            //.. оптимизирующих (см. "#OPT") проверок.

//#OUTDATED: float g_fMaxDltYWithPad = 50;  //Это dflt value, переопределяю в OnShow(), когда известные р-ры Pad'а и шарика.

enum GAME_STATE {LOST=0, RUN=1, WON=2, NEW=3};	//Состояние игры для проверки. Ввёл для оптимизации проверки коллизий; 
                                            //.. "NEW" нужна для варианта DONT_DELETE_ANI_BRICKS==1

#if DONT_DELETE_ANI_BRICKS 1
GAME_STATE gameState = NEW;
#else
GAME_STATE gameState = RUN;
#endif

const float   BOTTOM_GAP = 35;	//Зазор м/у paddle и нижн.гран.окна
float     g_fSceneHeight = 600;	//Пар-р игрового поля	//это dflt value, переопределяю в OnShow()				
float     g_fSceneWidth = 800;	//Пар-р игрового поля	//это dflt value, переопределяю в OnShow()				
float     g_fPaddleY1 = g_fSceneHeight - BOTTOM_GAP;	//Это dflt value, переопределяю в OnShow(), поэтому и НЕ `const`. ..
                                                //.. Ввёл `g_fPaddleY1`, т.к. возможно Pad будет в проц.игры перемещаться по OY ..
                                                //.. в зависимости от бонусов/малусов.
float     g_fPaddleY = g_fPaddleY1; //Это dflt value, переопределяю в OnShow()

class CBall;
class CBrick;
std::vector<CBall*>     g_vBalls;  ///@@@ ?name?
std::vector<CBrick*>    g_vBricks; ///@@@ ?name?

///@@@ ?name?:  ///@@@STOPED HERE. Convert to map
std::array<CPictureObject*, MAX_BALL_NUM>   ballsPictObjects;	//СВЯЗКА 2х МАССИВОВ: 1) массив CPict.Object'ов всех 5 (MAX_BALL_NUM) шариков
std::array<bool, MAX_BALL_NUM>              ballsPictObjectsUsage;		//СВЯЗКА 2х МАССИВОВ: 2) массив индикаторов занятости CPict.Object'ов из 1го массива	//инициализирую его нулями в методе initOfBallsPictObjects(), вызываемом единожды в OnShow()
//когда m_scene начинает существовать, инициализируем массив CPict.Object'ов всех 5 шариков (MAX_BALL_NUM)
void InitOfBallsPictObjects(CScene* sc)
{
    ballsPictObjectsUsage.fill(0);
	
    ballsPictObjects[0] = sc->FindPictureObject(PIC_SC01_BALL);
    ballsPictObjects[1] = sc->FindPictureObject(PIC_SC01_BALL2);
    ballsPictObjects[2] = sc->FindPictureObject(PIC_SC01_BALL3);
    ballsPictObjects[3] = sc->FindPictureObject(PIC_SC01_BALL4);
    ballsPictObjects[4] = sc->FindPictureObject(PIC_SC01_BALL5);
}

const u32   uNumOfHitChckPnts = 8;  //(N(north),E(east),S,W + 4 промежуточных)
float       fBallR = 25;  //Это dflt value, переопределяю в OnShow()

//Массив углов, на которых располагаются точки-датчики коллизии шарика (угол 0 соотв-ет точке "N"(north), угол PI/2 - "E"(east), ..
//.. угол -PI/2 - "W"(west) и т.д. Угол принадлежит (-PI : PI] как и угол m_fFi в рассчётах траектории шарика.
std::array<float, uNumOfHitChckPnts> aBallHitChckPntsAngles = 
    {   
        0,          //N
        PI/4,       //NE
        HALFPI,     //E
        PI*3/4,     //SE
        PI,         //S
        -PI*3/4,    //SW
        -HALFPI,    //W
        -PI/4       //NW
    };

//Массив коорд.датчиков коллизии шарика
std::array<vector2, uNumOfHitChckPnts> aBallHitChckPnts;    //Инициализирую в OnShow()	
//#OUTDATED:
//std::array<vector2, uNumOfHitChckPnts> aBallHitChckPnts =
//    {   //Т.к. тут имеем дело с растровым изображением, то, коль нам нужен первый пиксель вне контура шарика, то в, случае ..
//        //.. поиска координат такой точки в направлении оси координат, прибавляем 1-цу; в обратном направлении - отнимаем.
//        vector2((fBallR-1),  0-1),    //N  (24, -1)    //Тут и далее `(fBallR-1)`, т.к. начинаем с нуля.
//        vector2( /*myround(*/(fBallR-1) + fBallR * cos(PI/4)/*)*/+1,  /*myround(*/(fBallR-1) - fBallR * sin(PI/4)/*)*/ ), //NE (43,  6)
//        vector2((2*fBallR-1) + 1,  (fBallR-1)),    //E  (50, 24)
//        vector2((fBallR-1) + fBallR * cos(PI/4)+1,  (fBallR-1) + fBallR * cos(PI/4)+1),    //SE (43, 43)
//        vector2(24, 50),    //S  (24, 50)
//        vector2( 6, 43),    //SW ( 6, 43)
//        vector2(-1, 24),    //W  (-1, 24)
//        vector2( 6,  6)     //NW ( 6,  6)
//    };
//std::array<vector2,uNumOfHitChckPnts> ballHitChckPnts = 
//    {   vector2(24, -1),    //N
//        vector2(43,  6),    //NE
//        vector2(50, 24),    //E
//        vector2(43, 43),    //SE
//        vector2(24, 50),    //S
//        vector2( 6, 43),    //SW
//        vector2(-1, 24),    //W
//        vector2( 6,  6)     //NW
//    };

void InitBallHitChckPntsArr()
{
    float currAng;
    for (u32 i=0; i < aBallHitChckPnts.size(); ++i)
    {
        currAng = aBallHitChckPntsAngles[i]; //для лаконичности. Считаю, что можно, т.к. этот метод отрабатывает только однажды.
        //Использую выведенные формулы определения растровых x и y из уравнения окружности шарика:
        //Последнее условное слагаемое (1 или 0) нужно: т.к. тут имеем дело с растровым изображением, то, ..
        //.. коль нам нужен первый пиксель вне контура шарика, то в, случае поиска координат такой точки ..
        //.. _в_ направлении оси координат, прибавляем 1-цу; в обратном направлении - отнимаем.
        aBallHitChckPnts[i] = vector2(
                (fBallR-1) + fBallR * sin(currAng) 
                    +((currAng > 0   &&  currAng < PI) ? 1 : 0),    ///@@@ /*myround(*/ надо ли?
                (fBallR-1) - fBallR * cos(currAng) 
                    +(( (currAng > -PI  &&  currAng < -HALFPI)  ||  (currAng > HALFPI  /*&&  currAng <= PI*/) ) ? 1 : 0)
                    //Тут закомментированная часть условия не нужна, если угол не может быть больше PI (как сейчас)
            );
    }
}

enum DIRECTION {N=0, NE, E, SE, S, SW, W, NW, NO=-1};	//Имена датчиков коллизии (по сторонам света), "NO" - знач-е, ..
                                                        //.. возвращаемое ф-цией isHit(), если нет коллизий.
                                                        //#WARNING: кол-во значений DIRECTION == uNumOfHitChckPnts-1. Как это ..
                                                        //.. красиво завязать так, чтобы меняя только одно из глоб. значений, второе ..
                                                        //.. пересчитывалось автоматически, пока не придумал. Но обратить внимание в ..
                                                        //.. случае изменений одной из этих сущностей, что нужно поправить и другую.

std::map<DIRECTION,vector2> mFewPixelsShift;  ///@@@ ?name?
//Таблица смещений в зависимости от сработавшего датчика контакта. 
//.. ЗАЧЕМ?: Сдвигаю шарик на несколько (2-3) пикселя, чтобы выйти из тек.коллизии (след.итерация .isHit() начинается раньше, чем ..
//..   шарик успевает отлететь на этот пиксель, что приводит к "залипанию" шарика.
//.. АЛЬТЕРНАТИВА?: Можно более правдоподобно сделать: вместо этой сдвижки в методе .isHit() при после определения сработавшего ..
//..   датчика (N, ..., NW) дальнейшие действия по пересчёту траектории предпринимать только в случае, если m_fFi попадает в ..
//..   диапазон такой, что delta смещения направлена в полуплоскость, в которой лежит сработавший датчик. НО! это лишнее время ..
//..   программирования, + лишняя нагрузка на систему, кот.в данн.случ., (имхо) не оправданан.
//.. !ВАЖНО! Этого оказалось недостаточно - шарик иногда успевает залезть в текстуру больше чем на 1 пиксель. Решать проблему ..
//..   кол-вом пикселей не хочу, т.к. когда-нибудь протормозит какой-ндь комп. и залезет шарик на 4 пикселя и получиться беда. ..
//..   Поэтому реализую описанный как АЛЬТЕРНАТИВА метод, но этот оставляю, чтобы при оптимизации метода isHit() сократить ..
//..   кол-во лишних проверок.
const u32 NUM_PIXELS_4_SHIFT = 3;
void InitFewPixelsShift()
{
    float currAng;
    for (u32 i=0; i < aBallHitChckPnts.size(); ++i)
    {
        currAng = aBallHitChckPntsAngles[i]; //для лаконичности. Считаю, что можно, т.к. этот метод отрабатывает только однажды.
        mFewPixelsShift[static_cast<DIRECTION>(i)] = vector2(   //#COMPILER(type_cast)
            NUM_PIXELS_4_SHIFT *-sin(currAng), 
            NUM_PIXELS_4_SHIFT * cos(currAng)
            ); ///@@@ /*myround(*/ надо ли?
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
    ///@@@@@ STOPED HERE
//2 - (для > убодного дебага); а 12 - как планировалось
#if NUMBER_OF_BRICKS == 2
std::array<vector2, 2> bricksCoordsLvl1 = 	
    {   vector2( 59, 140),	//0
        vector2(210, 140)	//1
    };

#elif NUMBER_OF_BRICKS == 12
std::array<vector2, 12> bricksCoordsLvl1 = 
    {   vector2( 59, 140),	//0
        vector2(210, 140),	//1
        vector2(361, 140),	//2
        vector2(512, 140),	//3
        vector2(663, 140),	//4
        vector2(814, 140),	//5
        vector2( 59,  89),	//6(3)
        vector2(361,  89),	//7(3)
        vector2(663,  89),	//8(3)
        vector2(210, 191),	//9(1)
        vector2(512, 191),	//10(1)
        vector2(814, 191),	//11(1)
    };
#endif 

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
std::ofstream exceptPrintFile("exceptPrintFile.txt", ios::app);

vector2 vec3To2(const vector3 &v3) { return (vector2(v3.x, v3.y)); }

CPictureObject* g_pPad;     //Paddle
CPictureObject* g_pBall;	//Ball (ori. oblect)

    //<<< class BRICK >>>// ---------------------------------------------------------------------------------------------
//#ifndef BRICK_H
//#define BRICK_H
//
//#include "globals.h"
//#include "aniobject.h"


class CBrick
{
public:
    //C-tor
    CBrick (CAniObject *const brick, const vector2 position) 
        : m_pBrickAniObj(brick), m_state(ING) 
    { 
        m_pBrickAniObj->SetPos(position);
    }

    //Types:
    static enum BRICK_STATE {	//состояние: 0 - вне игры; 1 - в игре; 2 - отыгрывается анимация уничтожения(старт); 3 - анимация униченожения отыгралась (можно удалять)
        OFF=0, 
        ING=1, 
        DSTR_ST=2, 
        DSTR_EN=3
    };

    //Vars (pblc):
    BRICK_STATE m_state;

    //Интерфейсные методы:
    CAniObject* getBrickAniObj() const { return m_pBrickAniObj; }
#if LOCKS ==1
    static bool bricksLockOn(const /**/char/**/);	//что-то типа мютекса
    static bool bricksLockOff(const /**/char/**/);	//что-то типа мютекса
    static char getBricksLockVal() { return m_scBricksLock; }		//Костыли: добавил эту строку 
#endif//LOCKS
	
private:
    //Vars (prvt):
    CAniObject* m_pBrickAniObj;

#if LOCKS ==1
    static /*bool*/char m_scBricksLock;	//что-то типа мютекса	0 - открыт; 1 - закрыт isHit()-ом; 2 - закрыт delBrick()-ом
#endif//LOCKS
};


//#endif//BRICK_H

//---------------------

//#include "brick.h"

#if LOCKS ==1
/*bool*/char CBrick::m_scBricksLock=0;	//static member

inline bool CBrick::bricksLockOn(const /**/char cNewVal/**/) 	//`inline`, т.к. может быстрее будут отрабатывать, 
                                                        //.. чтобы какой-ндь race-condition не случился
{
    if (0 == m_scBricksLock)
    {
        m_scBricksLock=/*1*/cNewVal;
        return 1;
    }
    else
        return 0;
}

inline bool CBrick::bricksLockOff(const /**/char oldVal/**/) 
{
    if (/*1*/oldVal == m_scBricksLock)
    {
        m_scBricksLock=0;
        return 1;
    }
    else
        return 0;
}
#endif//LOCKS
    //xxx END_of class BRICK xxx// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


#if DONT_DELETE_ANI_BRICKS==1
    #if NUMBER_OF_BRICKS == 12
    std::array<CAniObject*, 12> apBricksAniObjects;

    #elif NUMBER_OF_BRICKS == 2
    std::array<CAniObject*, 2> apBricksAniObjects;

    #endif//NUMBER_OF_BRICKS

    void initOfBricksAniObjects(std::vector<CBrick*> const vpBricks)
    {	
        for (char i = 0; i < vpBricks.size(); ++i)
        {
            apBricksAniObjects[i] = vpBricks[i]->getBrickAniObj();
        }
    }

#if NUMBER_OF_BRICKS == 12
    void RetrieveBrick(
            std::array<CAniObject*, 12> const apBricksAniObjects, 
            std::array<vector2, 12> const aBricksCoordsLvl
         )
#elif NUMBER_OF_BRICKS == 2
    void RetrieveBrick(
            std::array<CAniObject*, 2> const apBricksAniObjects, 
            std::array<vector2, 2> const aBricksCoordsLvl
         )
#endif//NUMBER_OF_BRICKS
    {
        for (char i = 0; i < apBricksAniObjects.size(); ++i)
            g_vBricks.push_back(new CBrick(apBricksAniObjects[i], aBricksCoordsLvl[i]));
    }
#endif//DONT_DELETE_ANI_BRICKS


	//<<< class BALL >>>// -----------------------------------------------------------------------------------------------------------
//! Я бы вынес этот класс в отдельную пару .h и .cpp, но в условии задания указано, что модифицировать 1 файл controller.cpp. Поэтому пока всё тут.
class CBall
{
public:
    //C-tor:
    CBall(
        CPictureObject*         const pBall, 
        std::array<vector2,uNumOfHitChckPnts>&  aBallHitChckPnts, 
        const bool              bAttachToPad
        ) 
        : m_pBallPicObj(pBall), 
        m_fVelocity(DEFAULT_BALL_VEL), 
        m_fFi(0), 
        m_aHitChckPnts(aBallHitChckPnts) 
    { 
        if (bAttachToPad)
        {
            m_spIsAttachedToPad = this; 
            locateAttachedToPadBall();	//хотя, если двинуть Pad, то шарик позиционируется и без этого, Но для фен-шуя 
        }
    }

    //Vars (pblc):
    static float m_sfFiPlus;    //Корректировка к рассчитываемому после удара m_fFi (если удар был по Pad'у) ..
                                //.. в зависимости от удаления от середины Pad'а точки контакта шарика с Pad'ом.

    //Methods (pblc):
    bool        tryToInvertIsAttachedToPad(); 
    void        locateAttachedToPadBall();
    DIRECTION   isHit(const float fDltPxlsToChckHit = 3/*5*/);  //Пар-тр `fDltPxlsToChckHit` - зазор в пикселях м/у объектами ..
                                                               //.. , когда в проверке isHit() перехожу от поиска коллизии ..
                                                              //.. вмещающих rect'ов к детальной проверке по контрольным точкам. ..
                                                             //.. Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. ..
                                                            //.. успевает пролететь сразу на несколько пикселей между итерациями ..
                                                           //.. просчёта оптимизирующих (см. "#OPT") проверок. Используется в ..
                                                          //.. вызываемых из isHit() методах isHitWithPad(..) и isHitWithBricks(..).

    //Интерфейсные методы:
    vector2                 getNewMoveCenter(const DIRECTION dir);
    static CBall*           getIsAttachedToPaddle() { return m_spIsAttachedToPad; }
    CPictureObject*         getBallPicObj() { return m_pBallPicObj; }
    float                   getVel() { return m_fVelocity; }
    std::array<vector2,uNumOfHitChckPnts>&  getHitChckPnts() { return m_aHitChckPnts; }
#if DEBUG ==1
    float                   getFi() const { return m_fFi; }
#endif

private:
    //Vars (prvt):
    CPictureObject*         m_pBallPicObj;
    static CBall*           m_spIsAttachedToPad;
    float                   m_fVelocity;
    float                   m_fFi;		//тек.знач. угла направления движ.шарика == (-PI ; PI]. 0 соотв-ет направлению вертикально up.
    std::array<vector2,uNumOfHitChckPnts>&  m_aHitChckPnts;

    //Methods (prvt):
    void        computeNewFi(const DIRECTION dir);
    void        computeStartFi(
                    const float rndStartAng = 10*(float)PI/180,   //Модуль макс.отклонения от 0 случайного угла старт. ..
                                                                  //.. запуска шарика - 10 градусов перевожу в радианы.
                    const u32 multiplForGen = pow(10.0, 4)  //Пар-тр нужен, чтобы скормить float ф-ции ..
                                                            //.. Generate() ч/з u32, который она принимает.
                );
    void        normalizeFi();
    vector2     computeNewMoveCenter(u32 moveCCoordScale = pow(10.0, 3)) const;
    DIRECTION   hitReview(const DIRECTION dir) const;
    void        computeFiPlus();
    //Вспомагательные методы для isHit()
    void        isHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
    void        isHitWithPad(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    bool& needToCheckBricks, 
                    const float fMaxDltYWithPad,    //Определяю при вызове этого метода из isHit() след. образом: ..
                                                    //.. g_pPad->GetCenter().y - pOriBall->GetCenter().y  ..
                                                    //.. ПОЧЕМУ?: if (ball[i]..->GetPos().y  >  g_fPaddleY +f MaxDltYWithPad), то ..
                                                    //.. больше не нужно просчитывать isHit() с Pad'ом - шарик уже не вернуть в поле.
                    const float& fDltPxlsToChckHit      //Получает от isHit(). Назначение см. в isHit()
                );
    void        isHitWithBricks(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    const float& fDltPxlsToChckHit  //Получает от isHit(). Назначение см. в isHit()
                );
    DIRECTION   interpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
};

//Static members:
float  CBall::m_sfFiPlus = 0;
CBall* CBall::m_spIsAttachedToPad = nullptr;

void CBall::computeNewFi(const DIRECTION dir)
{
    if (N==dir || S==dir)	//тут попарные сравнения, т.к. правила рассчёта fi одинаковы для этих случаев
    {
        m_fFi = -m_fFi + PI +m_sfFiPlus;	//корректировка угла в зависимости от точки удара по Pad'у только для S датчика.
        m_sfFiPlus=0;	//использовали бонус и обнулили.
    }
    else if (NE==dir || SW==dir)
        m_fFi = -m_fFi -PI/2.0;
    else if (E==dir || W==dir)
        m_fFi = -m_fFi;
    else /*if (SE==dir || NW==dir)*/	//последнее условие можно и не проверять
#if DEBUG==1
        if (SE==dir || NW==dir)
#endif
            m_fFi = -m_fFi + PI/2;
#if DEBUG==1
        else throw CMyExcptn(2);
#endif

    normalizeFi();
}


void CBall::normalizeFi()
{
#if DEBUG==1
    if (2*PI <= m_fFi  ||  -2*PI >= m_fFi) throw CMyExcptn(3);
#endif
    if (PI < m_fFi)
        m_fFi=m_fFi-2*PI*(int( (m_fFi+PI)/(2*PI) ));		//Посл.множитель - убираем столько раз по 2пи, чтобы ..
                                                            //.. рез-тат == (-PI ; PI].     //(m_fFi+PI) - тут +PI, чтобы ближайшее ..
                                                            //.. меньшее нечётное целое число PI в составе m_fFi при делении нацело ..
                                                            //.. на 2*PI давало +1. Это нужно,чтобы вписать m_fFi в диапазон (-PI;PI].
    else if (-PI >= m_fFi)
        m_fFi=m_fFi+2*PI*(int( (m_fFi-PI)/(-2*PI) ));
}


vector2 CBall::computeNewMoveCenter(u32 moveCCoordScale/* = pow(10.0, 3) */) const  //Закомментированы значения by def., ..
                                                                                   //.. установленные в объявлении метода.
                                                                                  //Пар-тр `moveCCoordScale` - const множитель ..
                                                                                 //.. координаты центра движения (по-сути может ..
                                                                                //.. быть другим числом, главное, чтобы был дост. ..
                                                                               //.. далёк от единицы). Иначе может черезчур ..
                                                                              //.. увеличиться значение второй координаты. ..
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
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(PI-m_fFi);	//Тут и в след.ветке ..
                                                                                                //.. юзаю ctg, т.к. ч/з tg считать ..
                                                                                                //.. опасно в этих областях углов.
    else if (m_fFi > -3/4*PI  &&  m_fFi < -PI/4) 	// < left
                                        mc.x = m_pBallPicObj->GetPos().x + moveCCoordScale,
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(m_fFi);
#if DEBUG==1
    else throw CMyExcptn(1);
#endif
    return mc;
}


void CBall::computeStartFi(
        const float rndStartAng,/* = 10*(float)PI/180, */   //Закомментированы значения by def., установленные в объявлении метода.
        const u32 multiplForGen/* = pow(10.0, 4) */         //Закомментированы значения by def., установленные в объявлении метода.
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


vector2 CBall::getNewMoveCenter(const DIRECTION dir)
{
#if DEBUG==1
    if (NO==dir) throw CMyExcptn(5);
#endif

    if (this != m_spIsAttachedToPad)		
    {
        m_pBallPicObj->StopMoveTo();	///@@@ ск.вс. не нужно
        computeNewFi(dir);
    }
    else /*if (this == m_spIsAttachedToPad)*/ computeStartFi();
    return computeNewMoveCenter();
}


bool CBall::tryToInvertIsAttachedToPad() 
{ 
    if (this == m_spIsAttachedToPad)	        { m_spIsAttachedToPad = nullptr; return 1; }
    else if (m_spIsAttachedToPad == nullptr)    { m_spIsAttachedToPad = this; return 1; }
    else /*if other ball is attached*/	return 0; 
}


void CBall::locateAttachedToPadBall()
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad) throw CMyExcptn(9);
#endif
    m_pBallPicObj->SetPos( g_pPad->GetCenter().x - m_pBallPicObj->GetSize().x / 2,   
                           g_fPaddleY - m_pBallPicObj->GetSize().y - 1 );
}

//Рассчёт поправки (m_sfFiPlus) к углу отражения (m_fFi) в зависимости от удаления точки контакта с Pad'ом от середины последнего
void CBall::computeFiPlus()
{
    float dlt = m_pBallPicObj->GetCenter().x - g_pPad->GetCenter().x;	//Степень отклонения шарика от центра Pad'а при ударе
    float weightedDlt = dlt/g_pPad->GetSize().x/2; //delta взвешенная по отношению к 1/2 длины Pad'а (т.е. получается в пределах [0, 1]
    m_sfFiPlus= (PI/2/*4*/)*weightedDlt*((fabs(m_fFi)-PI/2)/PI/2);	//Максимальная корректировка к углу == PI/4, корректируется ..
                                                                   //.. взвешенной dlt'ой и разницей m_fFi и PI/2, выраженной в ..
                                                                  //.. долях PI/2 (т.е. тоже [0, 1]). Для m_fFi ==почти PI/2 ..
                                                                 //.. (3,1417/2) угол отлёта всё равно получается < PI/2. Т.е. ..
                                                                //.. корректировка безопасная - шарик не полетит вниз из-за неё.
}


void CBall::isHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (0  >=  m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y)     vTrggrdHitChckPnts.push_back(N);
    if (0  >=  m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x)     vTrggrdHitChckPnts.push_back(W);
    else if (m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  >=  g_fSceneWidth)    vTrggrdHitChckPnts.push_back(E);
}


void CBall::isHitWithPad(
        std::vector<DIRECTION>& vTrggrdHitChckPnts,  
        bool& needToCheckBricks, 
        const float fMaxDltYWithPad,  
        const float& fDltPxlsToChckHit
     )
{
    //#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. успевает пролететь сразу ..
    //                                        //.. на несколько пикселей между итерациями просчёта оптимизирующих (см. "#OPT") проверок.

    //Оптимизация (далее #OPT): если шарик выше горизонтали Pad'а, или если улетел уже ниже (горизонтали Pad'a +10), ..
    //.. то коллизию с Pad'ом искать не нужно - начинаем перебор акт.объектов начиная со 2го (Pad всегда 1ый)
    if ( m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y  <  g_fPaddleY - fDltPxlsToChckHit  ||	
         m_pBallPicObj->GetPos().y  >  g_fPaddleY + fMaxDltYWithPad  ||   //if центр шарика ниже середины Pad'а,то уже game over.
         m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  <  g_pPad->GetPos().x - fDltPxlsToChckHit  ||
         m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x  >  g_pPad->GetPos().x + g_pPad->GetSize().x + fDltPxlsToChckHit )
         /*То ничего. Рабочая, но менее вероятная, ветка - следующая*/;
    else
    {
        for (int d=E/*2*/; d<=W/*6*/; ++d)	// диапазон [2, 6] == [E, W] (остальные точки проверять для Pad'а не нужно)
        {                                   //#WARNING(possibly). Возможно заменить implicit преобразование типов явным.
            if ( g_pPad->Hit(m_aHitChckPnts[d] + vec3To2(m_pBallPicObj->GetPos())/*,0*/) )
            {
                vTrggrdHitChckPnts.push_back(static_cast<DIRECTION>(d));
                needToCheckBricks=0;
                //Рассчёт поправки (m_sfFiPlus) к углу (m_fFi) в зависимости от удаления точки контакта с Pad'ом от середины Pad'а:
                if (S == vTrggrdHitChckPnts.back())	//не корректирую угол, если касание по углу или боков.поверхности
                    computeFiPlus();
            }
        }
    }
}


void CBall::isHitWithBricks(std::vector<DIRECTION>& vTrggrdHitChckPnts,  const float& fDltPxlsToChckHit)
{
#if LOCKS==1
        while (CBrick::bricksLockOn(1) != 1)	///In isHit()
        {	/*включаю замок на изменения в g_vBricks. Если уже включён кем-то другим, жду*/
#if LOW_LVL_LOCKS==1		
            //Костыли: вставляю временно для проверки
            if (CBrick::bricksLockOn(1) != 1)
                return NO;
            else
                break;
#endif//LOW_LVL_LOCKS
        }
#endif//LOCKS

//#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. успевает пролететь сразу ..
//                                        //.. на несколько пикселей между итерациями просчёта оптимизирующих (см. "#OPT") проверок.

        //for (auto it = g_vBricks.begin(); it!=g_vBricks.end(); ++it)	//была проблема с инвалидным итератором из-за многопоточности
        //for (auto it = g_vBricks.rbegin(); it!=g_vBricks.rend(); ++it)	///@@@ использую ITERAOR N
        //for (char ib = g_vBricks.size()-1; ib>=0; --ib)
        for (char ib = 0; ib < g_vBricks.size(); ++ib)	//Костыли
        {		
            /*КОСТЫЛЬ*/ CAniObject ao = g_vBricks[ib]->getBrickAniObj();	//Костыли: пробую сохранять себе копию объекта - не помогает
            //CAniObject *ao = (*it)->getBrickAniObj();	//для лаконичности
            //CAniObject ao (g_vBricks[ib]->getBrickAniObj());	//для лаконичности
                //if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao->GetPos().y -dlt  ||	//Opt.: проверка пересечения описывающих прямоугольников
                //m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao->GetPos().y + ao->GetSize().y +dlt ||	//opt
                //m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao->GetPos().x -dlt  ||					//opt
                //m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao->GetPos().x + ao->GetSize().x +dlt)	//opt
            //#OPT: проверка пересечения описывающих прямоугольников:
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
                    if (/*(*it)*/ g_vBricks[ib]/**/->m_state == CBrick::ING)
                        /*(*it)*/ g_vBricks[ib]/**/->m_state=CBrick::DSTR_ST;
                }
            }
        }
#if LOCKS==1
#if DEBUG==1
        if (CBrick::bricksLockOff(1)==0) throw CMyExcptn(16); /*ВЫключаю замок на изменения в g_vBricks. Если уже включён кем-то другим, жду*/
#else
        CBrick::bricksLockOff(1)
#endif//DEBUG
#endif//LOCKS
}


//Корректировка и интерпретация результатов
DIRECTION CBall::interpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (vTrggrdHitChckPnts.empty()) return NO;	//нет коллизий

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	
        // Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1й из 2х частн.случаев предыд.триггера
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2й из 2х частн.случаев предыд.триггера	//тут второй набор условия пока невозможен, но оставляю для возможных будущих нужд
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//т.е. if единств.датчик сработал
    else throw CMyExcptn(11);	//Не нужно - проверка на .empty() была в начале

    return hitReview(dir);
}


DIRECTION CBall::isHit(const float fDltPxlsToChckHit/* = 3*/)   //Закомментировано знач-е by def., установленное в объявлении метода.
{																		///@@@ !!! ОПТИМИЗИРОВАТЬ !!!
    if (this == m_spIsAttachedToPad  ||  RUN != gameState) return NO;
    bool needToCheckBricks=1;	//Ввёл эту булку для отсечения проверки кирпичей: Т.к. кирпичи проверяются последними, то, ..
                                //.. если выявлено столкновение с Pad'ом, то проверка кирпичей бессмысленна.
    std::vector<DIRECTION> vTrggrdHitChckPnts;	///@@@Может после отладки сделать его сразу размерным (ск.вс. 2 достаточно бу) - НЕТ - он заполняется.

//Поиск столкновения с <^> краями экрана
    isHitWithScreenBorders(vTrggrdHitChckPnts);

//Поиск столкновения с Pad'ом
    isHitWithPad(vTrggrdHitChckPnts, needToCheckBricks, g_pPad->GetCenter().y - g_pBall->GetCenter().y, fDltPxlsToChckHit);

//Поиск столкновения с кирпичами
    if (needToCheckBricks)	//см.пояснение выше - у описания этой переменной
        isHitWithBricks(vTrggrdHitChckPnts, fDltPxlsToChckHit);

//Корректировка и интерпретация результатов
    interpretIsHitResults(vTrggrdHitChckPnts);
    if (vTrggrdHitChckPnts.empty()) return NO;	//нет коллизий

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	// Т.е. if сработало 2 датчика ч/з 1	//по модулю, т.к. после добавления отдельного поиска коллизий с краями экрана, порядок сработавших датчиков в векторе может быть любой
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1й из 2х частн.случаев предыд.триггера
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2й из 2х частн.случаев предыд.триггера	//тут второй набор условия пока невозможен, но оставляю для возможных будущих нужд
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//т.е. if единств.датчик сработал
    else throw CMyExcptn(11);	//Не нужно - проверка на .empty() была в начале

    return hitReview(dir);
}

//Проверка на повторное определение уже обработанного контакта, пересчёт траектории по которому уже был произведён
DIRECTION CBall::hitReview(DIRECTION dir) const
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


//ввести шарик в игру
void addBall(std::array<vector2,uNumOfHitChckPnts> &ballHitChckPnts, bool attachToPad)		//CPictureObject'ы для ещё 4х шариков добавил в xml и в obj.h, но держу их за пределами видимого игр.поля, пока не нужны. Старт.координаты задаются в xml, поэтому тут не нужны
{
    if (g_vBalls.size() < MAX_BALL_NUM)
    {
        for (u32 i = 0; i<MAX_BALL_NUM; ++i)
            if ( ballsPictObjectsUsage[i] == 0 )
            {
                g_vBalls.push_back(new CBall(ballsPictObjects[i], ballHitChckPnts, attachToPad));
                ballsPictObjectsUsage[i]=1;
                break; //!!!
            }
    }
}

//вывести шарик из игры
void delBall(u32 i)	//was (std::vector<CBall*>::iterator it)
{
#if DEBUG==1
    if (g_vBalls.size()<=i) throw CMyExcptn(10);
#endif

        g_vBalls[i]->getBallPicObj()->StopMoveTo();
        g_vBalls[i]->getBallPicObj()->SetPos(-200,-200);
        //Помечаем CPict.Obj. данного шарика как неиспользуемый:
        for (auto it2 = ballsPictObjects.begin(); it2!=ballsPictObjects.end(); ++it2)
        {
            if ( g_vBalls[i]->getBallPicObj() == (*it2) )
            {
                ballsPictObjectsUsage[std::distance(ballsPictObjects.begin(), it2)]=false;	
                break; /*не было*/
            }
        }

	
        delete g_vBalls[i];
        g_vBalls.erase(g_vBalls.begin()+i);
	
        if (g_vBalls.empty() && gameState==RUN)
            gameState = LOST;		//`processWinLostState();` не вызываю - отрабатываю в OnRender(), иначе всё, что сделаю, останется "за кадром"
}

void addBrick(CAniObject* baseBrick, vector2 position, CScene* sc)
{
    if (!g_vBricks.empty())	//если это первый кирпич, то значит не создаём копии аниобъекта, а берём уже готовый, который загружен из xml
    {
        g_vBricks.push_back(new CBrick(new CAniObject(*baseBrick), position));
        sc->AddAni(g_vBricks./*front()*/back()->getBrickAniObj()/**/, 1/**/);	//Добавить ани-объект в сцену 
        ///@@@ Тут^ последний атрибут iscopy - разобраться, что это
    }
    else
        g_vBricks.push_back(new CBrick(baseBrick, position));
}

void delBrick(u32 i, CScene* sc)
{

#if LOCKS==1
    //Костыли: перенёс сюда от немного снизу
    while (CBrick::bricksLockOn(2) != 1)	//включаю замок, чтобы никто не пользовался пока итераторами и индексами
    {

#if LOW_LVL_LOCKS==1
        //if ( CBrick::getBricksLockVal() == 2) break;	//Костыли: добавил эту ветку строку условия
        if ( CBrick::bricksLockOn(2) != 1) 
            return;
        else
            break;
#endif//LOW_LVL_LOCKS

    }
#endif//LOCKS

#if DEBUG==1
    if (g_vBricks.size()<=i) throw CMyExcptn(14);
#endif

    if (g_vBricks[i]->m_state == CBrick::DSTR_ST)
    {
        g_vBricks[i]->getBrickAniObj()->StartMovement(MV_SC01_BRICK);
        g_vBricks[i]->m_state = CBrick::DSTR_EN;


        //Костыли: тут это имеет смысл только пока bricksLockOn() переехал выше этого места. Когда вернётся, убрать это.
#if LOCKS==1
#if DEBUG==1
        if (CBrick::bricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::bricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

        return;
    }
    if (g_vBricks[i]->m_state == CBrick::DSTR_EN  &&  g_vBricks[i]->getBrickAniObj()->IsStopped())
    {
        g_vBricks[i]->m_state = CBrick::OFF;	/*Костылю*/
//#if LOCKS==1
//      while (CBrick::bricksLockOn() != 1)	//включаю замок, чтобы никто не пользовался пока итераторами и индексами
//      {
//      }
//#endif

#if DONT_DELETE_ANI_BRICKS==0
        //Если это не Базовый кирпич
        if (i!=0)
        {
            sc->RemoveAni(g_vBricks[i]->getBrickAniObj());	//Удалить ани-объект из сцены
            delete g_vBricks[i]->getBrickAniObj();
        }

        //Если это первый (базовый) кирпич, то обрабатываю отдельно - первые 2 строки для него выполнять нельзя, (т.к. остальные его копии - будут ли нормально работать после его удаления) Да и создавал его не я
        else
#endif//DONT_DELETE_ANI_BRICKS
        {
            g_vBricks[i]->getBrickAniObj()->SetPos(-300,-300); //убираю из игрового поля
            g_vBricks[i]->getBrickAniObj()->SetStatic(/*ANI_SC01_BRICK*/ST_SC01_BRICK_BEGIN);
            g_vBricks[i]->m_state = CBrick::OFF;
        }
		
        //Дальше код универсален для любого кирпича
        delete g_vBricks[i];

        g_vBricks.erase(g_vBricks.begin()+i);	//Костыли - пробовал вместо этой строки обменять перед удалением с последним или 1ым эл-том - проблема не ушла
        ////Костыли: перед удалением меняю с последним
        //if (g_vBricks.size()>1)
        //{
        //  //CBrick *tmpB = *(g_vBricks.begin()+i);
        //  //*(g_vBricks.begin()+i)=*(g_vBricks.end()-1);
        //  //*(g_vBricks.end()-1) = tmpB;
        //  //g_vBricks.erase(g_vBricks.end()-1);
        //  CBrick *tmpB = *(g_vBricks.begin()+i);
        //  *(g_vBricks.begin()+i)=*(g_vBricks.begin());
        //  *(g_vBricks.begin()) = tmpB;
        //  g_vBricks.erase(g_vBricks.begin());
        //}

        m_uGameScore+=10;

        if (g_vBricks.empty()  &&  gameState==RUN)
        {
            gameState=WON;
        }
//#if LOCKS==1
//#if DEBUG==1
//      if (CBrick::bricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//      CBrick::bricksLockOff(2)
//#endif
//#endif
    }

//Костыли: перенёс под скобку
#if LOCKS==1
#if DEBUG==1
        if (CBrick::bricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::bricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

}


/////////////// PRINTS /////////////// PRINTS /////////////// PRINTS /////////////// 
#if DEBUG==1 //! Объявления для Debug-версии																																

template <typename T> void dbgPrint(const std::string &txt, const T &var, const u32 y, const u32 x=10,
                                    const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100)	
{ 																									
    static std::ostringstream strStream; 												
    strStream.str("");	 											
    strStream<< txt<< var; 																
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																			

template <typename T1, typename T2> void dbgPrint(const std::string &txt, const T1 &var1, const T2 &var2,	
                                                  const std::string &delim, const u32 y, const u32 x=10,
                                                  const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100)	
{															
    static std::ostringstream strStream;										
    strStream.str("");																	
    strStream<< txt<< var1<< delim<< var2; 						
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																								

void dbgPrint(const std::string &txt, const u32 y, const u32 x=10,
              const u32 r=1, const u32 g=255, const u32 b=255, const u32 a=100)		
{ 																			
    static std::ostringstream strStream; 									
    strStream.str("");	 										
    strStream<< txt; 															
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());	
}															

#endif												

void print(const std::string &txt, const u32 y, const u32 x/*=10*/,
            const u32 r/*=10*/, const u32 g/*=220*/, const u32 b/*=220*/, const u32 a/*=100*/)	//эти дефолтн.значения заданы в объявлении (см. выше)
{ 													
    static std::ostringstream strStream; 						
    strStream.str("");	 					
    strStream<< txt; 								
    GET_FONT("dlg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());
}

template <typename T> void print(const std::string &txt, const T &var, const u32 y, const u32 x/*=10*/,
                                 const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/)
{ 												
    static std::ostringstream strStream; 		
    strStream.str("");	 	//strStream.flush() не работает
    strStream<< txt<< var; 									
    GET_FONT("dlg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());
}
/////////////// END of PRINTS /////////////// END of PRINTS /////////////// END of PRINTS /////////////// 


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
void initNewGame(CScene *sc)
{
#if DONT_DELETE_ANI_BRICKS==1
    if (gameState==NEW)
#else
    if (gameState!=LOST)
#endif
    {
        addBrick(sc->FindAniObject(ANI_SC01_BRICK), bricksCoordsLvl1[0], sc);
        for (u32 ibr=1; ibr < BRICKS_ON_LEVEL[0]; ++ibr)	//frm "1", т.к. 0й кирпич внесли отдельно - он базовый
        {
            addBrick(g_vBricks[0]->getBrickAniObj(), bricksCoordsLvl1[ibr], sc);
        }
        /*после отладки дописать ещё кубики до общ.кол-ва 12 и исправить соотв. константу*/
        //вообще наверное тут лучше научиться работать с xml и загружать из него всю переменную информацию по уровню.
#if DONT_DELETE_ANI_BRICKS==1
        initOfBricksAniObjects(g_vBricks);
#endif
    }
#if DONT_DELETE_ANI_BRICKS==1
    else if (gameState==WON)
    {
        RetrieveBrick(apBricksAniObjects, bricksCoordsLvl1);
    }
#endif
    //далее код универсален и для "LOST" и для "WON" и для 1го запуска
    addBall(aBallHitChckPnts, true);
    m_uGameScore=0;
    gameState = RUN;
}


char waitForClick=0;	//0 состояние по-умолчанию; 1 - высталвяет метод processWinLostState() и не проходит дальше определённого места, пока OnMouseKeyDown() не изменит на 2; 2 - значение, с которым в методе processWinLostState() выполняется проверка, пропускающая курсор до конца метода (в конце выставляется снова дефолтное значение 0)
void processWinLostState(CScene *sc)
{
    if (gameState==RUN)
        return;

    //GameOver
    if (gameState==LOST)
    {
        print("G A M E   O V E R", (g_fSceneHeight-15)/2, (g_fSceneWidth-100)/2, 1,250,250, 100);
        print("Click to continue", (g_fSceneHeight+40)/2,g_fSceneWidth*2/5+40, 40,40,40, 100);
		
        //ожидание клика (подбробное описание см.в месте определения переменной)
        if (waitForClick==0)
            waitForClick=1;
        if (waitForClick==1)
            return;

        sleepMy(5);///@@@@ надпись не успевает почему-то показаться. Попробовать писать тут в файл, а в OnRender() проверять флаг, и печатать из файла. Пока обошёлся waitForClick()'ом
        print("", (g_fSceneHeight-10)/2, (g_fSceneWidth-30)/2, 10,220,220, 100);
        //ballsPictObjectsUsage.fill(0);	//это лишнее
        initNewGame(sc);

        waitForClick=0;
    }
	
    //Victory
    else if (gameState==WON)
    {
        print("C O N G R A T U L A T I O N S!   Y O U   W O N!  SCORE IS: ", m_uGameScore, (g_fSceneHeight-15)/2,g_fSceneWidth/4, 220,10,220, 100);
        print("Click to continue", (g_fSceneHeight+40)/2,g_fSceneWidth*2/5+40, 40,40,40, 100);
        //удалить все мячики в цикле (проследить, чтобы граф.объекты их тоже уехали за поле
#if DEBUG==1
        if (!g_vBricks.empty()) throw CMyExcptn(10);
#endif

        if (!g_vBalls.empty())
            for (char i=g_vBalls.size()-1;  i>=0; --i)
                delBall(i);

		//ожидание клика (подбробное описание см.в месте определения переменной)
        if (waitForClick==0)
            waitForClick=1;
        if (waitForClick==1)
            return;
		
        initNewGame(sc);
        print("", (g_fSceneHeight-10)/2, (g_fSceneWidth-30)/2, 220,10,220, 100);

        waitForClick=0;
    }
}																											



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



CArkanoidController::CArkanoidController(u32 sceneId)
:ISceneController(sceneId), m_uGameScore(0)
{
}



CArkanoidController::~CArkanoidController()
{
}


	// OnShow() //
void CArkanoidController::OnShow()
try {
    g_fSceneHeight = CRender::GetInst().GetHeight()/*768*/;
    g_fSceneWidth = CRender::GetInst().GetWidth()/*1024*/;
    CScene *sc = m_scene;
    InitBallHitChckPntsArr();
    InitOfBallsPictObjects(sc);	//инициализирую массив CPict.Object'ов шариков
    g_pPad = sc->FindPictureObject(PIC_SC01_PADDLE);
    g_pBall = sc->FindPictureObject(PIC_SC01_BALL);
//#OUTDATED:    CPictureObject* pOriBall = sc->FindPictureObject(PIC_SC01_BALL);  ///@@@ может вынести в глоб. vars (тем более, если его, как и oriBrick не задейтсвовать в самой игре
    g_fPaddleY1 = g_fSceneHeight - BOTTOM_GAP - (u32)g_pPad->GetSize().y;  //#COMPILER(Warning) `(u32)`
    g_fPaddleY = g_fPaddleY1;
//#OUTDATED:    g_fMaxDltYWithPad = g_pPad->GetCenter().y - pOriBall->GetCenter().y;   //if(ball[i]..->GetPos().y > g_fPaddleY+g_fMaxDltYWithPad) ..
//                                                                            //.. то больше не просчитывать isHit() с Pad'ом.
    std::srand((int)time(NULL));		//для того, чтобы rand() выдавал не одно и то же
    InitFewPixelsShift();	//инициализирую таблицу 1-пиксельных смещений шарика при контакте
	
    initNewGame(sc);
}
EXCPTN_CATCH



	// OnUpdate() //
void CArkanoidController::OnUpdate(u32 _dt)
try {

    //Костыли: перенёс сюда из OnRender()
    //Вызываю метод, отыгрывающий анимацию уничтожения в 1й проход и удаляющий объект во второй
    for (char i=g_vBricks.size()-1; i>=0; --i)	//Костыли: переносил это в OnUpdate() - не помогло
    {
	    if (g_vBricks[i]->m_state == CBrick::DSTR_ST || g_vBricks[i]->m_state == CBrick::DSTR_EN)
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



    // OnRender() //
void CArkanoidController::OnRender(vector2 _scroll)
try {
    LOG_INFO("CArkanoidController::OnRender");
    vector2 pos = CCursorController::GetInst().GetCursorPos();

    CScene *sc = m_scene;

    ////Вызываю метод, отыгрывающий анимацию уничтожения в 1й проход и удаляющий объект во второй
    //for (char i=g_vBricks.size()-1; i>=0; --i)	//Костыли: переносил это в OnUpdate() - не помогло
    //{
    //	if (g_vBricks[i]->m_state == CBrick::DSTR_ST || g_vBricks[i]->m_state == CBrick::DSTR_EN)
    //		delBrick(i, sc);
    //}

    processWinLostState(sc);

    print("SCORE: ", m_uGameScore, 15,g_fSceneWidth-100, 240,10,240,100);

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



bool CArkanoidController::OnMouseKeyDown(u32 _key, u32 _x, u32 _y)
try {
    if (0 == _key)
    {
        if (waitForClick==1)
	        waitForClick=2;

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