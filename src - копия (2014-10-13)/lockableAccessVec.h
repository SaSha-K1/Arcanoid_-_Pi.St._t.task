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
//  ������� � ������ DelBrick(). ��� ������� � UB.                          //
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
                                                                    //#UPD2: upd ������ �� ������ (�����) #UPD: !! key==0 - ����� ������ ��� ����������.
                                                                    //"1" - RetrieveBrick() in f. "bricksManager.h"
                                                                    //"2" - CArkanoidController::CBallsManager::CBall::IsHitWithBricks() in f. "ball.cpp"
                                                                    //"3" - CArkanoidController::CBricksManager::AddBrick() in f. "bricksManager.cpp"
                                                                    //"4" - CArkanoidController::CBricksManager::DelBrick() in f. "bricksManager.cpp"
                                                                    //"5" - CArkanoidController::CBricksManager::InitOfBricksAniObjects() in f. "bricksManager.cpp"
                                                                    //"6" - CArkanoidController::ProcessWinLostState() in f. "controller.cpp"   //only in DEBUG
                                                                    //"7" - CArkanoidController::OnUpdate() in f. "controller.cpp" (������� CBricksManager::CleanBricks())

private:
    //������ ����������� � ������������:
    CLockableAccessVec (const CLockableAccessVec&);
    CLockableAccessVec& operator= (const CLockableAccessVec&);

    //// Types (prvt):
    ///@@@? #OUTDATED (�������� ��������� ���� - ��� 1-��� ����������� � ������ `public:`, �.�. ����-� ��������, ��� `udefined`. @@@?: ����� �����������, �.�. ����� � ������ � �������� ������ ������.):
    //class CLavHandler;  //Lockable Access Vector Handler

    //// Datas (prvt):
    std::vector<T>* m_pVec;
    u32 m_uLockCode;    //���. ����. ����������, �������� ��� ���������� ������� � ������������� ������� CLockableAccessObj
};



//@@ Stoped here (2014-08-30)
template <typename T>
class CLockableAccessVec<T>::CLavHandler    //Lockable Access Vector Handler
{
public:
    //// C-tor:
    CLavHandler(
            const u32 key,
            std::vector<T>* pVec,       ///@@@? �.�. ����� ������� �� ���� �������, ����� ��� ����� ���-�� ��������� �����, ������ ��� ������� �����, ��������� � ��������� (����., ����� ���� ��������?)
            u32& rLavLockCode
        );

    //// D-tor:
    ~CLavHandler();


    //// Methods (pblc):
    std::vector<T>* operator ->() const   { return m_pVec; }   ///@@@? ����� �������� �� ���, ��� � ������: pLavHndlrObj-> �� ��� ������ � ����� ����������� ���-�� ...
    std::vector<T>& operator *()  const   { return *m_pVec; }  
#if DEBUG==1
    u32 GetCurrValOfLockCode() const    { return u32 result = m_ruLavLockCode; }    ///@@@ #WARNING: ��� �����??))
    u32 GetReservedKey() const  { return m_uResKey; }
#endif //DEBUG==1

    //#OUTDATED (������ ���� �������/����������, ������������ ������ ����-�� `->` � ������������� `*`): 
    //std::vector<T1>::size_type          size() const    { return m_pVec->size();    }    ///@@@ #WARNING: �������� �� ������ <int> ������ <CBrick*>
    //std::vector<T1>::iterator           begin()         { return m_pVec->begin();   }     ///@@@ #WARNING: ���� ���������� const, �����-��� const_iterator
    //std::vector<T1>::iterator           end()           { return m_pVec->end();     }
    //std::vector<T1>::reverse_iterator   rbegin()        { return m_pVec->rbegin();  }   //�������� �� ������ ����.
    //std::vector<T1>::reverse_iterator   rend()          { return m_pVec->rend();    }   //�������� �� ������ ����.
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
    //������ ����������� � ������������:
    CLavHandler(const CLavHandler&);
    CLavHandler& operator=(const CLavHandler&);


    //// Datas (prvt):
    std::vector<T>* m_pVec;
    u32& m_ruLavLockCode;    //������ �� ����������, �������� ��� ���������� ������� � ������������� ������� CLockableAccessObj
#if DEBUG==1
    const u32 m_uResKey;    //��������� ����� ����� ��� ������� ��� ������ ���������� � D-tor'�
#endif

};

//typedef CLockableAccessVec<_BRICK*>::CLavHandler    _LAV_HANDLER;
typedef /**/typename/**/ CLockableAccessVec<_BRICK*>::CLavHandler _LAV_OFBRICKS_HANDLER;



#endif //LOCKABLE_ACCESS_VEC_H


///#NOTE:
    /// "...  if ( _BRICK::DSTR_ST == (*(*(*ppLavHndlr)))[i]->m_state )  ..."
    //#NOTE: 
    //���   1� (*..) - �� ptr'� �� `auto_ptr` ��������� � `auto_ptr`
    //      2� (*..) - �� `auto_ptr` ��������� � _LAV_HANDLER
    //      3� (*..) - �.�. � _LAV_HANDLER ����-�� `*` � `->` �����������, �� ��� �� _LAV_HANDLER ��������� ����� � std::vector<_BRICK*> ..
    //.. ptr, �� ������� ���� member data ������.
    //      �� � [i] - ��� ��������� � ��-�� ������� ���� _BRICK*
    ///.. �������, ��� �����!
