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
//  элемент в методе CleanBricks(). Что приведёт к UB.                       //
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
//
//    Не могу обойтись одним классом и ptr'ом на статич. фабричный метод,    //
//  т.к. мне где-то нужно содержать ptr на данные, но без возможности к ним  //
//  обратиться напрямую. Поэтому вынужнден иметь объект-обёртку для этого    //
//  ptr'а на данные и др. класс, реализующий RAII (именно другой, а не в     //
//  этом же классе, т.к. специфические C-tor и D-tor для RAII).
//    Ну а коль уж вынужден иметь отдельный класс-обёртку для ptr'а, то и    //
//  фабричный метод реализую тоже в нём.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LOCKABLE_ACCESS_VEC_H
#define LOCKABLE_ACCESS_VEC_H



#include <vector>
#include <memory>   //for unique_ptr
#include "globals.h"
#include "types.h"  //можно выключить, пока включён "globals.h", содержащий это инклуд.
//#include "brick.h"    ///@@@ Зачем нужен этот инклуд? (для _BRICK* вряд ли). Да и этот typedef тут некместу - переношу туда, где юзаю
#include "myExcptn.h"   //д/ cpp-части



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
    class CLavHandler;  //! В public - т.к. принимаю вовне unique_ptr, параметризированный этим типом; ..
                        //!.. в bricksManager.cpp объявляю typedef, используя этот тип.

    //// Methods (pblc):
    std::unique_ptr<CLavHandler> CreateAccessHandler(const u32 key) /*const*/;   // `key` - is u32 value unique for each function, and! must be >0.
                                                                    //#UPD2: upd правее не верный (отбой) #UPD: !! key==0 - тогда доступ без блокировки.
                                                                    //"1" - RetrieveBrick() in f. "bricksManager.h"
                                                                    //"2" - CArkanoidController::CBallsManager::CBall::IsHitWithBricks() in f. "ball.cpp"
                                                                    //"3" - CArkanoidController::CBricksManager::AddBrick() in f. "bricksManager.cpp"
                                                                    //#OLD: //"4" - CArkanoidController::CBricksManager::DelBrick() in f. "bricksManager.cpp"
                                                                    //"5" - CArkanoidController::CBricksManager::InitOfBricksAniObjects() in f. "bricksManager.cpp"
                                                                    //"6" - _BRICKS_MNGR::IsBricksEmptyCheck() called frm CArkanoidController::ProcessWinLostState() in f. "controller.cpp"   //only in DEBUG
                                                                    //"7" - CArkanoidController::OnUpdate() in f. "controller.cpp" (вызываю CBricksManager::CleanBricks())
    u32 ReleaseAccessHandler(std::unique_ptr<CLavHandler>/**/& apLavHndlr);    //! Принимаю параметром, а не храню копию, чтобы не следить за тем, чтобы ..
                                                                        //!.. obj или ptr не удалили снаружи непосредственно (в этом случае тут ..
                                                                        //!.. остался бы невалидный ptr или ptr на несущестующий obj).

private:
    //Запрет копирования и присваивания:
    CLockableAccessVec (const CLockableAccessVec&);
    CLockableAccessVec& operator= (const CLockableAccessVec&);


    //// Datas (prvt):
    std::vector<T>*             m_pVec;
    u32                         m_uLockCode;    //! Тек. знач. кода блокировки доступа в обслуживаемом объекте CLockableAccessObj ..
                                                //!.. (с которым был получен доступ)
                                                //! 0 - означает, что в дан.мом. доступ ни для кого не открыт.
    //#DEADLOCK_BRANCH:
    //std::auto_ptr<CLavHandler>* m_apLavHndlr; //! Ptr на auto_ptr, возвращённый
};



//@@ Stoped here (2014-08-30)
///@@@@@ STOPED_HERE (2014-11-01): что-то кажется тут не так. См. как работает IsHitWithBricks() в ball.cpp. Мы вроде в Д-торе тут должны снимать замок. Ну и наверное, чтобы не дожидаться Д-тора метод "Анлок()" какой-то добавить, или просто удалять delet'ом pLavHndlr из метода клиента. {засыпаю уже - голова не варит, но подумать...}
template <typename T>
class CLockableAccessVec<T>::CLavHandler    //Lockable Access Vector Handler
{
public:
    //// C-tor:
    CLavHandler(
            const u32       key,
            std::vector<T>* pVec,       ///@@@? т.к. класс зависим от типа шаблона, может его нужно как-то объявлять иначе, нежели как простой класс, вложенный в шаблонный (напр., может тоже шаблоном?)
            u32&            rLavLockCode
        );

    //// D-tor:
    ~CLavHandler();


    //#WRONG:
    //// Methods (pblc):
    //u32 ReleaseLav();


    //// Operators (pblc):
    std::vector<T>* operator ->() const   { return m_pVec; }   ///@@@? вроде работает не так, как я ожидал: pLavHndlrObj-> не даёт доступ к полям охраняемого объ-та ...
    std::vector<T>& operator *()  const   { return *m_pVec; }  
#if DEBUG==1
    u32 GetCurrValOfLockCode() const { u32 result = m_ruLavLockCode; return result; }   
        ///@@@ #TRY_IT: { return u32 result = m_ruLavLockCode; }    ///@@@ #WARNING: так можно??))
    u32 GetReservedKey() const  { return m_uResKey; }
#endif //DEBUG==1


private:
    //запрет копирования и присваивания:
    CLavHandler(const CLavHandler&);
    CLavHandler& operator=(const CLavHandler&);


    //// Datas (prvt):
    std::vector<T>* m_pVec;
    u32&            m_ruLavLockCode;    //Ссылка на переменную, хранящую код блокировки доступа в обслуживаемом объекте CLockableAccessObj
#if DEBUG==1
    const u32       m_uResKey;    //резервная копия ключа для проерки при снятии блокировки в D-tor'е
#endif

};



///////////////////////////////////////////////
//// То, что было до озарения в CPP-файле: ////
///////////////////////////////////////////////

    //// class - CLockableAccessVec ////
//// CreateAccessHandler() ////
template <typename T>
///@@@#TO_DO: переписать с устаревшего `auto_ptr` на ск.вс. `unique_ptr`
//std::auto_ptr</**/CLockableAccessVec<T>::/**/CLavHandler> CLockableAccessVec<T>::CreateAccessHandler(const u32 key) ///@@@ тут можно попробовать return-type задать с пом. суффикс-синтаксиса: (что-то вроде "auto ...() -> ...")
auto CLockableAccessVec<T>::CreateAccessHandler(const u32 key) /*const*/   -> std::unique_ptr<CLavHandler>    //Suffix return type syntax использую, т.к. в кач-ве шаблонного пар-ра unique_ptr отказывался принимать конструкцию  " CLockableAccessVec<T>::CLavHandler " . Видимо, из-за наличия ещё одного шаблонного пар-ра.   ///@@@ #WARNING: d правильном ли месте поставил 'const' метода?
{
    //#OUTDATED:
    //if (0 == m_uLockCode)
    //    return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@ #WARNING: возможно тут нужно явно инициализировать auto_ptr
    //else
    //    return nullptr;     ///@@@ #WARNING: возможно тут нужно  /* static_cast<CLaoHandle*>(nullptr); */

    while (0 != m_uLockCode)
    {
#if OS==WIN //т.к. ф-ция `_sleep()` is Windows specific. В Unix, кажется `Sleep()`.
  
  #if HARD_DEBUG==1
        time_t currT = time(NULL);
        exceptPrintFile << asctime(localtime(&currT))<< "!!!: _sleep() starts in CreateAccessHandler(). m_uLockCode == "
            << m_uLockCode<< std::endl;
  #endif //HARD_DEBUG==1

        _sleep(10); ///@@@ #WARNING: 1) возможно, не пауза этого потока, а останов всей программы, 2) может грузит ЦП, 3) может в сек, а не в милисек.
                    ///@@@? IntlelliSense грозился, что `_sleep()` устаревшая - рекомендует `Sleep()`. So poss-ly try.
#else //OS=="WIN"
        Sleep(10);  //In Unix it seems..
#endif //OS=="WIN"
    }
    //return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@? так видимо нельзя, т.к. к-тор создаёт вроде лок. объ    
    
    //#XI:
    //CLavHandler* tmpCLavHndlrPtr = new CLavHandler(key, m_pVec, m_uLockCode);
    //return *(new std::auto_ptr<CLavHandler> (/* * */tmpCLavHndlrPtr));   //Without `*` I get err: C2664: 'std::auto_ptr<_Ty>::auto_ptr<CLockableAccessVec<T>::CLavHandler>(std::auto_ptr<_Ty> &) throw()' : cannot convert parameter 1 from 'std::auto_ptr<_Ty> *' to 'std::auto_ptr<_Ty> &' 186

    //return *( new std::unique_ptr<CLavHandler> (new CLavHandler(key, m_pVec, m_uLockCode)) ); //Without `*` I get err: C2664: 'std::auto_ptr<_Ty>::auto_ptr<CLockableAccessVec<T>::CLavHandler>(std::auto_ptr<_Ty> &) throw()' : cannot convert parameter 1 from 'std::auto_ptr<_Ty> *' to 'std::auto_ptr<_Ty> &' 186
                                                                                            //Тут возвращаю unique_ptr а не raw (который снаружи уже просто схватить unique_ptr'ом), т.к. не уверен, что снаружи всегда будут принимать правильно. А если нет, то memory leaks!
    return std::move( *(new unique_ptr<CLavHandler>(new CLavHandler(key, m_pVec, m_uLockCode)) ) );
    ///@@@#EXP: try тут auto_ptr создавать без `new` - интересно, доживёт он как локальный объ до того мом., когда его примут снаружи?

///@@@ #TRY_IT(instead of upper couple of strings):    return /**/*(/**/ new /**/std::auto_ptr<CLavHandler>/**/ /* * */(new CLavHandler(key, m_pVec, m_uLockCode))/**/)/**/;    ///@@@ #WARNING: возможно тут нужно явно инициализировать auto_ptr
}                                                                                           ///@@@? х.е.з., прокатит ли разыменованный `new`. Если нет, то см.в EXP-проект. Там я вроде ч/з tmp-var сделал


//// ReleaseAccessHandler() ////
template <typename T>
u32 CLockableAccessVec<T>::ReleaseAccessHandler(std::unique_ptr<CLavHandler>/**/& apLavHndlr)
{
    apLavHndlr.reset();
    
    if (m_uLockCode == 0)
        return 0;
    else
        return m_uLockCode;
}


    //// class - CLockableAccessVec:: CLavHandler ////
//// C-tor ////
template <typename T>
CLockableAccessVec<T>::CLavHandler::CLavHandler(
        const u32       key,  
        std::vector<T>* pVec,
        u32&            rLavLockCode
    )
    : m_pVec(pVec),
    m_ruLavLockCode(rLavLockCode)

#if DEBUG==1
    , m_uResKey(key)
{
    if (0 != rLavLockCode) throw CMyExcptn(24); ///@@@? if  `CLockableAccessVec<T>::m_uLockCode`  присваивается снаружи к-тора Hndlr'а, то эта проверка m!b. наоборот - "if(0==.."
#else
{
#endif //DEBUG==1

//#OUTDATED (для наружного цикла вокруг DelBrick() в CArkanoidController::OnUpdate() допускаю возможность key==0 - тогда доступ без блокировки):
    //#OLD: #UPD: отбой. Решил по-другому обходить - передавать в DelBrick() необяз-ным посл-ним пар-ром obj std::auto_ptr<_LAV_OFBRICKS_HNDLR>.
#if DEBUG==1
    if (0 == key)
        throw CMyExcptn(20);
    //if (0 == m_ruLavLockCode) ///@@@? эта проверка вроде === CMyExcptn(24).  Если так, то согласовать их! (оставить обе возможно чтобы 2жды: чуть раньше и чуть позже проверить условие, не влез ли кто)
    if (0 != m_ruLavLockCode) //was "0==.."  ///@@@? эта проверка вроде === CMyExcptn(24).  Если так, то согласовать их! (оставить обе возможно чтобы 2жды: чуть раньше и чуть позже проверить условие, не влез ли кто)        
        throw CMyExcptn(18);
#endif
    m_ruLavLockCode = key;
}


//// D-tor ////
template <typename T>
CLockableAccessVec<T>::CLavHandler/*<T>*/::~CLavHandler()   ///*
{
#if DEBUG==1
    if (m_uResKey != m_ruLavLockCode) throw CMyExcptn(19);  //это возможно, если параллельный доступ #upd: возможность которого пока отменяю 
    else
#endif
        m_ruLavLockCode = 0;
}



#endif //LOCKABLE_ACCESS_VEC_H


///#NOTE:
    /// "...  if ( _BRICK::DSTR_ST == (*(*(*ppLavHndlr)))[i]->m_state )  ..."
    //#NOTE: 
    //тут   1й (*..) - от ptr'а на `unique_ptr` переходим к `unique_ptr`
    //      2й (*..) - от `unique_ptr` переходим к _LAV_OFBRICKS_HNDLR
    //      3й (*..) - т.к. в _LAV_OFBRICKS_HNDLR опер-ры `*` и `->` перегружены, то тут от _LAV_OFBRICKS_HNDLR переходим сразу ..
    //.. к std::vector<_BRICK*>, ptr на который есть member data класса.
    //      ну и [i] - уже обращаеся к эл-ту вектора типа _BRICK*
    ///.. конечно, это жесть!
