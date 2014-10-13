#ifndef BALL_H
#define BALL_H


#include "pictureobj.h"
#include "ballsManager.h"
#include <array>					//для std::array	
//#include "types.h"    Не помогло


class CArkanoidController::CBallsManager::CBall
{
public:
    //C-tor:
    CBall(
            CPictureObject* const   pBall, 
            //#OUTDATED (ввожу static const'анту в класс СБол):     const u32               numOfHitChckPnts,
            //#OUTDATED: std::array<vector2,/*uNumOfHitChckPnts*/0U>& aBallHitChckPnts,     ///#WARNING: сработает ли "0U" ..? - НЕТ!
            const bool              bAttachToPad
        );

    //Datas (pblc):
    /*static*/ float m_sfFiPlus;    //Корректировка к рассчитываемому после удара m_fFi (если удар был по Pad'у) ..     ///@@@ #OUTDATED (Не пойму, зачем ..FiPlus делал static'ом)
                                //.. в зависимости от удаления от середины Pad'а точки контакта шарика с Pad'ом.

    //Methods (pblc):
    bool        TryToInvertIsAttachedToPad(); 
    void        LocateAttachedToPadBall();
    //forward declaration   ///@@@ #WARNING: сработает ли?
    enum DIRECTION;
    DIRECTION   IsHit(const float fDltPxlsToChckHit = 3/*5*/);  //Пар-тр `fDltPxlsToChckHit` - зазор в пикселях м/у объектами ..
                                                               //.. , когда в проверке isHit() перехожу от поиска коллизии ..
                                                              //.. вмещающих rect'ов к детальной проверке по контрольным точкам. ..
                                                             //.. Без этой дельты, иногда шарик пролетает сковозь, видимо, т.к. ..
                                                            //.. успевает пролететь сразу на несколько пикселей между итерациями ..
                                                           //.. просчёта оптимизирующих (см. "#OPT") проверок. Используется в ..
                                                          //.. вызываемых из isHit() методах isHitWithPad(..) и isHitWithBricks(..).

    //Интерфейсные методы:
    vector2                 GetNewMoveCenter(const DIRECTION dir);
    static CBall*           GetIsAttachedToPaddle() { return m_spIsAttachedToPad; }
    CPictureObject*         GetBallPicObj() { return m_pBallPicObj; }
    float                   GetVel() { return m_fVelocity; }
    //#OUTDATED (не вижу смысла в такой prvt data'е):     std::array<vector2, uNumOfHitChckPnts>&  GetHitChckPnts() { return m_aHitChckPnts; }
#if DEBUG ==1
    float                   GetFi() const { return m_fFi; }
#endif

private:
    //Datas (prvt):
    CPictureObject*         m_pBallPicObj;
    static CBall*           m_spIsAttachedToPad;
    float                   m_fVelocity;
    float                   m_fFi;		//тек.знач. угла направления движ.шарика == (-PI ; PI]. 0 соотв-ет направлению вертикально up.
    static const u32        m_suNUM_OF_HIT_CHCK_PNTS;      //(см.прим.к объявлению соотв-щей глоб.datы в cpp-шнике).
    //#OUTDATED: std::array<vector2, m_suNumOfHitChckPnts>&  m_aHitChckPnts;
    static std::vector<vector2> m_vHitChckPnts;
    static const float m_fDEFAULT_BALL_VEL;     //Скорость мячика by def.
    //#DEL_IT?: float       fBallR;
    //#OUTDATED:    std::array<vector2, uNumOfHitChckPnts> m_aBallHitChckPnts;   //Массив коорд.датчиков коллизии шарика //Инициализирую в OnShow()	
    std::map<DIRECTION,vector2> m_mFewPixelsShift;  ///@@@ ?name?
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
    static const u32 m_suNUM_PIXELS_4_SHIFT;    //==3   Number Of Pixels For Shift Ball after contact

    //#OUTDATED (заменяю статич.вектором):
    //std::array<float, m_uNumOfHitChckPnts> aBallHitChckPntsAngles = 
    //    {   
    //        0,          //N
    //        PI/4,       //NE
    //        HALFPI,     //E
    //        PI*3/4,     //SE
    //        PI,         //S
    //        -PI*3/4,    //SW
    //        -HALFPI,    //W
    //        -PI/4       //NW
    //    };

    //Vector углов, на которых располагаются точки-датчики коллизии шарика (угол 0 соотв-ет точке "N"(north), угол PI/2 - "E"(east), ..
    //.. угол -PI/2 - "W"(west) и т.д. Угол принадлежит (-PI : PI] как и угол m_fFi в рассчётах траектории шарика.
    static std::vector<float> m_vfBallHitChckPntsAngles;    //#IMPORTANT: в случ.необходимости изменения кол-ва эл-тов, менять только на чётное кол-во >=2. Увязать с новым знач-ем enum DIRECTION и class CBall static member m_suNUM_OF_HIT_CHCK_PNTS.


    //Types (prvt):
    enum DIRECTION {N=0, NE, E, SE, S, SW, W, NW, NO=-1};	//Имена датчиков коллизии (по сторонам света), "NO" - знач-е, ..
                                                            //.. возвращаемое ф-цией isHit(), если нет коллизий.
                                                            //#WARNING: кол-во значений DIRECTION == uNumOfHitChckPnts-1. Как это ..
                                                            //.. красиво завязать так, чтобы меняя только одно из глоб. значений, второе ..
                                                            //.. пересчитывалось автоматически, пока не придумал. Но обратить внимание в ..
                                                            //.. случае изменений одной из этих сущностей, что нужно поправить и другую.




    //Methods (prvt):
    void        ComputeNewFi(const DIRECTION dir);
    void        ComputeStartFi(
                    const float rndStartAng = 10*(float)PI/180,   //Модуль макс.отклонения от 0 случайного угла старт. ..
                                                                  //.. запуска шарика - 10 градусов перевожу в радианы.
                    const u32 multiplForGen = pow(10.0, 4)  //Пар-тр нужен, чтобы скормить float ф-ции ..
                                                            //.. Generate() ч/з u32, который она принимает.
                );
    void        NormalizeFi();
    vector2     ComputeNewMoveCenter(u32 moveCCoordScale = pow(10.0, 3)) const;
    DIRECTION   HitReview(const DIRECTION dir) const;
    void        ComputeFiPlus();
    void        InitBallHitChckPntsAnglesVec();
    void        InitHitChckPntsVec();

    //Вспомагательные методы для isHit()
    void        IsHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
    void        IsHitWithPad(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    bool& needToCheckBricks, 
                    const float fMaxDltYWithPad,    //Определяю при вызове этого метода из isHit() след. образом: ..
                                                    //.. g_pPad->GetCenter().y - pOriBall->GetCenter().y  ..
                                                    //.. ПОЧЕМУ?: if (ball[i]..->GetPos().y  >  g_fPaddleY +f MaxDltYWithPad), то ..
                                                    //.. больше не нужно просчитывать isHit() с Pad'ом - шарик уже не вернуть в поле.
                    const float& fDltPxlsToChckHit      //Получает от isHit(). Назначение см. в isHit()
                );
    void        IsHitWithBricks(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    const float& fDltPxlsToChckHit  //Получает от isHit(). Назначение см. в isHit()
                );
    DIRECTION   InterpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
    void InitFewPixelsShift();
};



#endif//BALL_H