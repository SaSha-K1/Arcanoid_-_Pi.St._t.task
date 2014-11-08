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



//#include <array>
#include <vector>
#include "types.h"      //��� u32
//#include "pictureobj.h"
#include "controller.h" //����., �/  " CArkanoidController::GAME_STATE&    gameState ..."
#include "lockableAccessVec.h"  //�������� �������� � ProcessIsHit()
#include "dbgPrint.h"   //�������� �-��� ��������
#include "brick.h"      ///@@@!! ����� ���� �� ���, ���� �� ��� C++0x. � ���� ��������, �.�. �� �������� frwrd-decl. ����� `typename` (����� ���������):  //typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED! ///@@@! But not allowed before C++0x.  ---  ����! ������� ��������� ����.h =((


//// Forward declatations:
class CPictureObject;
//class CArkanoidController::CBricksManager;    //�� �����, �.�. ���:  #include "controller.h" 
//class CArkanoidController::CBricksManager::CBrick;  ///@@@ Nestsed class Frwrd-decl.-tion is NOT ALLOWED!
//typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED! ///@@@! But not allowed before C++0x.  ---  ����! ������� ��������� ����.h =((
//typedef CArkanoidController::CBricksManager::CBrick _BRICK; //No need more because of forced include of brick.h (see string above)



class CArkanoidController::CBallsManager /* : public CSingleton<CArkanoidController::CBallsManager> */
{
public:
    //// C-tor:
    CBallsManager(
            //#OUTDATED (� cpp �� ����. ��� ������-�� ����. ������ ��������):     CPictureObject*                                     pBallPicObj,
            
            //#OUTDATED (array's size is static. ������� ���� vector): 
            //std::array<CPictureObject*, 0U/*m_uMAX_BALL_NUM*/>&    raBallsPictObjects   ///#WARNING: ��������� �� "0U"..? ANSWR: ���
            const std::vector<CPictureObject*>&    rvBallsPictObjects
        );

    //Methods (pblc):
    void AddBall(   //������ ����� � ����	//CPictureObject'� ��� ��� 3� ������� ������� � xml � � obj.h, �� ����� �� �� ��������� �������� ���.����, ���� �� �����. �����.���������� �������� � xml, ������� ��� �� �����
            //#OUTDATED: std::array<vector2, m_suNUM_OF_HIT_CHCK_PNTS>&  ballHitChckPnts, 
            const bool                  attachToPad,
            /*const*/ CPictureObject* const pPad    //@@ ������ `const`, �.�. ������� � ����� ������ � CBall::LocateAttachedToPadBall(), � ��� ������� ������������� (������-��) ������ GetCenter()
        );     

    void DelBall(
            const u32                           i,          //������ ���������� ������ � m_vBalls
            CArkanoidController::GAME_STATE&    rGameState
        );	//������� ����� �� ����     //was (std::vector<CBall*>::iterator it)

    void CleanBalls(    //������� ������� ������ CBall ���������� �� ������� ���� �������
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

    void BallThrowIn();     //������ ������, ������������� � Pad'� �� ����� ����

    void CleanBalls(CArkanoidController::GAME_STATE& gameState);    //�������� ������ ������� ��� ������ (� ����. ��������� ��������� ��� �������� ���� ������ ������ �������)

#if DEBUG==1
    float GetBallFi (const u32 ballNum) const;  //I need this method only for using in `DbgPrint()` in `OnRender()`
#endif //DEBUG


    //// Types (pblc):
    class CBall;    //forward declaration


    //// Friends:
#if DEBUG==1
    friend void CArkanoidController::OnRender(vector2);
    
    ///@@@? � ����� �� ��������� ��� �-��� ��������? �������� ���, ���� ��� ����� ������ � �������� ����� ������ � ������ ���-���?
    friend void dbgPrnt::DbgPrint(
            const std::string &txt, 
            const u32 y,    const u32 x/*=10*/,                                        //����������
            const u32 r/*=1*/, const u32 g/*=255*/, const u32 b/*=255*/, const u32 a/*=100*/     //����
        );														
    template <typename T> 
    friend void dbgPrnt::DbgPrint/*<T>/**/(
        const std::string &txt,
        const T &var,       ///@@@ ���� ������ ����� ��������, ����� �� �-�� DbgPrint() �����������, �� ����������� ��� ��������� ptr �� var-�� - ����� ��� ���������� - ��������� ����� ���������� �-� ����������
        const u32 y,    const u32 x/*=10*/,                                     //����������
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //����
    );
    template <typename T1, typename T2> 
    friend void DbgPrint/*<T1, T2>/**/(
        const std::string &txt,
        const T1 &var1, 
        const T2 &var2, 
        const std::string &delim, 
        const u32 y, const u32 x/*=10*/,                                        //����������
        const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/     //����
    );				
#endif //DEBUG





private:
    //// Datas (prvt):
    const u32                       m_uMAX_BALL_NUM;    // = 4 (���������� � C-tor'�)
    std::vector<CBall*>             m_vBalls;           //������ ������� ?, ������� �� ����.���.� ����

    ///@@@ Convert to map
    //#OUTDATED:
    //std::array<CPictureObject*, m_uMAX_BALL_NUM>&  m_aBallsPictObjects;	//������ 2� ��������: 1) ������ CPict.Object'�� ���� 5 (m_uMAX_BALL_NUM) �������
    //std::array<bool, m_uMAX_BALL_NUM>              m_aBallsPictObjectsUsage;		//������ 2� ��������: 2) ������ ����������� ��������� CPict.Object'�� �� 1�� �������	//������������� ��� ������ � ������ initOfBallsPictObjects(), ���������� �������� � OnShow()
    std::map<CPictureObject*, bool> m_mBallsPictObjects;



//Methods(prvt):

    //#OUTDATED:    ����� ���������� CPict.Obj* '� �������, �� � ����� ������� ��������� �� ���-�� ������� ���. ������� ����� �������� ������ CPict.Obj* '�� �� �����.���� � ���������� ����, ������ ����� array �� ������.
    //void InitOfBallsPictObjects(CScene* sc);    //����� m_scene �������� ������������, �������������� ������  ..
    //                                            //.. CPict.Object'�� ���� ������� � ���� (m_uMAX_BALL_NUM) /*���� �� 4*/
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