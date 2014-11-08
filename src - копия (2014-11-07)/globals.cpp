#include "globals.h"



///@@@ TRY to back `&`. Temporary replaced it because of WRN: C4239: nonstandard extension used : 'argument' : conversion from 'vector2' to 'vector2 &' 129
///.. возможно это связано с тем, что передаю я сюда, напр., из .GetPos() r-value, у кот.невозможно взять ссылку
vector2 Vec3To2(const vector3 /*&*/v3) 
{ 
    return (vector2(v3.x, v3.y)); 
}