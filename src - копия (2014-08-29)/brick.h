#ifndef BRICK_H
#define BRICK_H

#include "globals.h"
#include "aniobject.h"
#include "bricksManager.h"


    //<<< class BRICK >>>// ---------------------------------------------------------------------------------------------
class CArkanoidController::CBricksManager::CBrick
{
public:
    //C-tor
    CBrick (CAniObject *const brick, const vector2 position); 

    //Types:
    static enum BRICK_STATE {	//���������: 0 - ��� ����; 1 - � ����; 2 - ������������ �������� �����������(�����); 3 - �������� ������������ ���������� (����� �������)
        OFF=0, 
        ING=1, 
        DSTR_ST=2, 
        DSTR_EN=3
    };

    //Datas (pblc):
    BRICK_STATE m_state;

    //������������ ������:
    CAniObject* GetBrickAniObj() const { return m_pBrickAniObj; }
#if LOCKS ==1
    static bool BricksLockOn(const char);	//���-�� ���� �������
    static bool BricksLockOff(const char);	//���-�� ���� �������
    static char GetBricksLockVal() { return m_scBricksLock; }		//�������: ������� ��� ������ 
#endif//LOCKS
	
private:
    //Datas (prvt):
    CAniObject* m_pBrickAniObj;



#if LOCKS ==1
    static char m_scBricksLock;	//���-�� ���� �������	0 - ������; 1 - ������ isHit()-��; 2 - ������ delBrick()-��
#endif//LOCKS
};


#endif//BRICK_H