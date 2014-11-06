//////////////////////////////////
//
//  ...
//      #TODO (#DEVELOP):
//  ���� ������ ������ ��������, �� _BALL::m_svHitChckPnts ������ �� �����������. 
//
//////////////////////////////////


#ifndef BALL_H
#define BALL_H



//#include <array>
//#include "pictureobj.h"   //����� �� frwd-decl.
#include "ballsManager.h" //�������� �������� forward-decl., �� "inconplete type is not allowed"
#include "lockableAccessVec.h"    ///@@@ #TODO: ������� �������� frwrd-decl'�� (��.���.14)
//#include "brick.h"    //����� �� frwd-decl.
#include "brick.h"      ///@@@!! ����� ���� �� ���, ���� �� ��� C++0x. � ���� ��������, �.�. �� �������� frwrd-decl. ����� `typename` (����� ���������):  //typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED! ///@@@! But not allowed before C++0x.  ---  ����! ������� ��������� ����.h =((
                        //����� ��� ����� �������, �.�. �� � ������������ ��� ballsMngr.h ����.

//// Forward declatations:
class CPictureObject;
//class CArkanoidController::CBricksManager::CBrick;      ///@@@ #WARNIRNG: ����� ��� ����������� � ����� �������� �������� � ��. `CBicksManager`. ��� �� ����� ������ ���� ��� ���� ����� ������, �.�., ������, ���-�� ��������� � "brick.h" � ��. �����, ��� ���������� ������.   ///@@@ Nestsed class Frwrd-decl.-tion is NOT ALLOWED!
//typename CArkanoidController::CBricksManager::CBrick;   //`typename` instead of `class` because of (see above): "Nestsed class Frwrd-decl.-tion is NOT ALLOWED!     //������ ��� �� �����������, �.�. ����� �� ���� � ballsManager.h, ���.��� ����������. ///@@@! But not allowed before C++0x.  ---  ����! ������� ��������� ����.h =((
//typedef CArkanoidController::CBricksManager::CBrick _BRICK; //��������� ��� frwrd-decl., ���� ���������� ���, ������, �� ������ ���.  //No need more because of forced include of brick.h (see string above)
                    ///@@@? �� ����, ����� �� Nestsed class Frwrd-decl.?

///@@@ #TODO: ����������� ���������� �� ������ ��������. ����.,  "#include "lockableAccessVec.h"  ��� �������� ��������� frwrd-decl'�� (�������, ������� �����) 
//template <typename T>
//class CLockableAccessVec;


class CArkanoidController::CBallsManager::CBall
{
public:
    //// C-tor:
    CBall(
            CPictureObject* const           pBall, 
            const bool                      bAttachToPad,
            /*const*/ CPictureObject* const     pPad    //@@ ������ `const` (������ - ��.� ����.� LocateAttachedToPadBall()
        );


    //// Datas (pblc):
    float m_fFiPlus;    //������������� � ��������������� ����� ����� m_fFi (���� ���� ��� �� Pad'�). ..
                         //.. ��� ������������� ������� �� �������� ����� �������� ������ � Pad'�� �� ��� ��������.


    //// Methods (pblc):
    bool        TryToInvertIsAttachedToPad(); 
    void        LocateAttachedToPadBall(/*const*/ CPictureObject* const pPad);  //@@ ������ `const`, �.�. ���������� ����� GetCenter() ������-�� �� const
    enum        _DIRECTION;     //forward declaration   ///@@@ #WARNING: ��������� ��?
    _DIRECTION   IsHit(
            //#XI: CPictureObject* const               pCurrBall,
            CPictureObject* const               pPad,
            /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,              // :Inp.
            const float                         fSceneW,
            CArkanoidController::GAME_STATE     gameState,
            const float                         fDltPxlsToChckHit = 3   //���-�� `fDltPxlsToChckHit` - ����� � �������� �/� ���������, ..
                                                                        //.. ������������, ����� � �������� IsHit() �������� �� ������ ..
                                                                        //.. �������� ��������� rect'�� � ��������� �������� �� ..
                                                                        //.. ����������� ������. ..
                                                                        //.. ��� ���� ������, ������ ����� ��������� �������**, ������, ..
                                                                        //.. �.�. �������� ��������� ����� �� ��������� �������� �/� ..
                                                                        //.. ���������� �������� �������������� (��. "#OPT") ��������. ..
                                                                        //.. ������������ � ���������� �� IsHit() ������� ..
                                                                        //.. isHitWithPad(..) � isHitWithBricks(..).
                                                                        //**)- ��������� ������, �.�. ������������ ����������� ������ ..
                                                                        //.. �������� ���������, ��� ��������� - ������� � ���� ..
                                                                        //.. ��������� ���������� � ����� ������ ���� �� �������������. ..
                                                                        //.. ���� ����� � ������ 1.01 �������.
        );   


    //// ������������ ������:
    vector2                 GetNewMoveCenter(const _DIRECTION dir);
    static CBall*           GetIsAttachedToPaddle() { return m_spIsAttachedToPad; } ///@@ ?: ����� �� ��� `static`? �.�. ���������� �� ���� ����� ���-�� ��������, �� ����� ������ ������?
    CPictureObject*         GetBallPicObj() { return m_pBallPicObj; }
    float                   GetVel() const { return m_fVelocity; }
#if DEBUG ==1
    float                   GetFi() const { return m_fFi; }
#endif
    static vector2          GetNumOfPixelsForShift(const _DIRECTION dir) { return m_smFewPixelsShift[dir]; }
    

    //// Types (pblc):
    enum _DIRECTION {N=0, NE, E, SE, S, SW, W, NW, NO=-1};	//����� �������� �������� (�� �������� �����), "NO" - ����-�, ..
                                                            //.. ������������ �-���� IsHit(), ���� ��� ��������.
                                                            //#WARNING: ���-�� �������� _DIRECTION == m_suNUM_OF_HIT_CHCK_PNTS-1. ��� ��� ..
                                                            //.. ������� �������� ���, ����� ����� ������ ���� �� ����. ��������, ������ ..
                                                            //.. ��������������� �������������, ���� �� ��������. �� �������� �������� � ..
                                                            //.. ������ ��������� ����� �� ���� ���������, ��� ����� ��������� � ������.


private:
    //// Datas (prvt):
    CPictureObject*             m_pBallPicObj;
    static CBall*               m_spIsAttachedToPad;
    float                       m_fVelocity;
    float                       m_fFi;		//���.����. ���� ����������� ����.������ == (-PI ; PI]. 0 �����-�� ����������� ����������� up.
    static const u32            m_suNUM_OF_HIT_CHCK_PNTS;       //(��.����.� ���������� �����-��� ����.dat� � cpp-�����).
    static std::vector<float>   m_svfBallHitChckPntsAngles;      //����, �����-��� ������-�������� ��������   ///@@@��������, ���������� � m_svHitChckPnts � std::map (��������)
                                                                //#IMPORTANT: � ����.������������� ��������� ���-�� ��-���, ������ ������ �� ������ ���-�� >=2. ������� � ����� ����-�� enum _DIRECTION � class CBall static member m_suNUM_OF_HIT_CHCK_PNTS.
    static std::vector<vector2> m_svHitChckPnts;
    static const float          m_fDEFAULT_BALL_VEL; //�������� ������ by def.      ///@@@ ?may be ��������� � CArkanoidController?
    static std::map<_DIRECTION,vector2> m_smFewPixelsShift;  //������� �������� � ����������� �� ������������ ������� ��������. 
                                                   //.. �����?: ������� ����� �� ��������� (2-3) �������, ����� ����� �� ���.�������� ..
                                                  //..   (����.�������� .IsHit() ���������� ������, ��� ����� �������� �������� �� ���� ..
                                                 //..   �������, ��� �������� � "���������" ������. ..
                                                //.. ������������?: ����� ����� ������������� �������: ������ ���� ������� � ������ ..
                                               //..   .IsHit() ��� ����� ����������� ������������ ������� (N, ..., NW) ���������� ..
                                              //..   �������� �� ��������� ���������� ������������� ������ � ����., ���� m_fFi �������� ..
                                             //..   � �������� �����, ��� delta �������� ���������� � �������������, � ������� ����� ..
                                            //..   ����������� ������. ��! ��� ������ ����� ����������������, + ������ �������� �� ..
                                           //..   �������, ���.� ����.����., (����) �� ����������. ..
                                          //.. !�����! ����� ��������� ������������ - ����� ������ �������� ������� � �������� ������ ..
                                         //..   ��� �� 1 �������. ������ �������� ���-��� �������� �� ����, �.�. �����-������ ..
                                        //..   ����������� �����-��� ����. � ������� ����� �� 4 ������� � ���������� ����. ������� ..
                                       //..   �������� ��������� ��� ������������ �����, �� ���� ��������, ����� ��� ����������� ..
                                      //..   ������ IsHit() ��������� ���-�� ������ ��������.
    static const u32            m_suNUM_PIXELS_4_SHIFT;    //==3   Number Of Pixels For Shift Ball after contact

    //Vector �����, �� ������� ������������� �����-������� �������� ������ (���� 0 �����-�� ����� "N"(north), ���� PI/2 - "E"(east), ..
    //.. ���� -PI/2 - "W"(west) � �.�. ���� ����������� (-PI : PI] ��� � ���� m_fFi � ��������� ���������� ������.
    

    //// Methods (prvt):
    void        ComputeNewFi(const _DIRECTION dir);
    void        ComputeStartFi(
                        const float rndStartAng = 10*(float)PI/180,     //������ ����.���������� �� 0 ���������� ���� �����. ..
                                                                        //.. ������� ������ - 10 �������� �������� � �������.
                        const u32 multiplForGen = pow(10.0, 4)  //���-�� �����, ����� �������� float �-��� ..
                                                                //.. Generate() �/� u32, ������� ��� ���������.
                    );
    void        NormalizeFi();
    vector2     ComputeNewMoveCenter(const u32 moveCCoordScale = pow(10.0, 3)) const;
    _DIRECTION   HitReview(_DIRECTION dir) const;
    void        ComputeFiPlus(CPictureObject* pPad);
    void        InitBallHitChckPntsAnglesVec();
    void        InitHitChckPntsVec();

    //��������������� ������ ��� IsHit()
    void        IsHitWithScreenBorders(
                        const float             fSceneW,                // :Inp.
                        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts   // :Outp.
                    ) const;
    void        IsHitWithPad(
                        //Inp.:
                        CPictureObject* const   pPad,
                        const float             fMaxDltYWithPad, //��������� ��� ������ ����� ������ �� IsHit() ����. �������: ..
                                                                //.. m_pPad->GetCenter().y - pOriBall->GetCenter().y  ..
                                                               //.. ������?: if (ball[i]..->GetPos().y  >  g_fPaddleY +fMaxDltYWithPad), ..
                                                              //.. �� �� ����� ������������ IsHit() � Pad'�� - ����� ��� �� ������� � ����.
                        const float             fDltPxlsToChckHit,  //��� ���� ������, ������ ����� ��������� �������, ������, �.�. ..
                                                                    //.. �������� ��������� ����� �� ��������� �������� ����� ���������� ..
                                                                    //.. �������� �������������� (��."#OPT") ��������. �������� �� IsHit().
                        //Outp.:
                        std::vector<_DIRECTION>& rsltTrggrdHitChckPnts,  
                        bool&                   rsltNeedToCheckBricks
                    );
    void        IsHitWithBricks(
                        const float                         fDltPxlsToChckHit,      // :Inp.
                        /*const*/ CLockableAccessVec<_BRICK*>&  lavBricks,              // :Inp.
                        std::vector<_DIRECTION>&             rsltTrggrdHitChckPnts   // :Outp.   //�������� �� IsHit(). ���������� ��. � IsHit()
                    );
    _DIRECTION   InterpretIsHitResults(std::vector<_DIRECTION>& vTrggrdHitChckPnts) const;
    static void InitFewPixelsShift();
};

typedef _BALLS_MNGR::CBall    _BALL;



#endif //BALL_H