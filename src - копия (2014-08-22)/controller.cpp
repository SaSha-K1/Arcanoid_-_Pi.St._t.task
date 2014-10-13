////////////////////////////////////////////////////////////////
//
//  controller.cpp: a part of 2DTemlate application
//  authors: Yulia Malyarevich
//  �2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#include "controller.h"
#include "cursor.h"
#include "obj.h"
#include <sstream>					//��� std::ostringstream
#include <time.h>					//��� std::srand(time(NULL))
//#include <random>					//��� MyRandom::GetInst()()  �� �� ��������
#include <array>					//��� std::array	
#include <vector>					//��� std::vector
#include <time.h>					//��� sleepMy()
#include <fstream>
#include "myExcptn.h"
#include "globals.h"


#define EXCPTN_CATCH catch (const CMyExcptn myEx)\
    {\
        time_t t = time(NULL);\
        switch (myEx.m_code)\
        {\
            case 1: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"1.Unexpected last `else` in CBall::computeNewMoveCenter().\"" << std::endl; break;\
            case 2: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"2.Unexpected last `else` in CBall::computeNewFi().\"\n" << std::endl; break;\
            case 3: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"3.Condition (m_fFi>=2*PI || m_fFi<=-2*PI) in CBall::normalizeFi() didn't satisfied.\"" << std::endl;  break;\
            case 4: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"4.vHP.size() is >2! in isHit().\"" << std::endl; break;\
            case 5: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"5.CBall::getNewMoveCenterIfHit() got dir==NO.\"" << std::endl; break;\
            case 6: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"6.CBall::computeStartFi() was called by ball, that isn't attached to paddle.\"" << std::endl; break;\
            case 7: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"7.In isHit()  vHP.size()==2 but values combination is unexpected.\"" << std::endl; break;\
            case 9: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"9.The locateAttachedToPadBall() was called for ball, that isn't attached to Pad.\"" << std::endl; break;\
            case 10: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"10.The delBall() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 11: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"11.Unexpected last `else` in CBall::isHit().\"\n" << std::endl; break;\
            case 12: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"12.The m_fFi>PI || <=-PI in CBall::isHit() before `switch(dir)`.\"\n" << std::endl; break;\
            case 13: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"13.The `default` condition-branch in `switch(dir)` in CBall::isHit().\"\n" << std::endl; break;\
            case 14: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"14.The delBrick() get i, that is bigger than vector size().\"" << std::endl; break;\
            case 15: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"15.g_vBricks.empty() condition isn't complete, but \"WON\" state is set.\"" << std::endl; break;\
            case 16: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"16.CBrick::bricksLockOff() returned \"0\" in isHit().\"" << std::endl; break;\
            case 17: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"17.CBrick::bricksLockOff() returned \"0\" in delBrick().\"" << std::endl; break;\
            default: exceptPrintFile << asctime(localtime(&t))<< "   Exception invoked: \"Unexpected exception!\"" << std::endl; break;\
        }\
    }


//#OUTDATED: const u32   MULTIPL_FOR_GEN = pow(10.0, 4);      //��� ����� float ��������� � u32, ������� ������ Generate()
//#OUTDATED: const u32   MOVE_C_COORD_SCALE = pow(10.0, 3);	//Const ��������� ���������� ������ �������� (��-���� ����� ���� ������ ������, 
                                        //.. �������, ����� ��� ����.���� �� �������)
const char  MAX_BALL_NUM = 5;
const u32   NUMBER_OF_LEVELS=1;	//���-�� ������� � ����
const u32   BRICKS_ON_LEVEL [NUMBER_OF_LEVELS] = {/*2*/12};	//���-�� �������� �� ������ - ��� ����������� ������� ActObjects.objects
const float HALFPI = PI/2;	//�������� ����� ������ CTG()
const float DEFAULT_BALL_VEL = 200;     //�������� ������ by def.

//#OUTDATED: u32 gameScore = 0;  ///@@@ m!b. ������ ������ Arcanoid

//#OUTDATED: const float RAND_START_ANGLE=10*(float)PI/180;	//������ ���������� �� ���� ���������� ���� �����.������� ������, ������

//#OUTDATED:
//const float DLT_PXLS_TO_CHCK_HIT = 3/*5*/;  //����� � �������� �/� ���������, ����� � �������� isHit() �������� �� ������ ..
//                                            //.. �������� ��������� rect'�� � ��������� �������� �� ����������� ������. ..
//                                            //.. ��� ���� ������, ������ ����� ��������� �������, ������, �.�. �������� ..
//                                            //.. ��������� ����� �� ��������� �������� ����� ���������� �������� ..
//                                            //.. �������������� (��. "#OPT") ��������.

//#OUTDATED: float g_fMaxDltYWithPad = 50;  //��� dflt value, ������������� � OnShow(), ����� ��������� �-�� Pad'� � ������.

enum GAME_STATE {LOST=0, RUN=1, WON=2, NEW=3};	//��������� ���� ��� ��������. ��� ��� ����������� �������� ��������; 
                                            //.. "NEW" ����� ��� �������� DONT_DELETE_ANI_BRICKS==1

#if DONT_DELETE_ANI_BRICKS 1
GAME_STATE gameState = NEW;
#else
GAME_STATE gameState = RUN;
#endif

const float   BOTTOM_GAP = 35;	//����� �/� paddle � ����.����.����
float     g_fSceneHeight = 600;	//���-� �������� ����	//��� dflt value, ������������� � OnShow()				
float     g_fSceneWidth = 800;	//���-� �������� ����	//��� dflt value, ������������� � OnShow()				
float     g_fPaddleY1 = g_fSceneHeight - BOTTOM_GAP;	//��� dflt value, ������������� � OnShow(), ������� � �� `const`. ..
                                                //.. ��� `g_fPaddleY1`, �.�. �������� Pad ����� � ����.���� ������������ �� OY ..
                                                //.. � ����������� �� �������/�������.
float     g_fPaddleY = g_fPaddleY1; //��� dflt value, ������������� � OnShow()

class CBall;
class CBrick;
std::vector<CBall*>     g_vBalls;  ///@@@ ?name?
std::vector<CBrick*>    g_vBricks; ///@@@ ?name?

///@@@ ?name?:  ///@@@STOPED HERE. Convert to map
std::array<CPictureObject*, MAX_BALL_NUM>   ballsPictObjects;	//������ 2� ��������: 1) ������ CPict.Object'�� ���� 5 (MAX_BALL_NUM) �������
std::array<bool, MAX_BALL_NUM>              ballsPictObjectsUsage;		//������ 2� ��������: 2) ������ ����������� ��������� CPict.Object'�� �� 1�� �������	//������������� ��� ������ � ������ initOfBallsPictObjects(), ���������� �������� � OnShow()
//����� m_scene �������� ������������, �������������� ������ CPict.Object'�� ���� 5 ������� (MAX_BALL_NUM)
void InitOfBallsPictObjects(CScene* sc)
{
    ballsPictObjectsUsage.fill(0);
	
    ballsPictObjects[0] = sc->FindPictureObject(PIC_SC01_BALL);
    ballsPictObjects[1] = sc->FindPictureObject(PIC_SC01_BALL2);
    ballsPictObjects[2] = sc->FindPictureObject(PIC_SC01_BALL3);
    ballsPictObjects[3] = sc->FindPictureObject(PIC_SC01_BALL4);
    ballsPictObjects[4] = sc->FindPictureObject(PIC_SC01_BALL5);
}

const u32   uNumOfHitChckPnts = 8;  //(N(north),E(east),S,W + 4 �������������)
float       fBallR = 25;  //��� dflt value, ������������� � OnShow()

//������ �����, �� ������� ������������� �����-������� �������� ������ (���� 0 �����-�� ����� "N"(north), ���� PI/2 - "E"(east), ..
//.. ���� -PI/2 - "W"(west) � �.�. ���� ����������� (-PI : PI] ��� � ���� m_fFi � ��������� ���������� ������.
std::array<float, uNumOfHitChckPnts> aBallHitChckPntsAngles = 
    {   
        0,          //N
        PI/4,       //NE
        HALFPI,     //E
        PI*3/4,     //SE
        PI,         //S
        -PI*3/4,    //SW
        -HALFPI,    //W
        -PI/4       //NW
    };

//������ �����.�������� �������� ������
std::array<vector2, uNumOfHitChckPnts> aBallHitChckPnts;    //������������� � OnShow()	
//#OUTDATED:
//std::array<vector2, uNumOfHitChckPnts> aBallHitChckPnts =
//    {   //�.�. ��� ����� ���� � ��������� ������������, ��, ���� ��� ����� ������ ������� ��� ������� ������, �� �, ������ ..
//        //.. ������ ��������� ����� ����� � ����������� ��� ���������, ���������� 1-��; � �������� ����������� - ��������.
//        vector2((fBallR-1),  0-1),    //N  (24, -1)    //��� � ����� `(fBallR-1)`, �.�. �������� � ����.
//        vector2( /*myround(*/(fBallR-1) + fBallR * cos(PI/4)/*)*/+1,  /*myround(*/(fBallR-1) - fBallR * sin(PI/4)/*)*/ ), //NE (43,  6)
//        vector2((2*fBallR-1) + 1,  (fBallR-1)),    //E  (50, 24)
//        vector2((fBallR-1) + fBallR * cos(PI/4)+1,  (fBallR-1) + fBallR * cos(PI/4)+1),    //SE (43, 43)
//        vector2(24, 50),    //S  (24, 50)
//        vector2( 6, 43),    //SW ( 6, 43)
//        vector2(-1, 24),    //W  (-1, 24)
//        vector2( 6,  6)     //NW ( 6,  6)
//    };
//std::array<vector2,uNumOfHitChckPnts> ballHitChckPnts = 
//    {   vector2(24, -1),    //N
//        vector2(43,  6),    //NE
//        vector2(50, 24),    //E
//        vector2(43, 43),    //SE
//        vector2(24, 50),    //S
//        vector2( 6, 43),    //SW
//        vector2(-1, 24),    //W
//        vector2( 6,  6)     //NW
//    };

void InitBallHitChckPntsArr()
{
    float currAng;
    for (u32 i=0; i < aBallHitChckPnts.size(); ++i)
    {
        currAng = aBallHitChckPntsAngles[i]; //��� ������������. ������, ��� �����, �.�. ���� ����� ������������ ������ �������.
        //��������� ���������� ������� ����������� ��������� x � y �� ��������� ���������� ������:
        //��������� �������� ��������� (1 ��� 0) �����: �.�. ��� ����� ���� � ��������� ������������, ��, ..
        //.. ���� ��� ����� ������ ������� ��� ������� ������, �� �, ������ ������ ��������� ����� ����� ..
        //.. _�_ ����������� ��� ���������, ���������� 1-��; � �������� ����������� - ��������.
        aBallHitChckPnts[i] = vector2(
                (fBallR-1) + fBallR * sin(currAng) 
                    +((currAng > 0   &&  currAng < PI) ? 1 : 0),    ///@@@ /*myround(*/ ���� ��?
                (fBallR-1) - fBallR * cos(currAng) 
                    +(( (currAng > -PI  &&  currAng < -HALFPI)  ||  (currAng > HALFPI  /*&&  currAng <= PI*/) ) ? 1 : 0)
                    //��� ������������������ ����� ������� �� �����, ���� ���� �� ����� ���� ������ PI (��� ������)
            );
    }
}

enum DIRECTION {N=0, NE, E, SE, S, SW, W, NW, NO=-1};	//����� �������� �������� (�� �������� �����), "NO" - ����-�, ..
                                                        //.. ������������ �-���� isHit(), ���� ��� ��������.
                                                        //#WARNING: ���-�� �������� DIRECTION == uNumOfHitChckPnts-1. ��� ��� ..
                                                        //.. ������� �������� ���, ����� ����� ������ ���� �� ����. ��������, ������ ..
                                                        //.. ��������������� �������������, ���� �� ��������. �� �������� �������� � ..
                                                        //.. ������ ��������� ����� �� ���� ���������, ��� ����� ��������� � ������.

std::map<DIRECTION,vector2> mFewPixelsShift;  ///@@@ ?name?
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
const u32 NUM_PIXELS_4_SHIFT = 3;
void InitFewPixelsShift()
{
    float currAng;
    for (u32 i=0; i < aBallHitChckPnts.size(); ++i)
    {
        currAng = aBallHitChckPntsAngles[i]; //��� ������������. ������, ��� �����, �.�. ���� ����� ������������ ������ �������.
        mFewPixelsShift[static_cast<DIRECTION>(i)] = vector2(   //#COMPILER(type_cast)
            NUM_PIXELS_4_SHIFT *-sin(currAng), 
            NUM_PIXELS_4_SHIFT * cos(currAng)
            ); ///@@@ /*myround(*/ ���� ��?
    }

    //#OUTDATED:
    //mFewPixelsShift [N]  = vector2( 0, 3);      // ( 0, 3)
    //mFewPixelsShift [NE] = vector2(-2, 2);      // (-2, 2)
    //mFewPixelsShift [E]  = vector2(-3, 0);      // (-3, 0)
    //mFewPixelsShift [SE] = vector2(-2,-2);      // (-2,-2)
    //mFewPixelsShift [S]  = vector2( 0,-3);      // ( 0,-3)
    //mFewPixelsShift [SW] = vector2( 2,-2);      // ( 2,-2)
    //mFewPixelsShift [W]  = vector2( 3, 0);      // ( 3, 0)
    //mFewPixelsShift [NW] = vector2( 2, 2);      // ( 2, 2)
}
    ///@@@@@ STOPED HERE
//2 - (��� > �������� ������); � 12 - ��� �������������
#if NUMBER_OF_BRICKS == 2
std::array<vector2, 2> bricksCoordsLvl1 = 	
    {   vector2( 59, 140),	//0
        vector2(210, 140)	//1
    };

#elif NUMBER_OF_BRICKS == 12
std::array<vector2, 12> bricksCoordsLvl1 = 
    {   vector2( 59, 140),	//0
        vector2(210, 140),	//1
        vector2(361, 140),	//2
        vector2(512, 140),	//3
        vector2(663, 140),	//4
        vector2(814, 140),	//5
        vector2( 59,  89),	//6(3)
        vector2(361,  89),	//7(3)
        vector2(663,  89),	//8(3)
        vector2(210, 191),	//9(1)
        vector2(512, 191),	//10(1)
        vector2(814, 191),	//11(1)
    };
#endif 

float dbgTmp0, dbgTmp2, dbgTmp3, dbgTmp6;

////��������� � ������ ��� ����������� ������ ��������� �� �����������	//���� �� ���������
//struct printContent	
//{
//  const std::string txt;
//  const u32 y;
//  const u32 x;
//  const u32 r;
//  const u32 g;
//  const u32 b;
//  printContent(std::string &txtIn, u32 yIn, u32 xIn, u32 rIn, u32 gIn, u32 bIn) 
//      : txt(txtIn), y(yIn), x(xIn), r(rIn), g(gIn), b(bIn)
//  {
//  }
//};

//std::vector<printContent> exceptPrintVec;
std::ofstream exceptPrintFile("exceptPrintFile.txt", ios::app);

vector2 vec3To2(const vector3 &v3) { return (vector2(v3.x, v3.y)); }

CPictureObject* g_pPad;     //Paddle
CPictureObject* g_pBall;	//Ball (ori. oblect)

    //<<< class BRICK >>>// ---------------------------------------------------------------------------------------------
//#ifndef BRICK_H
//#define BRICK_H
//
//#include "globals.h"
//#include "aniobject.h"


class CBrick
{
public:
    //C-tor
    CBrick (CAniObject *const brick, const vector2 position) 
        : m_pBrickAniObj(brick), m_state(ING) 
    { 
        m_pBrickAniObj->SetPos(position);
    }

    //Types:
    static enum BRICK_STATE {	//���������: 0 - ��� ����; 1 - � ����; 2 - ������������ �������� �����������(�����); 3 - �������� ������������ ���������� (����� �������)
        OFF=0, 
        ING=1, 
        DSTR_ST=2, 
        DSTR_EN=3
    };

    //Vars (pblc):
    BRICK_STATE m_state;

    //������������ ������:
    CAniObject* getBrickAniObj() const { return m_pBrickAniObj; }
#if LOCKS ==1
    static bool bricksLockOn(const /**/char/**/);	//���-�� ���� �������
    static bool bricksLockOff(const /**/char/**/);	//���-�� ���� �������
    static char getBricksLockVal() { return m_scBricksLock; }		//�������: ������� ��� ������ 
#endif//LOCKS
	
private:
    //Vars (prvt):
    CAniObject* m_pBrickAniObj;

#if LOCKS ==1
    static /*bool*/char m_scBricksLock;	//���-�� ���� �������	0 - ������; 1 - ������ isHit()-��; 2 - ������ delBrick()-��
#endif//LOCKS
};


//#endif//BRICK_H

//---------------------

//#include "brick.h"

#if LOCKS ==1
/*bool*/char CBrick::m_scBricksLock=0;	//static member

inline bool CBrick::bricksLockOn(const /**/char cNewVal/**/) 	//`inline`, �.�. ����� ������� ����� ������������, 
                                                        //.. ����� �����-��� race-condition �� ��������
{
    if (0 == m_scBricksLock)
    {
        m_scBricksLock=/*1*/cNewVal;
        return 1;
    }
    else
        return 0;
}

inline bool CBrick::bricksLockOff(const /**/char oldVal/**/) 
{
    if (/*1*/oldVal == m_scBricksLock)
    {
        m_scBricksLock=0;
        return 1;
    }
    else
        return 0;
}
#endif//LOCKS
    //xxx END_of class BRICK xxx// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


#if DONT_DELETE_ANI_BRICKS==1
    #if NUMBER_OF_BRICKS == 12
    std::array<CAniObject*, 12> apBricksAniObjects;

    #elif NUMBER_OF_BRICKS == 2
    std::array<CAniObject*, 2> apBricksAniObjects;

    #endif//NUMBER_OF_BRICKS

    void initOfBricksAniObjects(std::vector<CBrick*> const vpBricks)
    {	
        for (char i = 0; i < vpBricks.size(); ++i)
        {
            apBricksAniObjects[i] = vpBricks[i]->getBrickAniObj();
        }
    }

#if NUMBER_OF_BRICKS == 12
    void RetrieveBrick(
            std::array<CAniObject*, 12> const apBricksAniObjects, 
            std::array<vector2, 12> const aBricksCoordsLvl
         )
#elif NUMBER_OF_BRICKS == 2
    void RetrieveBrick(
            std::array<CAniObject*, 2> const apBricksAniObjects, 
            std::array<vector2, 2> const aBricksCoordsLvl
         )
#endif//NUMBER_OF_BRICKS
    {
        for (char i = 0; i < apBricksAniObjects.size(); ++i)
            g_vBricks.push_back(new CBrick(apBricksAniObjects[i], aBricksCoordsLvl[i]));
    }
#endif//DONT_DELETE_ANI_BRICKS


	//<<< class BALL >>>// -----------------------------------------------------------------------------------------------------------
//! � �� ����� ���� ����� � ��������� ���� .h � .cpp, �� � ������� ������� �������, ��� �������������� 1 ���� controller.cpp. ������� ���� �� ���.
class CBall
{
public:
    //C-tor:
    CBall(
        CPictureObject*         const pBall, 
        std::array<vector2,uNumOfHitChckPnts>&  aBallHitChckPnts, 
        const bool              bAttachToPad
        ) 
        : m_pBallPicObj(pBall), 
        m_fVelocity(DEFAULT_BALL_VEL), 
        m_fFi(0), 
        m_aHitChckPnts(aBallHitChckPnts) 
    { 
        if (bAttachToPad)
        {
            m_spIsAttachedToPad = this; 
            locateAttachedToPadBall();	//����, ���� ������� Pad, �� ����� ��������������� � ��� �����, �� ��� ���-��� 
        }
    }

    //Vars (pblc):
    static float m_sfFiPlus;    //������������� � ��������������� ����� ����� m_fFi (���� ���� ��� �� Pad'�) ..
                                //.. � ����������� �� �������� �� �������� Pad'� ����� �������� ������ � Pad'��.

    //Methods (pblc):
    bool        tryToInvertIsAttachedToPad(); 
    void        locateAttachedToPadBall();
    DIRECTION   isHit(const float fDltPxlsToChckHit = 3/*5*/);  //���-�� `fDltPxlsToChckHit` - ����� � �������� �/� ��������� ..
                                                               //.. , ����� � �������� isHit() �������� �� ������ �������� ..
                                                              //.. ��������� rect'�� � ��������� �������� �� ����������� ������. ..
                                                             //.. ��� ���� ������, ������ ����� ��������� �������, ������, �.�. ..
                                                            //.. �������� ��������� ����� �� ��������� �������� ����� ���������� ..
                                                           //.. �������� �������������� (��. "#OPT") ��������. ������������ � ..
                                                          //.. ���������� �� isHit() ������� isHitWithPad(..) � isHitWithBricks(..).

    //������������ ������:
    vector2                 getNewMoveCenter(const DIRECTION dir);
    static CBall*           getIsAttachedToPaddle() { return m_spIsAttachedToPad; }
    CPictureObject*         getBallPicObj() { return m_pBallPicObj; }
    float                   getVel() { return m_fVelocity; }
    std::array<vector2,uNumOfHitChckPnts>&  getHitChckPnts() { return m_aHitChckPnts; }
#if DEBUG ==1
    float                   getFi() const { return m_fFi; }
#endif

private:
    //Vars (prvt):
    CPictureObject*         m_pBallPicObj;
    static CBall*           m_spIsAttachedToPad;
    float                   m_fVelocity;
    float                   m_fFi;		//���.����. ���� ����������� ����.������ == (-PI ; PI]. 0 �����-�� ����������� ����������� up.
    std::array<vector2,uNumOfHitChckPnts>&  m_aHitChckPnts;

    //Methods (prvt):
    void        computeNewFi(const DIRECTION dir);
    void        computeStartFi(
                    const float rndStartAng = 10*(float)PI/180,   //������ ����.���������� �� 0 ���������� ���� �����. ..
                                                                  //.. ������� ������ - 10 �������� �������� � �������.
                    const u32 multiplForGen = pow(10.0, 4)  //���-�� �����, ����� �������� float �-��� ..
                                                            //.. Generate() �/� u32, ������� ��� ���������.
                );
    void        normalizeFi();
    vector2     computeNewMoveCenter(u32 moveCCoordScale = pow(10.0, 3)) const;
    DIRECTION   hitReview(const DIRECTION dir) const;
    void        computeFiPlus();
    //��������������� ������ ��� isHit()
    void        isHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
    void        isHitWithPad(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    bool& needToCheckBricks, 
                    const float fMaxDltYWithPad,    //��������� ��� ������ ����� ������ �� isHit() ����. �������: ..
                                                    //.. g_pPad->GetCenter().y - pOriBall->GetCenter().y  ..
                                                    //.. ������?: if (ball[i]..->GetPos().y  >  g_fPaddleY +f MaxDltYWithPad), �� ..
                                                    //.. ������ �� ����� ������������ isHit() � Pad'�� - ����� ��� �� ������� � ����.
                    const float& fDltPxlsToChckHit      //�������� �� isHit(). ���������� ��. � isHit()
                );
    void        isHitWithBricks(
                    std::vector<DIRECTION>& vTrggrdHitChckPnts, 
                    const float& fDltPxlsToChckHit  //�������� �� isHit(). ���������� ��. � isHit()
                );
    DIRECTION   interpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const;
};

//Static members:
float  CBall::m_sfFiPlus = 0;
CBall* CBall::m_spIsAttachedToPad = nullptr;

void CBall::computeNewFi(const DIRECTION dir)
{
    if (N==dir || S==dir)	//��� �������� ���������, �.�. ������� �������� fi ��������� ��� ���� �������
    {
        m_fFi = -m_fFi + PI +m_sfFiPlus;	//������������� ���� � ����������� �� ����� ����� �� Pad'� ������ ��� S �������.
        m_sfFiPlus=0;	//������������ ����� � ��������.
    }
    else if (NE==dir || SW==dir)
        m_fFi = -m_fFi -PI/2.0;
    else if (E==dir || W==dir)
        m_fFi = -m_fFi;
    else /*if (SE==dir || NW==dir)*/	//��������� ������� ����� � �� ���������
#if DEBUG==1
        if (SE==dir || NW==dir)
#endif
            m_fFi = -m_fFi + PI/2;
#if DEBUG==1
        else throw CMyExcptn(2);
#endif

    normalizeFi();
}


void CBall::normalizeFi()
{
#if DEBUG==1
    if (2*PI <= m_fFi  ||  -2*PI >= m_fFi) throw CMyExcptn(3);
#endif
    if (PI < m_fFi)
        m_fFi=m_fFi-2*PI*(int( (m_fFi+PI)/(2*PI) ));		//����.��������� - ������� ������� ��� �� 2��, ����� ..
                                                            //.. ���-��� == (-PI ; PI].     //(m_fFi+PI) - ��� +PI, ����� ��������� ..
                                                            //.. ������� �������� ����� ����� PI � ������� m_fFi ��� ������� ������ ..
                                                            //.. �� 2*PI ������ +1. ��� �����,����� ������� m_fFi � �������� (-PI;PI].
    else if (-PI >= m_fFi)
        m_fFi=m_fFi+2*PI*(int( (m_fFi-PI)/(-2*PI) ));
}


vector2 CBall::computeNewMoveCenter(u32 moveCCoordScale/* = pow(10.0, 3) */) const  //���������������� �������� by def., ..
                                                                                   //.. ������������� � ���������� ������.
                                                                                  //���-�� `moveCCoordScale` - const ��������� ..
                                                                                 //.. ���������� ������ �������� (��-���� ����� ..
                                                                                //.. ���� ������ ������, �������, ����� ��� ����. ..
                                                                               //.. ���� �� �������). ����� ����� �������� ..
                                                                              //.. ����������� �������� ������ ����������. ..
{
    vector2 mc;
    //#OUTDATED: const u32 k = MOVE_C_COORD_SCALE;

    if (-PI/4 <= m_fFi  &&  PI/4 >= m_fFi) 	// ^ up
                                        mc.y = m_pBallPicObj->GetPos().y + moveCCoordScale,
                                        mc.x = m_pBallPicObj->GetPos().x - moveCCoordScale * tan(m_fFi);
    else if (( m_fFi > -PI  &&  m_fFi <= -3/4*PI )  ||  ( m_fFi >= 3/4*PI  &&  m_fFi <= PI )) 	// v down
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale,
                                        mc.x = m_pBallPicObj->GetPos().x + moveCCoordScale * tan(PI+m_fFi);
    else if (m_fFi > PI/4  &&  m_fFi < 3/4*PI) 	// > right
                                        mc.x = m_pBallPicObj->GetPos().x - moveCCoordScale,
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(PI-m_fFi);	//��� � � ����.����� ..
                                                                                                //.. ���� ctg, �.�. �/� tg ������� ..
                                                                                                //.. ������ � ���� �������� �����.
    else if (m_fFi > -3/4*PI  &&  m_fFi < -PI/4) 	// < left
                                        mc.x = m_pBallPicObj->GetPos().x + moveCCoordScale,
                                        mc.y = m_pBallPicObj->GetPos().y - moveCCoordScale * CTG(m_fFi);
#if DEBUG==1
    else throw CMyExcptn(1);
#endif
    return mc;
}


void CBall::computeStartFi(
        const float rndStartAng,/* = 10*(float)PI/180, */   //���������������� �������� by def., ������������� � ���������� ������.
        const u32 multiplForGen/* = pow(10.0, 4) */         //���������������� �������� by def., ������������� � ���������� ������.
     )    
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad)
        throw CMyExcptn(6);
    else
#endif
    {
        m_fFi = (float)CRndGen::GetInst().Generate(static_cast<u32>(2 * rndStartAng * multiplForGen)) 
            / multiplForGen - rndStartAng;
    }
}


vector2 CBall::getNewMoveCenter(const DIRECTION dir)
{
#if DEBUG==1
    if (NO==dir) throw CMyExcptn(5);
#endif

    if (this != m_spIsAttachedToPad)		
    {
        m_pBallPicObj->StopMoveTo();	///@@@ ��.��. �� �����
        computeNewFi(dir);
    }
    else /*if (this == m_spIsAttachedToPad)*/ computeStartFi();
    return computeNewMoveCenter();
}


bool CBall::tryToInvertIsAttachedToPad() 
{ 
    if (this == m_spIsAttachedToPad)	        { m_spIsAttachedToPad = nullptr; return 1; }
    else if (m_spIsAttachedToPad == nullptr)    { m_spIsAttachedToPad = this; return 1; }
    else /*if other ball is attached*/	return 0; 
}


void CBall::locateAttachedToPadBall()
{
#if DEBUG==1
    if (this != m_spIsAttachedToPad) throw CMyExcptn(9);
#endif
    m_pBallPicObj->SetPos( g_pPad->GetCenter().x - m_pBallPicObj->GetSize().x / 2,   
                           g_fPaddleY - m_pBallPicObj->GetSize().y - 1 );
}

//������� �������� (m_sfFiPlus) � ���� ��������� (m_fFi) � ����������� �� �������� ����� �������� � Pad'�� �� �������� ����������
void CBall::computeFiPlus()
{
    float dlt = m_pBallPicObj->GetCenter().x - g_pPad->GetCenter().x;	//������� ���������� ������ �� ������ Pad'� ��� �����
    float weightedDlt = dlt/g_pPad->GetSize().x/2; //delta ���������� �� ��������� � 1/2 ����� Pad'� (�.�. ���������� � �������� [0, 1]
    m_sfFiPlus= (PI/2/*4*/)*weightedDlt*((fabs(m_fFi)-PI/2)/PI/2);	//������������ ������������� � ���� == PI/4, �������������� ..
                                                                   //.. ���������� dlt'�� � �������� m_fFi � PI/2, ���������� � ..
                                                                  //.. ����� PI/2 (�.�. ���� [0, 1]). ��� m_fFi ==����� PI/2 ..
                                                                 //.. (3,1417/2) ���� ����� �� ����� ���������� < PI/2. �.�. ..
                                                                //.. ������������� ���������� - ����� �� ������� ���� ��-�� ��.
}


void CBall::isHitWithScreenBorders(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (0  >=  m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y)     vTrggrdHitChckPnts.push_back(N);
    if (0  >=  m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x)     vTrggrdHitChckPnts.push_back(W);
    else if (m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  >=  g_fSceneWidth)    vTrggrdHitChckPnts.push_back(E);
}


void CBall::isHitWithPad(
        std::vector<DIRECTION>& vTrggrdHitChckPnts,  
        bool& needToCheckBricks, 
        const float fMaxDltYWithPad,  
        const float& fDltPxlsToChckHit
     )
{
    //#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //��� ���� ������, ������ ����� ��������� �������, ������, �.�. �������� ��������� ����� ..
    //                                        //.. �� ��������� �������� ����� ���������� �������� �������������� (��. "#OPT") ��������.

    //����������� (����� #OPT): ���� ����� ���� ����������� Pad'�, ��� ���� ������ ��� ���� (����������� Pad'a +10), ..
    //.. �� �������� � Pad'�� ������ �� ����� - �������� ������� ���.�������� ������� �� 2�� (Pad ������ 1��)
    if ( m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y  <  g_fPaddleY - fDltPxlsToChckHit  ||	
         m_pBallPicObj->GetPos().y  >  g_fPaddleY + fMaxDltYWithPad  ||   //if ����� ������ ���� �������� Pad'�,�� ��� game over.
         m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x  <  g_pPad->GetPos().x - fDltPxlsToChckHit  ||
         m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x  >  g_pPad->GetPos().x + g_pPad->GetSize().x + fDltPxlsToChckHit )
         /*�� ������. �������, �� ����� ���������, ����� - ���������*/;
    else
    {
        for (int d=E/*2*/; d<=W/*6*/; ++d)	// �������� [2, 6] == [E, W] (��������� ����� ��������� ��� Pad'� �� �����)
        {                                   //#WARNING(possibly). �������� �������� implicit �������������� ����� �����.
            if ( g_pPad->Hit(m_aHitChckPnts[d] + vec3To2(m_pBallPicObj->GetPos())/*,0*/) )
            {
                vTrggrdHitChckPnts.push_back(static_cast<DIRECTION>(d));
                needToCheckBricks=0;
                //������� �������� (m_sfFiPlus) � ���� (m_fFi) � ����������� �� �������� ����� �������� � Pad'�� �� �������� Pad'�:
                if (S == vTrggrdHitChckPnts.back())	//�� ����������� ����, ���� ������� �� ���� ��� �����.�����������
                    computeFiPlus();
            }
        }
    }
}


void CBall::isHitWithBricks(std::vector<DIRECTION>& vTrggrdHitChckPnts,  const float& fDltPxlsToChckHit)
{
#if LOCKS==1
        while (CBrick::bricksLockOn(1) != 1)	///In isHit()
        {	/*������� ����� �� ��������� � g_vBricks. ���� ��� ������� ���-�� ������, ���*/
#if LOW_LVL_LOCKS==1		
            //�������: �������� �������� ��� ��������
            if (CBrick::bricksLockOn(1) != 1)
                return NO;
            else
                break;
#endif//LOW_LVL_LOCKS
        }
#endif//LOCKS

//#OUTDATED: const float dlt = DLT_PXLS_TO_CHCK_HIT; //��� ���� ������, ������ ����� ��������� �������, ������, �.�. �������� ��������� ����� ..
//                                        //.. �� ��������� �������� ����� ���������� �������� �������������� (��. "#OPT") ��������.

        //for (auto it = g_vBricks.begin(); it!=g_vBricks.end(); ++it)	//���� �������� � ���������� ���������� ��-�� ���������������
        //for (auto it = g_vBricks.rbegin(); it!=g_vBricks.rend(); ++it)	///@@@ ��������� ITERAOR N
        //for (char ib = g_vBricks.size()-1; ib>=0; --ib)
        for (char ib = 0; ib < g_vBricks.size(); ++ib)	//�������
        {		
            /*�������*/ CAniObject ao = g_vBricks[ib]->getBrickAniObj();	//�������: ������ ��������� ���� ����� ������� - �� ��������
            //CAniObject *ao = (*it)->getBrickAniObj();	//��� ������������
            //CAniObject ao (g_vBricks[ib]->getBrickAniObj());	//��� ������������
                //if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao->GetPos().y -dlt  ||	//Opt.: �������� ����������� ����������� ���������������
                //m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao->GetPos().y + ao->GetSize().y +dlt ||	//opt
                //m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao->GetPos().x -dlt  ||					//opt
                //m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao->GetPos().x + ao->GetSize().x +dlt)	//opt
            //#OPT: �������� ����������� ����������� ���������������:
            if (m_aHitChckPnts[S].y + m_pBallPicObj->GetPos().y < ao.GetPos().y -fDltPxlsToChckHit  ||	
                m_aHitChckPnts[N].y + m_pBallPicObj->GetPos().y > ao.GetPos().y + ao.GetSize().y +fDltPxlsToChckHit || 
                m_aHitChckPnts[E].x + m_pBallPicObj->GetPos().x < ao.GetPos().x -fDltPxlsToChckHit  || 
                m_aHitChckPnts[W].x + m_pBallPicObj->GetPos().x > ao.GetPos().x + ao.GetSize().x +fDltPxlsToChckHit) 
                continue;
	
            for (u32 i=0; i<uNumOfHitChckPnts; ++i)
            {
                if ( /*ao->*/ao.Hit(m_aHitChckPnts[i] + vec3To2(m_pBallPicObj->GetPos())/*,0*/) )
                {
                    vTrggrdHitChckPnts.push_back(static_cast<DIRECTION>(i));
                    if (/*(*it)*/ g_vBricks[ib]/**/->m_state == CBrick::ING)
                        /*(*it)*/ g_vBricks[ib]/**/->m_state=CBrick::DSTR_ST;
                }
            }
        }
#if LOCKS==1
#if DEBUG==1
        if (CBrick::bricksLockOff(1)==0) throw CMyExcptn(16); /*�������� ����� �� ��������� � g_vBricks. ���� ��� ������� ���-�� ������, ���*/
#else
        CBrick::bricksLockOff(1)
#endif//DEBUG
#endif//LOCKS
}


//������������� � ������������� �����������
DIRECTION CBall::interpretIsHitResults(std::vector<DIRECTION>& vTrggrdHitChckPnts) const
{
    if (vTrggrdHitChckPnts.empty()) return NO;	//��� ��������

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	
        // �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1� �� 2� �����.������� ������.��������
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2� �� 2� �����.������� ������.��������	//��� ������ ����� ������� ���� ����������, �� �������� ��� ��������� ������� ����
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//�.�. if �������.������ ��������
    else throw CMyExcptn(11);	//�� ����� - �������� �� .empty() ���� � ������

    return hitReview(dir);
}


DIRECTION CBall::isHit(const float fDltPxlsToChckHit/* = 3*/)   //���������������� ����-� by def., ������������� � ���������� ������.
{																		///@@@ !!! �������������� !!!
    if (this == m_spIsAttachedToPad  ||  RUN != gameState) return NO;
    bool needToCheckBricks=1;	//��� ��� ����� ��� ��������� �������� ��������: �.�. ������� ����������� ����������, ��, ..
                                //.. ���� �������� ������������ � Pad'��, �� �������� �������� ������������.
    std::vector<DIRECTION> vTrggrdHitChckPnts;	///@@@����� ����� ������� ������� ��� ����� ��������� (��.��. 2 ���������� ��) - ��� - �� �����������.

//����� ������������ � <^> ������ ������
    isHitWithScreenBorders(vTrggrdHitChckPnts);

//����� ������������ � Pad'��
    isHitWithPad(vTrggrdHitChckPnts, needToCheckBricks, g_pPad->GetCenter().y - g_pBall->GetCenter().y, fDltPxlsToChckHit);

//����� ������������ � ���������
    if (needToCheckBricks)	//��.��������� ���� - � �������� ���� ����������
        isHitWithBricks(vTrggrdHitChckPnts, fDltPxlsToChckHit);

//������������� � ������������� �����������
    interpretIsHitResults(vTrggrdHitChckPnts);
    if (vTrggrdHitChckPnts.empty()) return NO;	//��� ��������

    DIRECTION dir;
    if (vTrggrdHitChckPnts.size()==2)
    {
        if (abs(vTrggrdHitChckPnts[1]-vTrggrdHitChckPnts[0])==2)	dir= static_cast<DIRECTION>(vTrggrdHitChckPnts[0]+1);	// �.�. if ��������� 2 ������� �/� 1	//�� ������, �.�. ����� ���������� ���������� ������ �������� � ������ ������, ������� ����������� �������� � ������� ����� ���� �����
        else if ( (vTrggrdHitChckPnts[0]==N && vTrggrdHitChckPnts[1]==W) || (vTrggrdHitChckPnts[0]==W && vTrggrdHitChckPnts[1]==N) )		dir= NW;			//1� �� 2� �����.������� ������.��������
        else if ( (vTrggrdHitChckPnts[0]==NE && vTrggrdHitChckPnts[1]==NW) /**/|| (vTrggrdHitChckPnts[0]==NW && vTrggrdHitChckPnts[1]==NE) /**/)	dir= N;		//2� �� 2� �����.������� ������.��������	//��� ������ ����� ������� ���� ����������, �� �������� ��� ��������� ������� ����
#if DEBUG==1
        else throw CMyExcptn(7);
#endif
    }

#if DEBUG==1
    else if (vTrggrdHitChckPnts.size()>2) throw CMyExcptn(4);		//assert(vHP.size()<=2);
#endif
    else /**/if (!vTrggrdHitChckPnts.empty())/**/ dir= vTrggrdHitChckPnts[0];	//�.�. if �������.������ ��������
    else throw CMyExcptn(11);	//�� ����� - �������� �� .empty() ���� � ������

    return hitReview(dir);
}

//�������� �� ��������� ����������� ��� ������������� ��������, �������� ���������� �� �������� ��� ��� ���������
DIRECTION CBall::hitReview(DIRECTION dir) const
{
#if DEBUG==1
    if (m_fFi>PI || m_fFi<= -PI) throw CMyExcptn(12);
#endif
    switch (dir)
    {
        case N:  if (m_fFi>= PI/2 || m_fFi<=-PI/2) dir=NO; break;
        case NE: if (m_fFi>= PI*3/4 || m_fFi<=-PI/4) dir=NO; break;
        case E:  if (m_fFi == PI || m_fFi<= 0) dir=NO; break;
        case SE: if (m_fFi>= -PI*3/4 && m_fFi<= PI/4) dir=NO; break;
        case S:  if (m_fFi>= -PI/2 && m_fFi<= PI/2) dir=NO; break;
        case SW: if (m_fFi>= -PI/4 && m_fFi<= PI*3/4) dir=NO; break;
        case W:  if (m_fFi >= 0) dir=NO; break;
        case NW: if (m_fFi>= PI/4 || m_fFi<= -PI*3/4) dir=NO; break;
        default: throw CMyExcptn(13);
    }
    return dir;
}
    //xxx END_of class BALL xxx// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


//������ ����� � ����
void addBall(std::array<vector2,uNumOfHitChckPnts> &ballHitChckPnts, bool attachToPad)		//CPictureObject'� ��� ��� 4� ������� ������� � xml � � obj.h, �� ����� �� �� ��������� �������� ���.����, ���� �� �����. �����.���������� �������� � xml, ������� ��� �� �����
{
    if (g_vBalls.size() < MAX_BALL_NUM)
    {
        for (u32 i = 0; i<MAX_BALL_NUM; ++i)
            if ( ballsPictObjectsUsage[i] == 0 )
            {
                g_vBalls.push_back(new CBall(ballsPictObjects[i], ballHitChckPnts, attachToPad));
                ballsPictObjectsUsage[i]=1;
                break; //!!!
            }
    }
}

//������� ����� �� ����
void delBall(u32 i)	//was (std::vector<CBall*>::iterator it)
{
#if DEBUG==1
    if (g_vBalls.size()<=i) throw CMyExcptn(10);
#endif

        g_vBalls[i]->getBallPicObj()->StopMoveTo();
        g_vBalls[i]->getBallPicObj()->SetPos(-200,-200);
        //�������� CPict.Obj. ������� ������ ��� ��������������:
        for (auto it2 = ballsPictObjects.begin(); it2!=ballsPictObjects.end(); ++it2)
        {
            if ( g_vBalls[i]->getBallPicObj() == (*it2) )
            {
                ballsPictObjectsUsage[std::distance(ballsPictObjects.begin(), it2)]=false;	
                break; /*�� ����*/
            }
        }

	
        delete g_vBalls[i];
        g_vBalls.erase(g_vBalls.begin()+i);
	
        if (g_vBalls.empty() && gameState==RUN)
            gameState = LOST;		//`processWinLostState();` �� ������� - ����������� � OnRender(), ����� ��, ��� ������, ��������� "�� ������"
}

void addBrick(CAniObject* baseBrick, vector2 position, CScene* sc)
{
    if (!g_vBricks.empty())	//���� ��� ������ ������, �� ������ �� ������ ����� ����������, � ���� ��� �������, ������� �������� �� xml
    {
        g_vBricks.push_back(new CBrick(new CAniObject(*baseBrick), position));
        sc->AddAni(g_vBricks./*front()*/back()->getBrickAniObj()/**/, 1/**/);	//�������� ���-������ � ����� 
        ///@@@ ���^ ��������� ������� iscopy - �����������, ��� ���
    }
    else
        g_vBricks.push_back(new CBrick(baseBrick, position));
}

void delBrick(u32 i, CScene* sc)
{

#if LOCKS==1
    //�������: ������ ���� �� ������� �����
    while (CBrick::bricksLockOn(2) != 1)	//������� �����, ����� ����� �� ����������� ���� ����������� � ���������
    {

#if LOW_LVL_LOCKS==1
        //if ( CBrick::getBricksLockVal() == 2) break;	//�������: ������� ��� ����� ������ �������
        if ( CBrick::bricksLockOn(2) != 1) 
            return;
        else
            break;
#endif//LOW_LVL_LOCKS

    }
#endif//LOCKS

#if DEBUG==1
    if (g_vBricks.size()<=i) throw CMyExcptn(14);
#endif

    if (g_vBricks[i]->m_state == CBrick::DSTR_ST)
    {
        g_vBricks[i]->getBrickAniObj()->StartMovement(MV_SC01_BRICK);
        g_vBricks[i]->m_state = CBrick::DSTR_EN;


        //�������: ��� ��� ����� ����� ������ ���� bricksLockOn() �������� ���� ����� �����. ����� �������, ������ ���.
#if LOCKS==1
#if DEBUG==1
        if (CBrick::bricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::bricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

        return;
    }
    if (g_vBricks[i]->m_state == CBrick::DSTR_EN  &&  g_vBricks[i]->getBrickAniObj()->IsStopped())
    {
        g_vBricks[i]->m_state = CBrick::OFF;	/*�������*/
//#if LOCKS==1
//      while (CBrick::bricksLockOn() != 1)	//������� �����, ����� ����� �� ����������� ���� ����������� � ���������
//      {
//      }
//#endif

#if DONT_DELETE_ANI_BRICKS==0
        //���� ��� �� ������� ������
        if (i!=0)
        {
            sc->RemoveAni(g_vBricks[i]->getBrickAniObj());	//������� ���-������ �� �����
            delete g_vBricks[i]->getBrickAniObj();
        }

        //���� ��� ������ (�������) ������, �� ����������� �������� - ������ 2 ������ ��� ���� ��������� ������, (�.�. ��������� ��� ����� - ����� �� ��������� �������� ����� ��� ��������) �� � �������� ��� �� �
        else
#endif//DONT_DELETE_ANI_BRICKS
        {
            g_vBricks[i]->getBrickAniObj()->SetPos(-300,-300); //������ �� �������� ����
            g_vBricks[i]->getBrickAniObj()->SetStatic(/*ANI_SC01_BRICK*/ST_SC01_BRICK_BEGIN);
            g_vBricks[i]->m_state = CBrick::OFF;
        }
		
        //������ ��� ����������� ��� ������ �������
        delete g_vBricks[i];

        g_vBricks.erase(g_vBricks.begin()+i);	//������� - �������� ������ ���� ������ �������� ����� ��������� � ��������� ��� 1�� ��-��� - �������� �� ����
        ////�������: ����� ��������� ����� � ���������
        //if (g_vBricks.size()>1)
        //{
        //  //CBrick *tmpB = *(g_vBricks.begin()+i);
        //  //*(g_vBricks.begin()+i)=*(g_vBricks.end()-1);
        //  //*(g_vBricks.end()-1) = tmpB;
        //  //g_vBricks.erase(g_vBricks.end()-1);
        //  CBrick *tmpB = *(g_vBricks.begin()+i);
        //  *(g_vBricks.begin()+i)=*(g_vBricks.begin());
        //  *(g_vBricks.begin()) = tmpB;
        //  g_vBricks.erase(g_vBricks.begin());
        //}

        m_uGameScore+=10;

        if (g_vBricks.empty()  &&  gameState==RUN)
        {
            gameState=WON;
        }
//#if LOCKS==1
//#if DEBUG==1
//      if (CBrick::bricksLockOff(2)==0) throw CMyExcptn(17);
//#else
//      CBrick::bricksLockOff(2)
//#endif
//#endif
    }

//�������: ������ ��� ������
#if LOCKS==1
#if DEBUG==1
        if (CBrick::bricksLockOff(2)==0) throw CMyExcptn(17);
#else
        CBrick::bricksLockOff(2)
#endif//DEBUG
#endif//LOCKS

}


/////////////// PRINTS /////////////// PRINTS /////////////// PRINTS /////////////// 
#if DEBUG==1 //! ���������� ��� Debug-������																																

template <typename T> void dbgPrint(const std::string &txt, const T &var, const u32 y, const u32 x=10,
                                    const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100)	
{ 																									
    static std::ostringstream strStream; 												
    strStream.str("");	 											
    strStream<< txt<< var; 																
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																			

template <typename T1, typename T2> void dbgPrint(const std::string &txt, const T1 &var1, const T2 &var2,	
                                                  const std::string &delim, const u32 y, const u32 x=10,
                                                  const u32 r=10, const u32 g=10, const u32 b=10, const u32 a=100)	
{															
    static std::ostringstream strStream;										
    strStream.str("");																	
    strStream<< txt<< var1<< delim<< var2; 						
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());		
}																								

void dbgPrint(const std::string &txt, const u32 y, const u32 x=10,
              const u32 r=1, const u32 g=255, const u32 b=255, const u32 a=100)		
{ 																			
    static std::ostringstream strStream; 									
    strStream.str("");	 										
    strStream<< txt; 															
    GET_FONT("dbg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());	
}															

#endif												

void print(const std::string &txt, const u32 y, const u32 x/*=10*/,
            const u32 r/*=10*/, const u32 g/*=220*/, const u32 b/*=220*/, const u32 a/*=100*/)	//��� �������.�������� ������ � ���������� (��. ����)
{ 													
    static std::ostringstream strStream; 						
    strStream.str("");	 					
    strStream<< txt; 								
    GET_FONT("dlg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());
}

template <typename T> void print(const std::string &txt, const T &var, const u32 y, const u32 x/*=10*/,
                                 const u32 r/*=10*/, const u32 g/*=10*/, const u32 b/*=10*/, const u32 a/*=100*/)
{ 												
    static std::ostringstream strStream; 		
    strStream.str("");	 	//strStream.flush() �� ��������
    strStream<< txt<< var; 									
    GET_FONT("dlg.xml")->Print(/*DEF_COLOR*/color(r,g,b,a),x,y,0,strStream.str().c_str());
}
/////////////// END of PRINTS /////////////// END of PRINTS /////////////// END of PRINTS /////////////// 


//Delay ��� gameOver()
void sleepMy(int seconds)
{
    clock_t time_end;
    time_end = clock() + seconds * CLOCKS_PER_SEC;
    while (clock() < time_end)
    {
    }
}

//����� ���������� ������� � OnShow() � ����� ����� ������� WIN'�
void initNewGame(CScene *sc)
{
#if DONT_DELETE_ANI_BRICKS==1
    if (gameState==NEW)
#else
    if (gameState!=LOST)
#endif
    {
        addBrick(sc->FindAniObject(ANI_SC01_BRICK), bricksCoordsLvl1[0], sc);
        for (u32 ibr=1; ibr < BRICKS_ON_LEVEL[0]; ++ibr)	//frm "1", �.�. 0� ������ ������ �������� - �� �������
        {
            addBrick(g_vBricks[0]->getBrickAniObj(), bricksCoordsLvl1[ibr], sc);
        }
        /*����� ������� �������� ��� ������ �� ���.���-�� 12 � ��������� �����. ���������*/
        //������ �������� ��� ����� ��������� �������� � xml � ��������� �� ���� ��� ���������� ���������� �� ������.
#if DONT_DELETE_ANI_BRICKS==1
        initOfBricksAniObjects(g_vBricks);
#endif
    }
#if DONT_DELETE_ANI_BRICKS==1
    else if (gameState==WON)
    {
        RetrieveBrick(apBricksAniObjects, bricksCoordsLvl1);
    }
#endif
    //����� ��� ����������� � ��� "LOST" � ��� "WON" � ��� 1�� �������
    addBall(aBallHitChckPnts, true);
    m_uGameScore=0;
    gameState = RUN;
}


char waitForClick=0;	//0 ��������� ��-���������; 1 - ���������� ����� processWinLostState() � �� �������� ������ ������������ �����, ���� OnMouseKeyDown() �� ������� �� 2; 2 - ��������, � ������� � ������ processWinLostState() ����������� ��������, ������������ ������ �� ����� ������ (� ����� ������������ ����� ��������� �������� 0)
void processWinLostState(CScene *sc)
{
    if (gameState==RUN)
        return;

    //GameOver
    if (gameState==LOST)
    {
        print("G A M E   O V E R", (g_fSceneHeight-15)/2, (g_fSceneWidth-100)/2, 1,250,250, 100);
        print("Click to continue", (g_fSceneHeight+40)/2,g_fSceneWidth*2/5+40, 40,40,40, 100);
		
        //�������� ����� (���������� �������� ��.� ����� ����������� ����������)
        if (waitForClick==0)
            waitForClick=1;
        if (waitForClick==1)
            return;

        sleepMy(5);///@@@@ ������� �� �������� ������-�� ����������. ����������� ������ ��� � ����, � � OnRender() ��������� ����, � �������� �� �����. ���� �������� waitForClick()'��
        print("", (g_fSceneHeight-10)/2, (g_fSceneWidth-30)/2, 10,220,220, 100);
        //ballsPictObjectsUsage.fill(0);	//��� ������
        initNewGame(sc);

        waitForClick=0;
    }
	
    //Victory
    else if (gameState==WON)
    {
        print("C O N G R A T U L A T I O N S!   Y O U   W O N!  SCORE IS: ", m_uGameScore, (g_fSceneHeight-15)/2,g_fSceneWidth/4, 220,10,220, 100);
        print("Click to continue", (g_fSceneHeight+40)/2,g_fSceneWidth*2/5+40, 40,40,40, 100);
        //������� ��� ������ � ����� (����������, ����� ����.������� �� ���� ������ �� ����
#if DEBUG==1
        if (!g_vBricks.empty()) throw CMyExcptn(10);
#endif

        if (!g_vBalls.empty())
            for (char i=g_vBalls.size()-1;  i>=0; --i)
                delBall(i);

		//�������� ����� (���������� �������� ��.� ����� ����������� ����������)
        if (waitForClick==0)
            waitForClick=1;
        if (waitForClick==1)
            return;
		
        initNewGame(sc);
        print("", (g_fSceneHeight-10)/2, (g_fSceneWidth-30)/2, 220,10,220, 100);

        waitForClick=0;
    }
}																											



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



CArkanoidController::CArkanoidController(u32 sceneId)
:ISceneController(sceneId), m_uGameScore(0)
{
}



CArkanoidController::~CArkanoidController()
{
}


	// OnShow() //
void CArkanoidController::OnShow()
try {
    g_fSceneHeight = CRender::GetInst().GetHeight()/*768*/;
    g_fSceneWidth = CRender::GetInst().GetWidth()/*1024*/;
    CScene *sc = m_scene;
    InitBallHitChckPntsArr();
    InitOfBallsPictObjects(sc);	//������������� ������ CPict.Object'�� �������
    g_pPad = sc->FindPictureObject(PIC_SC01_PADDLE);
    g_pBall = sc->FindPictureObject(PIC_SC01_BALL);
//#OUTDATED:    CPictureObject* pOriBall = sc->FindPictureObject(PIC_SC01_BALL);  ///@@@ ����� ������� � ����. vars (��� �����, ���� ���, ��� � oriBrick �� ������������� � ����� ����
    g_fPaddleY1 = g_fSceneHeight - BOTTOM_GAP - (u32)g_pPad->GetSize().y;  //#COMPILER(Warning) `(u32)`
    g_fPaddleY = g_fPaddleY1;
//#OUTDATED:    g_fMaxDltYWithPad = g_pPad->GetCenter().y - pOriBall->GetCenter().y;   //if(ball[i]..->GetPos().y > g_fPaddleY+g_fMaxDltYWithPad) ..
//                                                                            //.. �� ������ �� ������������ isHit() � Pad'��.
    std::srand((int)time(NULL));		//��� ����, ����� rand() ������� �� ���� � �� ��
    InitFewPixelsShift();	//������������� ������� 1-���������� �������� ������ ��� ��������
	
    initNewGame(sc);
}
EXCPTN_CATCH



	// OnUpdate() //
void CArkanoidController::OnUpdate(u32 _dt)
try {

    //�������: ������ ���� �� OnRender()
    //������� �����, ������������ �������� ����������� � 1� ������ � ��������� ������ �� ������
    for (char i=g_vBricks.size()-1; i>=0; --i)	//�������: ��������� ��� � OnUpdate() - �� �������
    {
	    if (g_vBricks[i]->m_state == CBrick::DSTR_ST || g_vBricks[i]->m_state == CBrick::DSTR_EN)
		    delBrick(i, /*sc*/m_scene);
    }

    //������� ������� ������ CBall ���������� �� ������� ���� �������
    if (!g_vBalls.empty())	//tmp: using ITERATOR 2
    {
        for (char i=g_vBalls.size()-1;  i >= 0;  --i) //�� ����������, �.�. ������ ���������� ��� .erase()	//(auto it=g_vBalls.begin();  it != g_vBalls.end();  ++it)
        {
            CPictureObject *bo = g_vBalls[i]->getBallPicObj();
            if (bo->GetPos().x < -bo->GetSize().x -5  ||  bo->GetPos().x > g_fSceneWidth +5  ||  
                bo->GetPos().y < -bo->GetSize().y -5  ||  bo->GetPos().y > g_fSceneHeight +5)
            {
                delBall(i);	//������� ��������� �������� � ������ delBall()
                //�� ��������� ��� ���������, � ��������� � �������� �������, ������ ��� ������ delBall() ��������� it �, ==> ���������� ������ �������, �������� ���� ����� ����������������, �.�. ��������� ��� ���������������.
            }
        }
    }

    ////������� �������� � �������� ����������
    //DIRECTION dir;	//�������: ������ ������ � OnRender() - �� �������
    //for (auto itb=g_vBalls.begin(); itb!= g_vBalls.end(); ++itb)	///@@@ ������������� ITERATOR'� 
    //{
    //	dir=(*itb)->isHit();
    //	if (dir != -1)
    //	{
    //		(*itb)->getBallPicObj()->MoveToPolarRo(FLT_MAX,  (*itb)->getNewMoveCenter(dir),  (*itb)->getVel());
    //		//������� ����� �� 2-3������z, ����� ����� �� ���.��������. ���������� ������ � ������������ ��. � ����.� �������� ������ initOf1PixelShift()
    //		(*itb)->getBallPicObj()->SetPos( vec3To2((*itb)->getBallPicObj()->GetPos()) + mFewPixelsShift[dir] );
    //	}
    //}
}
EXCPTN_CATCH



    // OnRender() //
void CArkanoidController::OnRender(vector2 _scroll)
try {
    LOG_INFO("CArkanoidController::OnRender");
    vector2 pos = CCursorController::GetInst().GetCursorPos();

    CScene *sc = m_scene;

    ////������� �����, ������������ �������� ����������� � 1� ������ � ��������� ������ �� ������
    //for (char i=g_vBricks.size()-1; i>=0; --i)	//�������: ��������� ��� � OnUpdate() - �� �������
    //{
    //	if (g_vBricks[i]->m_state == CBrick::DSTR_ST || g_vBricks[i]->m_state == CBrick::DSTR_EN)
    //		delBrick(i, sc);
    //}

    processWinLostState(sc);

    print("SCORE: ", m_uGameScore, 15,g_fSceneWidth-100, 240,10,240,100);

#if DEBUG==1																										
    CPictureObject* ball1 = sc->FindPictureObject(PIC_SC01_BALL);
    //dbgPrint("rndmAng1 = ", dbgTmp0, 10,10, 255,10,255);
    if (!g_vBalls.empty())
	    dbgPrint("currBallFi = ", g_vBalls[0]->getFi(), 10,200, 255,10,255);
    dbgPrint("currMouse X : Y = ", pos.x, pos.y, " : ", 25,10, 10,10,10);
	
    dbgPrint("ball1->GetPos().x : .y    ", ball1->GetPos().x, ball1->GetPos().y, " : ", 300);
    //dbgPrint("rand()%... :   ", dbgTmp6, 355,10, 50,50,1);
#endif						

    //�������: ������ ���� �� OnUpdate()
    //������� �������� � �������� ����������
    DIRECTION dir;
    for (auto itb=g_vBalls.begin(); itb!= g_vBalls.end(); ++itb)	///@@@ ������������� ITERATOR'� 
    {
        dir=(*itb)->isHit();
        if (dir != -1)
        {
            (*itb)->getBallPicObj()->MoveToPolarRo(FLT_MAX,  (*itb)->getNewMoveCenter(dir),  (*itb)->getVel());
           //������� ����� �� 2-3������z, ����� ����� �� ���.��������. ���������� ������ � ������������ ��. � ����.� �������� ������ initOf1PixelShift()
           (*itb)->getBallPicObj()->SetPos( vec3To2((*itb)->getBallPicObj()->GetPos()) + mFewPixelsShift[dir] );
        }
    }

}
EXCPTN_CATCH



void CArkanoidController::OnMouseMove(u32 _x, u32 _y)
try {	///@@@ ��������� ������ ���� � ������ paddle'� � �������� �����������, ����� �� ������������ � ����� ����. � ����� ���������������� ��� ��������. � ������� ���� ���� ��� � ��������� �-���.
    CPictureObject* pad = m_scene->FindPictureObject(PIC_SC01_PADDLE);
	
    //! Move Pad
    if (_x > g_fSceneWidth - pad->GetSize().x)
        pad->SetPos((float)g_fSceneWidth - pad->GetSize().x, (float)g_fPaddleY);									//w(tc)
    else
        pad->SetPos((float)_x, (float)g_fPaddleY);															//S$-c

    //! Move Ball with Pad
    if (CBall::getIsAttachedToPaddle())	//���� �������� �������� ����� �� ������ �� ����� �� ����.��������� ������������� �-���, �.�. ����� ��� �� ����� ����������� 
    {
        CPictureObject *bo = CBall::getIsAttachedToPaddle()->getBallPicObj();
        bo->SetPos(pad->GetCenter().x - bo->GetSize().x/2,   g_fPaddleY - bo->GetSize().y-1 );
    }
}
EXCPTN_CATCH



bool CArkanoidController::OnMouseKeyDown(u32 _key, u32 _x, u32 _y)
try {
    if (0 == _key)
    {
        if (waitForClick==1)
	        waitForClick=2;

        /**///m_scene->FindAniObject(ANI_SC01_BRICK)->StartMovement(MV_SC01_BRICK);

        if (CBall::getIsAttachedToPaddle()) 
        {
            CBall *b = CBall::getIsAttachedToPaddle();
            CPictureObject* bo = b->getBallPicObj();
            bo->MoveToPolarRo(FLT_MAX, b->getNewMoveCenter(S), b->getVel());	
            b->tryToInvertIsAttachedToPad();
        }
#if DEBUG==1
        dbgTmp2 = static_cast<float>( CRndGen::GetInst().Generate(/*0U, */static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180);
        dbgTmp3 = static_cast<float>( CRndGen::GetInst().Generate(/*0U, */static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180);
        dbgTmp6 = static_cast<float>( std::rand()%static_cast<u32>(2*10*(float)PI/180*pow(10.0, 4)) ) / pow(10.0, 4) - 10*(float)PI/180;
#endif
    }
    return false;
}
EXCPTN_CATCH