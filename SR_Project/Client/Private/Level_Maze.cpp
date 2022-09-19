#include "stdafx.h"
#include "..\Public\Level_Maze.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "House.h"
#include "Player.h"
#include "CameraManager.h"
#include "Level_Loading.h"
#include "WoodWall.h"
#include "Portal.h"

CLevel_Maze::CLevel_Maze(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Maze::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		//return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	CPickingMgr::Get_Instance()->Clear_PickingMgr();
	CPickingMgr::Get_Instance()->Ready_PickingMgr(LEVEL::LEVEL_MAZE);

	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_MAZE);
	m_dwTime = GetTickCount();
	return S_OK;
}

void CLevel_Maze::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	__super::Tick(fTimeDelta);

	if (pGameInstance->Key_Up('0'))
	{
		if (m_bPuzzleSolved)
			m_bPuzzleSolved = false;
		else
			m_bPuzzleSolved = true;
	}


	if (m_bNextLevel)
	{
		LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, iLevel))))
			return;
	}

	if (!m_bNextLevel)
	{
		CPickingMgr::Get_Instance()->Picking();
	}

	Start_Camera_Motion();
	Update_Camera_Motion();

	Safe_Release(pGameInstance);
}

void CLevel_Maze::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("�̷η����Դϴ�."));
}

HRESULT CLevel_Maze::Ready_Layer_BackGround(const _tchar * pLayerTag)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	int iTextNum = 1;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_MAZE, TEXT("Layer_Sky"), &iTextNum)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectLoad(TEXT("Prototype_GameObject_Terrain"), LEVEL_MAZE, pLayerTag,
		TEXT("Prototype_Component_VIBuffer_Terrain_Load0"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Water"), LEVEL_MAZE, pLayerTag, _float3(-20.f, +0.001f, -20.f))))
		return E_FAIL;


	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	pPlayer->Set_Position(_float3(9.0, 0.5f, 7.f));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Maze::Ready_Layer_Monster(const _tchar * pLayerTag)
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Spider_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;
	_float3 ObjectPos(0, 0, 0);
	_uint iNum = 0;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Spider"), LEVEL_MAZE, pLayerTag, ObjectPos)))
			return E_FAIL;
	}

	CloseHandle(hFile);



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Maze::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPortal::PORTALDESC PortalDesc;


	PortalDesc.m_eType = CPortal::PORTAL_BOSS;
	PortalDesc.vPosition = _float3(25.f, 2.f, 25.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_MAZE, pLayerTag, &PortalDesc)))
		return E_FAIL;

	PortalDesc.m_eType = CPortal::PORTAL_GAMEPLAY;
	PortalDesc.vPosition = _float3(9.0f, 2.f, 8.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_MAZE, pLayerTag, &PortalDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dirt"), LEVEL_MAZE, pLayerTag, _float3(9.f, 2.f, 45.f))))
		return E_FAIL;

	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/RockWall_Stage4_Vertical_and_Horizontal.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;


	_ulong dwByte = 0;
	CWoodWall::WALLDESC WallDesc;
	_uint iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(WallDesc), sizeof(CWoodWall::WALLDESC), &dwByte, nullptr);
		WallDesc.etype = CWoodWall::WALL_MAZE;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WoodWall"), LEVEL_MAZE, TEXT("Layer_Wall"), &WallDesc);
	}


	CloseHandle(hFile);


	hFile = CreateFile(TEXT("../Bin/Resources/Data/PuzzleDoor_MazeMap.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;
	dwByte = 0;
	iNum = 0;
	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(WallDesc), sizeof(CWoodWall::WALLDESC), &dwByte, nullptr);
		WallDesc.eDir = CWoodWall::WALL_DIREND;
		WallDesc.etype = CWoodWall::WALL_PUZZLE;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WoodWall"), LEVEL_MAZE, TEXT("Layer_Wall"), &WallDesc);
	}
	CloseHandle(hFile);


	// Test Spawner
	hFile = CreateFile(TEXT("../Bin/Resources/Data/House_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CHouse::HOUSEDECS HouseDesc;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(HouseDesc), sizeof(CHouse::HOUSEDECS), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_MAZE, TEXT("Layer_House"), &HouseDesc);
	}
	CloseHandle(hFile);


	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Statue"), LEVEL_MAZE, TEXT("Layer_Statue"), _float3(39.f, 0.f, 9.f));

	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CLevel_Maze::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCameraDynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCameraDynamic::CAMERADESC_DERIVED));

	CameraDesc.vDistance = _float3(0, 7, -6);

	CameraDesc.CameraDesc.vEye = _float3(0.f, 2.f, -5.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_MAZE, pLayerTag, &CameraDesc)))
		return E_FAIL;


	CameraDesc.CameraDesc.fFovy = D3DXToRadian(70.0f);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_FPS"), LEVEL_MAZE, pLayerTag, &CameraDesc)))
		return E_FAIL;

	CameraDesc.CameraDesc.vEye = _float3(0.f, 6.f, -7.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Target"), LEVEL_MAZE, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Maze::Start_Camera_Motion()
{
	if (!m_bTargetCam && m_dwTime + 1000 < GetTickCount())
	{
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_TARGET);
		CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_MAZE, TEXT("Layer_Object"));
		pCamera->Set_Target(pGameObject);
		pCamera->Set_GoingMode(true);
		m_bFirst = true;
		m_bTargetCam = true;
		m_dwTime = GetTickCount();
	}

	if (m_dwTime + 4000 < GetTickCount() && m_bFirst)
	{
		CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		pCamera->Set_GoingMode(false);
		m_bFirst = false;
	}
}


void CLevel_Maze::Update_Camera_Motion()
{
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	if (m_bFlowerPicked)
	{
		if (m_dwTime + 3000 < GetTickCount())
		{
			dynamic_cast<CPlayer*>(pGameObject)->Set_FPSMode(true);
			CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_FPS);
			m_bFlowerPicked = false;
		}
	}


	if (pGameObject->Get_Position().x > 19 && pGameObject->Get_Position().z > 0.5 &&
		pGameObject->Get_Position().x < 29 && pGameObject->Get_Position().z < 13 )
	{
		dynamic_cast<CPlayer*>(pGameObject)->Set_FPSMode(true);
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_FPS);
		m_bPlayerCam = true;
	
	}
	else if (m_bPlayerCam)
	{
		dynamic_cast<CPlayer*>(pGameObject)->Set_FPSMode(false);
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
		m_bPlayerCam = false;
	}
	

}


CLevel_Maze * CLevel_Maze::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Maze*	pInstance = new CLevel_Maze(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Maze"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Maze::Free()
{
	__super::Free();


}
