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