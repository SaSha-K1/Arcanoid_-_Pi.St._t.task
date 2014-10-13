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
    static enum BRICK_STATE {	//состояние: 0 - вне игры; 1 - в игре; 2 - отыгрывается анимация уничтожения(старт); 3 - анимация униченожения отыгралась (можно удалять)
        OFF=0, 
        ING=1, 
        DSTR_ST=2, 
        DSTR_EN=3
    };

    //Datas (pblc):
    BRICK_STATE m_state;

    //Интерфейсные методы:
    CAniObject* GetBrickAniObj() const { return m_pBrickAniObj; }
#if LOCKS ==1
    static bool BricksLockOn(const char);	//что-то типа мютекса
    static bool BricksLockOff(const char);	//что-то типа мютекса
    static char GetBricksLockVal() { return m_scBricksLock; }		//Костыли: добавил эту строку 
#endif//LOCKS
	
private:
    //Datas (prvt):
    CAniObject* m_pBrickAniObj;



#if LOCKS ==1
    static char m_scBricksLock;	//что-то типа мютекса	0 - открыт; 1 - закрыт isHit()-ом; 2 - закрыт delBrick()-ом
#endif//LOCKS
};


#endif//BRICK_H