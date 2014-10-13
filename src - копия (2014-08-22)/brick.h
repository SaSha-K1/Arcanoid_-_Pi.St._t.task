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
	static enum BrickState {OFF=0, ING=1, DSTR_ST=2, DSTR_EN=3};	//состояние: 0 - вне игры; 1 - в игре; 2 - отыгрывается анимация уничтожения(старт); 3 - анимация униченожения отыгралась (можно удалять)
	BrickState state;

	//интерфейсные методы:
	CAniObject* getBrickAniObj () { return brickAniObj_; }
#if LOCKS ==1
	static bool bricksLockOn (/**/char/**/);	//что-то типа мютекса
	static bool bricksLockOff (/**/char/**/);	//что-то типа мютекса
	static char getBricksLockVal() { return bricksLock_;}		//Костыли: добавил эту строку 
#endif//LOCKS
	
private:
	CAniObject *brickAniObj_;

#if LOCKS ==1
	static /*bool*/char bricksLock_;	//что-то типа мютекса	0 - открыт; 1 - закрыт isHit()-ом; 2 - закрыт delBrick()-ом
#endif//LOCKS
};


#endif//BRICK_H