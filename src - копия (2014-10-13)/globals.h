#ifndef GLOBALS_H
#define GLOBALS_H


#include "types.h"


#define DEBUG 1

//�������
#define LOCKS 1		//1 - �������� ��� � BricksLock'���;  0 - ����.
#define LOW_LVL_LOCKS 0		//����� �������� ������, ���� LOCKS==1. ��� ����� ���������� ����� BricksLock'�� �� ����� ������������ �����, � ������ ���������� ���� ���, �������, �������� ������ ��� ��������� ��������. (0-����; 1-���)
#define NUMBER_OF_BRICKS 12	//��������:  2 - (��� > �������� ������); � 12 - ��� �������������
#define DONT_DELETE_ANI_BRICKS 1	//���������� ����� 8 ��� ������� ������� (�������� �������� � ������ ���� ���������), ����� ������� � ������ �, �� ������ ���-�������. ���� ��� ���� ����������, ����� ������ ��������� ��� ���������� (1-���; 0-����)
#define OS WIN

#define CTG(x) (tan(HALFPI - (x)))

const float HALFPI = PI/2;	//�������� ����� ������ CTG()

vector2 Vec3To2(const vector3 &v3) { return (vector2(v3.x, v3.y)); }    //���������� ���� �� ��������� � "types.h", �� ���� ���, ����� ..
                                                                        //.. �� ������������ ������� ����� ����� �� �� ���� ������.

///@@ Stoped here (2014/09/02): � ����.������� ������, ���������� ����� ������� :: ��� inaccessible - ��������� � public?;  
///����� ����� ��������� ������ `m_vpBricks` � �������� ������������� � `m_LavBricks` (� ��������� � f bricksManager.h ������ 72 ����������)
//// Types: ////
//#OUTDATED (������ � �����-��� .h �����, �.�. ����� �������� �� ���� ��������� ��� .h-�����, ��� ���� �� �������� ��������, �.�. � ��� ��� ������ ���� ���������):
//typedef CArkanoidController::CBricksManager             _BRICKS_MNGR;
//typedef CArkanoidController::CBricksManager::CBrick     _BRICK;
//typedef CLockableAccessVec<_BRICK*>::CLavHandler        _LAV_HANDLER;



#endif //GLOBALS_H

