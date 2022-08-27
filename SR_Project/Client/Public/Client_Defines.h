#pragma once

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	typedef struct ObjectInfo
	{
		unsigned int iMaxHp;
		unsigned int iCurrentHp;
	} OBJINFO;
}

#include <process.h>

// Client_Defines.h

extern HWND		g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;


/* Enum */

enum OBJID { OBJ_PLAYER, OBJ_MOUSE, OBJ_BUTTON, OBJ_MONSTER, OBJ_BULLET, OBJ_ITEM, OBJ_NPC, OBJ_PORTAL, OBJ_END };

enum ITEMID { ITEM_FOOD, ITEM_EQUIPMENT, ITEM_MATERIAL, ITEM_END };

enum ITEMNAME {ITEMNAME_CARROT, ITEMNAME_SWORD,ITEMNAME_WOOD,ITEMNAME_TEST1,ITEMNAME_TEST2,ITEMNAME_END};

