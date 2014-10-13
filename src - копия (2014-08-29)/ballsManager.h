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


#include <array>
#include "pictureobj.h"
#include "controller.h"


class CArkanoidController::CBallsManager : public CSingleton<CArkanoidController::CBallsManager>
{
public:
    //C-tor
    CBallsManager(
            CPictureObject* pBallPicObj,
            std::array<CPictureObject*, 0U/*MAX_BALL_NUM*/>&  raBallsPictObjects   ///#WARNING: сработает ли "0U"..?
        );

    //Methods (pblc):
    void AddBall(std::array<vector2, uNumOfHitChckPnts> &ballHitChckPnts, bool attachToPad);     //Ввести шарик в игру	//CPictureObject'ы для ещё 3х шариков добавил в xml и в obj.h, но держу их за пределами видимого игр.поля, пока не нужны. Старт.координаты задаются в xml, поэтому тут не нужны
    void DelBall(u32 i);	//Вывести шарик из игры     //was (std::vector<CBall*>::iterator it)


private:
    const char  MAX_BALL_NUM = 4;


    class CBall;
    std::vector<CBall*>     m_vBalls;  

    ///@@@ Convert to map
    //#OUTDATED:
    //std::array<CPictureObject*, MAX_BALL_NUM>&  m_aBallsPictObjects;	//СВЯЗКА 2х МАССИВОВ: 1) массив CPict.Object'ов всех 5 (MAX_BALL_NUM) шариков
    //std::array<bool, MAX_BALL_NUM>              m_aBallsPictObjectsUsage;		//СВЯЗКА 2х МАССИВОВ: 2) массив индикаторов занятости CPict.Object'ов из 1го массива	//инициализирую его нулями в методе initOfBallsPictObjects(), вызываемом единожды в OnShow()
    std::map<CPictureObject*, bool> m_mBallsPictObjects;



//Methods(prvt):

    //#OUTDATED:    думал передавать CPict.Obj* 'ы шариков, но и тогда получаю зависимый от кол-ва шариков код. Поэтому решил готовить массив CPict.Obj* 'ов во веншн.коде и передавать сюда, хватая такой array по ссылке.
    //void InitOfBallsPictObjects(CScene* sc);    //Когда m_scene начинает существовать, инициализируем массив  ..
    //                                            //.. CPict.Object'ов всех шариков в игре (MAX_BALL_NUM) /*пока их 4*/
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


#endif//BALLS_MANAGER_H