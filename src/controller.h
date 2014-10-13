////////////////////////////////////////////////////////////////
//
//  controller.h: a part of 2DTemlate application
//  authors: Yulia Malyarevich
//  ©2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#ifndef DT_CTRL_H 
#define DT_CTRL_H 



//#include <array>
#include "types.h"          //д/ u32
#include "game.h"       //@@надо ли? (if нет, то нужно подключить или frwrd-decl. кл.ISceneController)
#include "findobj.h"    //@@надо ли?
//#include "random.h"   //@@пробую убрать. Вроде не мой код, но не нужно.
//#include "globals.h"  //@@пробую убрать. Вроде тут не исп-ся



class CArkanoidController: public ISceneController
{
public:
    //// Datas (pblc):
    
    //#OUTDATED: т.к. arrays, в которых пар-ром юзал эту data, заменяю vector'ами, т.к. см.консп.p.(+10), ТО надобность в этой data пока отпадает. Если станет нужна, то можно будет взять как .size() вектора координат бриков, напр.
    //const u32   m_uNUM_OF_BRICKS_ON_LVL1;     //Кол-во кирпичей на ур.1. Массивом не получилось, т.к. мне эти данные нужны при ..
                                                //.. инициализации vector'ов / array'ев, а внутри "< >" опер-р [] запрещён. ..
                                                //.. Init. in C-tor.
                                                ///@@@ дописать в описание, но проверить, так ли это. ЧТо это данное д/ передачи в C-tor BricksManager'a

    //// Methods (pblc):
	CArkanoidController(u32 sceneId);   //C-tor
	virtual ~CArkanoidController();     //D-tor
	void OnShow();
	void OnUpdate(u32 _dt);
	void OnRender(vector2 _scroll);
	void OnMouseMove(u32 _x, u32 _y);
	bool OnMouseKeyDown(u32 _key, u32 _x, u32 _y);


    //// Types (pblc):
    class CBallsManager;
    class CBricksManager;
    enum GAME_STATE {LOST=0, RUN=1, WON=2, NEW=3};	//Состояние игры для проверки. Ввёл для оптимизации проверки коллизий; 
                                                    //.. "NEW" нужна для варианта DONT_DELETE_ANI_BRICKS==1
                                                    //.. Перенёс в `public`, чтобы в CBallsManager::DelBall() смочь проверить и ..
                                                    //.. изменить m_GameState (собираюсь передать по ссылке).


protected:
    //// Datas (prtctd):
	CFont*  m_font;
	vector2 m_start;
	vector2 m_end;


    //// Types (prtctd):
    //#OUTDATED (ругается КОМП-р при `typedef` типов этих классов - переношу в `public:` @@@ позже разрбраться - может можно вернуть в `private:`):
    //class CBallsManager;
    //class CBricksManager;

    //Datas (prtctd):
    ///@@@ #TODO: уйти от магич.чисел в пар-рах array. Ск.вс.попробовать объявить с пар-ром 0U, и сделать ptr'ом на array, а проиниц-ть позже
    ///@@@ #TODO: Переопределить исходя из р-ров бриков (150х50)
    //2 - (для > удобного дебага); а 12 - как планировалось 

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


private:
    //Запрет копирования и присваивания:
    CArkanoidController& operator = (const CArkanoidController&);
    CArkanoidController (const CArkanoidController&);
//#WAS:     ///@@@ Warning	72	warning C4512: 'CArkanoidController' : assignment operator could not be generated	d:\arkanoid\src\controller.h	138


//// Datas (prtctd):        ///@@ Stoped_here (27/08/14)
    std::vector< std::vector<vector2>* >    m_vpvBricksOnLvlsCoords;// (1, nullptr);

    //@@@ переношу в `public`, чтобы в DelBall() из `CBallsManager` смочь проверить и изменить m_GameState
    //enum GAME_STATE {LOST=0, RUN=1, WON=2, NEW=3};	//Состояние игры для проверки. Ввёл для оптимизации проверки коллизий; 
                                                    //.. "NEW" нужна для варианта DONT_DELETE_ANI_BRICKS==1
    const float m_fBOTTOM_GAP;      //Зазор м/у paddle и нижн.гран.окна   ///@@Init. in C-tor.
    float       m_fSceneHeight;     //Пар-р игрового поля			
    float       m_fSceneWidth;      //Пар-р игрового поля		
    float       m_fPaddleY1;        //Ввёл `m_fPaddleY1`, т.к. возможно Pad будет в проц.игры дискретно перемещаться по OY ..
                                    //.. в зависимости от бонусов/малусов.
    float       m_fCurrPaddleY;     //Текущ. горизонталь Pad'а
    CBallsManager* /*const*/    m_pBallsMngr;   //ptr на singleton-экземпляр менеджера шариков  //@@не получ-ся `const`, т.к. иниц-рую не при создании
    CBricksManager* /*const*/   m_pBricksMngr;  //ptr на singleton-экземпляр менеджера кирпичей  //@@не получ-ся `const`, т.к. иниц-рую не при создании
    CPictureObject*         m_pPad;         //Paddle
    //CPictureObject* g_pBall;	//Ball (ori. oblect)    ///@@@ Видимо не нужен
    GAME_STATE  m_GameState;
//// Types (prvt):
    enum WFC_STATE {WFC_OFF=0, WFC_ON, WCF_DONE};   //WFC==Wait For Click
                                                    // 0 - состояние по-умолчанию; 
                                                    // 1 - высталвяет метод processWinLostState() и не проходит дальше определённого места, пока OnMouseKeyDown() не изменит на 2; 
                                                    // 2 - значение, с которым в методе processWinLostState() успешно выполняется проверка, и пропускает курсор до конца метода (в конце выставляется снова дефолтное значение 0)
//// Datas (prtctd) continue:
    WFC_STATE m_WaitForClick;     //Флаг ожидания клика


//// Methods (prvt):
    void InitBricksOnLvlsCoords();
    void InitNewGame(CScene* sc);
    void MovePad(u32 _x, u32 _y);
    void ProcessWinLostState(CScene *sc);
    void SleepMy(int seconds);  //Delay для gameOver()  ///@@@ может этот метод не нужен?

//#OUTDATED (пришлось перенести перед объявлением data'ы этого типа):   ///@@@ Проверить, когда всё заработает, может можно вернуть сюда..?
////// Types (prvt):
//    enum WFC_STATE {WFC_OFF=0, WFC_ON, WCF_DONE};   //WFC==Wait For Click
//                                                    // 0 - состояние по-умолчанию; 
//                                                    // 1 - высталвяет метод processWinLostState() и не проходит дальше определённого места, пока OnMouseKeyDown() не изменит на 2; 
//                                                    // 2 - значение, с которым в методе processWinLostState() успешно выполняется проверка, и пропускает курсор до конца метода (в конце выставляется снова дефолтное значение 0)


public: u32 m_uGameScore;    ///@@@ #UGLY: попробовать сделать его `protected`, когда все глоб. сейчас ф-ции перекачуют в класс CArkanoidController

};

///**/const u32   CArkanoidController::m_uNUM_OF_BRICKS_ON_LEVEL1 = 12;/**/

#endif //DT_CTRL_H 