#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "CameraDynamic.h"
#include "PickingMgr.h"
#include "House.h"
#include "Level_Loading.h"
#include "CameraManager.h"
#include "Player.h"
#include "Portal.h"
#include "Particle.h"
#include "Trap.h"
#include <time.h>

#include "NPC.h"
#include "DayCycle.h"

_bool g_bUIMadefirst = false;

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	CGameInstance::Get_Instance()->StopAll();
	CDayCycle::Get_Instance()->AllRemoveObserver();

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	if (FAILED(Ready_LayerNPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	if (g_bUIMadefirst == false)
	{
		if (FAILED(Ready_Layer_MainInventory(TEXT("Layer_UI"))))
			return E_FAIL;

		if (FAILED(Ready_Layer_Equipment_back(TEXT("Layer_UI"))))
			return E_FAIL;

		if (FAILED(Ready_Layer_PlayerStatUI(TEXT("Layer_UI"))))
			return E_FAIL;

		if (FAILED(Ready_Layer_WeaponToolbox(TEXT("Layer_UI"))))
			return E_FAIL;

		if (FAILED(Ready_Layer_MainToolbox(TEXT("Layer_UI"))))
			return E_FAIL;

		CGameInstance::Get_Instance()->StopAll();

		g_bUIMadefirst = true;
	}

	
	CDayCycle::Get_Instance()->RegisterObserver(this, CDayCycle::CYCLE_NONSTATIC);
	srand(unsigned int(time(NULL)));
	CPickingMgr::Get_Instance()->Clear_PickingMgr();
	CPickingMgr::Get_Instance()->Ready_PickingMgr(LEVEL::LEVEL_GAMEPLAY);
	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_GAMEPLAY);
	CGameInstance::Get_Instance()->PlayBGM(TEXT("waterlogged_amb_spring_day_LP_DST.wav"), 0.5f);
	CGameInstance::Get_Instance()->PlayBGM(TEXT("CreepyForest_vinyl_mastered.wav"), 0.3f);
	m_fMusicTime = GetTickCount();
	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	if (m_fMusicTime + 9000 < GetTickCount())
	{
		if (!m_bMusicStart)
		{
			if (m_bBeargerAdd)
			{
				CGameInstance::Get_Instance()->PlayBGM(TEXT("DST_BossFightNo3_V1.wav"), 0.3f);
			}
			else
			{
				switch (m_eDayState)
				{
				case Client::DAY_MORNING:
					CGameInstance::Get_Instance()->PlayBGM(TEXT("CreepyForest_vinyl_mastered.wav"), 0.3f);
					break;
				case Client::DAY_DINNER:
					CGameInstance::Get_Instance()->PlayBGM(TEXT("CreepyForest_vinyl_mastered.wav"), 0.3f);
					break;
				case Client::DAY_NIGHT:
					CGameInstance::Get_Instance()->PlayBGM(TEXT("DSS_marsh_mild_NIGHT_LP.wav"), 0.2f);
					break;
				}
			}
			
			m_bMusicStart = true;
		}
		m_fMusicTime = GetTickCount();
	}

	CDayCycle::Get_Instance()->DayCycleTick();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	

	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();

	if (m_bNextLevel)
	{
		
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, iLevel))))
			return;
	}

	if (!m_bNextLevel)
	{
		CPickingMgr::Get_Instance()->Picking();
	}

	m_fTimeAcc += CGameInstance::Get_Instance()->Get_TimeDelta(TEXT("Timer_60"));
	if (m_fTimeAcc > 1.f / 10.f)
	{
		CParticle::STATEDESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticle::STATEDESC));
		ParticleDesc.eTextureScene = LEVEL_GAMEPLAY;
		ParticleDesc.pTextureKey = TEXT("Prototype_Component_Texture_Leaf");
		ParticleDesc.iTextureNum = rand() % 7;
		ParticleDesc.vVelocity = _float3((rand() % 3)*0.1f, -0.1f, -(rand() % 3) * 0.1f);

		if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("GameObject_Particle"), ParticleDesc.eTextureScene, TEXT("Layer_Particle"), &ParticleDesc)))
			return;

		m_fTimeAcc = 0.f;
	}

	Safe_Release(pGameInstance);


}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CLevel_GamePlay::Update(_uint eDayState)
{
	m_eDayState = DAY_STATE(eDayState);
	m_fMusicTime = GetTickCount(); 
	CGameInstance::Get_Instance()->StopSound(SOUND_BGM);
	m_bMusicStart = false;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, TEXT("Layer_Sky"), nullptr)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectLoad(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag,
		TEXT("Prototype_Component_VIBuffer_Terrain_Load0"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Water"), LEVEL_GAMEPLAY, pLayerTag, _float3(-10.f, -0.75f, -10.f))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_Bearger"), LEVEL_GAMEPLAY, TEXT("Layer_Bear"), _float3(10.f, 0.f, 40.f))))
		return E_FAIL;*/

	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Pig_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;
	_float3 ObjectPos(0, 0, 0);
	_uint iNum = 0;

	
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pig"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);

	/*pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrorbeak"), LEVEL_GAMEPLAY, pLayerTag, _float3(40.f, 0.f, 20.f));*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* Load Tree */
	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Tree_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;
	_float3 ObjectPos(0, 0, 0);
	_uint iNum = 0;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tree"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load Grass */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/Grass_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load Rock */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/Rock_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boulder"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load Berry */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/Berry_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Berry_Bush"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);

	/* Load House */
	hFile = CreateFile(TEXT("../Bin/Resources/Data/House_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CHouse::HOUSEDECS HouseDesc;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(HouseDesc), sizeof(CHouse::HOUSEDECS), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_GAMEPLAY, TEXT("Layer_House"), &HouseDesc);
	}
	CloseHandle(hFile);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pig_King"), LEVEL_GAMEPLAY, pLayerTag, _float3(40.f, 1.f, 30.f))))
		return E_FAIL;


	CPortal::PORTALDESC PortalDesc;
	PortalDesc.m_eType = CPortal::PORTAL_HUNT;
	PortalDesc.vPosition = _float3(45.f, 1.f, 26.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_GAMEPLAY, pLayerTag, &PortalDesc)))
		return E_FAIL;


	PortalDesc.m_eType = CPortal::PORTAL_MAZE;
	PortalDesc.vPosition = _float3(20.f, 1.f, 26.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_GAMEPLAY, pLayerTag, &PortalDesc)))
		return E_FAIL;


	PortalDesc.m_eType = CPortal::PORTAL_BOSS;
	PortalDesc.vPosition = _float3(30.f, 1.f, 16.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_GAMEPLAY, pLayerTag, &PortalDesc)))
		return E_FAIL;


	hFile = CreateFile(TEXT("../Bin/Resources/Data/Carrot_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	ObjectPos = _float3(0, 0, 0);
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carrot"), LEVEL_GAMEPLAY, pLayerTag, ObjectPos);
	}
	CloseHandle(hFile);


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")) == nullptr)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGround"), LEVEL_STATIC, pLayerTag, _float3(10.f, 0.0f, 5.f))))
			return E_FAIL;
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

		LEVEL ePastLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_PastLevelIndex();
		switch (ePastLevel)
		{
		case Client::LEVEL_HUNT:
			pPlayer->Set_Position(_float3(45.f, 0.5f, 27.f));
			break;
		case Client::LEVEL_MAZE:
			pPlayer->Set_Position(_float3(22.f, 0.5f, 26.f));
			break;
		case Client::LEVEL_BOSS:
			pPlayer->Set_Position(_float3(32.f, 0.5f, 17.f));
			break;
		}
		
		
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCameraDynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCameraDynamic::CAMERADESC_DERIVED));

	CameraDesc.vDistance = _float3(0, 4, -9);

	CameraDesc.CameraDesc.vEye = _float3(0.f, 2.f, -5.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.5f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;
	CameraDesc.CameraDesc.fFovy = D3DXToRadian(60.0f);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_FPS"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;
	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.vEye = _float3(0.f, 6.f, -7.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Target"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_PlayerStatUI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Playerhp"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Playerhunger"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerMentality"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Daycount"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Daycountpont"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_quest"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_questpont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}
	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Line"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}
	


	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_hungerPont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MentalityPont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MainInventory(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BagInventory"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;


	for (int i = 0; i < 18; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}


	for (int i = 0; i < 18; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;
	}


	for (int i = 0; i < 36; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MainToolbox(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainToolbox"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainToolbox_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainToolbox_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_talk"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Deadmain"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerStatus"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skillicon"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 2; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Deadcount"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 2; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Daypont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Monsterhp_pont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerStatus_pont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Button"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;


	

	
	
	
	/*_bool bearger = true; //for bossnametest

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BossName"), LEVEL_STATIC, pLayerTag, (bool*)&bearger)))
		return E_FAIL;*/

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Party"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Partyhp"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	
	
	



	//vector<_uint> randombox;  //cardgame random shufflle

	//for (int i = 0; i < 4; i++)
	//{
	//	randombox.push_back(i);
	//	randombox.push_back(i);
	//}

	//randombox.push_back(4);

	//random_shuffle(randombox.begin(), randombox.end());

	//for (auto& iter = randombox.begin(); iter != randombox.end();)
	//{
	//	
	//	for (int i = 0; i < 9; ++i)
	//	{
	//		int number = *iter;

	//		if (failed(pgameinstance->add_gameobject(text("prototype_gameobject_cardgame"), level_gameplay, playertag, (int*)&number)))
	//			return e_fail;

	//		++iter;

	//	}
	//}

	
	

	

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_player"), LEVEL_STATIC, pLayerTag)))
	//	return E_FAIL;

	
	
	
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Endingscene"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	
	
	


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_WeaponToolbox(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WeaponToolbox"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 5; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WeaponToolbox_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 5; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WeaponToolbox_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_GatherToolbox"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_GatherToolbox_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 3; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_GatherToolbox_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ConstructToolbox"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 5; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ConstructToolbox_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 5; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ConstructToolbox_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MaterialToolbox"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 5; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MaterialToolbox_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 5; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MaterialToolbox_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}


	for (int i = 0; i < 1; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Craftmain"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 2; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Craftmain_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 2; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Craftmain_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Craftbutton"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	for (int i = 0; i < 8; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CraftPont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Potmain"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

		for (int i = 0; i < 4; ++i)
		{
			int number = i;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pot_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
				return E_FAIL;

		}
		for (int i = 0; i < 4; ++i)
		{
			int number = i;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pot_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
				return E_FAIL;

		}
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Potbutton"), LEVEL_STATIC, pLayerTag)))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mouse_item"), LEVEL_STATIC, pLayerTag)))
			return E_FAIL;
		
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_questbutton"), LEVEL_STATIC, pLayerTag)))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mouse_iteminfo"), LEVEL_STATIC, pLayerTag)))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Eateffect"), LEVEL_STATIC, pLayerTag)))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mouse_Monster"), LEVEL_STATIC, pLayerTag)))
			return E_FAIL;
		
		
	

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LayerPont(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	for (int i = 0; i < 10; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pont"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LayerNPC(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_NPC")) == nullptr)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NPC_Wendy"), LEVEL_STATIC, pLayerTag, _float3(10.f, 1.f, 5.f))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NPC_Winona"), LEVEL_STATIC, pLayerTag, _float3(20.f, 1.f, 10.f))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NPC_Woodie"), LEVEL_STATIC, pLayerTag, _float3(20.f, 1.f, 10.f))))
			return E_FAIL;
	}
	else
	{
		/*CNPC* pPlayer = (CNPC*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_NPC"));
		pPlayer->Set_Position(_float3(45.f, 0.5f, 27.f));*/
	}

	Safe_Release(pGameInstance);

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Equipment_back(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Equipment_back"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	for (int i = 0; i < 4; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Equipment_front"), LEVEL_STATIC, pLayerTag, (int*)&i)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}




CLevel_GamePlay * CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}
