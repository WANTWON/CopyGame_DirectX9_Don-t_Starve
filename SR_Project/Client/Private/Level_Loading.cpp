#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Hunt.h"
#include "Level_Boss.h"
#include "Loadingscene.h"
#include "Level_Manager.h"


_bool g_bLoadingfirst = true;

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	/*For.Prototype_Component_Texture_loadingscene */
	if (g_bLoadingfirst == true)
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_loading"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/scene3/loading_%03d.png"), 303))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_loadingback"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/UI/Loading_Scene/loading6.png"), 1))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loadingscene"),
			CLoadingscene::Create(m_pGraphic_Device))))
			return E_FAIL;
		g_bLoadingfirst = false;
			
	}
	
	
	if (FAILED(Ready_Layer_Loadingscene(TEXT("Layer_Loadingscene"))))
		return E_FAIL;
	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pGraphic_Device, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	if (true == m_pLoader->Get_Finished())
	{
		if (GetKeyState(VK_RETURN) & 0x8000)
		{
			/* 넥스트레벨에 대한 준비가 끝나면 실제 넥스트레벨을 할당한다. */
			CLevel*			pNewLevel = nullptr;

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pGraphic_Device);
				break;
			case LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay::Create(m_pGraphic_Device);
				break;
			case LEVEL_HUNT:
				pNewLevel = CLevel_Hunt::Create(m_pGraphic_Device);
				break;
			case LEVEL_BOSS:
				pNewLevel = CLevel_Boss::Create(m_pGraphic_Device);
				break;
			}

			if (nullptr == pNewLevel)
				return;

 			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			if (nullptr == pGameInstance)
				return;
			Safe_AddRef(pGameInstance);

			if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				return;

			Safe_Release(pGameInstance);
		}		
	}
}

void CLevel_Loading::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
}

CLevel_Loading * CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevel)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CLevel_Loading::Ready_Layer_Loadingscene(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Loadingscene"), LEVEL_LOADING, pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
