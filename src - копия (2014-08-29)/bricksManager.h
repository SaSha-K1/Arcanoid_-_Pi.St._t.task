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
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef BRICKS_MANAGER_H 
#define BRICKS_MANAGER_H 


#include <array>
#include "types.h"
#include "controller.h"


class CArkanoidController::CBricksManager
{
public:
    //C-tor
    CBricksManager(const u32 numOfBricksOnLvl);
    
    //Methods (pblc):
    void AddBrick(CAniObject* baseBrick, vector2 position, CScene* sc);
    void DelBrick(u32 i, CScene* sc);
    friend class CArkanoidController;   ///@@@ #UGLY: попробовать красиво разрулить без `friend`


private:
    //Datas (prvt):
    const u32 m_uNUM_OF_BRICKS_ON_LVL;   //Кол-во кирпичей на тек.уровне - для размерности вектора ActObjects.objects
    class CBrick;
    std::vector<CBrick*>    m_vpBricks;

    #if DONT_DELETE_ANI_BRICKS==1
        //#OUTDATED: заменяю array vector'ом
        //std::array<CAniObject*, /*CArkanoidController::m_uNUM_OF_BRICKS_ON_LEVEL1*/0U>* papBricksAniObjects;    ///@@@ проблема в том, что array хочет const параметром, а то, что я пытаюсь кормить, инициализируется только при создании экз.класса (видимо, причина в этом) - позже разобраться! отдельно в причинах. ПОКА РЕШАЮ ТАК: тут делаю ptr на array<...,0U> (не ссылку, т.к. нужно иниц-ть null'ём, чтобы проверку в D-tor'е забацать), инициализирую его в C-tor'е. В D-tor'е проверяю если !=nullptr, то delete
        std::vector<CAniObject*> vpBricksAniObjs;

        //#OUTDATED: 
        //void InitOfBricksAniObjects(std::vector<CBricksManager::CBrick*> const vpBricks)
        //{	
        //    for (char i = 0; i < vpBricks.size(); ++i)
        //    {
        //        papBricksAniObjects[i] = vpBricks[i]->getBrickAniObj();
        //    }
        //}
        void InitOfBricksAniObjects();
        
        void RetrieveBrick(
                std::array<CAniObject*, m_uNUM_OF_BRICKS_ON_LEVEL1> const apBricksAniObjects, 
                std::array<vector2, m_uNUM_OF_BRICKS_ON_LEVEL1> const aBricksCoordsLvl
            )
        {
            for (char i = 0; i < apBricksAniObjects.size(); ++i)
                m_vpBricks.push_back(new CBricksManager::CBrick(apBricksAniObjects[i], aBricksCoordsLvl[i]));
        }
    #endif//DONT_DELETE_ANI_BRICKS

// ПЕРЕНОШУ В PUBLIC
////Methods (prvt):
//    void AddBrick(CAniObject* baseBrick, vector2 position, CScene* sc);
//    void DelBrick(u32 i, CScene* sc)


};




#endif//BRICKS_MANAGER_H 