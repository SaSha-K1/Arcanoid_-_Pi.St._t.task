///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  ballsManager.h                                                           //
//  authors: Sasha Kislun                                                    //
//  2014, Test task                                                          //
//                                                                           //
//  Границы ответственности классов CBallsManager и CBallsManager::CBall :   //
//  Класс CBall отвечает за всё, что касается отдельного шарика (кроме       //
//  введения его в игру и вывода из неё).                                    //
//  Класс CBallsManager отвечает за то, что относится к администрированию    //
//  всех шариков в игре (в т.ч. за введение шарика в игру и вывод из неё).   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef BALLS_MANAGER_H 
#define BALLS_MANAGER_H



//#include <array>
#include <vector>
#include "types.h"      //для u32
//#include "pictureobj.h"
#include "controller.h" //напр., д/  " CArkanoidController::GAME_STATE&    gameState ..."
#include "lockableAccessVec.h"  //принимаю параметр в ProcessIsHit()
#include "dbgPrint.h"   //объявляю ф-ции друзьями
#include "brick.h"      ///@@@!! Можно было бы без, если бы уже C++0x. А пока вынужден, т.к. не работает frwrd-decl. через `typename` (далее подробнее):  //typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED! ///@@@! But not allowed before C++0x.  ---  Блин! Придётся инклудить брик.h =((


//// Forward declatations:
class CPictureObject;
//class CArkanoidController::CBricksManager;    //не нужно, т.к. уже:  #include "controller.h" 
//class CArkanoidController::CBricksManager::CBrick;  ///@@@ Nestsed class Frwrd-decl.-tion is NOT ALLOWED!
//typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED! ///@@@! But not allowed before C++0x.  ---  Блин! Придётся инклудить брик.h =((
//typedef CArkanoidController::CBricksManager::CBrick _BRICK; //No need more because of forced include of brick.h (see string above)



class CArkanoidController::CBallsManager /* : public CSingleton<CArkanoidController::CBallsManager> */
{
public:
    //// C-tor:
    CBallsManager(
            //#OUTDATED (в cpp не было. Тут почему-то есть. Видимо рудимент):     CPictureObject*                                     pBallPicObj,
            
            //#OUTDATED (array's size is static. Поэтому юзаю vector): 
            //std::array<CPictureObject*, 0U/*m_uMAX_BALL_NUM*/>&    raBallsPictObjects   ///#WARNING: сработает ли "0U"..? ANSWR: НЕТ
            const std::vector<CPictureObject*>&    rvBallsPictObjects
        );

    //Methods (pblc):
    void AddBall(   //Ввести шарик в игру	//CPictureObject'ы для ещё 3х шариков добавил в xml и в obj.h, но держу их за пределами видимого игр.поля, пока не нужны. Старт.координаты задаются в xml, поэтому тут не нужны
            //#OUTDATED: std::array<vector2, m_suNUM_OF_HIT_CHCK_PNTS>&  ballHitChckPnts, 
            const bool                  attachToPad,
            /*const*/ CPictureObject* const pPad    //@@ нельзя `const`, т.к. передаю в конце концов в CBall::LocateAttachedToPadBall(), а там вызываю неконстантный (почему-то) метода GetCenter()
        );     

    void DelBall(
            const u32                           i,          //индекс удаляемого шарика в m_vBalls
            CArkanoidController::GAME_STATE&    rGameState
        );	//Вывести шарик из игры     //was (std::vector<CBall*>::iterator it)

    void CleanBalls(    //Удалить объекты класса CBall вылетевших за пределы поля шариков
            const float                         fSceneH,  
            const float                         fSceneW,
            CArkanoidController::GAME_STATE&    rGameState
        );
    
    void ProcessIsHit(
            //std::vector<CBall*> vpBalls
            CPictureObject*                 pPad,
            CLockableAccessVec<_BRICK*>*    pLavBricks,
            float                           fSceneW,
            CArkanoidController::GAME_STATE gameState
        );

    bool IsBallsEmpty()     { return m_vBalls.empty(); }

    void MoveBallWithPad(CPictureObject* pPad, float fCurrPadY);

    void BallThrowIn();     //Запуск шарика, прикреплённого к Pad'у по клику мыши

    void CleanBalls(CArkanoidController::GAME_STATE& gameState);    //Очистить вектор шариков при победе (в случ. поражения собстенно его условием есть пустой вектор шариков)

#if DEBUG==1
    float GetBallFi (const u32 ballNum) const;  //I need this method only for using in `DbgPrint()` in `OnRender()`
#endif //DEBUG


    //// Types (pblc):
    class CBall;    //forward declaration


    //// Friends:
#if DEBUG==1
    friend void CArkanoidController::OnRender(vector2);
    
    ///@@@? а нужно ли объявлять эти ф-ции друзьями? Возможно НЕТ, если мне нужен доступ к закрытым полям только в списке арг-тов?
    friend void dbgPrnt::DbgPrint(
            const std::string &txt, 
            const u32 y,    const u32 x/*=10*/,                                        //Координаты
            const u32 r/*=1*/, const u32 g/*=255*/, const u32 b/*=255*/, const u32 a/*=100*/     //Цвет
        );														
    template <typename T> 
    friend void dbgPrnt::DbgPrint/*<T>/**/(
        const std::string &txt,
        const T &var,       ///@@@ если парсер будет париться, какую из ф-ий DbgPrint() подставлять, то попробовать тут принимать ptr на var-ую - может тип переменной - указатель будет определять ф-ю однозначно
        const u32 y,    const u32 x/*=10*/,                                     //Координаты
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //Цвет
    );
    template <typename T1, typename T2> 
    friend void DbgPrint/*<T1, T2>/**/(
        const std::string &txt,
        const T1 &var1, 
        const T2 &var2, 
        const std::string &delim, 
        const u32 y, const u32 x/*=10*/,                                        //Координаты
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //Цвет
    );				
#endif //DEBUG





private:
    //// Datas (prvt):
    const u32                       m_uMAX_BALL_NUM;    // = 4 (присваиваю в C-tor'е)
    std::vector<CBall*>             m_vBalls;           //веткор шариков ?, которые на данн.мом.в игре

    ///@@@ Convert to map
    //#OUTDATED:
    //std::array<CPictureObject*, m_uMAX_BALL_NUM>&  m_aBallsPictObjects;	//СВЯЗКА 2х МАССИВОВ: 1) массив CPict.Object'ов всех 5 (m_uMAX_BALL_NUM) шариков
    //std::array<bool, m_uMAX_BALL_NUM>              m_aBallsPictObjectsUsage;		//СВЯЗКА 2х МАССИВОВ: 2) массив индикаторов занятости CPict.Object'ов из 1го массива	//инициализирую его нулями в методе initOfBallsPictObjects(), вызываемом единожды в OnShow()
    std::map<CPictureObject*, bool> m_mBallsPictObjects;



//Methods(prvt):

    //#OUTDATED:    думал передавать CPict.Obj* 'ы шариков, но и тогда получаю зависимый от кол-ва шариков код. Поэтому решил готовить массив CPict.Obj* 'ов во веншн.коде и передавать сюда, хватая такой array по ссылке.
    //void InitOfBallsPictObjects(CScene* sc);    //Когда m_scene начинает существовать, инициализируем массив  ..
    //                                            //.. CPict.Object'ов всех шариков в игре (m_uMAX_BALL_NUM) /*пока их 4*/
    //    void CBallsManager::InitOfBallsPictObjects(CScene* sc)
    //{
    //    m_aBallsPictObjectsUsage.fill(0);
	//
    //    m_aBallsPictObjects[0] = sc->FindPictureObject(PIC_SC01_BALL);
    //    m_aBallsPictObjects[1] = sc->FindPictureObject(PIC_SC01_BALL2);
    //    m_aBallsPictObjects[2] = sc->FindPictureObject(PIC_SC01_BALL3);
    //    m_aBallsPictObjects[3] = sc->FindPictureObject(PIC_SC01_BALL4);
    //    m_aBallsPictObjects[4] = sc->FindPictureObject(PIC_SC01_BALL5);
    //}


};

typedef CArkanoidController::CBallsManager _BALLS_MNGR;

///@@@!: warning C4512: 'CArkanoidController::CBallsManager' : assignment operator could not be generated	d:\arkanoid\src\ballsmanager.h	150




#endif //BALLS_MANAGER_H