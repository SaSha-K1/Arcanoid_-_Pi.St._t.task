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
//    ��������������, ��� ������ _BRICKS_MNGR �������� ��� ��������� ������ //
//  ������. ����� ��.�������, ������� ������ � ������ ����� ��� ����������  //
//  ������.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef BRICKS_MANAGER_H 
#define BRICKS_MANAGER_H 



#include <vector>
#include "types.h"              //��� u32
#include "controller.h"   //@@@������ �������� �� frwrd-decl. - ���� ����� �� ����������. ������, �/ ������::������ ����� ������ ����������� �������
#include "myExcptn.h"
#include "lockableAccessVec.h"  //@@@����� ����� ���� �� ����� �������? | �� ���������, �.�. ��������� ������ ����� obj-��

//class CArkanoidController;
class CScene;


////////////////////////////// EXP ZONE VVVVVVVVVVVVVVVVVVVV

//#define _LAV_MACRO CLockableAccessVec<_BRICK*>::CLavHandler   //��� ������, �.�. ��� _BRICK �� ���������� (����.h �� ��������)

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
            /*const*/ CAniObject* const baseBrick,  //@@ ��� � ����!: 1� `const` �������� ������, �.�. � ������ ������������� ������������� ������. ��.��. ����������� �����_����
            const vector2           position, 
            CScene* const           sc
        );

///@@@ #XI:
/// ����� �������� �����-�� ���������� �� `CleanBricks()` � ���� ����� ptr �� auto_ptr (� �������� #UGLY ���), ..
///.. ��� �������� � shared_ptr (��� (����) � ���.����. �� ������ ���������), ������� ���� ����� � `CleanBricks()` ..
///.. ����� ���, �.�. ��� ����������� ���� ������ 3 ������ � ����� ����� ������: 
//    void DelBrick(
//            const u32                       i, 
//            CScene* const                   sc,
//            CArkanoidController* const      pArkCntrllr,
//
/////@@@ ���� ���??: (� � ��������� �����, � ������� typedef)
/////@@@ error C2065: '_LAV_OFBRICKS_HNDLR' : undeclared identifier             
/////@@@..            std::auto_ptr<_LAV_OFBRICKS_HNDLR>*    ppLavHndlr = nullptr    //���������������� ����-� by def., ������������� � ���������� ������.     //@@��� �� ������ `const`'�, �.�. � ��������� ������������� � ������� �������.������ ��� ��������������� ptr'�.
//            std::auto_ptr<CLockableAccessVec</*_BRICK*/CBrick*>::CLavHandler/*<_BRICK*>*/>*    ppLavHndlr = nullptr    //���������������� ����-� by def., ������������� � ���������� ������.     //@@��� �� ������ `const`'�, �.�. � ��������� ������������� � ������� �������.������ ��� ��������������� ptr'�.
//        );

    ///@@@ #WARNING (���� ����������� - ��� ������� ������ �� �����. ��, ���� ���, �������� ������������ ��� ���������� ��������� ����������������� ���������): 
    ///friend class CArkanoidController;   ///@@@ #UGLY: ����������� ������� ��������� ��� `friend`
    

    void RetrieveBrick(/*std::vector<*/ const std::vector<vector2>* /*>*/ const  /*vpvBricksOnLvlsCoords*/pvBricksOnCurrLvlCoords);

///@@@ ���� ���??: (� � ��������� �����, � ������� typedef)
///@@@ error C2065: '_LAV_OFBRICKS_HNDLR' : undeclared identifier             
///@@@..    std::auto_ptr<_LAV_OFBRICKS_HNDLR> GetLavHndlr(const u32 key);     //������������ ������, ������������ m_LavBricks.CreateAccessHandler(key).   ///@@@�����������, �������� ���-��, ��� ���
//    std::auto_ptr</*CLockableAccessVec<_BRICK*>::CLavHandler*/_LAV_MACRO> GetLavHndlr(const u32 key);     //������������ ������, ������������ m_LavBricks.CreateAccessHandler(key).   ///@@@�����������, �������� ���-��, ��� ���
    //std::auto_ptr<_LAV_OFBRICKS_HNDLR> GetLavHndlr(const u32 key); //�������, �.�. _LAV_OFBRICKS_HNDLR �� ���������
    std::unique_ptr<CLockableAccessVec<CBrick*>::CLavHandler> GetLavHndlr(const u32 key);     //������������ ������, ������������ m_LavBricks.CreateAccessHandler(key).   ///@@@�����������, �������� ���-��, ��� ���
    void CleanBricks(                           //���������� ������� �� `m_LavBricks` � (��� �����-��) �������
            CScene* const                   sc,
            CArkanoidController* const      pArkCntrllr
        );

    CLockableAccessVec<CBrick*>* GetLavBricksPtr() { return &m_LavBricks; }     //�/ �������� � _BALLS_MNGR::ProcessIsHit() (� ���.����� � IsHitWithBricks())

    void InitOfBricksAniObjects();

    void AddBricksOnStart(    //��������� ������ �������� � ��������� �� � �����. (����������� �� ������ ���� � (������ � ������ �������� ���������� � ��������� ���-�����) ����� ����� ��������)
            CScene*                 sc, 
            std::vector<vector2>*   pvBricksOnLvlCoords
        );

#if DEBUG==1
    void IsBricksEmptyCheck();       //�/ ������  `CArkanoidController::ProcessWinLostState()`  ��������, ��� ������ �������� ����. If ���, ����� MyExcptn.  
#endif



private:
    //// Datas (prvt):
    const u32 m_uNUM_OF_BRICKS_ON_LVL;  //���-�� �������� �� ���.������ - ��� ����������� ������� ActObjects.objects
                                        ///@@@? ���� ��������� ����������, �� ��� data ��� ����� (�������� �� ������� � �������.h ��������� NUMBER_OF_BRICKS (==12)), �.�. � ������� ����� ������ ����������� ���-�� �������� �� ����.�������, � � ��� data-member �� ����� �������� ���� �� ��� (�� ���. �������).

    //// Types (prvt):
    //#OUTDATED (�������� ����-� ��� `typedef` ����� ���� ������� - �������� � `public:` @@@ ����� ����������� - ����� ����� ������� � `private:`):
    //class CBrick;
    
    //#OUTDATED:
    //std::vector<CBrick*>    m_vpBricks;
    CLockableAccessVec<CBrick*> m_LavBricks;    //'LavBricks' == Lockable Access Vector "Bricks"

#if DONT_DELETE_ANI_BRICKS==1   ///@@@ ���� �������� �� ����� �����, ������ �� ��������� �������

    //#OUTDATED: ������� array vector'��
    //std::array<CAniObject*, /*CArkanoidController::m_uNUM_OF_BRICKS_ON_LEVEL1*/0U>* papBricksAniObjects;    ///@@@ �������� � ���, ��� array ����� const ����������, � ��, ��� � ������� �������, ���������������� ������ ��� �������� ���.������ (������, ������� � ����) - ����� �����������! �������� � ��������. ���� ����� ���: ��� ����� ptr �� array<...,0U> (�� ������, �.�. ����� ����-�� null'��, ����� �������� � D-tor'� ��������), ������������� ��� � C-tor'�. � D-tor'� �������� ���� !=nullptr, �� delete
    std::vector<CAniObject*> m_vpBricksAniObjs; ///#?/(X.11) ����� ���� ����? - ���������� � ���� ���������� ������*s �������� �� ���.`m_LavBricks` ������� _BRICKS_MNGR::InitOfBricksAniObjects(). �� �����? - ���� ��� �������� ������ �� `m_LavBricks` (������� ���� ���-�� ������ BR.MNGR)

    //#OUTDATED: 
    //void InitOfBricksAniObjects(std::vector<CBricksManager::CBrick*> const vpBricks)
    //{	
    //    for (char i = 0; i < vpBricks.size(); ++i)
    //    {
    //        papBricksAniObjects[i] = vpBricks[i]->getBrickAniObj();
    //    }
    //}
    
    //void InitOfBricksAniObjects(); - �������� � ������
        
#endif //DONT_DELETE_ANI_BRICKS

// �������� � PUBLIC
////Methods (prvt):
//    void AddBrick(CAniObject* baseBrick, vector2 position, CScene* sc);
//#OLD:    void DelBrick(u32 i, CScene* sc)

    ///@@@ Warning	81	warning C4512: 'CArkanoidController::CBricksManager' : assignment operator could not be generated	d:\arkanoid\src\bricksmanager.h	122

};

typedef CArkanoidController::CBricksManager     _BRICKS_MNGR;



#endif //BRICKS_MANAGER_H 