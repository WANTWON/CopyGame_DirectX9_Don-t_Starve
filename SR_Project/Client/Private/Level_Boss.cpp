
#include "stdafx.h"
#include "..\Public\Level_Boss.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "House.h"
#include "Player.h"
#include "CameraManager.h"
#include "Level_Loading.h"

CLevel_Boss::CLevel_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Boss::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	//if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
	//	return E_FAIL;



	//CPickingMgr::Get_Instance()->Clear_PickingMgr();
	//CPickingMgr::Get_Instance()->Ready_PickingMgr(LEVEL::LEVEL_BOSS);

	//CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_BOSS);

	return S_OK;
}

void CLevel_Boss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

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

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Water"), LEVEL_BOSS, pLayerTag, _float3(-20.f, -0.01f, -20.f))))
		return E_FAIL;


	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	pPlayer->Set_Position(_float3(25, 1, 25));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const _tchar * pLayerTag)
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
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Spider"), LEVEL_BOSS, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Tree_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;
	_float3 ObjectPos(0, 0, 0);
	_uint iNum = 0;

	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tree"), LEVEL_BOSS, pLayerTag, ObjectPos);
	}

	CloseHandle(hFile);

	//hFile = CreateFile(TEXT("../Bin/Resources/Data/Grass_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//	return E_FAIL;

	//dwByte = 0;
	//ObjectPos = _float3(0, 0, 0);
	//iNum = 0;

	///* ù���� object ����Ʈ�� size �޾Ƽ� ������ŭ for�� ������ �Ϸ��� �����س���*/
	//ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	//for (_uint i = 0; i < iNum; ++i)
	//{
	//	ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
	//	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_HUNT, pLayerTag, ObjectPos);
	//}

	//CloseHandle(hFile);


	//hFile = CreateFile(TEXT("../Bin/Resources/Data/Rock_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//	return E_FAIL;

	//dwByte = 0;
	//ObjectPos = _float3(0, 0, 0);
	//iNum = 0;

	///* ù���� object ����Ʈ�� size �޾Ƽ� ������ŭ for�� ������ �Ϸ��� �����س���*/
	//ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	//for (_uint i = 0; i < iNum; ++i)
	//{
	//	ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
	//	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boulder"), LEVEL_HUNT, pLayerTag, ObjectPos);
	//}

	//CloseHandle(hFile);

	//hFile = CreateFile(TEXT("../Bin/Resources/Data/Berry_Stage1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//	return E_FAIL;

	//dwByte = 0;
	//ObjectPos = _float3(0, 0, 0);
	//iNum = 0;

	///* ù���� object ����Ʈ�� size �޾Ƽ� ������ŭ for�� ������ �Ϸ��� �����س���*/
	//ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	//for (_uint i = 0; i < iNum; ++i)
	//{
	//	ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
	//	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Berry_Bush"), LEVEL_HUNT, pLayerTag, ObjectPos);
	//}

	//CloseHandle(hFile);

	hFile = CreateFile(TEXT("../Bin/Resources/Data/House_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	dwByte = 0;
	CHouse::HOUSEDECS HouseDesc;
	iNum = 0;

	/* ù���� object ����Ʈ�� size �޾Ƽ� ������ŭ for�� ������ �Ϸ��� �����س���*/
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(HouseDesc), sizeof(CHouse::HOUSEDECS), &dwByte, nullptr);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_BOSS, TEXT("Layer_House"), &HouseDesc);
	}

	CloseHandle(hFile);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_BOSS, pLayerTag, _float3(25.f, 1.5f, 26.f))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CLevel_Boss::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_BOSS, pLayerTag, &CameraDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_FPS"), LEVEL_BOSS, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
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
