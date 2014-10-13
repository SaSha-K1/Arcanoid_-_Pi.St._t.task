////////////////////////////////////////////////////////////////
//
//  settings.h: a part of Arkanoid application
//  authors: 
//  ©2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#ifndef SETTINGS_H 
#define SETTINGS_H 

#include "types.h"
#include "singleton.h"
#include "registry.h"
#include "pathmngr.h"
#include <vector>
using namespace std;

#define REGKEY_PRODUCT "Arkanoid"

class CSettings: public CSingleton<CSettings>
{
public:
	virtual ~CSettings();
	void Load();
	void Save();
protected:
};

#endif //SETTINGS_H 