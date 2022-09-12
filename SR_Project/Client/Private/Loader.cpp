#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Equip_Animation.h"

#include "Skeleton.h"
#include "Bullet.h"
#include "Player.h"
#include "Pig.h"
#include "PigKing.h"
#include "Spider.h"
#include "SpiderWarrior.h"
#include "Bearger.h"
#include "Boarrior.h"
#include "Terrain.h"
#include "Sky.h"
#include "Tree.h"
#include "Grass.h"
#include "Boulder.h"
#include "BerryBush.h"
#include "Carrot.h"
#include "Item.h"

#include "Wendy.h"

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
#include "BagInventory.h"
#include "Mouse_item.h"
#include "Water.h"

#include "ParticleSystem.h"
#include "Particle.h"

#include "ToolboxMain.h"
#include "ToolboxMain_back.h"
#include "ToolboxMain_front.h"
#include "ToolboxWeapon.h"
#include "ToolboxWeapon_front.h"
#include "ToolboxWeapon_back.h"
#include "ToolboxGather.h"
#include "ToolboxGather_back.h"
#include "ToolboxGather_front.h"
#include "ToolboxConstruct.h"
#include "ToolboxConstruct_back.h"
#include "ToolboxConstruct_front.h"
#include "ToolboxMaterial.h"
#include "ToolboxMaterial_back.h"
#include "ToolboxMaterial_front.h"

#include "Craftmain.h"
#include "Craftmain_back.h"
#include "Craftmain_front.h"
#include "Craftbutton.h"
#include "CraftPont.h"

#include "AttackRange.h"
#include "House.h"
#include "Portal.h"
#include "WoodWall.h"
#include "CookPot.h"
#include "Tent.h"
#include "Special_Attack.h"

#include "Daycount.h"
#include "Daycountpont.h"
#include "Quest.h"
#include "Questpont.h"
#include "Line.h"
#include "Talk.h"
#include "Loadingscene.h"
#include "Logoscene.h"
#include "Startbutton.h"

#include "PotMain.h"
#include "Pot_back.h"
#include "Pot_front.h"
#include "Potbutton.h"
#include "Questbutton.h"

#include "Mouse_iteminfo.h"

#include "Deadcountpont.h"
#include "Deadmain.h"
#include "Daypont.h"

#include "Eatitem.h"
#include "CameraManager.h"


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
	case LEVEL_HUNT:
		pLoader->Loading_ForHuntLevel();
		break;
	case LEVEL_BOSS:
		pLoader->Loading_ForBossLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	if (!pLoader->Get_IsFinished())
	{
		ERR_MSG(TEXT("Errpr : IsFinished Error"));
	}

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

	lstrcpy(m_szLoadingText, TEXT("Loading_Prototype"));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_DebugLine"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/DebugLine_%03d.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading_Player Texture"));

#pragma region Add_Texture_Player
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

	/*Build*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Build_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Build/Player_Build_Up_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Build_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Build/Player_Build_Down_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Build_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Build/Player_Build_Side_%03d.png"), 30))))
		return E_FAIL;

	/*Pickup*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Pickup_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Pickup/Player_Pickup_Up_%03d.png"), 11))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Pickup_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Pickup/Player_Pickup_Down_%03d.png"), 11))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Pickup_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Pickup/Player_Pickup_Side_%03d.png"), 11))))
		return E_FAIL;

	/*Damaged*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Damaged_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Damaged/Player_Damaged_Up_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Damaged_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Damaged/Player_Damaged_Down_%03d.png"), 30))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Damaged_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Damaged/Player_Damaged_Side_%03d.png"), 30))))
		return E_FAIL;

	/*Jump to Teleport*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Jump_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Jump/Player_Jump_Up_%03d.png"), 40))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Jump_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Jump/Player_Jump_Down_%03d.png"), 40))))
		return E_FAIL;																								   
																													   
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Jump_Side"),		   
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Jump/Player_Jump_Side_%03d.png"), 40))))
		return E_FAIL;

	/*Eat*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Eat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Eat/Player_Eat_%03d.png"), 22))))
		return E_FAIL;

	/*Dead*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dead"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Dead/Player_Dead_%03d.png"), 56))))
		return E_FAIL;

	/*Ghost_Idle*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Ghost_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Ghost_Idle/Player_Ghost_Idle_%03d.png"), 65))))
		return E_FAIL;

	/*Revive*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Revive"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Revive/Player_Revive_%03d.png"), 17))))
		return E_FAIL;

	/*Angry*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Angry"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Angry/Player_Angry_%03d.png"), 30))))
		return E_FAIL;
#pragma endregion Add_Texture_Player

#pragma  region Add_Texture_others
	/*Skeleton*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Skeleton"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Skeleton/Player_Skeleton_%03d.png"), 1))))
		return E_FAIL;

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

	/*Bath_Bomb*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BathBomb"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/Bomb/Bath_Bomb_%03d.png"), 1))))
		return E_FAIL;
	/*Bullet*/
	/*Lightning*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/Lightning/Lightning_%03d.png"), 10))))
		return E_FAIL;

	/*IceSpike1*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/IceSpike/IceSpike1_%03d.png"), 28))))
		return E_FAIL;

	/*IceSpike2*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/IceSpike/IceSpike2_%03d.png"), 28))))
		return E_FAIL;

	/*IceSpike3*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/IceSpike/IceSpike3_%03d.png"), 28))))
		return E_FAIL;

	/*IceSpike4	*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/IceSpike/IceSpike4_%03d.png"), 26))))
		return E_FAIL;

	/*SandSpike_Small_Create*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Small_Create"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/SandSpike/SandSpike_Small_Create_%03d.png"), 17))))
		return E_FAIL;

	/*SandSpike_Small_Break*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Small_Break"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/SandSpike/SandSpike_Small_Break_%03d.png"), 16))))
		return E_FAIL;

	/*SandSpike_Small_PreBreak*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Small_PreBreak"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/SandSpike/SandSpike_Small_Pre_%03d.png"), 9))))
		return E_FAIL;

	/*SandSpike_Tall_Create*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Tall_Create"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/SandSpike/SandSpike_Tall_Create_%03d.png"), 17))))
		return E_FAIL;

	/*SandSpike_Tall_Break*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Tall_Break"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Bullet/SandSpike/SandSpike_Tall_Break_%03d.png"), 16))))
		return E_FAIL;

	/*Small_Puff*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Small_Puff"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Small_Puff/Small_Puff_%03d.png"), 17))))
		return E_FAIL;
	//BulletEnd
	
	/*AttackRange & Picker*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Picker"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Picker/Picker_%03d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_AttackRange"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Picker/Attack_Range_%03d.png"), 2))))
		return E_FAIL;
#pragma  endregion Add_Texture_others

#pragma  region Add_Texture_Effects
	/*Smoke*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Laser_Hit1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Laser_Hit/Laser_Hit1_%03d.png"), 12))))
		return E_FAIL;

	/*Build_Effect*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuildEffect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Build_Effect/Build_Effect_%03d.png"), 26))))
		return E_FAIL;

	/*Bearger_Ring*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Ring"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Bearger_Ring/Bearger_Ring_%03d.png"), 18))))
		return E_FAIL;

	/*Bearger_Rocks*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Rocks"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Bearger_Rocks/Bearger_Rocks_%03d.png"), 36))))
		return E_FAIL;

	/*Boarrior_Eruption*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Eruption"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Boarrior_Eruption/Boarrior_Eruption_%03d.png"), 32))))
		return E_FAIL;
#pragma  endregion Add_Texture_Effects
	/* ���̴� �ε� ��. */

	/*For.Prototype_Component_Texture_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Terrain/tile%03d.png"), 19))))
		return E_FAIL;

	/* ���̴� �ε� ��. */
	lstrcpy(m_szLoadingText, TEXT("Loading_UI Texture"));

#pragma region Add_Texture UI

	/*For.Prototype_Component_Texture_Startbutton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_startbutton"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/startbutton/start%d.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_MainInventory */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainInventory"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/MainInventory.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Craftmain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Craftmain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Craft/craft%d.png"), 13))))
		return E_FAIL;


	/*For.Prototype_Component_Texture_Craftmain_back */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Craftmain_back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Craft/craftback%d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Craftbutton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Craftbutton"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Craft/craftbutton%d.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Gatherbox_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GatherToolbox_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Toolbox/gather%d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_WeaponToolbox_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponToolbox_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Toolbox/weapon%d.png"), 5))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_ConstructToolbox_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ConstructToolbox_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Toolbox/construct%d.png"), 3))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_MaterialToolbox_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MaterialToolbox_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Toolbox/material%d.png"), 3))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_MainToolbox */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainToolbox"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/Toolbox.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_MainToolbox_back */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainToolbox_back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Toolbox/Toolboxback%d.png"), 3))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_MainToolbox_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainToolbox_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Toolbox/Toolboxfront%d.png"), 4))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_BagInventory */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BagInventory"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/baginven3.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bagopen"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/bagsprite/BackPackOpen_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bagclose"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/bagsprite/BackPackClose_%03d.png"), 11))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_PotMain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/Pot.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Potopen"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Potopen/open_%03d.png"), 11))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Potclose"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Potclose/close_%03d.png"), 9))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_MainInventory_back */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainInventory_back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/MainInventory_back.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_MainInventory_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainInventory_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Item/item%d.png"), 34))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Equipment_back */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Inventory/jangbi%d.png"), 4))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Equipment_front */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Item/item%d.png"), 34))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Pont */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pont"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/pont/Pont%d.png"), 11))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_HpPont */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpPont"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/pont/Pont%d.png"), 11))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Playerhp */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Playerhp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Health/Health_%03d.png"), 50))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Playerhunger */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Playerhunger"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Hunger/Hunger_%03d.png"), 50))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Playermentality */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerMentality"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Sanity/Sanity_%03d.png"), 50))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Playermentality */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Daycount"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Day/Day_%03d.png"), 30))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Playermentality */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Daycountpont"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Day/day%d.png"), 20))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_quest */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_quest"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/quest/quest%d.png"), 3))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_quest */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Line"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Line/line%d.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_talk */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_talk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/talk/talk%d.png"), 13))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_logoscene */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logoscene"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Logo/logo%d.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Potbutton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Potbutton"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/potbutton/potbutton%d.png"), 2))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Questbutton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_questbutton"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/questbutton/questbutton%d.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Questbutton */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Iteminfo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Iteminfo/Iteminfo%d.png"), 9))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_DeadUI */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Deadmain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/DeadUI/dead%d.png"), 1))))
		return E_FAIL;



	

	///*For.Prototype_Component_Texture_loadingscene */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_loading"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/scene2/loading_%03d.png"), 125))))
	//	return E_FAIL;
#pragma endregion Add_Texture UI


	/* Others Prototype */
	lstrcpy(m_szLoadingText, TEXT("Loading_Object"));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Equipment"),
		CEquip_Animation::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Picker"),
		CAttackRange::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_Special"),
		CSpecial_Attack::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*NPC Prototype*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_Wendy"),
		CWendy::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading_UI"));

#pragma region Add_Prototype UI Object


	/*For.Prototype_GameObject_Craftmain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Craftmain"),
		CCraftmain::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Craftmain_back"),
		CCraftmain_back::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Craftmain_front"),
		CCraftmain_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Craftbutton"),
		CCraftbutton::Create(m_pGraphic_Device))))
		return E_FAIL;

	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CraftPont"),
		CCraftPont::Create(m_pGraphic_Device))))
		return E_FAIL;


	/*For.Prototype_GameObject_MainInventory */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainInventory"),
		CMainInventory::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponToolbox"),
		CToolboxWeapon::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponToolbox_back"),
		CToolboxWeapon_back::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WeaponToolbox_front"),
		CToolboxWeapon_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GatherToolbox"),
		CToolboxGather::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GatherToolbox_back"),
		CToolboxGather_back::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GatherToolbox_front"),
		CToolboxGather_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ConstructToolbox"),
		CToolboxConstruct::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ConstructToolbox_back"),
		CToolboxConstruct_back::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ConstructToolbox_front"),
		CToolboxConstruct_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MaterialToolbox"),
		CToolboxMaterial::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MaterialToolbox_back"),
		CToolboxMaterial_back::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MaterialToolbox_front"),
		CToolboxMaterial_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_MainInToolbox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainToolbox"),
		CToolboxMain::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainIToolbox_back */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainToolbox_back"),
		CToolboxMain_back::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_MainIToolbox_back */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainToolbox_front"),
		CToolboxMain_front::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_BagInventory */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BagInventory"),
		CBagInventory::Create(m_pGraphic_Device))))
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
		/*For.Prototype_GameObject_Daycount */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Daycount"),
		CDaycount::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_Daycountpont */
    if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Daycountpont"),
		CDaycountpont::Create(m_pGraphic_Device))))
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

	/*For.Prototype_GameObject_MentalityPont */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse_item"),
		CMouse_item::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_MentalityPont */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse_iteminfo"),
		CMouse_iteminfo::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Eateffect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Eateffect"),
		CEateffect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_quest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_quest"),
		CQuest::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_questpont"),
		CQuestpont::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Line"),
		CLine::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_questbutton"),
		CQuestbutton::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_talk */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_talk"),
		CTalk::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_logoscene"),
		CLogoscene::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_startbutton"),
		CStartbutton::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Potmain"),
		CPotMain::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pot_back"),
		CPot_back::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pot_front"),
		CPot_front::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Potbutton"),
		CPotbutton::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Deadmain"),
		CDeadmain::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Deadcount"),
		CDeadcountpont::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Daypont"),
		CDaypont::Create(m_pGraphic_Device))))
		return E_FAIL;

	



	/*For.Prototype_GameObject_talk */
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loadingscene"),
		CLoadingscene::Create(m_pGraphic_Device))))
		return E_FAIL;*/


#pragma endregion Add_Prototype UI Object

	lstrcpy(m_szLoadingText, TEXT("Finished_Loading"));

	/*For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;


	/*For. Prototype_Component_Texture_Portal*/
#pragma region Add_Texture_Portal
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Idle_Open"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Portal/Idle_Open/Idle_Open_%03d.png"), 50))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Idle_Close"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Portal/Idle_Close/Idle_Close_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Open"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Portal/Open/Open_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Close"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Portal/Close/Close_%03d.png"), 17))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossPortal_Idle_Close"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/BossPortal/Idle/Idle_%03d.png"), 20))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossPortal_Idle_Open"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/BossPortal/Close/Close_%03d.png"), 18))))
		return E_FAIL;
#pragma endregion Add_Texture_Portal

	/*For. Prototype_Component_Texture_Wendy*/
#pragma region Add_Texture_Wendy
	//Wendy Idle
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Idle_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Idle/Wendy_Idle_Down_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Idle_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Idle/Wendy_Idle_Up_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Idle_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Idle/Wendy_Idle_Side_%03d.png"), 1))))
		return E_FAIL;

	//Wendy Run
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Run_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Run/Wendy_Run_Down_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Run_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Run/Wendy_Run_Up_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Run_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Run/Wendy_Run_Side_%03d.png"), 17))))
		return E_FAIL;

	//Wendy Build
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Build_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Build/Wendy_Build_Down_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Build_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Build/Wendy_Build_Up_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Build_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Build/Wendy_Build_Side_%03d.png"), 17))))
		return E_FAIL;

	//Wendy PickUp
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Pickup_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Pickup/Wendy_Pickup_Down_%03d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Pickup_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Pickup/Wendy_Pickup_Up_%03d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Pickup_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Pickup/Wendy_Pickup_Side_%03d.png"), 6))))
		return E_FAIL;

	//Wendy Give
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Give_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Give/Wendy_Give_Down_%03d.png"), 14))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Give_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Give/Wendy_Give_Up_%03d.png"), 14))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wendy_Give_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Wendy/Give/Wendy_Give_Side_%03d.png"), 14))))
		return E_FAIL;

#pragma endregion Add_Texture_Wendy

	/*For.Prototype_Component_SpiderHouse Texture */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spider_House"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Construct/SpiderHouse.png"), 1))))
		return E_FAIL;



	/*For. Prototype_Component_Texture_House*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pig_House"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Construct/PigHouse.png"), 1))))
		return E_FAIL;

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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, TerrainDesc))))
		return E_FAIL;

	/*For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	


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

	/* �ؽ��� �ε� ��. */
	lstrcpy(m_szLoadingText, TEXT("Loading_Texture"));


	/*For.Prototype_Component_Texture_RockEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rock"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/Roc.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_LeafEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Leaf"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/fff-%d.png"), 4))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_SnowEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/Snow.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading_Interactive Object Texture"));


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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_STUMP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Stump/Stump_%03d.png"), 1))))
		return E_FAIL;
	
#pragma endregion Add_Texture_Tree

	/*For. Prototype_Component_Texture_Grass*/
#pragma region Add_Texture_Grass
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Idle/Idle_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_PICK"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Pick/Pick_%03d.png"), 15))))
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

	/*For. Prototype_Component_Texture_WoodWall*/
#pragma region Add_Texture_WoodWall
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WoodWall_HEALTHY"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/WoodWall/Healthy/Healthy_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WoodWall_DAMAGED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/WoodWall/Damaged/Damaged_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WoodWall_BROKEN"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/WoodWall/Broken/Broken_%03d.png"), 1))))
		return E_FAIL;
	
#pragma endregion Add_Texture_WoodWall

	/*For. Prototype_Component_Texture_Cook_Pot*/
#pragma region Add_Texture_Cook_Pot
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Cook_Pot/Idle/Idle_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Place"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Cook_Pot/Place/Place_%03d.png"), 23))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Cooking"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Cook_Pot/Cooking/Cooking_%03d.png"), 18))))
		return E_FAIL;
#pragma endregion Add_Texture_Cook_Pot

	/*For. Prototype_Component_Texture_Tent*/
#pragma region Add_Texture_Tent
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Tent/Idle/Idle_%03d.png"), 56))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Place"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Tent/Place/Place_%03d.png"), 23))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Enter"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Tent/Enter/Enter_%03d.png"), 23))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Sleep"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Tent/Sleep/Sleep_%03d.png"), 65))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Hit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Tent/Hit/Hit_%03d.png"), 13))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Destroy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Tent/Destroy/Destroy_%03d.png"), 68))))
		return E_FAIL;
#pragma endregion Add_Texture_Tent

	/*For. Prototype_Component_Texture_Berry_Bush*/
#pragma region Add_Texture_Berry_Bush
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Berry_Bush_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Idle/Idle_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Berry_Bush_PICK"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Pick/Pick_%03d.png"), 14))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Berry_Bush_PICKED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Picked/Picked_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Berry_Bush

	/*For. Prototype_Component_Texture_Carrot*/
#pragma region Add_Texture_Carrot
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Carrot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Carrot/Carrot_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Carrot

	lstrcpy(m_szLoadingText, TEXT("Loading_Monster Texture"));

	/*For. Prototype_Component_Texture_Pig*/
#pragma region Add_Texture_Pig
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Idle_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Idle_Up/Idle_Up_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Idle_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Idle_Down/Idle_Down_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Idle_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Idle_Side/Idle_Side_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Walk_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Walk_Up/Walk_Up_%03d.png"), 21))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Walk_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Walk_Down/Walk_Down_%03d.png"), 21))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Walk_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Walk_Side/Walk_Side_%03d.png"), 21))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Run_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Run_Up/Run_Up_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Run_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Run_Down/Run_Down_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Run_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Run_Side/Run_Side_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Attack_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Attack_Up/Attack_Up_%03d.png"), 31))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Attack_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Attack_Down/Attack_Down_%03d.png"), 33))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Attack_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Attack_Side/Attack_Side_%03d.png"), 37))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Hit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Hit/Hit_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Death/Death_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_Happy"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Happy/Happy_%03d.png"), 34))))
		return E_FAIL;
#pragma endregion Add_Texture_Pig

	/*For. Prototype_Component_Texture_Pig_King*/
#pragma region Add_Texture_Pig_King
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_King_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Pig_King/Idle/Idle_%03d.png"), 70))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_King_Cointoss"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Pig_King/Cointoss/Cointoss_%03d.png"), 48))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_King_Unimpressed"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/NPC/Pig_King/Unimpressed/Unimpressed_%03d.png"), 57))))
		return E_FAIL;
#pragma endregion Add_Texture_Pig_King


	/*For. Prototype_Component_Texture_Bearger*/
#pragma region Add_Texture_Bearger
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Attack_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Attack_Down/Attack_Down_%03d.png"), 59))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Attack_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Attack_Side/Attack_Side_%03d.png"), 59))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Attack_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Attack_Up/Attack_Up_%03d.png"), 59))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Charge_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Charge_Down/Charge_Down_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Charge_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Charge_Side/Charge_Side_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Charge_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Charge_Up/Charge_Up_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Death/Death_%03d.png"), 59))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Eat"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Eat/Eat_%03d.png"), 33))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Eat_Post"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Eat_Post/Eat_Post_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Eat_Pre"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Eat_Pre/Eat_Pre_%03d.png"), 24))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Ground_Pound_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Ground_Pound_Down/Ground_Pound_Down_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Ground_Pound_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Ground_Pound_Side/Ground_Pound_Side_%03d.png"), 36))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Ground_Pound_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Ground_Pound_Up/Ground_Pound_Up_%03d.png"), 35))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Hit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Hit/Hit_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Aggro_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Idle_Aggro_Down/Idle_Aggro_Down_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Aggro_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Idle_Aggro_Side/Idle_Aggro_Side_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Aggro_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Idle_Aggro_Up/Idle_Aggro_Up_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Idle_Down/Idle_Down_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Idle_Side/Idle_Side_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Idle_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Idle_Up/Idle_Up_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Run_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Run_Down/Run_Down_%03d.png"), 33))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Run_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Run_Side/Run_Side_%03d.png"), 33))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Run_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Run_Up/Run_Up_%03d.png"), 33))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Walk_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Walk_Down/Walk_Down_%03d.png"), 57))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Walk_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Walk_Side/Walk_Side_%03d.png"), 57))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bearger_Walk_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Bearger/Walk_Up/Walk_Up_%03d.png"), 58))))
		return E_FAIL;
#pragma endregion Add_Texture_Bearger

#pragma region Add_Texture_Boarrior
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Attack_1_Down/Attack_1_Down_%03d.png"), 26))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Attack_1_Side/Attack_1_Side_%03d.png"), 26))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Attack_1_Up/Attack_1_Up_%03d.png"), 26))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Attack_2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Attack_2/Attack_2_%03d.png"), 45))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Attack_3_Down/Attack_3_Down_%03d.png"), 58))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Attack_3_Side/Attack_3_Side_%03d.png"), 58))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Attack_3_Up/Attack_3_Up_%03d.png"), 58))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Dash_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Dash_Down/Dash_Down_%03d.png"), 16))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Dash_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Dash_Side/Dash_Side_%03d.png"), 16))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Dash_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Dash_Up/Dash_Up_%03d.png"), 16))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Death"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Death/Death_%03d.png"), 93))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Hit_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Hit_Down/Hit_Down_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Hit_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Hit_Side/Hit_Side_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Hit_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Hit_Up/Hit_Up_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Idle_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Idle_Down/Idle_Down_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Idle_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Idle_Side/Idle_Side_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Idle_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Idle_Up/Idle_Up_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Stun"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Stun/Stun_%03d.png"), 7))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Walk_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Walk_Down/Walk_Down_%03d.png"), 37))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Walk_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Walk_Side/Walk_Side_%03d.png"), 37))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Boarrior_Walk_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Boarrior/Walk_Up/Walk_Up_%03d.png"), 37))))
		return E_FAIL;
#pragma endregion Add_Texture_Boarrior

	/* �� �ε� ��. */
	lstrcpy(m_szLoadingText, TEXT("Loading Model"));

	

	/* For.Prototype_Component_VIBuffer_Terrain by MapTool */
	if (FAILED(Loading_Terrain_ForGamePlayLevel()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading_Objetct"));



	/*For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pGraphic_Device))))
	{
		lstrcpy(m_szLoadingText, TEXT("Loading_Finish"));

		Safe_Release(pGameInstance);
		m_isFinished = true;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Water"),
		CWater::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Item*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"),
		CItem::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Pig*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pig"),
		CPig::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For. Prototype_GameObject_Pig_King */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pig_King"),
		CPigKing::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Bearger*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bearger"),
		CBearger::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Boarrior*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boarrior"),
		CBoarrior::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCameraDynamic::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_FPS */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_FPS"),
		CCameraFPS::Create(m_pGraphic_Device))))
		return E_FAIL;


	/*For.Prototype_GameObject_Camera_Target */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Target"),
		CCameraTarget::Create(m_pGraphic_Device))))
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

	/*For.Prototype_GameObject_WoodWall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WoodWall"),
		CWoodWall::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Cook_Pot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cook_Pot"),
		CCookPot::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Tent */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tent"),
		CTent::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Berry_Bush */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Berry_Bush"),
		CBerryBush::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Carrot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Carrot"),
		CCarrot::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_ParticleSystem
	if (FAILED(pGameInstance->Add_Prototype(TEXT("GameObject_ParticleSystem"), 
		CParticleSystem::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("GameObject_Particle"),
		CParticle::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_House */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_House"),
		CHouse::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Portal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Portal"),
		CPortal::Create(m_pGraphic_Device))))
		return E_FAIL;
	/*For.Prototype_GameObject_Skeleton*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skeleton"),
		CSkeleton::Create(m_pGraphic_Device))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading_Finish"));

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
	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Terrain_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain_Load0"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
		return E_FAIL;

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Terrain_ForHuntLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	int			iNum;
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Terrain_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_VIBuffer_Terrain_Load0"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
		return E_FAIL;

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Terrain_ForBossLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	int			iNum;
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Boss_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_VIBuffer_Terrain_Load0"), CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte))))
		return E_FAIL;

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}


HRESULT CLoader::Loading_ForHuntLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading_Texture"));

	/*For. Prototype_Component_Texture_Tree*/
#pragma region Add_Texture_Tree
	// Tall
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/LivingWood/Tall/Idle/Idle_%03d.png"), 80))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_CHOP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/LivingWood/Tall/Chop/Chop_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_RIGHT"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/LivingWood/Tall/Fall_Right/Fall_Right_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_LEFT"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/LivingWood/Tall/Fall_Left/Fall_Left_%03d.png"), 38))))
		return E_FAIL;

#pragma endregion Add_Texture_Tree

	/*For. Prototype_Component_Texture_Spider*/
#pragma region Add_Texture_Spider
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Idle/Idle_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Move_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Move_Up/Move_Up_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Move_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Move_Down/Move_Down_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Move_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Move_Side/Move_Side_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Attack_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Attack_Up/Attack_Up_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Attack_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Attack_Down/Attack_Down_%03d.png"), 34))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Attack_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Attack_Side/Attack_Side_%03d.png"), 44))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Taunt"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Taunt/Taunt_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Hit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Hit/Hit_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Die/Die_%03d.png"), 28))))
		return E_FAIL;
#pragma endregion Add_Texture_Spider

	/*For.Prototype_Component_Texture_RockEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Rock"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/Roc.png"), 1))))
		return E_FAIL;


	/*For.Prototype_Component_Texture_LeafEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Leaf"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/fff-%d.png"), 4))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_LeafEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/Snow.png"), 1))))
		return E_FAIL;

	/*For. Prototype_Component_Texture_Spider_Warrior*/
#pragma region Add_Texture_Spider_Warrior
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Idle/Idle_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Move_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Move_Up/Move_Up_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Move_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Move_Down/Move_Down_%03d.png"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Move_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Move_Side/Move_Side_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Attack_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Attack_Up/Attack_Up_%03d.png"), 28))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Attack_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Attack_Down/Attack_Down_%03d.png"), 29))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Attack_Side"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Attack_Side/Attack_Side_%03d.png"), 30))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Taunt"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Taunt/Taunt_%03d.png"), 32))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Hit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Hit/Hit_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_Texture_Spider_Warrior_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider_Warrior/Die/Die_%03d.png"), 28))))
		return E_FAIL;
#pragma endregion Add_Texture_Spider_Warrior

	/*For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_HUNT, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain by MapTool */
	if (FAILED(Loading_Terrain_ForHuntLevel()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading_Object & Component"));


	/*For. Prototype_GameObject_Spider */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spider"),
		CSpider::Create(m_pGraphic_Device))))
	{
		lstrcpy(m_szLoadingText, TEXT("Finished_Loading"));

		Safe_Release(pGameInstance);
		m_isFinished = true;
		return E_FAIL;
	}
		

	/*For. Prototype_GameObject_Spider_Warrior */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spider_Warrior"),
		CSpiderWarrior::Create(m_pGraphic_Device))))
		return E_FAIL;




	lstrcpy(m_szLoadingText, TEXT("Finished_Loading"));

	Safe_Release(pGameInstance);
	SetWindowText(g_hWnd, TEXT("Loading_HunLevel."));
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_ForBossLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading Texture"));

	/*For.Prototype_Component_Texture_RockEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Rock"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/Roc.png"), 1))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_LeafEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Leaf"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/fff-%d.png"), 4))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_SnowEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Particle/Snow.png"), 1))))
		return E_FAIL;

	if (FAILED(Loading_Terrain_ForBossLevel()))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_Component_Texture_RockWall_HEALTHY"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/RockWall/Healthy/Healthy_%03d.png"), 3))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	SetWindowText(g_hWnd, TEXT("Loading_HunLevel."));
	m_isFinished = true;
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
