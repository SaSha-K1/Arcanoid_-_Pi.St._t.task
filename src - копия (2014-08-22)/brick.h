#ifndef BRICK_H
#define BRICK_H

#include "globals.h"
#include "aniobject.h"


class Brick
{
public:
	Brick (CAniObject *brick, vector2 position) 
		: brickAniObj_(brick), state(ING) 
	{ 
		brick->SetPos(position);
	}
	static enum BrickState {OFF=0, ING=1, DSTR_ST=2, DSTR_EN=3};	//���������: 0 - ��� ����; 1 - � ����; 2 - ������������ �������� �����������(�����); 3 - �������� ������������ ���������� (����� �������)
	BrickState state;

	//������������ ������:
	CAniObject* getBrickAniObj () { return brickAniObj_; }
#if LOCKS ==1
	static bool bricksLockOn (/**/char/**/);	//���-�� ���� �������
	static bool bricksLockOff (/**/char/**/);	//���-�� ���� �������
	static char getBricksLockVal() { return bricksLock_;}		//�������: ������� ��� ������ 
#endif//LOCKS
	
private:
	CAniObject *brickAniObj_;

#if LOCKS ==1
	static /*bool*/char bricksLock_;	//���-�� ���� �������	0 - ������; 1 - ������ isHit()-��; 2 - ������ delBrick()-��
#endif//LOCKS
};


#endif//BRICK_H