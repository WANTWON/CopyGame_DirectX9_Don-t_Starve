#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Equip_Animation.h"

#include "Bullet.h"
#include "Player.h"
#include "Monster.h"
#include "Terrain.h"
#include "CameraDynamic.h"
#include "Sky.h"
#include "Tree.h"
#include "Grass.h"
#include "Boulder.h"
#include "BerryBush.h"
#include "Item.h"
#include "MainInventory.h"
#include "MainInventory_back.h"
#include "MainInventory_front.h"
#include "Pont.h"
#include "Equipment_back.h"
#include "Equipment_front.h"
#include "Playerhp.h"
#include "Playerhp_pont.h"
#include "Playerhunger.h"
#include "Playerhunger_pont.h"
#include "PlayerMentality.h"
#include "PlayerMentality_pont.h"

CLoader::CLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));

	/* run*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_DebugLine"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/DebugLine_%03d.png"), 1))))
		return E_FAIL;

#pragma region Add_Texture_Player
	/* run*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Run/Side/Player_Run_Side_%03d.png"), 33))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Run/Up/Player_Run_Up_%03d.png"), 33))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Run/Down/Player_Run_Down_%03d.png"), 33))))
		return E_FAIL;
	/*Idle*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Idle/Side/Player_IDLE_SIDE_%03d.png"), 66))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Idle/Up/Player_IDLE_UP_%03d.png"), 66))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Idle/Down/Player_IDLE_DOWN_%03d.png"), 66))))
		return E_FAIL;
	/* Attack_Weapon*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Weapon/Player_Attack_Weapon_Up_%03d.png"), 37))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Weapon/Player_Attack_Weapon_Down_%03d.png"), 37))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Weapon/Player_Attack_Weapon_Side_%03d.png"), 37))))
		return E_FAIL;

	/* Punch*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Punch/Player_PUNCH_UP_%03d.png"), 25))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Punch/Player_PUNCH_DOWN_%03d.png"), 25))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Punch/Player_PUNCH_SIDE_%03d.png"), 25))))
		return E_FAIL;

	/* Attack_Dart*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Dart/Player_Attack_Dart_Up_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Dart/Player_Attack_Dart_Down_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Attack/Dart/Player_Attack_Dart_Side_%03d.png"), 30))))
		return E_FAIL;

	/*Mining*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Mining/Player_Mining_Up_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Mining/Player_Mining_Down_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Mining/Player_Mining_Side_%03d.png"), 30))))
		return E_FAIL;

	/*Chop*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Chop/Player_Chop_Up_%03d.png"), 40))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Chop/Player_Chop_Down_%03d.png"), 40))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Chop/Player_Chop_Side_%03d.png"), 40))))
		return E_FAIL;

#pragma endregion Add_Texture_Player

#pragma  region Add_Texture_others
	/*Hambat_Anim*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_Hambat_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Weapon_Anim/Hambat/Hambat_Up_%03d.png"), 37))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_Hambat_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Weapon_Anim/Hambat/Hambat_Down_%03d.png"), 37))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_Hambat_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Weapon_Anim/Hambat/Hambat_Side_%03d.png"), 37))))
		return E_FAIL;

	/*Staff_Anim*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_FireStaff_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Weapon_Anim/FireStaff/FireStaff_Up_%03d.png"), 37))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_FireStaff_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Weapon_Anim/FireStaff/FireStaff_Down_%03d.png"), 37))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_FireStaff_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Weapon_Anim/FireStaff/FireStaff_Side_%03d.png"), 37))))
		return E_FAIL;

	/*Fireball*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fireball"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/Fireball/Fireball_%03d.png"), 10))))
		return E_FAIL;

	/*Dart*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dart"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/Dart/Dart_%03d.png"), 3))))
		return E_FAIL;

#pragma  endregion Add_Texture_others
	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));


	/* 객체 원형 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Equipment"),
		CEquip_Animation::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pGraphic_Device))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));

	/*For.Prototype_Component_Texture_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Terrain/tile%03d.png"), 18))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_MainInventory */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MainInventory"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/MainInventory.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_MainInventory_back */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MainInventory_back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/MainInventory_back.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_MainInventory_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MainInventory_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Item/item%d.png"), 23))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Equipment_back */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Equipment_back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/jangbi%d.png"), 4))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Equipment_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Equipment_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Item/item%d.png"), 23))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Pont */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pont"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/pont/Pont%d.png"), 10))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_HpPont */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpPont"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/pont/Pont%d.png"), 10))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Playerhp */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Playerhp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Health/Health_%03d.png"), 50))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Playerhunger */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Playerhunger"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Hunger/Hunger_%03d.png"), 50))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Playermentality */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerMentality"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Sanity/Sanity_%03d.png"), 50))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/*For. Prototype_Component_Texture_Tree*/
#pragma region Add_Texture_Tree
	// Tall
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Idle/Idle_%03d.png"), 80))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_CHOP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Chop/Chop_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_RIGHT"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Fall_Right/Fall_Right_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_LEFT"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Fall_Left/Fall_Left_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_SHAKE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Shake/Shake_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_STUMP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Stump/Stump_%03d.png"), 1))))
		return E_FAIL;
	// Normal
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_IDLE"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Normal/Idle/Idle_%03d.png"), 80))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_CHOP"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Normal/Chop/Chop_%03d.png"), 15))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_FALL_RIGHT"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Normal/Fall_Right/Fall_Right_%03d.png"), 38))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_FALL_LEFT"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Normal/Fall_Left/Fall_Left_%03d.png"), 38))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_SHAKE"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Normal/Shake/Shake_%03d.png"), 32))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_GROW"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Normal/Grow/Grow_%03d.png"), 20))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_STUMP"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Normal/Stump/Stump_%03d.png"), 1))))
	//	return E_FAIL;
	//// Short
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_IDLE"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Short/Idle/Idle_%03d.png"), 80))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_CHOP"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Short/Chop/Chop_%03d.png"), 15))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_FALL_RIGHT"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Short/Fall_Right/Fall_Right_%03d.png"), 38))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_FALL_LEFT"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Short/Fall_Left/Fall_Left_%03d.png"), 38))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_SHAKE"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Short/Shake/Shake_%03d.png"), 31))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_GROW"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Short/Grow/Grow_%03d.png"), 20))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_STUMP"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Short/Stump/Stump_%03d.png"), 1))))
	//	return E_FAIL;
#pragma endregion Add_Texture_Tree

	/*For. Prototype_Component_Texture_Grass*/
#pragma region Add_Texture_Grass
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Idle/Idle_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_PICK"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Pick/Pick_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_RUSTLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Rustle/Rustle_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_PICKED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Picked/Picked_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Grass

	/*For. Prototype_Component_Texture_Boulder*/
#pragma region Add_Texture_Boulder
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boulder_HEALTHY"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Boulder/Healthy/Healthy_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boulder_DAMAGED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Boulder/Damaged/Damaged_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boulder_BROKEN"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Boulder/Broken/Broken_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Boulder

	/*For. Prototype_Component_Texture_Berry_Bush*/
#pragma region Add_Texture_Berry_Bush
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Berry_Bush_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Idle/Idle_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Berry_Bush_PICK"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Pick/Pick_%03d.png"), 14))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Berry_Bush_SHAKE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Shake/Shake_%03d.png"), 43))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Berry_Bush_PICKED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Picked/Picked_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Berry_Bush


	/*For. Prototype_Component_Texture_Pig*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_RUN_DOWN"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Run/Pig_RUN_DOWN_%03d.png"), 33))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_RUN_UP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Run/Pig_RUN_UP_%03d.png"), 33))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_RUN_SIDE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Run/Pig_RUN_SIDE_%03d.png"), 33))))
		return E_FAIL;

	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));

	CVIBuffer_Terrain::TERRAINDESC		TerrainDesc;
	ZeroMemory(&TerrainDesc, sizeof(CVIBuffer_Terrain::TERRAINDESC));

	TerrainDesc.m_iPosVerticesX = 0;
	TerrainDesc.m_iPosVerticesZ = 0;
	TerrainDesc.m_iNumVerticesX = 100;
	TerrainDesc.m_iNumVerticesZ = 100;
	TerrainDesc.m_fTextureSize = 1.f;
	TerrainDesc.m_fSizeX = 1;
	TerrainDesc.m_fSizeZ = 1;
	TerrainDesc.m_iTextureNum = 0;

	/*For.Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, TerrainDesc))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain by MapTool */
	if (FAILED(Loading_Terrain_ForGamePlayLevel()))
		return E_FAIL;

	/*For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;


	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));


	/* 객체 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));

	/*For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Item*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"),
		CItem::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Monster*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCameraDynamic::Create(m_pGraphic_Device))))
		return E_FAIL;
	//여기서부터 Test 주석
	/*For.Prototype_GameObject_MainInventory */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainInventory"),
		CMainInventory::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInventory_back */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainInventory_back"),
		CMainInventory_back::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInventory_front */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainInventory_front"),
		CMainInventory_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Equipment_back */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Equipment_back"),
		CEquipment_back::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInventory_front */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Equipment_front"),
		CEquipment_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Pont */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pont"),
		CPont::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Playerhp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Playerhp"),
		CPlayerhp::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Playerhunger */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Playerhunger"),
		CPlayerhunger::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_PlayerMentality */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerMentality"),
		CPlayerMentality::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_HpPont */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpPont"),
		CPlayerhp_pont::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_hungerPont */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_hungerPont"),
		CPlayerhunger_pont::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_MentalityPont */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MentalityPont"),
		CPlayerMentality_pont::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Tree */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Grass */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Boulder */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boulder"),
		CBoulder::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Berry_Bush */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Berry_Bush"),
		CBerryBush::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Terrain_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	int			iNum;
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Vertex.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Load0"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Load1"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Load2"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Load3"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Load4"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Load5"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_Load6"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
	//	return E_FAIL;

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGraphic_Device);
}
