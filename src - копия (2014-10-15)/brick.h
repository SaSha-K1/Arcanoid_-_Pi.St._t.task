#ifndef BRICK_H
#define BRICK_H



#include "globals.h"        //��� `LOCKS`   ///@@@����� ������, ���� �������� �� `LOCKS`
//#include "aniobject.h"    //����� �� frwrd-decl.
#include "bricksManager.h"  ///@@@����� �������� �� frwrd-decl.?

//// Forward declatations:
class CAniObject;



    //<<< class BRICK >>>// ---------------------------------------------------------------------------------------------
class CArkanoidController::CBricksManager::CBrick
{
public:
    //// C-tor:
    CBrick (
            CAniObject* const   brick,      //@@ ������� ��� `const`, �.�. � C-tor'� ������������� ������������� ������. ����������� �� �����_����
            const vector2       position
        ); 

    //// Types (pblc):
    enum BRICK_STATE {	//���������: 0 - ��� ����; 1 - � ����; 2 - ������������ �������� �����������(�����); 3 - �������� ������������ ���������� (����� �������)
        OFF=0, 
        ING=1, 
        DSTR_ST=2, 
        DSTR_EN=3
    };

    //// Datas (pblc):
    BRICK_STATE m_state;

    //// ������������ ������:
    CAniObject* GetBrickAniObj() const { return m_pBrickAniObj; }   ///@@@����� �� ��� `const`? (���� ����� � ����� ������ ������ ����.������ �� ��������)

//#OUTDATED (������� CLockableAccessVec):
//#if LOCKS ==1
//    static bool BricksLockOn(const char);	//���-�� ���� �������
//    static bool BricksLockOff(const char);	//���-�� ���� �������
//    static char GetBricksLockVal() { return m_scBricksLock; }		//�������: ������� ��� ������ 
//#endif //LOCKS
//#if LOCKS ==1
//private:
//    static char m_scBricksLock;	//���-�� ���� �������	0 - ������; 1 - ������ IsHit()-��; 2 - ������ DelBrick()-��     ///@@@ #UPD: ��� ������, ��. � f. "lockableAccessVec.h"
//#endif //LOCKS
	
private:
    //// Datas (prvt):
    CAniObject* m_pBrickAniObj;

};

//typedef CArkanoidController::CBricksManager::CBrick     _BRICK;   ///@@@ ��� ������� ����-� �� ��, ��� `CArkanoidController::CBricksManager` - undefined type, ���� .h ��������� - �����������, wtf?
typedef _BRICKS_MNGR::CBrick _BRICK;


#endif //BRICK_H