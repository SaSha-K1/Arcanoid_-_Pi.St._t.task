////////////////////////////////////////////////////////////////
//
//  controller.h: a part of 2DTemlate application
//  authors: Yulia Malyarevich
//  ©2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#ifndef DT_CTRL_H 
#define DT_CTRL_H 

#include "game.h"
#include "findobj.h"
#include "random.h"

class CArkanoidController: public ISceneController
{
public:
	CArkanoidController(u32 sceneId);
	virtual ~CArkanoidController();
	void OnShow();
	void OnUpdate(u32 _dt);
	void OnRender(vector2 _scroll);
	bool OnMouseKeyDown(u32 _key, u32 _x, u32 _y);
	void OnMouseMove(u32 _x, u32 _y);
protected:
	CFont* m_font;
	vector2 m_start;
	vector2 m_end;
public: u32 m_uGameScore;    ///#UGLY: попробовать сделать его `protected`, когда все глоб. сейчас ф-ции перекачуют в класс CArkanoidController
};

#endif //DT_CTRL_H 