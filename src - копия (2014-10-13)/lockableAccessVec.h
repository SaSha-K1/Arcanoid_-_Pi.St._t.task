///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  lockableAccessVec.h                                                      //
//  authors: Sasha Kislun                                                    //
//  2014, Test task                                                          //
//                                                                           //
//    Класс CLockableAccessVec и вложенный в него CLavHandler использую для  //
//  предоставления доступа к вектору объектов кл. СBrick, блокируя при этом  //
//  возможность получения параллельного доступа к нему в др.потоке.          //
//  Необходимо это, т.к. возможно (я предполагаю) ситуация, когда в методе   //
//  IsHit() этот вектор используется для перебора его эл-тов при поиске      //
//  контакта, а в это же время из этого же вектора в др.потоке удаляется     //
//  элемент в методе DelBrick(). Что приведёт к UB.                          //
//    Вложенный класс CLavHandler есть реализация идиомы RAII для объекта    //
//  "Доступ к вектору с блокировкой возможности параллельного доступа".      //
//    Класс CLockableAccessVec служит обёрткой для указателя на вектор       //
//  кирпичей (объектов кл. СBrick), и позволяет безопасно хранить этот       //
//  вектор во вмещающем объекте CBricksManager, обеспечивая возможность      //
//  доступа к вектору только ч/з безопасный метод с блокировкой параллель-   //
//  -ного доступа. Кроме того класс-обёртка CLockableAccessVec позволяет     //
//  хранить значение ключа, с которым был получен доступ к вектору, необхо-  //
//  димое для разблокировки вектора и закрытии текущего сеанса доступа.      //     ///@@@ Если добиться того, чтобы отпала необходимость в идентификации получателя доступа, то хранить ключ будет не нужно (сам ключ будет не нужен). И тогда вместо класса-обёртки, возможно, можно попробовать использовать статич.фабричную ф-цию того же класса CLavHandler.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  (2014/X/13): 
//      Qstn: Почему нужен кл. "Lav" (почему недостаточно просто ф-ции       //
//  Get() со стат.полем, хранящем тек. ключ доступа (т.е. зачем понадобился  //
//  ещё влож-й кл. "LavHndlr")? - 
//      Потому, что нужно ptr на объ хранить в кл.-обёртке, иначе объ смогут //
//  использовать напрямую, не заморачиваясь вызовом ф-ции Get(), реализующей //
//  блокировку параллельного доступа к объ-ту.
//                                                                           //
//      Qstn: Почему не ограничиться одним классом "Lav", кот-й бы по вызову //
//  метода .GetObjPtr(), выдавал бы (после проверки вып-ния усл-я блокиров-  //
//  -ки) ptr на оберегаемый объ?
//      Для того, чтобы Re-ть идиому RAII. Что, в свою оч., нужно для того,  //
//  чтобы блокировка доступа к объ снималась (т.е. обнулялся хранимый тек.   //
//  ключ доступа) автоматич-ки при выходе объекта из scope.
//                                                                           //
//  Возможно, эту мою конструкцию можно было бы заменить указ-лем на ф-цию,  //
//  возвращающую LavHndlr объ и статич-ким private членом - ptr'ом на охра-  //
//  -няемый объ...
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef LOCKABLE_ACCESS_VEC_H
#define LOCKABLE_ACCESS_VEC_H



#include <vector>
#include "globals.h"
#include "types.h"
#include "brick.h"



template <typename T>
class CLockableAccessVec 
{
public:
    //// C-tor:
    CLockableAccessVec(std::vector<T>* pVec)
        : m_pVec(pVec),
        m_uLockCode(0)
    {
    }

    //// Types (pblc):
    class CLavHandler;

    //// Methods (pblc):
    std::auto_ptr<CLavHandler> CreateAccessHandler(const u32 key) /*const*/;   // `key` - is u32 value unique for each function, and! must be >0.
                                                                    //#UPD2: upd правее не верный (отбой) #UPD: !! key==0 - тогда доступ без блокировки.
                                                                    //"1" - RetrieveBrick() in f. "bricksManager.h"
                                                                    //"2" - CArkanoidController::CBallsManager::CBall::IsHitWithBricks() in f. "ball.cpp"
                                                                    //"3" - CArkanoidController::CBricksManager::AddBrick() in f. "bricksManager.cpp"
                                                                    //"4" - CArkanoidController::CBricksManager::DelBrick() in f. "bricksManager.cpp"
                                                                    //"5" - CArkanoidController::CBricksManager::InitOfBricksAniObjects() in f. "bricksManager.cpp"
                                                                    //"6" - CArkanoidController::ProcessWinLostState() in f. "controller.cpp"   //only in DEBUG
                                                                    //"7" - CArkanoidController::OnUpdate() in f. "controller.cpp" (вызываю CBricksManager::CleanBricks())

private:
    //Запрет копирования и присваивания:
    CLockableAccessVec (const CLockableAccessVec&);
    CLockableAccessVec& operator= (const CLockableAccessVec&);

    //// Types (prvt):
    ///@@@? #OUTDATED (вынужден перенести выше - над 1-вым упоминанием в секции `public:`, т.к. КОМП-р ругается, что `udefined`. @@@?: Позже разобраться, т.к. вроде ж должен в пределах класса видеть.):
    //class CLavHandler;  //Lockable Access Vector Handler

    //// Datas (prvt):
    std::vector<T>* m_pVec;
    u32 m_uLockCode;    //Тек. знач. переменной, хранящей код блокировки доступа в обслуживаемом объекте CLockableAccessObj
};



//@@ Stoped here (2014-08-30)
template <typename T>
class CLockableAccessVec<T>::CLavHandler    //Lockable Access Vector Handler
{
public:
    //// C-tor:
    CLavHandler(
            const u32 key,
            std::vector<T>* pVec,       ///@@@? т.к. класс зависим от типа шаблона, может его нужно как-то объявлять иначе, нежели как простой класс, вложенный в шаблонный (напр., может тоже шаблоном?)
            u32& rLavLockCode
        );

    //// D-tor:
    ~CLavHandler();


    //// Methods (pblc):
    std::vector<T>* operator ->() const   { return m_pVec; }   ///@@@? вроде работает не так, как я ожидал: pLavHndlrObj-> не даёт доступ к полям охраняемого объ-та ...
    std::vector<T>& operator *()  const   { return *m_pVec; }  
#if DEBUG==1
    u32 GetCurrValOfLockCode() const    { return u32 result = m_ruLavLockCode; }    ///@@@ #WARNING: так можно??))
    u32 GetReservedKey() const  { return m_uResKey; }
#endif //DEBUG==1

    //#OUTDATED (вместо этих методов/операторов, переопределю просто опер-ры `->` и разыменования `*`): 
    //std::vector<T1>::size_type          size() const    { return m_pVec->size();    }    ///@@@ #WARNING: возможно не пройдёт <int> вместо <CBrick*>
    //std::vector<T1>::iterator           begin()         { return m_pVec->begin();   }     ///@@@ #WARNING: есть перегрузка const, возвр-щая const_iterator
    //std::vector<T1>::iterator           end()           { return m_pVec->end();     }
    //std::vector<T1>::reverse_iterator   rbegin()        { return m_pVec->rbegin();  }   //добавляю на всякий случ.
    //std::vector<T1>::reverse_iterator   rend()          { return m_pVec->rend();    }   //добавляю на всякий случ.
    //bool                                empty()         { return m_pVec->empty();   }
    //std::vector<T1>::iterator           erase(std::vector<T1>::const_iterator pos)  { return m_pVec->erase(pos);    }
    //std::vector<T1>::iterator           erase(
    //                                            std::vector<T1>::const_iterator first,
    //                                            std::vector<T1>::const_iterator last
    //                                        )  
    //                                    { return m_pVec->erase(first, last);    }
    //...
    //operator [] () ... 
    //back() ... 
    //push_back() ... 


private:
    //запрет копирования и присваивания:
    CLavHandler(const CLavHandler&);
    CLavHandler& operator=(const CLavHandler&);


    //// Datas (prvt):
    std::vector<T>* m_pVec;
    u32& m_ruLavLockCode;    //Ссылка на переменную, хранящую код блокировки доступа в обслуживаемом объекте CLockableAccessObj
#if DEBUG==1
    const u32 m_uResKey;    //резервная копия ключа для проерки при снятии блокировки в D-tor'е
#endif

};

//typedef CLockableAccessVec<_BRICK*>::CLavHandler    _LAV_HANDLER;
typedef /**/typename/**/ CLockableAccessVec<_BRICK*>::CLavHandler _LAV_OFBRICKS_HANDLER;



#endif //LOCKABLE_ACCESS_VEC_H


///#NOTE:
    /// "...  if ( _BRICK::DSTR_ST == (*(*(*ppLavHndlr)))[i]->m_state )  ..."
    //#NOTE: 
    //тут   1й (*..) - от ptr'а на `auto_ptr` переходим к `auto_ptr`
    //      2й (*..) - от `auto_ptr` переходим к _LAV_HANDLER
    //      3й (*..) - т.к. в _LAV_HANDLER опер-ры `*` и `->` перегружены, то тут от _LAV_HANDLER переходим сразу к std::vector<_BRICK*> ..
    //.. ptr, на который есть member data класса.
    //      ну и [i] - уже обращаеся к эл-ту вектора типа _BRICK*
    ///.. конечно, это жесть!
