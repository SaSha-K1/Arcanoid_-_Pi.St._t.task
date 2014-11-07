////////////////////////////////////////////////////////////////
//
//  settigns.cpp: a part of Arkanoid application
//  authors: 
//  ©2010 PIPE Studio
//
////////////////////////////////////////////////////////////////

#include "settings.h"
#include "game.h"

CSettings::~CSettings()
{
}

void CSettings::Load()
{
	CGame::GetInst().m_volSound = LoadRegistryInt(REGKEY_PRODUCT, "Settings", "Sound", 60);
	CGame::GetInst().m_volMusic = LoadRegistryInt(REGKEY_PRODUCT, "Settings", "Music", 30);
//	m_player = LoadRegistryInt(REGKEY_PRODUCT, "Settings", "Current player");
}

void CSettings::Save()
{
	SaveRegistryInt(REGKEY_PRODUCT, "Settings", "Sound", CGame::GetInst().m_volSound);
	SaveRegistryInt(REGKEY_PRODUCT, "Settings", "Music", CGame::GetInst().m_volMusic);
//	SaveRegistryInt(REGKEY_PRODUCT, "Settings", "Current player", m_player);
}
