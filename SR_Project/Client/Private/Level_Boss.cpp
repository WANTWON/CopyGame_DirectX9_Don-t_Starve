
#include "stdafx.h"
#include "..\Public\Level_Boss.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "House.h"
#include "Player.h"
#include "CameraManager.h"
#include "Level_Loading.h"
#include "WoodWall.h"
#include "DecoObject.h"
#include "Totem.h"
#include "Portal.h"
#include "Particle.h"
#include "DayCycle.h"

CLevel_Boss::CLevel_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Boss::Initialize()
{
	CGameInstance::Get_Instance()->StopAll();
	CDayCycle::Get_Instance()->AllRemoveObserver();
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;



	CPickingMgr::Get_Instance()->Clear_PickingMgr();
	CPickingMgr::Get_Instance()->Ready_PickingMgr(LEVEL::LEVEL_BOSS);

	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_BOSS);
	m_dwTime = GetTickCount();
	CGameInstance::Get_Instance()->PlayBGM(TEXT("DSTER - Boss 1 Orchestration V2.wav"), 0.5f);
	return S_OK;
}

void CLevel_Boss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	CDayCycle::Get_Instance()->DayCycleTick();
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_bNextLevel)
	{
		LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, iLevel))))
			return;
	}

	if (m_bPortalMake)
	{
		m_bPortalMake = false;
		CPortal::PORTALDESC PortalDesc;
		PortalDesc.m_eType = CPortal::PORTAL_GAMEPLAY;
		PortalDesc.vPosition = _float3(14.f, 1.f, 15.f);

		if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_BOSS, TEXT("Layer_Object"), &PortalDesc)))
			return;


	}

	if (!m_bNextLevel)
		CPickingMgr::Get_Instance()->Picking();

	m_fTimeAcc += CGameInstance::Get_Instance()->Get_TimeDelta(TEXT("Timer_60"));
	if (m_fTimeAcc > 1.f / 10.f)
	{
		CParticle::STATEDESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticle::STATEDESC));
		ParticleDesc.eTextureScene = LEVEL_BOSS;
		ParticleDesc.pTextureKey = TEXT("Prototype_Component_Texture_Magma");
		ParticleDesc.iTextureNum = 0;
		ParticleDesc.vVelocity = _float3((rand() % 3)*0.1f, -0.1f, -(rand() % 3) * 0.1f);

		if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("GameObject_Particle"), ParticleDesc.eTextureScene, TEXT("Layer_Particle"), &ParticleDesc)))
			return;

		m_fTimeAcc = 0.f;
	}

	Start_Camera_Motion();

	Safe_Release(pGameInstance);
}

void CLevel_Boss::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("보스레벨입니다."));
}

HRESULT CLevel_Boss::Ready_Layer_BackGround(const _tchar * pLayerTag)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	int iTextNum = 0;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_BOSS, TEXT("Layer_Sky"), &iTextNum)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectLoad(TEXT("Prototype_GameObject_Terrain"), LEVEL_BOSS, pLayerTag,
		TEXT("Prototype_Component_VIBuffer_Terrain_Load0"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Water"), LEVEL_BOSS, pLayerTag, _float3(-20.f, -0.5f, -20.f))))
		return E_FAIL;


	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	pPlayer->Set_Position(_float3(15, 0.5f, 5));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boarrior"), LEVEL_BOSS, pLayerTag, _float3(15.f, 0.f, 15.f))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/RockWall_Stage4.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	
	_ulong dwByte = 0;
	CWoodWall::WALLDESC WallDesc;
	_uint iNum = 0;
	//vector<_tchar*> vecPath;


	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(WallDesc), sizeof(CWoodWall::WALLDESC), &dwByte, nullptr);
		WallDesc.eDir = CWoodWall::WALL_DIREND;
		WallDesc.etype = CWoodWall::WALL_BOSS;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WoodWall"), LEVEL_BOSS, TEXT("Layer_Wall"), &WallDesc);
	}

	
	CloseHandle(hFile);

	// Test Spawner
	hFile = CreateFile(TEXT("../Bin/Resources/Data/House_Stage4.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CHouse::HOUSEDECS HouseDesc;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(HouseDesc), sizeof(CHouse::HOUSEDECS), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_BOSS, TEXT("Layer_House"), &HouseDesc);
	}
	CloseHandle(hFile);

	hFile = CreateFile(TEXT("../Bin/Resources/Data/Deco_Stage4.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CDecoObject::DECODECS DecoDesc;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(DecoDesc), sizeof(CDecoObject::DECODECS), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_BOSS, TEXT("Layer_Deco"), &DecoDesc);
	}
	CloseHandle(hFile);

	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CLevel_Boss::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCameraDynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCameraDynamic::CAMERADESC_DERIVED));

	CameraDesc.vDistance = _float3(0.f, 8.f, -15.f);

	CameraDesc.CameraDesc.vEye = _float3(0.f, 1.f, -8.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 1.5f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_BOSS, pLayerTag, &CameraDesc)))
		return E_FAIL;


	CameraDesc.CameraDesc.fFovy = D3DXToRadian(45.0f);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_FPS"), LEVEL_BOSS, pLayerTag, &CameraDesc)))
		return E_FAIL;

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Target"), LEVEL_BOSS, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Boss::Start_Camera_Motion()
{
	if (!m_bTargetCam && m_dwTime + 1000 < GetTickCount())
	{
		
		
			
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		
		pGameInstance->PlaySounds(TEXT("bosstalk2.wav"), SOUND_UI, 0.6f);
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_TARGET);
		CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_BOSS, TEXT("Layer_Monster"));
		pCamera->Set_Target(pGameObject);

		_bool bearger = false;

		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BossName"), LEVEL_STATIC, TEXT("Layer_BossName"), (bool*)&bearger);
			
			
		pCamera->Set_TalkingMode(true);
		m_dwTime = GetTickCount();
		m_bFirst = true;
		m_bTargetCam = true;
	}

	if (m_dwTime + 5000 < GetTickCount() && m_bFirst)
	{
		CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		pCamera->Set_TalkingMode(false);
		m_bFirst = false;
	}
}



CLevel_Boss * CLevel_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Boss*	pInstance = new CLevel_Boss(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Hunt"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Boss::Free()
{
	__super::Free();


}
