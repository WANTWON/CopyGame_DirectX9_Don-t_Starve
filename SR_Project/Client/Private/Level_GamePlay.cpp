#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "CameraDynamic.h"

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

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"), _float3(0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainInventory(TEXT("Layer_MainInventory"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainInventory_back(TEXT("Layer_MainInventory_back"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainInventory_front(TEXT("Layer_MainInventory_front"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Pont(TEXT("Layer_Pont"))))
		return E_FAIL;
	
	


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag, _float3 Pos)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag, &Pos)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Monster"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGround"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
		return E_FAIL;

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

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MainInventory(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_MainInventory_back(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 10; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory_back"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MainInventory_front(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 10; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory_front"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Pont(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	for (int i = 0; i < 10; ++i)
	{
		int number = i;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pont"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
			return E_FAIL;

	}

	Safe_Release(pGameInstance);

	return S_OK;
}

//HRESULT CLevel_GamePlay::Ready_Layer_MainInventory_front(const _tchar * pLayerTag)
//{
//	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
//	Safe_AddRef(pGameInstance);
//
//
//	for (int i = 0; i < 10; ++i)
//	{
//		int number = i;
//
//		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MainInventory_back"), LEVEL_GAMEPLAY, pLayerTag, (int*)&i)))
//			return E_FAIL;
//	}
//
//	Safe_Release(pGameInstance);
//
//	return S_OK;
//}



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
