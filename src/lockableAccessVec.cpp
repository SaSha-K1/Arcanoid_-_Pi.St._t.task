
#include "lockableAccessVec.h"
#include "myExcptn.h"


    //// class - CLockableAccessVec ////
//// CreateAccessHandler() ////
template <typename T>
//std::auto_ptr</**/CLockableAccessVec<T>::/**/CLavHandler> CLockableAccessVec<T>::CreateAccessHandler(const u32 key) ///@@@ ��� ����� ����������� return-type ������ � ���. �������-����������: (���-�� ����� "auto ...() -> ...")
auto CLockableAccessVec<T>::CreateAccessHandler(const u32 key) /*const*/   -> std::auto_ptr<CLavHandler>    //Suffix return type syntax ���������, �.�. � ���-�� ���������� ���-�� auto_ptr ����������� ��������� �����������  " CLockableAccessVec<T>::CLavHandler " . ������, ��-�� ������� ��� ������ ���������� ���-��.   ///@@@ #WARNING: d ���������� �� ����� �������� 'const' ������?
{
    //#OUTDATED:
    //if (0 == m_uLockCode)
    //    return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@ #WARNING: �������� ��� ����� ���� ���������������� auto_ptr
    //else
    //    return nullptr;     ///@@@ #WARNING: �������� ��� �����  /* static_cast<CLaoHandle*>(nullptr); */

    while (0 != m_uLockCode)
#if OS==WIN //�.�. �-��� `_sleep()` is Windows specific. � Unix, ������� `Sleep()`.
        _sleep(10); ///@@@ #WARNING: 1) ��������, �� ����� ����� ������, � ������� ���� ���������, 2) ����� ������ ��, 3) ����� � ���, � �� � �������.
#endif //OS=="WIN"
    return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@ #WARNING: �������� ��� ����� ���� ���������������� auto_ptr
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

//#OUTDATED (��� ��������� ����� ������ DelBrick() � CArkanoidController::OnUpdate() �������� ����������� key==0 - ����� ������ ��� ����������):
    //#UPD: �����. ����� ��-������� �������� - ���������� � DelBrick() ������-��� ����-��� ���-��� obj std::auto_ptr<_LAV_HANDLER>.
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
    if (m_uResKey != m_ruLavLockCode) throw CMyExcptn(19);  //��� ��������, ���� ������������ ������ #upd: ����������� �������� ���� ������� 
    else
#endif
        m_ruLavLockCode = 0;
}