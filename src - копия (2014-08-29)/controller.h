////////////////////////////////////////////////////////////////
//
//  controller.h: a part of 2DTemlate application
//  authors: Yulia Malyarevich
//  �2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#ifndef DT_CTRL_H 
#define DT_CTRL_H 



#include <array>
#include "types.h"
#include "game.h"
#include "findobj.h"
#include "random.h"
#include "globals.h"

//class CBallsManager;
//class CBricksManager;



class CArkanoidController: public ISceneController
{
public:
    //Datas (pblc):
    
    //#OUTDATED: �.�. arrays, � ������� ���-��� ���� ��� data, ������� vector'���, �.�. ��.�����.p.(+10), �� ���������� � ���� data ���� ��������. ���� ������ �����, �� ����� ����� ����� ��� .size() ������� ��������� ������, ����.
    //const u32   m_uNUM_OF_BRICKS_ON_LVL1;     //���-�� �������� �� ��.1. �������� �� ����������, �.�. ��� ��� ������ ����� ��� ..
                                                //.. ������������� vector'�� / array'��, � ������ "< >" ����-� [] ��������. ..
                                                //.. Init. in C-tor.
                                                ///@@@ �������� � ��������, �� ���������, ��� �� ���. ��� ��� ������ �/ �������� � C-tor BricksManager'a

    //Methods (pblc):
	CArkanoidController(u32 sceneId);   //C-tor
	virtual ~CArkanoidController();     //D-tor
	void OnShow();
	void OnUpdate(u32 _dt);
	void OnRender(vector2 _scroll);
	bool OnMouseKeyDown(u32 _key, u32 _x, u32 _y);
	void OnMouseMove(u32 _x, u32 _y);

protected:
	CFont* m_font;
	vector2 m_start;
	vector2 m_end;

    //Types (prtctd):
    class CBallsManager;
    class CBricksManager;

    //Datas (prtctd):
    ///@@@ #TODO: ���� �� �����.����� � ���-��� array. ��.��.����������� �������� � ���-��� 0U, � ������� ptr'�� �� array, � �������-�� �����
    ///@@@ #TODO: �������������� ������ �� �-��� ������ (150�50)
    //2 - (��� > �������� ������); � 12 - ��� ������������� 

//#OUTDATED:
//#if NUMBER_OF_BRICKS == 2
//    std::array<vector2, /*m_uNUM_OF_BRICKS_ON_LVL1*/2> m_aBricksCoordsLvl1 = 	
//        {   vector2( 59, 140),	//0
//            vector2(210, 140)	//1
//        };
//
//#elif NUMBER_OF_BRICKS == 12
//    std::array<vector2, /*m_uNUM_OF_BRICKS_ON_LVL1*/12> m_aBricksCoordsLvl1 = 
//        {   vector2( 59, 140),	//0
//            vector2(210, 140),	//1
//            vector2(361, 140),	//2
//            vector2(512, 140),	//3
//            vector2(663, 140),	//4
//            vector2(814, 140),	//5
//            vector2( 59,  89),	//6(3)
//            vector2(361,  89),	//7(3)
//            vector2(663,  89),	//8(3)
//            vector2(210, 191),	//9(1)
//            vector2(512, 191),	//10(1)
//            vector2(814, 191),	//11(1)
//        };
//#endif 
    ///@@@@@ STOPED HERE (27/08/14)
    std::vector< std::vector<vector2>* > m_vpvBricksOnLvlsCoords;// (1, nullptr);
    void InitBricksOnLvlsCoords()
    {
        m_vpvBricksOnLvlsCoords.push_back(new std::vector<vector2>);    //������ ���-� ��������� ������ �/ lvl �1
    
        //����� ���������� �������� �� ������ �1:
    #if NUMBER_OF_BRICKS == 2
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2( 59, 140));	//0
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(210, 140));	//1
    #elif NUMBER_OF_BRICKS == 12
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2( 59, 140));	//0
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(210, 140));	//1
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(361, 140));	//2
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(512, 140));	//3
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(663, 140));	//4
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(814, 140));	//5
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2( 59,  89));	//6(3)
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(361,  89));	//7(3)
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(663,  89));	//8(3)
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(210, 191));	//9(1)
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(512, 191));	//10(1)
        m_vpvBricksOnLvlsCoords[0]->push_back(vector2(814, 191));	//11(1)
    #endif 

        //#DEVELOP: ��� 2�� � ����.������� ������� ��� .push_back() ������ ������� ..
        //.. ������� �/ ������� ���.������ ����� 1�� � ��������� ��� ������������.
    }

    enum GAME_STATE {LOST=0, RUN=1, WON=2, NEW=3};	//��������� ���� ��� ��������. ��� ��� ����������� �������� ��������; 
                                                    //.. "NEW" ����� ��� �������� DONT_DELETE_ANI_BRICKS==1
    GAME_STATE m_GameState;
    const float m_fBOTTOM_GAP;  //����� �/� paddle � ����.����.����   ///@@Init. in C-tor.
    float m_fSceneHeight;   //���-� �������� ����			
    float m_fSceneWidth;    //���-� �������� ����		
    float m_fCurrPaddleY;   //�����. ����������� Pad'�
    float m_fPaddleY1;      //��� `m_fPaddleY1`, �.�. �������� Pad ����� � ����.���� ��������� ������������ �� OY ..
                            //.. � ����������� �� �������/�������.
    CBallsManager* const m_pBallsMngr;     //ptr �� singleton-��������� ��������� ������� 
    CBricksManager* const m_pBricksMngr;   //ptr �� singleton-��������� ��������� �������� 
    CPictureObject* g_pPad;     //Paddle
    //CPictureObject* g_pBall;	//Ball (ori. oblect)    ///@@@ ������ �� �����
    enum WFC_STATE {WFC_OFF=0, WFC_ON, WCF_DONE};   //WFC==Wait For Click
                                                    // 0 - ��������� ��-���������; 
                                                    // 1 - ���������� ����� processWinLostState() � �� �������� ������ ������������ �����, ���� OnMouseKeyDown() �� ������� �� 2; 
                                                    // 2 - ��������, � ������� � ������ processWinLostState() ������� ����������� ��������, � ���������� ������ �� ����� ������ (� ����� ������������ ����� ��������� �������� 0)
    WFC_STATE m_WaitForClick;     //���� �������� �����


//Methods (prtctd):
    vector2 Vec3To2(const vector3 &v3) { return (vector2(v3.x, v3.y)); }
    void InitNewGame(CScene *sc);
    void ProcessWinLostState(CScene *sc);


public: u32 m_uGameScore;    ///#UGLY: ����������� ������� ��� `protected`, ����� ��� ����. ������ �-��� ���������� � ����� CArkanoidController
};

///**/const u32   CArkanoidController::m_uNUM_OF_BRICKS_ON_LEVEL1 = 12;/**/

#endif //DT_CTRL_H 