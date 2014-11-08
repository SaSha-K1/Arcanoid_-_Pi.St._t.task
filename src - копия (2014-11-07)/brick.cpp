
#include "brick.h"



//// C-tor ////
_BRICK::CBrick (
        CAniObject* const   brick, 
        const vector2       position
    ) 
    : m_pBrickAniObj(brick), 
    m_state(ING)
{ 
    m_pBrickAniObj->SetPos(position);
}


//#OUTDATED (заменил CLockableAccessVec):
//#if LOCKS ==1
//    char _BRICK::m_scBricksLock = 0; //static member
//
//    //// BricksLockOn() ////
//    inline bool _BRICK::BricksLockOn(const char cNewVal) 	//`inline`, т.к. может быстрее будут отрабатывать, ..
//                                                                            //.. чтобы какой-ндь race-condition не случился
//    {
//        if (0 == m_scBricksLock)
//        {
//            m_scBricksLock=cNewVal;
//            return 1;
//        }
//        else
//            return 0;
//    }
//
//    //// BricksLockOff() ////
//    inline bool _BRICK::BricksLockOff(const char oldVal) 
//    {
//        if (oldVal == m_scBricksLock)
//        {
//            m_scBricksLock=0;
//            return 1;
//        }
//        else
//            return 0;
//    }
//#endif //LOCKS