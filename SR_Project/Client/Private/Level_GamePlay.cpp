#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "CameraDynamic.h"
#include "PickingMgr.h"
#include "House.h"
#include "Level_Loading.h"
#include "CameraManager.h"
#include "Player.h"

_bool g_bUIMadefirst = false;

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
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

	//if (FAILED(Ready_LayerNPC(TEXT("Layer_NPC"))))
		//return E_FAIL;

	if (g_bLoadingfirst == false)
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

		g_bUIMadefirst = true;
	}

	

	CPickingMgr::Get_Instance()->Clear_PickingMgr();
	CPickingMgr::Get_Instance()->Ready_PickingMgr(LEVEL::LEVEL_GAMEPLAY);

	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_GAMEPLAY);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->PlayBGM(TEXT("Filed.mp3"), 0.1f);

	if (m_bNextLevel)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_HUNT))))
			return;
	}

	if (!m_bNextLevel)
	{
		CPickingMgr::Get_Instance()->Picking();
	}

	Safe_Release(pGameInstance);


}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

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
	
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bearger"), LEVEL_GAMEPLAY, pLayerTag, _float3(22.f, 0.f, 22.f))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boarrior"), LEVEL_GAMEPLAY, pLayerTag, _float3(30.f, 0.f, 22.f))))
		return E_FAIL;

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

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_GAMEPLAY, pLayerTag, _float3(45.f, 1.f, 26.f))))
		return E_FAIL;

	// Test: CARROT
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carrot"), LEVEL_GAMEPLAY, pLayerTag, _float3(36.f, 1.f, 26.f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carrot"), LEVEL_GAMEPLAY, pLayerTag, _float3(42.f, 1.f, 26.f))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carrot"), LEVEL_GAMEPLAY, pLayerTag, _float3(40.f, 1.f, 28.f))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")) == nullptr)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGround"), LEVEL_STATIC, pLayerTag, _float3(10.f, 0.5f, 5.f))))
			return E_FAIL;
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		pPlayer->Set_Position(_float3(45.f, 0.5f, 27.f));
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

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float3(0.f, 2.f, -5.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(30.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_FPS"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
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


	for (int i = 0; i < 20; ++i)
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
