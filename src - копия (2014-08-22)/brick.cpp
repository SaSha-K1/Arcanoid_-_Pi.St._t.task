
#include "brick.h"

#if LOCKS ==1
/*bool*/char Brick::bricksLock_=0;	//static member

inline bool Brick::bricksLockOn (/**/char newVal/**/) 	//`inline`, т.к. может быстрее будут отрабатывать, чтобы какой-ндь race-condition не случился
{
	if (bricksLock_==0)
	{
		bricksLock_=/*1*/newVal;
		return 1;
	}
	else
		return 0;
}

inline bool Brick::bricksLockOff (/**/char oldVal/**/) 
{
	if (bricksLock_==/*1*/oldVal)
	{
		bricksLock_=0;
		return 1;
	}
	else
		return 0;
}
#endif//LOCKS