#include "globals.h"



///@@@ TRY to back `&`. Temporary replaced it because of WRN: C4239: nonstandard extension used : 'argument' : conversion from 'vector2' to 'vector2 &' 129
///.. �������� ��� ������� � ���, ��� ������� � ����, ����., �� .GetPos() r-value, � ���.���������� ����� ������
vector2 Vec3To2(const vector3 /*&*/v3) 
{ 
    return (vector2(v3.x, v3.y)); 
}