//////////////////////////////////
//
//  ...
//      #TODO (#DEVELOP):
//  Если шарики разных размеров, то _BALL::m_svHitChckPnts сдлать не статическим. 
//
//////////////////////////////////


#ifndef BALL_H
#define BALL_H



//#include <array>
//#include "pictureobj.h"   //меняю на frwd-decl.
#include "ballsManager.h" //пробовал заменить forward-decl., но "inconplete type is not allowed"
#include "lockableAccessVec.h"    ///@@@ #TODO: пытаюсь заменить frwrd-decl'ом (см.стр.14)
//#include "brick.h"    //меняю на frwd-decl.
#include "brick.h"      ///@@@!! Можно было бы без, если бы уже C++0x. А пока вынужден, т.к. не работает frwrd-decl. через `typename` (далее подробнее):  //typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED! ///@@@! But not allowed before C++0x.  ---  Блин! Придётся инклудить брик.h =((
                        //можно без этого инклуда, т.к. он в подключённом тут ballsMngr.h есть.

//// Forward declatations:
class CPictureObject;
//class CArkanoidController::CBricksManager::CBrick;      ///@@@ #WARNIRNG: может это неправильно и нужно отдельно объявить и кл. `CBicksManager`. Тут не выдаёт ошибок даже без этих строк вообще, т.к., видимо, где-то стыкуется с "brick.h" в др. файле, где инклудятся вместе.   ///@@@ Nestsed class Frwrd-decl.-tion is NOT ALLOWED!
//typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED!     //Строка эта не обязательна, т.к. такая же есть в ballsManager.h, кот.тут инклудится. ///@@@! But not allowed before C++0x.  ---  Блин! Придётся инклудить брик.h =((
//typedef CArkanoidController::CBricksManager::CBrick _BRICK; //использую как frwrd-decl., хотя технически это, видимо, не совсем так.  //No need more because of forced include of brick.h (see string above)
                    ///@@@? Не знаю, можно ли Nestsed class Frwrd-decl.?

///@@@ #TODO: попробовать избавиться от лишних инклудов. Напр.,  "#include "lockableAccessVec.h"  тут заменить следующим frwrd-decl'ом (надеюсь, написал верно) 
//template <typename T>
//class CLockableAccessVec;


class CArkanoidController::CBallsManager::CBall
{
public:
    //// C-tor:
    CBall(
            CPictureObject* const           pBall, 
            const bool                      bAttachToPad,
            /*const*/ CPictureObject* const     pPad    //@@ нельзя `const` (почему - см.в прим.к LocateAttachedToPadBall()
        );


    //// Datas (pblc):
    float m_fFiPlus;    //Корректировка к рассчитываемому после удара m_fFi (если удар был по Pad'у). ..
                         //.. Эта корректировка зависит от удаления точки контакта шарика с Pad'ом от его середины.


    //// Methods (pblc):
    bool        TryToInvertIsAttachedToPad(); 
    void        LocateAttachedToPadBall(/*const*/ CPictureObject* const pPad);  //@@ нельзя `const`, т.к. вызываемый метод GetCenter() почему-то не const
    enum        _DIRECTION;     //forward declaration   ///@@@ #WARNING: сработает ли?
    _DIRECTION   IsHit(
            //#XI: CPictureObject* const               pCurrBall,
            CPictureObject* const               pPad,
            /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,              // :Inp.
            const float                         fSceneW,
            CArkanoidController::GAME_STATE     gameState,
            const float                         fDltPxlsToChckHit = 3   //Пар-тр `fDltPxlsToChckHit` - зазор в пикселях м/у объектами, ..
                                                                        //.. определяющий, когда в проверке IsHit() перехожу от поиска ..
                                                                        //.. коллизии вмещающих rect'ов к детальной проверке по ..
                                                                        //.. контрольным точкам. ..
                                                                        //.. Без этой дельты, иногда шарик пролетает сковозь**, видимо, ..
                                                                        //.. т.к. успевает пролететь сразу на несколько пикселей м/у ..
                                                                        //.. итерациями просчёта оптимизирующих (см. "#OPT") проверок. ..
                                                                        //.. Используется в вызываемых из IsHit() методах ..
                                                                        //.. isHitWithPad(..) и isHitWithBricks(..).
                                                                        //**)- пролетает сквозь, т.к. одновременно срабатывает больше ..
                                                                        //.. датчиков котнтакта, чем ожидается - реакция в виде ..
                                                                        //.. изменения траектории в таком случае пока не предусмотрена. ..
                                                                        //.. Быть может в версии 1.01 добавлю.
        );   


    //// Интерфейсные методы:
    vector2                 GetNewMoveCenter(const _DIRECTION dir);
    static CBall*           GetIsAttachedToPaddle() { return m_spIsAttachedToPad; } ///@@ ?: нужно ли тут `static`? Т.е. используем ли этот метод где-то напрямую, не через объект класса?
    CPictureObject*         GetBallPicObj() { return m_pBallPicObj; }
    float                   GetVel() const { return m_fVelocity; }
#if DEBUG ==1
    float                   GetFi() const { return m_fFi; }
#endif
    static vector2          GetNumOfPixelsForShift(const _DIRECTION dir) { return m_smFewPixelsShift[dir]; }
    

    //// Types (pblc):
    enum _DIRECTION {N=0, NE, E, SE, S, SW, W, NW, NO=-1};	//Имена датчиков коллизии (по сторонам света), "NO" - знач-е, ..
                                                            //.. возвращаемое ф-цией IsHit(), если нет коллизий.
                                                            //#WARNING: кол-во значений _DIRECTION == m_suNUM_OF_HIT_CHCK_PNTS-1. Как это ..
                                                            //.. красиво завязать так, чтобы меняя только одно из глоб. значений, второе ..
                                                            //.. пересчитывалось автоматически, пока не придумал. Но обратить внимание в ..
                                                            //.. случае изменений одной из этих сущностей, что нужно поправить и другую.


private:
    //// Datas (prvt):
    CPictureObject*             m_pBallPicObj;
    static CBall*               m_spIsAttachedToPad;
    float                       m_fVelocity;
    float                       m_fFi;		//тек.знач. угла направления движ.шарика == (-PI ; PI]. 0 соотв-ет направлению вертикально up.
    static const u32            m_suNUM_OF_HIT_CHCK_PNTS;       //(см.прим.к объявлению соотв-щей глоб.datы в cpp-шнике).
    static std::vector<float>   m_svfBallHitChckPntsAngles;      //Углы, соотв-щие точкам-датчикам контакта   ///@@@возможно, объединить с m_svHitChckPnts в std::map (подумать)
                                                                //#IMPORTANT: в случ.необходимости изменения кол-ва эл-тов, менять только на чётное кол-во >=2. Увязать с новым знач-ем enum _DIRECTION и class CBall static member m_suNUM_OF_HIT_CHCK_PNTS.
    static std::vector<vector2> m_svHitChckPnts;
    static const float          m_fDEFAULT_BALL_VEL; //Скорость мячика by def.      ///@@@ ?may be перенести в CArkanoidController?
    static std::map<_DIRECTION,vector2> m_smFewPixelsShift;  //Таблица смещений в зависимости от сработавшего датчика контакта. 
                                                   //.. ЗАЧЕМ?: Сдвигаю шарик на несколько (2-3) пикселя, чтобы выйти из тек.коллизии ..
                                                  //..   (след.итерация .IsHit() начинается раньше, чем шарик успевает отлететь на этот ..
                                                 //..   пиксель, что приводит к "залипанию" шарика. ..
                                                //.. АЛЬТЕРНАТИВА?: Можно более правдоподобно сделать: вместо этой сдвижки в методе ..
                                               //..   .IsHit() при после определения сработавшего датчика (N, ..., NW) дальнейшие ..
                                              //..   действия по пересчёту траектории предпринимать только в случ., если m_fFi попадает ..
                                             //..   в диапазон такой, что delta смещения направлена в полуплоскость, в которой лежит ..
                                            //..   сработавший датчик. НО! это лишнее время программирования, + лишняя нагрузка на ..
                                           //..   систему, кот.в данн.случ., (имхо) не оправданан. ..
                                          //.. !ВАЖНО! Этого оказалось недостаточно - шарик иногда успевает залезть в текстуру больше ..
                                         //..   чем на 1 пиксель. Решать проблему кол-вом пикселей не хочу, т.к. когда-нибудь ..
                                        //..   протормозит какой-ндь комп. и залезет шарик на 4 пикселя и получиться беда. Поэтому ..
                                       //..   реализую описанный как АЛЬТЕРНАТИВА метод, но этот оставляю, чтобы при оптимизации ..
                                      //..   метода IsHit() сократить кол-во лишних проверок.
    static const u32            m_suNUM_PIXELS_4_SHIFT;    //==3   Number Of Pixels For Shift Ball after contact

    //Vector углов, на которых располагаются точки-датчики коллизии шарика (угол 0 соотв-ет точке "N"(north), угол PI/2 - "E"(east), ..
    //.. угол -PI/2 - "W"(west) и т.д. Угол принадлежит (-PI : PI] как и угол m_fFi в рассчётах траектории шарика.
    

    //// Methods (prvt):
    void        ComputeNewFi(const _DIRECTION dir);
    void        ComputeStartFi(
                        const float rndStartAng = 10*(float)PI/180,     //Модуль макс.отклонения от 0 случайного угла старт. ..
                                                                        //.. запуска шарика - 10 градусов перевожу в радианы.
                        const u32 multiplForGen = pow(10.0, 4)  //Пар-тр нужен, чтобы скормить float ф-ции ..
                                                                //.. Generate() ч/з u32, который она принимает.
                    );
    void        NormalizeFi();
    vector2     ComputeNewMoveCenter(const u32 moveCCoordScale = pow(10.0, 3)) const;
    _DIRECTION   HitReview(_DIRECTION dir) const;
    void        ComputeFiPlus(CPictureObject* pPad);
    void        InitBallHitChckPntsAnglesVec();
    void        InitHitChckPntsVec();

    //Вспомагательные методы для IsHit()
    void        IsHitWithScreenBorders(
                        const float             fSceneW,                // :Inp.
                        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts   // :Outp.
                    ) const;
    void        IsHitWithPad(
                        //Inp.:
                        CPictureObject* const   pPad,
                        const float             fMaxDltYWithPad, //Определяю при вызове этого метода из IsHit() след. образом: ..
                                                                //.. m_pPad->GetCenter().y - pOriBall->GetCenter().y  ..
                                                               //.. ПОЧЕМУ?: if (ball[i]..->GetPos().y  >  g_fPaddleY +fMaxDltYWithPad), ..
                                                              //.. то не нужно просчитывать IsHit() с Pad'ом - шарик уже не вернуть в поле.
                        const float             fDltPxlsToChckHit,  //Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. ..
                                                                    //.. успевает пролететь сразу на несколько пикселей между итерациями ..
                                                                    //.. просчёта оптимизирующих (см."#OPT") проверок. Получаем от IsHit().
                        //Outp.:
                        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts,  
                        bool&                   rsltNeedToCheckBricks
                    );
    void        IsHitWithBricks(
                        const float                         fDltPxlsToChckHit,      // :Inp.
                        /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,              // :Inp.
                        std::vector<_DIRECTION>&             rsltTrggrdHitChckPnts   // :Outp.   //Получаем от IsHit(). Назначение см. в IsHit()
                    );
    _DIRECTION   InterpretIsHitResults(std::vector<_DIRECTION>& vTrggrdHitChckPnts) const;
    static void InitFewPixelsShift();
};

typedef _BALLS_MNGR::CBall    _BALL;



#endif //BALL_H