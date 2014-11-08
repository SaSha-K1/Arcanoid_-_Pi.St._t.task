///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  lockableAccessVec.h                                                      //
//  authors: Sasha Kislun                                                    //
//  2014, Test task                                                          //
//                                                                           //
//    ����� CLockableAccessVec � ��������� � ���� CLavHandler ��������� ���  //
//  �������������� ������� � ������� �������� ��. �Brick, �������� ��� ����  //
//  ����������� ��������� ������������� ������� � ���� � ��.������.          //
//  ���������� ���, �.�. �������� (� �����������) ��������, ����� � ������   //
//  IsHit() ���� ������ ������������ ��� �������� ��� ��-��� ��� ������      //
//  ��������, � � ��� �� ����� �� ����� �� ������� � ��.������ ���������     //
//  ������� � ������ CleanBricks(). ��� ������� � UB.                       //
//    ��������� ����� CLavHandler ���� ���������� ������ RAII ��� �������    //
//  "������ � ������� � ����������� ����������� ������������� �������".      //
//    ����� CLockableAccessVec ������ ������� ��� ��������� �� ������       //
//  �������� (�������� ��. �Brick), � ��������� ��������� ������� ����       //
//  ������ �� ��������� ������� CBricksManager, ����������� �����������      //
//  ������� � ������� ������ �/� ���������� ����� � ����������� ���������-   //
//  -���� �������. ����� ���� �����-������ CLockableAccessVec ���������     //
//  ������� �������� �����, � ������� ��� ������� ������ � �������, ������-  //
//  ����� ��� ������������� ������� � �������� �������� ������ �������.      //     ///@@@ ���� �������� ����, ����� ������ ������������� � ������������� ���������� �������, �� ������� ���� ����� �� ����� (��� ���� ����� �� �����). � ����� ������ ������-������, ��������, ����� ����������� ������������ ������.��������� �-��� ���� �� ������ CLavHandler.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  (2014/X/13): 
//      Qstn: ������ ����� ��. "Lav" (������ ������������ ������ �-���       //
//  Get() �� ����.�����, �������� ���. ���� ������� (�.�. ����� �����������  //
//  ��� ����-� ��. "LavHndlr")? - 
//      ������, ��� ����� ptr �� ��� ������� � ��.-������, ����� ��� ������ //
//  ������������ ��������, �� ������������� ������� �-��� Get(), ����������� //
//  ���������� ������������� ������� � ���-��.
//                                                                           //
//      Qstn: ������ �� ������������ ����� ������� "Lav", ���-� �� �� ������ //
//  ������ .GetObjPtr(), ������� �� (����� �������� ���-��� ���-� ��������-  //
//  -��) ptr �� ����������� ���?
//      ��� ����, ����� Re-�� ������ RAII. ���, � ���� ��., ����� ��� ����,  //
//  ����� ���������� ������� � ��� ��������� (�.�. ��������� �������� ���.   //
//  ���� �������) ���������-�� ��� ������ ������� �� scope.
//                                                                           //
//  ��������, ��� ��� ����������� ����� ���� �� �������� ����-��� �� �-���,  //
//  ������������ LavHndlr ��� � ������-��� private ������ - ptr'�� �� ����-  //
//  -������ ���...
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//
//    �� ���� �������� ����� ������� � ptr'�� �� ������. ��������� �����,    //
//  �.�. ��� ���-�� ����� ��������� ptr �� ������, �� ��� ����������� � ���  //
//  ���������� ��������. ������� ��������� ����� ������-������ ��� �����    //
//  ptr'� �� ������ � ��. �����, ����������� RAII (������ ������, � �� �     //
//  ���� �� ������, �.�. ������������� C-tor � D-tor ��� RAII).
//    �� � ���� �� �������� ����� ��������� �����-������ ��� ptr'�, �� �    //
//  ��������� ����� �������� ���� � ��.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LOCKABLE_ACCESS_VEC_H
#define LOCKABLE_ACCESS_VEC_H



#include <vector>
#include <memory>   //for unique_ptr
#include "globals.h"
#include "types.h"  //����� ���������, ���� ������� "globals.h", ���������� ��� ������.
//#include "brick.h"    ///@@@ ����� ����� ���� ������? (��� _BRICK* ���� ��). �� � ���� typedef ��� �������� - �������� ����, ��� ����
#include "myExcptn.h"   //�/ cpp-�����



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
    class CLavHandler;  //! � public - �.�. �������� ����� unique_ptr, ������������������� ���� �����; ..
                        //!.. � bricksManager.cpp �������� typedef, ��������� ���� ���.

    //// Methods (pblc):
    std::unique_ptr<CLavHandler> CreateAccessHandler(const u32 key) /*const*/;   // `key` - is u32 value unique for each function, and! must be >0.
                                                                    //#UPD2: upd ������ �� ������ (�����) #UPD: !! key==0 - ����� ������ ��� ����������.
                                                                    //"1" - RetrieveBrick() in f. "bricksManager.h"
                                                                    //"2" - CArkanoidController::CBallsManager::CBall::IsHitWithBricks() in f. "ball.cpp"
                                                                    //"3" - CArkanoidController::CBricksManager::AddBrick() in f. "bricksManager.cpp"
                                                                    //#OLD: //"4" - CArkanoidController::CBricksManager::DelBrick() in f. "bricksManager.cpp"
                                                                    //"5" - CArkanoidController::CBricksManager::InitOfBricksAniObjects() in f. "bricksManager.cpp"
                                                                    //"6" - _BRICKS_MNGR::IsBricksEmptyCheck() called frm CArkanoidController::ProcessWinLostState() in f. "controller.cpp"   //only in DEBUG
                                                                    //"7" - CArkanoidController::OnUpdate() in f. "controller.cpp" (������� CBricksManager::CleanBricks())
    u32 ReleaseAccessHandler(std::unique_ptr<CLavHandler>/**/& apLavHndlr);    //! �������� ����������, � �� ����� �����, ����� �� ������� �� ���, ����� ..
                                                                        //!.. obj ��� ptr �� ������� ������� ��������������� (� ���� ������ ��� ..
                                                                        //!.. ������� �� ���������� ptr ��� ptr �� ������������� obj).

private:
    //������ ����������� � ������������:
    CLockableAccessVec (const CLockableAccessVec&);
    CLockableAccessVec& operator= (const CLockableAccessVec&);


    //// Datas (prvt):
    std::vector<T>*             m_pVec;
    u32                         m_uLockCode;    //! ���. ����. ���� ���������� ������� � ������������� ������� CLockableAccessObj ..
                                                //!.. (� ������� ��� ������� ������)
                                                //! 0 - ��������, ��� � ���.���. ������ �� ��� ���� �� ������.
    //#DEADLOCK_BRANCH:
    //std::auto_ptr<CLavHandler>* m_apLavHndlr; //! Ptr �� auto_ptr, ������������
};



//@@ Stoped here (2014-08-30)
///@@@@@ STOPED_HERE (2014-11-01): ���-�� ������� ��� �� ���. ��. ��� �������� IsHitWithBricks() � ball.cpp. �� ����� � �-���� ��� ������ ������� �����. �� � ��������, ����� �� ���������� �-���� ����� "�����()" �����-�� ��������, ��� ������ ������� delet'�� pLavHndlr �� ������ �������. {������� ��� - ������ �� �����, �� ��������...}
template <typename T>
class CLockableAccessVec<T>::CLavHandler    //Lockable Access Vector Handler
{
public:
    //// C-tor:
    CLavHandler(
            const u32       key,
            std::vector<T>* pVec,       ///@@@? �.�. ����� ������� �� ���� �������, ����� ��� ����� ���-�� ��������� �����, ������ ��� ������� �����, ��������� � ��������� (����., ����� ���� ��������?)
            u32&            rLavLockCode
        );

    //// D-tor:
    ~CLavHandler();


    //#WRONG:
    //// Methods (pblc):
    //u32 ReleaseLav();


    //// Operators (pblc):
    std::vector<T>* operator ->() const   { return m_pVec; }   ///@@@? ����� �������� �� ���, ��� � ������: pLavHndlrObj-> �� ��� ������ � ����� ����������� ���-�� ...
    std::vector<T>& operator *()  const   { return *m_pVec; }  
#if DEBUG==1
    u32 GetCurrValOfLockCode() const { u32 result = m_ruLavLockCode; return result; }   
        ///@@@ #TRY_IT: { return u32 result = m_ruLavLockCode; }    ///@@@ #WARNING: ��� �����??))
    u32 GetReservedKey() const  { return m_uResKey; }
#endif //DEBUG==1


private:
    //������ ����������� � ������������:
    CLavHandler(const CLavHandler&);
    CLavHandler& operator=(const CLavHandler&);


    //// Datas (prvt):
    std::vector<T>* m_pVec;
    u32&            m_ruLavLockCode;    //������ �� ����������, �������� ��� ���������� ������� � ������������� ������� CLockableAccessObj
#if DEBUG==1
    const u32       m_uResKey;    //��������� ����� ����� ��� ������� ��� ������ ���������� � D-tor'�
#endif

};



///////////////////////////////////////////////
//// ��, ��� ���� �� �������� � CPP-�����: ////
///////////////////////////////////////////////

    //// class - CLockableAccessVec ////
//// CreateAccessHandler() ////
template <typename T>
///@@@#TO_DO: ���������� � ����������� `auto_ptr` �� ��.��. `unique_ptr`
//std::auto_ptr</**/CLockableAccessVec<T>::/**/CLavHandler> CLockableAccessVec<T>::CreateAccessHandler(const u32 key) ///@@@ ��� ����� ����������� return-type ������ � ���. �������-����������: (���-�� ����� "auto ...() -> ...")
auto CLockableAccessVec<T>::CreateAccessHandler(const u32 key) /*const*/   -> std::unique_ptr<CLavHandler>    //Suffix return type syntax ���������, �.�. � ���-�� ���������� ���-�� unique_ptr ����������� ��������� �����������  " CLockableAccessVec<T>::CLavHandler " . ������, ��-�� ������� ��� ������ ���������� ���-��.   ///@@@ #WARNING: d ���������� �� ����� �������� 'const' ������?
{
    //#OUTDATED:
    //if (0 == m_uLockCode)
    //    return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@ #WARNING: �������� ��� ����� ���� ���������������� auto_ptr
    //else
    //    return nullptr;     ///@@@ #WARNING: �������� ��� �����  /* static_cast<CLaoHandle*>(nullptr); */

    while (0 != m_uLockCode)
    {
#if OS==WIN //�.�. �-��� `_sleep()` is Windows specific. � Unix, ������� `Sleep()`.
  
  #if HARD_DEBUG==1
        time_t currT = time(NULL);
        exceptPrintFile << asctime(localtime(&currT))<< "!!!: _sleep() starts in CreateAccessHandler(). m_uLockCode == "
            << m_uLockCode<< std::endl;
  #endif //HARD_DEBUG==1

        _sleep(10); ///@@@ #WARNING: 1) ��������, �� ����� ����� ������, � ������� ���� ���������, 2) ����� ������ ��, 3) ����� � ���, � �� � �������.
                    ///@@@? IntlelliSense ��������, ��� `_sleep()` ���������� - ����������� `Sleep()`. So poss-ly try.
#else //OS=="WIN"
        Sleep(10);  //In Unix it seems..
#endif //OS=="WIN"
    }
    //return new /**/std::auto_ptr<CLavHandler>/**/ CLavHandler(key, m_pVec, m_uLockCode);    ///@@@? ��� ������ ������, �.�. �-��� ������ ����� ���. ���    
    
    //#XI:
    //CLavHandler* tmpCLavHndlrPtr = new CLavHandler(key, m_pVec, m_uLockCode);
    //return *(new std::auto_ptr<CLavHandler> (/* * */tmpCLavHndlrPtr));   //Without `*` I get err: C2664: 'std::auto_ptr<_Ty>::auto_ptr<CLockableAccessVec<T>::CLavHandler>(std::auto_ptr<_Ty> &) throw()' : cannot convert parameter 1 from 'std::auto_ptr<_Ty> *' to 'std::auto_ptr<_Ty> &' 186

    //return *( new std::unique_ptr<CLavHandler> (new CLavHandler(key, m_pVec, m_uLockCode)) ); //Without `*` I get err: C2664: 'std::auto_ptr<_Ty>::auto_ptr<CLockableAccessVec<T>::CLavHandler>(std::auto_ptr<_Ty> &) throw()' : cannot convert parameter 1 from 'std::auto_ptr<_Ty> *' to 'std::auto_ptr<_Ty> &' 186
                                                                                            //��� ��������� unique_ptr � �� raw (������� ������� ��� ������ �������� unique_ptr'��), �.�. �� ������, ��� ������� ������ ����� ��������� ���������. � ���� ���, �� memory leaks!
    return std::move( *(new unique_ptr<CLavHandler>(new CLavHandler(key, m_pVec, m_uLockCode)) ) );
    ///@@@#EXP: try ��� auto_ptr ��������� ��� `new` - ���������, ������ �� ��� ��������� ��� �� ���� ���., ����� ��� ������ �������?

///@@@ #TRY_IT(instead of upper couple of strings):    return /**/*(/**/ new /**/std::auto_ptr<CLavHandler>/**/ /* * */(new CLavHandler(key, m_pVec, m_uLockCode))/**/)/**/;    ///@@@ #WARNING: �������� ��� ����� ���� ���������������� auto_ptr
}                                                                                           ///@@@? �.�.�., �������� �� �������������� `new`. ���� ���, �� ��.� EXP-������. ��� � ����� �/� tmp-var ������


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
    if (0 != rLavLockCode) throw CMyExcptn(24); ///@@@? if  `CLockableAccessVec<T>::m_uLockCode`  ������������� ������� �-���� Hndlr'�, �� ��� �������� m!b. �������� - "if(0==.."
#else
{
#endif //DEBUG==1

//#OUTDATED (��� ��������� ����� ������ DelBrick() � CArkanoidController::OnUpdate() �������� ����������� key==0 - ����� ������ ��� ����������):
    //#OLD: #UPD: �����. ����� ��-������� �������� - ���������� � DelBrick() ������-��� ����-��� ���-��� obj std::auto_ptr<_LAV_OFBRICKS_HNDLR>.
#if DEBUG==1
    if (0 == key)
        throw CMyExcptn(20);
    //if (0 == m_ruLavLockCode) ///@@@? ��� �������� ����� === CMyExcptn(24).  ���� ���, �� ����������� ��! (�������� ��� �������� ����� 2���: ���� ������ � ���� ����� ��������� �������, �� ���� �� ���)
    if (0 != m_ruLavLockCode) //was "0==.."  ///@@@? ��� �������� ����� === CMyExcptn(24).  ���� ���, �� ����������� ��! (�������� ��� �������� ����� 2���: ���� ������ � ���� ����� ��������� �������, �� ���� �� ���)        
        throw CMyExcptn(18);
#endif
    m_ruLavLockCode = key;
}


//// D-tor ////
template <typename T>
CLockableAccessVec<T>::CLavHandler/*<T>*/::~CLavHandler()   ///*
{
#if DEBUG==1
    if (m_uResKey != m_ruLavLockCode) throw CMyExcptn(19);  //��� ��������, ���� ������������ ������ #upd: ����������� �������� ���� ������� 
    else
#endif
        m_ruLavLockCode = 0;
}



#endif //LOCKABLE_ACCESS_VEC_H


///#NOTE:
    /// "...  if ( _BRICK::DSTR_ST == (*(*(*ppLavHndlr)))[i]->m_state )  ..."
    //#NOTE: 
    //���   1� (*..) - �� ptr'� �� `unique_ptr` ��������� � `unique_ptr`
    //      2� (*..) - �� `unique_ptr` ��������� � _LAV_OFBRICKS_HNDLR
    //      3� (*..) - �.�. � _LAV_OFBRICKS_HNDLR ����-�� `*` � `->` �����������, �� ��� �� _LAV_OFBRICKS_HNDLR ��������� ����� ..
    //.. � std::vector<_BRICK*>, ptr �� ������� ���� member data ������.
    //      �� � [i] - ��� ��������� � ��-�� ������� ���� _BRICK*
    ///.. �������, ��� �����!
