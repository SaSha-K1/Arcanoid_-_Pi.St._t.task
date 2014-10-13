///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  ballsManager.h                                                           //
//  authors: Sasha Kislun                                                    //
//  2014, Test task                                                          //
//                                                                           //
//  ������� ��������������� ������� CBallsManager � CBallsManager::CBall :   //
//  ����� CBall �������� �� ��, ��� �������� ���������� ������ (�����       //
//  �������� ��� � ���� � ������ �� ��).                                    //
//  ����� CBallsManager �������� �� ��, ��� ��������� � �����������������    //
//  ���� ������� � ���� (� �.�. �� �������� ������ � ���� � ����� �� ��).   //
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
            std::array<CPictureObject*, 0U/*MAX_BALL_NUM*/>&  raBallsPictObjects   ///#WARNING: ��������� �� "0U"..?
        );

    //Methods (pblc):
    void AddBall(std::array<vector2, uNumOfHitChckPnts> &ballHitChckPnts, bool attachToPad);     //������ ����� � ����	//CPictureObject'� ��� ��� 3� ������� ������� � xml � � obj.h, �� ����� �� �� ��������� �������� ���.����, ���� �� �����. �����.���������� �������� � xml, ������� ��� �� �����
    void DelBall(u32 i);	//������� ����� �� ����     //was (std::vector<CBall*>::iterator it)


private:
    const char  MAX_BALL_NUM = 4;


    class CBall;
    std::vector<CBall*>     m_vBalls;  

    ///@@@ Convert to map
    //#OUTDATED:
    //std::array<CPictureObject*, MAX_BALL_NUM>&  m_aBallsPictObjects;	//������ 2� ��������: 1) ������ CPict.Object'�� ���� 5 (MAX_BALL_NUM) �������
    //std::array<bool, MAX_BALL_NUM>              m_aBallsPictObjectsUsage;		//������ 2� ��������: 2) ������ ����������� ��������� CPict.Object'�� �� 1�� �������	//������������� ��� ������ � ������ initOfBallsPictObjects(), ���������� �������� � OnShow()
    std::map<CPictureObject*, bool> m_mBallsPictObjects;



//Methods(prvt):

    //#OUTDATED:    ����� ���������� CPict.Obj* '� �������, �� � ����� ������� ��������� �� ���-�� ������� ���. ������� ����� �������� ������ CPict.Obj* '�� �� �����.���� � ���������� ����, ������ ����� array �� ������.
    //void InitOfBallsPictObjects(CScene* sc);    //����� m_scene �������� ������������, �������������� ������  ..
    //                                            //.. CPict.Object'�� ���� ������� � ���� (MAX_BALL_NUM) /*���� �� 4*/
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