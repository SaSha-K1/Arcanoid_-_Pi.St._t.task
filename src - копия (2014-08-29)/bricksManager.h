///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  bricksManager.h                                                          //
//  authors: Sasha Kislun                                                    //
//  2014, Test task                                                          //
//                                                                           //
//  ������� ���-�� ������� CBricksManager � CBricksManager::CBrick :         //
//  ����� CBrick �������� �� ��, ��� �������� ���������� ������� (�����     //
//  �������� ��� � ���� � ������ �� ��).                                    //
//  ����� CBricksManager �������� �� ��, ��� ��������� � �����������������   //
//  ���� �������� � ���� (� �.�. �� �������� ������� � ���� � ����� �� ��). //
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
    friend class CArkanoidController;   ///@@@ #UGLY: ����������� ������� ��������� ��� `friend`


private:
    //Datas (prvt):
    const u32 m_uNUM_OF_BRICKS_ON_LVL;   //���-�� �������� �� ���.������ - ��� ����������� ������� ActObjects.objects
    class CBrick;
    std::vector<CBrick*>    m_vpBricks;

    #if DONT_DELETE_ANI_BRICKS==1
        //#OUTDATED: ������� array vector'��
        //std::array<CAniObject*, /*CArkanoidController::m_uNUM_OF_BRICKS_ON_LEVEL1*/0U>* papBricksAniObjects;    ///@@@ �������� � ���, ��� array ����� const ����������, � ��, ��� � ������� �������, ���������������� ������ ��� �������� ���.������ (������, ������� � ����) - ����� �����������! �������� � ��������. ���� ����� ���: ��� ����� ptr �� array<...,0U> (�� ������, �.�. ����� ����-�� null'��, ����� �������� � D-tor'� ��������), ������������� ��� � C-tor'�. � D-tor'� �������� ���� !=nullptr, �� delete
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

// �������� � PUBLIC
////Methods (prvt):
//    void AddBrick(CAniObject* baseBrick, vector2 position, CScene* sc);
//    void DelBrick(u32 i, CScene* sc)


};




#endif//BRICKS_MANAGER_H 