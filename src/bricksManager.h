///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  bricksManager.h                                                          //
//  authors: Sasha Kislun                                                    //
//  2014, Test task                                                          //
//                                                                           //
//  Границы отв-ти классов CBricksManager и CBricksManager::CBrick :         //
//  Класс CBrick отвечает за всё, что касается отдельного кирпича (кроме     //
//  введения его в игру и вывода из неё).                                    //
//  Класс CBricksManager отвечает за то, что относится к администрированию   //
//  всех кирпичей в игре (в т.ч. за введение корпича в игру и вывод из неё). //
//    Предполагается, что объект _BRICKS_MNGR создаётся для обработки одного //
//  уровня. Когда ур.пройден, удаляем объект и создаём новый для следующего  //
//  уровня.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef BRICKS_MANAGER_H 
#define BRICKS_MANAGER_H 



#include <vector>
#include "types.h"              //для u32
#include "controller.h"   //@@@пробую заменить на frwrd-decl. - пока вроде не получается. Видимо, д/ КлассА::КлассБ нужно полное определение КлассаА
#include "myExcptn.h"
#include "lockableAccessVec.h"  //@@@может можно уйти от этого инклуда? | НЕ получится, т.к. использую методы этого obj-та

//class CArkanoidController;
class CScene;


////////////////////////////// EXP ZONE VVVVVVVVVVVVVVVVVVVV

//#define _LAV_MACRO CLockableAccessVec<_BRICK*>::CLavHandler   //так нельзя, т.к. тут _BRICK не определено (брик.h не инклудим)

////////////////////////////// EXP ZONE XXXXXXXXXXXXXXXXXXXX


class CArkanoidController::CBricksManager
{
public:
    //// C-tor:
    CBricksManager(const u32 numOfBricksOnLvl);


    //// Types (pblc):
    class CBrick;
    

    //// Methods (pblc):
    void AddBrick(
            /*const*/ CAniObject* const baseBrick,  //@@ Так и есть!: 1й `const` возможно убрать, т.к. в методе инициализирую неконстантный объект. Ск.вс. понадобится конст_каст
            const vector2           position, 
            CScene* const           sc
        );

///@@@ #XI:
/// Чтобы избежать необх-ти передавать из `CleanBricks()` в этот метод ptr на auto_ptr (и получать #UGLY код), ..
///.. или работать с shared_ptr (что (имхо) в дан.случ. не вполне оправдано), включаю этот метод в `CleanBricks()` ..
///.. Решил так, т.к. там уникального кода только 3 строки и вызов этого метода: 
//    void DelBrick(
//            const u32                       i, 
//            CScene* const                   sc,
//            CArkanoidController* const      pArkCntrllr,
//
/////@@@ Чего так??: (я ж подключил хедер, в котором typedef)
/////@@@ error C2065: '_LAV_OFBRICKS_HNDLR' : undeclared identifier             
/////@@@..            std::auto_ptr<_LAV_OFBRICKS_HNDLR>*    ppLavHndlr = nullptr    //Закомментировано знач-е by def., установленное в объявлении метода.     //@@нет ни одного `const`'а, т.к. и указатель переопределяю и вызываю неконст.методы для разыменованного ptr'а.
//            std::auto_ptr<CLockableAccessVec</*_BRICK*/CBrick*>::CLavHandler/*<_BRICK*>*/>*    ppLavHndlr = nullptr    //Закомментировано знач-е by def., установленное в объявлении метода.     //@@нет ни одного `const`'а, т.к. и указатель переопределяю и вызываю неконст.методы для разыменованного ptr'а.
//        );

    ///@@@ #WARNING (пока закомментил - при ошибках решать по месту. Ну, если что, временно раскоментить для достижения стартовой работоспособности программы): 
    ///friend class CArkanoidController;   ///@@@ #UGLY: попробовать красиво разрулить без `friend`
    

    void RetrieveBrick(/*std::vector<*/ const std::vector<vector2>* /*>*/ const  /*vpvBricksOnLvlsCoords*/pvBricksOnCurrLvlCoords);

///@@@ Чего так??: (я ж подключил хедер, в котором typedef)
///@@@ error C2065: '_LAV_OFBRICKS_HNDLR' : undeclared identifier             
///@@@..    std::auto_ptr<_LAV_OFBRICKS_HNDLR> GetLavHndlr(const u32 key);     //интерфейсный метода, возвращающий m_LavBricks.CreateAccessHandler(key).   ///@@@Просмотреть, применяю где-то, или нет
//    std::auto_ptr</*CLockableAccessVec<_BRICK*>::CLavHandler*/_LAV_MACRO> GetLavHndlr(const u32 key);     //интерфейсный метода, возвращающий m_LavBricks.CreateAccessHandler(key).   ///@@@Просмотреть, применяю где-то, или нет
    //std::auto_ptr<_LAV_OFBRICKS_HNDLR> GetLavHndlr(const u32 key); //заменяю, т.к. _LAV_OFBRICKS_HNDLR не определяю
    std::unique_ptr<CLockableAccessVec<CBrick*>::CLavHandler> GetLavHndlr(const u32 key);     //интерфейсный метода, возвращающий m_LavBricks.CreateAccessHandler(key).   ///@@@Просмотреть, применяю где-то, или нет
    void CleanBricks(                           //перебирает кирпичи из `m_LavBricks` и (при необх-ти) удаляет
            CScene* const                   sc,
            CArkanoidController* const      pArkCntrllr
        );

    CLockableAccessVec<CBrick*>* GetLavBricksPtr() { return &m_LavBricks; }     //д/ передачи в _BALLS_MNGR::ProcessIsHit() (в кон.счёте в IsHitWithBricks())

    void InitOfBricksAniObjects();

    void AddBricksOnStart(    //Заполняет вектор кирпичей и размещает их в сцене. (применяется на старте игры и (только в случае варианта реализации с удалением ани-биков) также после выигрыша)
            CScene*                 sc, 
            std::vector<vector2>*   pvBricksOnLvlCoords
        );

#if DEBUG==1
    void IsBricksEmptyCheck();       //д/ метода  `CArkanoidController::ProcessWinLostState()`  Проверяю, что вектор кирпичей пуст. If нет, кидаю MyExcptn.  
#endif



private:
    //// Datas (prvt):
    const u32 m_uNUM_OF_BRICKS_ON_LVL;  //Кол-во кирпичей на тек.уровне - для размерности вектора ActObjects.objects
                                        ///@@@? Если правильно припоминаю, то эта data тут нужна (несмотря на наличие в глобалс.h константы NUMBER_OF_BRICKS (==12)), т.е. в глобалс будут лежать константами кол-ва кирпичей на кажд.уровень, а в эту data-member мы будем забирать одну из них (на тек. уровень).

    //// Types (prvt):
    //#OUTDATED (ругается КОМП-р при `typedef` типов этих классов - переношу в `public:` @@@ позже разрбраться - может можно вернуть в `private:`):
    //class CBrick;
    
    //#OUTDATED:
    //std::vector<CBrick*>    m_vpBricks;
    CLockableAccessVec<CBrick*> m_LavBricks;    //'LavBricks' == Lockable Access Vector "Bricks"

#if DONT_DELETE_ANI_BRICKS==1   ///@@@ если откажусь от этого блока, убрать из диаграммы классов

    //#OUTDATED: заменяю array vector'ом
    //std::array<CAniObject*, /*CArkanoidController::m_uNUM_OF_BRICKS_ON_LEVEL1*/0U>* papBricksAniObjects;    ///@@@ проблема в том, что array хочет const параметром, а то, что я пытаюсь кормить, инициализируется только при создании экз.класса (видимо, причина в этом) - позже разобраться! отдельно в причинах. ПОКА РЕШАЮ ТАК: тут делаю ptr на array<...,0U> (не ссылку, т.к. нужно иниц-ть null'ём, чтобы проверку в D-tor'е забацать), инициализирую его в C-tor'е. В D-tor'е проверяю если !=nullptr, то delete
    std::vector<CAniObject*> m_vpBricksAniObjs; ///#?/(X.11) Зачем этот член? - Фактически в него загоняются аниобъ*s кирпичей из век.`m_LavBricks` методом _BRICKS_MNGR::InitOfBricksAniObjects(). Но НАФИГ? - ведь они доступны просто из `m_LavBricks` (который тоже явл-ся членом BR.MNGR)

    //#OUTDATED: 
    //void InitOfBricksAniObjects(std::vector<CBricksManager::CBrick*> const vpBricks)
    //{	
    //    for (char i = 0; i < vpBricks.size(); ++i)
    //    {
    //        papBricksAniObjects[i] = vpBricks[i]->getBrickAniObj();
    //    }
    //}
    
    //void InitOfBricksAniObjects(); - переношу в паблик
        
#endif //DONT_DELETE_ANI_BRICKS

// ПЕРЕНОШУ В PUBLIC
////Methods (prvt):
//    void AddBrick(CAniObject* baseBrick, vector2 position, CScene* sc);
//#OLD:    void DelBrick(u32 i, CScene* sc)

    ///@@@ Warning	81	warning C4512: 'CArkanoidController::CBricksManager' : assignment operator could not be generated	d:\arkanoid\src\bricksmanager.h	122

};

typedef CArkanoidController::CBricksManager     _BRICKS_MNGR;



#endif //BRICKS_MANAGER_H 