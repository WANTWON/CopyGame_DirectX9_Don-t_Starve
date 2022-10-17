#pragma once
#include <d3d9.h>
#include <d3dx9.h>

namespace Tool
{
	const unsigned int	g_iToolViewCX = 1080;
	const unsigned int	g_iToolViewCY = 720;

	const unsigned int	g_iSideViewCX = 400;
	const unsigned int	g_iSideViewCY = 720;

	enum OBJECTID { OBJ_TERRAIN, OBJ_TREE, OBJ_GRASS, OBJ_ROCK, OBJ_BERRYBUSH, OBJ_HOUSE, OBJ_PIG, OBJ_SPIDER,
		OBJ_WALL, OBJ_CARROT, OBJ_DECO, OBJ_DIRT, OBJ_TRAP, OBJ_CARNIVAL, OBJ_EGG, OBJ_END };
	enum LEVEL { LEVEL_STATIC , LEVEL_END};
	enum HOUSETYPE { PIGHOUSE, SPIDERHOUSE, BOARONSPAWNER, MAZESPAWNER, HOUSE_END };
	enum DECOTYPE { FLOORFIRE, FLOOR_EFFECT, TORCH, FLIES, DECO_END };
	enum TRAPTYPE { TRAP_STAR, TRAP_PLANT, TRAP_MAX };
	enum INTERACTOBJ_ID { TREE, GRASS, BOULDER, BERRYBUSH, ITEMS, CARROT, NPC, PORTAL, COOKPOT, TENT, SKELETON, DIRT, ID_END };
	typedef struct tagBaseDesc
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vSize;

		tagBaseDesc()
		{
			ZeroMemory(this, sizeof(tagBaseDesc));
		};

		tagBaseDesc(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vSize)
		{
			vPos = _vPos;
			vSize = _vSize;
		}
	}BASEDESC;

	typedef struct ObjectInfo
	{
		unsigned int iMaxHp;
		unsigned int iCurrentHp;
	} OBJINFO;

	
}

using namespace Tool;

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

extern HWND g_Main_hWnd;
extern HINSTANCE g_Main_hInst;
