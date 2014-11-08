////////////////////////////////////////////////////////////////
//
//  main.cpp: a part of Arkanoid application
//  authors: 
//  ©2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

//#include <vld.h>
#include "game.h"
#include "random.h"
#include "controller.h"
#include "settings.h"
#include "obj.h"
#include <stdlib.h>						//д/ putenv()
//#include "printContent.h"	//S$-a      //д/ работы с вектором exceptPrintVec
#include <fstream>
#include <ctime>
#include "MyExcptn.h"

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{//S$-c
	char* dxPlacementEnvVarValue("DXSDK_DIR=C:\Program Files (x86)\Microsoft DirectX SDK (August 2007)");		//S$-a
	_putenv(dxPlacementEnvVarValue);																			//S$-a

	if (false != CGame::GetInst().Create("Arkanoid.xml"))
	{
		CGame::GetInst().SetSettings(new CBaseSettings("PIPE Studio", "Arkanoid"));
		CGame::GetInst().OnShow();
		new CArkanoidController(SC_01);
		CPreloadManager::GetInst().LoadFonts();
		CGame::GetInst().StartScene();
		CGame::GetInst().Run();
		
		delete CRndGen::GetPtr();
	}
	delete CGame::GetPtr();
	return 0;
}

///@@@#REVIEW: об€з.просмотреть все мои классы на предмет требовани€ стандарта кодировани€  "3.13	ѕроектирование класса."

////Errs and WRNs:
///2014/11/07 - 0 Errs, 832 Wrnngs - просмотрел.

////My notes
///2014/11/07 20:50
// ƒалее перехожу на алгоритм интегрального прохода по контру шарика дл€ поиска контакта и веткор этих точек буду ..
//.. исопльзовать д/ расчЄта точки нового мув-центра.
//.. Ќо по текущему решению (8 хит-чек-поинтов - датчиков контакта, в зависимости от сочетани€ сработавших из которых, ..
//.. решал описанные выше задачи), от которого отказываюсь, хочу таки запомнить текущие проблемы: причина всех проблем ..
//.. в том, что шарик за врем€ между 2-м€ проверками почему-то успевает провалитьс€ в преп€тствие (кирпич или Pad) больше, ..
//.. чем на 1 хит-чек-поинт (далее ’„ѕ) - т.е. вместо только N или E, например, срабатывают уже ещЄ и 2 соседние ’„ѕ.
//.. ѕровер€л dt м/у двум€ отработками методов OnRender и OnUpdate (они всегда ~равны), но, блин, почему-то прыгают ..
//.. значительно, хот€ на глаз этого и не видно, но по цифрам от 25мсек до 5 секунд! и даже больше!
//.. ѕричина, видимо, в движке - нужно вы€сн€ть.
//.. ≈щЄ мысль - использовать априорную обработку столкновений...