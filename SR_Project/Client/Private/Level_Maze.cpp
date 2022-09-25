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
#include "DecoObject.h"
#include "Trap.h"
#include "Shooting_Target.h"
#include "Dirt.h"
#include "CarnivalMemory.h"
#include "Cardgame.h"
#include "Inventory.h"

CLevel_Maze::CLevel_Maze(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Maze::Initialize()
{
	CGameInstance::Get_Instance()->StopSound(SOUND_BGM);
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

	//Start_Camera_Motion();
	Update_Fence_Motion();
	Update_Camera_Motion();
	Update_Floor_Motion();

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
	//pPlayer->Set_Position(_float3(39.75f, 0.5f, 38.0f));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Maze::Ready_Layer_Monster(const _tchar * pLayerTag)
{


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


	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/RockWall_Stage5_Vertical_and_Horizontal.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

	hFile = CreateFile(TEXT("../Bin/Resources/Data/Dirt_Stage4.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	_uint iRandKey = rand() % iNum;
	for (_uint i = 0; i < iNum; ++i)
	{
		CDirt::DIRTDESC DirtDesc;
		ReadFile(hFile, &(DirtDesc.vInitPosition), sizeof(_float3), &dwByte, nullptr);

		if (iRandKey == i)
			DirtDesc.bHasKey = true;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dirt"), LEVEL_MAZE, pLayerTag, &DirtDesc)))
			return E_FAIL;
	}
	CloseHandle(hFile);


	hFile = CreateFile(TEXT("../Bin/Resources/Data/Trap_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CTrap::TRAPDESC tTrapDesc;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(tTrapDesc), sizeof(CTrap::TRAPDESC), &dwByte, nullptr);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Trap"), LEVEL_MAZE, TEXT("Layer_Trap"), &tTrapDesc)))
			return E_FAIL;
	}
	CloseHandle(hFile);

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Statue"), LEVEL_MAZE, TEXT("Layer_Statue"), _float3(39.75f, 0.f, 9.f));

	CShooting_Target::TARGETDESC TargetDesc;
	TargetDesc.eType = CShooting_Target::TARGET_BAD;
	TargetDesc.vPosition = _float3(35.75f, 0.f, 42.f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ShootingTarget"), LEVEL_MAZE, TEXT("Layer_Shooting"), &TargetDesc);

	TargetDesc.vPosition = _float3(37.75f, 0.f, 42.2f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ShootingTarget"), LEVEL_MAZE, TEXT("Layer_Shooting"), &TargetDesc);

	TargetDesc.vPosition = _float3(39.75f, 0.f, 42.3f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ShootingTarget"), LEVEL_MAZE, TEXT("Layer_Shooting"), &TargetDesc);

	TargetDesc.vPosition = _float3(41.75f, 0.f, 42.2f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ShootingTarget"), LEVEL_MAZE, TEXT("Layer_Shooting"), &TargetDesc);

	TargetDesc.eType = CShooting_Target::TARGET_GOOD;
	TargetDesc.vPosition = _float3(43.75f, 0.f, 42.f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ShootingTarget"), LEVEL_MAZE, TEXT("Layer_Shooting"), &TargetDesc);

	// Carnival Memory Game
	hFile = CreateFile(TEXT("../Bin/Resources/Data/Carnival_Card_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CCarnivalCard::DESC Desc;
	Desc.eType = CCarnivalCard::TYPE::CARD;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(Desc.vInitPosition), sizeof(_float3), &dwByte, nullptr);
		Desc.vInitPosition += _float3(0.7f, 0.f, 1.f);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Card"), LEVEL_MAZE, pLayerTag, &Desc)))
			return E_FAIL;
	}
	CloseHandle(hFile);

	hFile = CreateFile(TEXT("../Bin/Resources/Data/Carnival_Bird_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	Desc.eType = CCarnivalCard::TYPE::BIRD;
	iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(Desc.vInitPosition), sizeof(_float3), &dwByte, nullptr);
		Desc.vInitPosition += _float3(0.1f, 0.f, 0.0f);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Card"), LEVEL_MAZE, pLayerTag, &Desc)))
			return E_FAIL;
	}
	CloseHandle(hFile);

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CCarnival_Shooter"), LEVEL_MAZE, TEXT("Layer_Shooter"), _float3(39.75f, 0.f, 38.0f));
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Shoot_Button"), LEVEL_MAZE, pLayerTag, _float3(41.75f, 0.f, 39.0f));


	CCardgame::CARDDESC CardDesc;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			CardDesc.iNumber = i*3 + j;
			CardDesc.pos = _float3(22.f + 3.f*i, 0.5f, 22.f + 3.f*j);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Cardgame"), LEVEL_MAZE, TEXT("Layer_Card"), &CardDesc)))
				return E_FAIL;
		}
	}


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

	CameraDesc.CameraDesc.vEye = _float3(0.f, 5.f, -5.f);
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

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.vEye = _float3(0.f, 5.f, -8.f);
	CameraDesc.vDistance = _float3(0, 7, -6);
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
		pGameObject->Get_Position().x < 29 && pGameObject->Get_Position().z < 15)
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

void CLevel_Maze::Update_Floor_Motion()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));



	if ((pGameObject->Get_Position().x > 35 && !m_bPuzzleStart[0]))
	{
		// Defend the Tower
		CDecoObject::DECODECS  DecoDesc;
		DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR;
		DecoDesc.vInitPosition = _float3(39.75f, 0.f, 9.f);
		DecoDesc.fRotate = 0.f;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);
		m_bPuzzleStart[0] = true;
	}
	else if ((pGameObject->Get_Position().x > 35) && (pGameObject->Get_Position().z > 20) && !m_bPuzzleStart[1])
	{
		// Find the Key
		/*CDecoObject::DECODECS  DecoDesc;
		DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR;
		DecoDesc.vInitPosition = _float3(39.75f, 0.f, 24.f);
		DecoDesc.fRotate = 1.f;
		CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);*/
		m_bPuzzleStart[1] = true;
	}
	else if ((pGameObject->Get_Position().x > 35) && (pGameObject->Get_Position().z > 36) && !m_bPuzzleStart[2])
	{
		// Shooting
		CDecoObject::DECODECS  DecoDesc;
		DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR;
		DecoDesc.fRotate = 5.f;
		DecoDesc.vInitPosition = _float3(39.75f, 0.f, 42.f);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);

		DecoDesc.fRotate = 2.f;
		DecoDesc.vInitPosition = _float3(39.75f, 0.f, 38.f);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);
		m_bPuzzleStart[2] = true;
	}
	else if ((pGameObject->Get_Position().x < 28) && (pGameObject->Get_Position().z > 35) && !m_bPuzzleStart[3])
	{
		// Memory
		CDecoObject::DECODECS  DecoDesc;
		DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR;
		DecoDesc.vInitPosition = _float3(25.f, 0.f, 40.f);
		DecoDesc.fRotate = 3.f;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);
		m_bPuzzleStart[3] = true;

		CCarnivalMemory::STATIONDESC StationDesc;
		StationDesc.eType = CCarnivalMemory::STATIONTYPE::STATION_MEMORY;
		StationDesc.vInitPosition = _float3(25.00f, 0.f, 41.8f);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Memory"), LEVEL_MAZE, TEXT("Layer_Object"), &StationDesc);
	}
	else if ((pGameObject->Get_Position().x < 11) && (pGameObject->Get_Position().z > 35) && !m_bPuzzleStart[4])
	{
		// Egg
		CCarnivalMemory::STATIONDESC StationDesc;
		StationDesc.eType = CCarnivalMemory::STATIONTYPE::STATION_EGG;
		StationDesc.vInitPosition = _float3(9.5f, 0.f, 40.f);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Memory"), LEVEL_MAZE, TEXT("Layer_Object"), &StationDesc);
		

		CDecoObject::DECODECS  DecoDesc;
		DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR;
		DecoDesc.vInitPosition = _float3(9.5f, 0.f, 40.f);
		DecoDesc.fRotate = 4.f;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);
		m_bPuzzleStart[4] = true;
		pGameInstance->PlaySounds(TEXT("Place_Carnivalgame_Herding_Station_DST_01.wav"), SOUND_OBJECT, 0.5f);
	}
	else if ((pGameObject->Get_Position().x < 11) && (pGameObject->Get_Position().z < 27) && (pGameObject->Get_Position().z > 20) && !m_bPuzzleStart[5])
	{
		// Bird
		CDecoObject::DECODECS  DecoDesc;
		DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR;
		DecoDesc.vInitPosition = _float3(9.5f, 0.f, 24.f);
		DecoDesc.fRotate = 1.f;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);
		m_bPuzzleStart[5] = true;

		CCarnivalMemory::STATIONDESC StationDesc;
		StationDesc.eType = CCarnivalMemory::STATIONTYPE::STATION_BIRD;
		StationDesc.vInitPosition = _float3(9.5f, 0.f, 24.f);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Memory"), LEVEL_MAZE, TEXT("Layer_Object"), &StationDesc);
	}

	else if ((pGameObject->Get_Position().x < 20) && (pGameObject->Get_Position().z < 26) && (pGameObject->Get_Position().z > 25) && !m_bPuzzleStart[6])
	{
		// cardgame
		CInventory_Manager::Get_Instance()->Start_Cardgame();
	}

	Safe_Release(pGameInstance);
}

void CLevel_Maze::Update_Fence_Motion()
{
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	if ((pGameObject->Get_Position().x > 13 && pGameObject->Get_Position().x < 20 ) && !m_bPuzzleStart[0])
		Set_PuzzleSolved(true);
	else if ( pGameObject->Get_Position().x > 20 && pGameObject->Get_Position().x < 29 && !m_bPuzzleStart[0])
		Set_PuzzleSolved(false);
	else if (pGameObject->Get_Position().x > 29 && !m_bPuzzleStart[0])
		Set_PuzzleSolved(true);
	if ((pGameObject->Get_Position().x > 35 && !m_bPuzzleStart[0]))
	{
		Set_PuzzleSolved(false);
	}
	else if ((pGameObject->Get_Position().x > 35) && (pGameObject->Get_Position().z > 20) && !m_bPuzzleStart[1])
	{
		Set_PuzzleSolved(false);
	}
	else if ((pGameObject->Get_Position().x > 35) && (pGameObject->Get_Position().z > 36) && !m_bPuzzleStart[2])
	{
		Set_PuzzleSolved(false);
	}
	else if ((pGameObject->Get_Position().x < 28) && (pGameObject->Get_Position().z > 35) && !m_bPuzzleStart[3])
	{
		Set_PuzzleSolved(false);
	}
	else if ((pGameObject->Get_Position().x < 11) && (pGameObject->Get_Position().z > 35) && !m_bPuzzleStart[4])
	{
		Set_PuzzleSolved(false);
	}
	else if ((pGameObject->Get_Position().x < 11) && (pGameObject->Get_Position().z < 27) && (pGameObject->Get_Position().z > 20) && !m_bPuzzleStart[5])
	{
		Set_PuzzleSolved(false);
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
