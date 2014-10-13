#ifndef BALL_H
#define BALL_H


#include "pictureobj.h"
#include "ballsManager.h"
#include <array>					//��� std::array	
//#include "types.h"    �� �������


class CArkanoidController::CBallsManager::CBall
{
public:
    //C-tor:
    CBall(
            CPictureObject* const   pBall, 
            //#OUTDATED (����� static const'���� � ����� ����):     const u32               numOfHitChckPnts,
            //#OUTDATED: std::array<vector2,/*uNumOfHitChckPnts*/0U>& aBallHitChckPnts,     ///#WARNING: ��������� �� "0U" ..? - ���!
            const bool              bAttachToPad
        );

    //Datas (pblc):
    /*static*/ float m_sfFiPlus;    //������������� � ��������������� ����� ����� m_fFi (���� ���� ��� �� Pad'�) ..     ///@@@ #OUTDATED (�� �����, ����� ..FiPlus ����� static'��)
                                //.. � ����������� �� �������� �� �������� Pad'� ����� �������� ������ � Pad'��.

    //Methods (pblc):
    bool        TryToInvertIsAttachedToPad(); 
    void        LocateAttachedToPadBall();
    //forward declaration   ///@@@ #WARNING: ��������� ��?
    enum DIRECTION;
    DIRECTION   IsHit(const float fDltPxlsToChckHit = 3/*5*/);  //���-�� `fDltPxlsToChckHit` - ����� � �������� �/� ��������� ..
                                                               //.. , ����� � �������� isHit() �������� �� ������ �������� ..
                                                              //.. ��������� rect'�� � ��������� �������� �� ����������� ������. ..
                                                             //.. ��� ���� ������, ������ ����� ��������� �������, ������, �.�. ..
                                                            //.. �������� ��������� ����� �� ��������� �������� ����� ���������� ..
                                                           //.. �������� �������������� (��. "#OPT") ��������. ������������ � ..
                                                          //.. ���������� �� isHit() ������� isHitWithPad(..) � isHitWithBricks(..).

    //������������ ������:
    vector2                 GetNewMoveCenter(const DIRECTION dir);
    static CBall*           GetIsAttachedToPaddle() { return m_spIsAttachedToPad; }
    CPictureObject*         GetBallPicObj() { return m_pBallPicObj; }
    float                   GetVel() { return m_fVelocity; }
    //#OUTDATED (�� ���� ������ � ����� prvt data'�):     std::array<vector2, uNumOfHitChckPnts>&  GetHitChckPnts() { return m_aHitChckPnts; }
#if DEBUG ==1
    float                   GetFi() const { return m_fFi; }
#endif

private:
    //Datas (prvt):
    CPictureObject*         m_pBallPicObj;
    static CBall*           m_spIsAttachedToPad;
    float                   m_fVelocity;
    float                   m_fFi;		//���.����. ���� ����������� ����.������ == (-PI ; PI]. 0 �����-�� ����������� ����������� up.
    static const u32        m_suNUM_OF_HIT_CHCK_PNTS;      //(��.����.� ���������� �����-��� ����.dat� � cpp-�����).
    //#OUTDATED: std::array<vector2, m_suNumOfHitChckPnts>&  m_aHitChckPnts;
    static std::vector<vector2> m_vHitChckPnts;
    static const float m_fDEFAULT_BALL_VEL;     //�������� ������ by def.
    //#DEL_IT?: float       fBallR;
    //#OUTDATED:    std::array<vector2, uNumOfHitChckPnts> m_aBallHitChckPnts;   //������ �����.�������� �������� ������ //������������� � OnShow()	
    std::map<DIRECTION,vector2> m_mFewPixelsShift;  ///@@@ ?name?
    //������� �������� � ����������� �� ������������ ������� ��������. 
    //.. �����?: ������� ����� �� ��������� (2-3) �������, ����� ����� �� ���.�������� (����.�������� .isHit() ���������� ������, ��� ..
    //..   ����� �������� �������� �� ���� �������, ��� �������� � "���������" ������.
    //.. ������������?: ����� ����� ������������� �������: ������ ���� ������� � ������ .isHit() ��� ����� ����������� ������������ ..
    //..   ������� (N, ..., NW) ���������� �������� �� ��������� ���������� ������������� ������ � ������, ���� m_fFi �������� � ..
    //..   �������� �����, ��� delta �������� ���������� � �������������, � ������� ����� ����������� ������. ��! ��� ������ ����� ..
    //..   ����������������, + ������ �������� �� �������, ���.� ����.����., (����) �� ����������.
    //.. !�����! ����� ��������� ������������ - ����� ������ �������� ������� � �������� ������ ��� �� 1 �������. ������ �������� ..
    //..   ���-��� �������� �� ����, �.�. �����-������ ����������� �����-��� ����. � ������� ����� �� 4 ������� � ���������� ����. ..
    //..   ������� �������� ��������� ��� ������������ �����, �� ���� ��������, ����� ��� ����������� ������ isHit() ��������� ..
    //..   ���-�� ������ ��������.
    static const u32 m_suNUM_PIXELS_4_SHIFT;    //==3   Number Of Pixels For Shift Ball after contact

    //#OUTDATED (������� ������.��������):
    //std::array<float, m_uNumOfHitChckPnts> aBallHitChckPntsAngles = 
    //    {   
    //        0,          //N
    //        PI/4,       //NE
    //        HALFPI,     //E
    //        PI*3/4,     //SE
    //        PI,         //S
    //        -PI*3/4,    //SW
    //        -HALFPI,    //W
    //        -PI/4       //NW
    //    };

    //Vector �����, �� ������� ������������� �����-������� �������� ������ (���� 0 �����-�� ����� "N"(north), ���� PI/2 - "E"(east), ..
    //.. ���� -PI/2 - "W"(west) � �.�. ���� ����������� (-PI : PI] ��� � ���� m_fFi � ��������� ���������� ������.
    static std::vector<float> m_vfBallHitChckPntsAngles;    //#IMPORTANT: � ����.������������� ��������� ���-�� ��-���, ������ ������ �� ������ ���-�� >=2. ������� � ����� ����-�� enum DIRECTION � class CBall static member m_suNUM_OF_HIT_CHCK_PNTS.


    //Types (prvt):
    enum DIRECTION {N=0, NE, E, SE, S, SW, W, NW, NO=-1};	//����� �������� �������� (�� �������� �����), "NO" - ����-�, ..
                                                            //.. ������������ �-���� isHit(), ���� ��� ��������.
                                                            //#WARNING: ���-�� �������� DIRECTION == uNumOfHitChckPnts-1. ��� ��� ..
                                                            //.. ������� �������� ���, ����� ����� ������ ���� �� ����. ��������, ������ ..
                                                            //.. ��������������� �������������, ���� �� ��������. �� �������� �������� � ..
                                                            //.. ������ ��������� ����� �� ���� ���������, ��� ����� ��������� � ������.




    //Methods (prvt):
    void        ComputeNewFi(const DIRECTION dir);
    void        ComputeStartFi(
                    const float rndStartAng = 10*(float)PI/180,   //������ ����.���������� �� 0 ���������� ���� �����. ..
                                                                  //.. ������� ������ - 10 �������� �������� � �������.
                    const u32 multiplForGen = pow(10.0, 4)  //���-�� �����, ����� �������� float �-��� ..
                                                            //.. Generate() �/� u32, ������� ��� ���������.
                );
    void        NormalizeFi();
    vector2     ComputeNewMoveCenter(u32 moveCCoordScale = pow(10.0, 3)) const;
    DIRECTION   HitReview(const DIRECTION dir) const;
    void        ComputeFiPlus();
    void        InitBallHitChckPntsAnglesVec();
    void        InitHitChckPntsVec();

    //��������������� ������ ��� isHit()
    void        IsHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
    void        IsHitWithPad(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    bool& needToCheckBricks, 
                    const float fMaxDltYWithPad,    //��������� ��� ������ ����� ������ �� isHit() ����. �������: ..
                                                    //.. g_pPad->GetCenter().y - pOriBall->GetCenter().y  ..
                                                    //.. ������?: if (ball[i]..->GetPos().y  >  g_fPaddleY +f MaxDltYWithPad), �� ..
                                                    //.. ������ �� ����� ������������ isHit() � Pad'�� - ����� ��� �� ������� � ����.
                    const float& fDltPxlsToChckHit      //�������� �� isHit(). ���������� ��. � isHit()
                );
    void        IsHitWithBricks(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    const float& fDltPxlsToChckHit  //�������� �� isHit(). ���������� ��. � isHit()
                );
    DIRECTION   InterpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
    void InitFewPixelsShift();
};



#endif//BALL_H