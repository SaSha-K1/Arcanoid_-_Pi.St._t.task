#ifndef BRICK_H
#define BRICK_H



#include "globals.h"        //для `LOCKS`   ///@@@может убрать, если откажусь от `LOCKS`
//#include "aniobject.h"    //меняю на frwrd-decl.
#include "bricksManager.h"  ///@@@можно заменить на frwrd-decl.?

//// Forward declatations:
class CAniObject;



    //<<< class BRICK >>>// ---------------------------------------------------------------------------------------------
class CArkanoidController::CBricksManager::CBrick
{
public:
    //// C-tor:
    CBrick (
            CAniObject* const   brick,      //@@ вначале нет `const`, т.к. в C-tor'е инициализирую неконстантный объект. Понадобился бы конст_каст
            const vector2       position
        ); 

    //// Types (pblc):
    enum BRICK_STATE {	//состояние: 0 - вне игры; 1 - в игре; 2 - отыгрывается анимация уничтожения(старт); 3 - анимация униченожения отыгралась (можно удалять)
        OFF=0, 
        ING=1, 
        DSTR_ST=2, 
        DSTR_EN=3
    };

    //// Datas (pblc):
    BRICK_STATE m_state;

    //// Интерфейсные методы:
    CAniObject* GetBrickAniObj() const { return m_pBrickAniObj; }   ///@@@верно ли тут `const`? (хотя вроде в самом методе объект данн.класса не меняется)

//#OUTDATED (заменил CLockableAccessVec):
//#if LOCKS ==1
//    static bool BricksLockOn(const char);	//что-то типа мютекса
//    static bool BricksLockOff(const char);	//что-то типа мютекса
//    static char GetBricksLockVal() { return m_scBricksLock; }		//Костыли: добавил эту строку 
//#endif //LOCKS
//#if LOCKS ==1
//private:
//    static char m_scBricksLock;	//что-то типа мютекса	0 - открыт; 1 - закрыт IsHit()-ом; 2 - закрыт DelBrick()-ом     ///@@@ #UPD: кем закрыт, см. в f. "lockableAccessVec.h"
//#endif //LOCKS
	
private:
    //// Datas (prvt):
    CAniObject* m_pBrickAniObj;

};

//typedef CArkanoidController::CBricksManager::CBrick     _BRICK;   ///@@@ Тут ругался КОМП-р на то, что `CArkanoidController::CBricksManager` - undefined type, хотя .h подключен - разобраться, wtf?
typedef _BRICKS_MNGR::CBrick _BRICK;


#endif //BRICK_H