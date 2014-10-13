
#include "lockableAccessVec.h"
#include "myExcptn.h"


    //// class - CLockableAccessVec ////
//// CreateAccessHandler() ////
template <typename T>
//std::auto_ptr</**/CLockableAccessVec<T>::/**/CLavHandler> CLockableAccessVec<T>::CreateAccessHandler(const u32 key) ///@@@ тут можно попробовать return-type задать с пом. суффикс-синтаксиса: (что-то вроде "auto ...() -> ...")
auto CLockableAccessVec<T>::CreateAccessHandler(const u32 key) /*const*/   -> std::auto_ptr<CLavHandler>    //Suffix return type syntax использую, т.к. в кач-ве шаблонного пар-ра auto_ptr отказывался принимать конструкцию  " CLockableAccessVec<T>::CLavHandler " . Видимо, из-за наличия ещё одного шаблонного пар-ра.   ///@@@ #WARNING: d правильном ли месте поставил 'const' метода?
{
    //#OUTDATED:
    //if (0 == m_uLockCode)
    //    return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@ #WARNING: возможно тут нужно явно инициализировать auto_ptr
    //else
    //    return nullptr;     ///@@@ #WARNING: возможно тут нужно  /* static_cast<CLaoHandle*>(nullptr); */

    while (0 != m_uLockCode)
#if OS==WIN //т.к. ф-ция `_sleep()` is Windows specific. В Unix, кажется `Sleep()`.
        _sleep(10); ///@@@ #WARNING: 1) возможно, не пауза этого потока, а останов всей программы, 2) может грузит ЦП, 3) может в сек, а не в милисек.
#endif //OS=="WIN"
    return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@ #WARNING: возможно тут нужно явно инициализировать auto_ptr
}



    //// class - CLockableAccessVec:: CLavHandler ////
//// C-tor ////
template <typename T>
CLockableAccessVec<T>::CLavHandler/*<T>*/::CLavHandler(
        const u32 key,  
        std::vector<T>* pVec,
        u32& rLavLockCode
    )
    : m_pVec(pVec)

#if DEBUG==1
    , m_uResKey(key)
{
    if (0 != rLavLockCode) throw CMyExcptn(24);
    m_ruLavLockCode = rLavLockCode;
#else
    , m_ruLavLockCode(rLavLockCode)
{
#endif //DEBUG==1

//#OUTDATED (для наружного цикла вокруг DelBrick() в CArkanoidController::OnUpdate() допускаю возможность key==0 - тогда доступ без блокировки):
    //#UPD: отбой. Решил по-другому обходить - передавать в DelBrick() необяз-ным посл-ним пар-ром obj std::auto_ptr<_LAV_HANDLER>.
#if DEBUG==1
    if (0 == key)
        throw CMyExcptn(20);
    if (0 == m_ruLavLockCode) 
        throw CMyExcptn(18);
#endif
    m_ruLavLockCode = key;
}


//// D-tor ////
template <typename T>
CLockableAccessVec<T>::CLavHandler/*<T>*/::~CLavHandler()
{
#if DEBUG==1
    if (m_uResKey != m_ruLavLockCode) throw CMyExcptn(19);  //это возможно, если параллельный доступ #upd: возможность которого пока отменяю 
    else
#endif
        m_ruLavLockCode = 0;
}