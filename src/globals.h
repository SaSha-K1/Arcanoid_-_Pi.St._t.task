#ifndef GLOBALS_H
#define GLOBALS_H


#include "types.h"


#define DEBUG 1
#define HARD_DEBUG 1    //��������� ���.�������� � ����������� ������ � ���.
#define SEARCH_4_VEC_ERR 2  ///@@@ ����� �����, ������ � ��� � ��� ���� "#if SEARCH_4_VEC_ERR==1", � ����� ��������� �� ������� ����� search4VecErr.h � .cpp
                            // "2" - ��������� �������� �/� ���������� OnUpdate() � OnRender()

//�������
#define LOCKS 1		//1 - �������� ��� � BricksLock'���;  0 - ����.
#define LOW_LVL_LOCKS 0		//����� �������� ������, ���� LOCKS==1. ��� ����� ���������� ����� BricksLock'�� �� ����� ������������ �����, � ������ ���������� ���� ���, �������, �������� ������ ��� ��������� ��������. (0-����; 1-���)
#define NUMBER_OF_BRICKS 12	//��������:  2 - (��� > �������� ������); � 12 - ��� �������������
#define DONT_DELETE_ANI_BRICKS 1	//���������� ����� 8 ��� ������� ������� (�������� �������� � ������ ���� ���������), ����� ������� � ������ �, �� ������ ���-�������. ���� ��� ���� ����������, ����� ������ ��������� ��� ���������� (1-���; 0-����)
#define OS WIN

#define CTG(x) (tan(HALFPI - (x)))

const float HALFPI = PI/2;	//�������� ����� ������ CTG()

///@@@ TRY to back `&`. Temporary replaced it because of WRN: C4239: nonstandard extension used : 'argument' : conversion from 'vector2' to 'vector2 &' 129
vector2 Vec3To2(const vector3 /*&*/v3);     //���������� ���� �� ��������� � "types.h", �� ���� ���, ����� ..
                                            //.. �� ������������ ������� ����� ����� �� �� ���� ������.

///@@ Stoped here (2014/09/02): � ����.������� ������, ���������� ����� ������� :: ��� inaccessible - ��������� � public?;  
///����� ����� ��������� ������ `m_vpBricks` � �������� ������������� � `m_LavBricks` (� ��������� � f bricksManager.h ������ 72 ����������)
//// Types: ////
//#OUTDATED (������ � �����-��� .h �����, �.�. ����� �������� �� ���� ��������� ��� .h-�����, ��� ���� �� �������� ��������, �.�. � ��� ��� ������ ���� ���������):
//typedef CArkanoidController::CBricksManager             _BRICKS_MNGR;
//typedef CArkanoidController::CBricksManager::CBrick     _BRICK;
//typedef CLockableAccessVec<_BRICK*>::CLavHandler        _LAV_OFBRICKS_HNDLR;



#endif //GLOBALS_H

