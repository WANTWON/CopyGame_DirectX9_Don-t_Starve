#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC		Graphic_Desc;
	ZeroMemory(&Graphic_Desc, sizeof(GRAPHIC_DESC));

	Graphic_Desc.hWnd = g_hWnd;
	Graphic_Desc.iWinSizeX = g_iWinSizeX;
	Graphic_Desc.iWinSizeY = g_iWinSizeY;
	Graphic_Desc.eWinMode = GRAPHIC_DESC::MODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, Graphic_Desc, &m_pGraphic_Device)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(fTimeDelta);
	m_pCollider->Update_ColliderGroup();

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif // _DEBUG

}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Render_Begin();

	m_pRenderer->Render_GameObjects();

	m_pGameInstance->Render_End();

	++m_iNumRender;

	if (m_fTimeAcc > 1.0f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);		

		OutputDebugString(m_szFPS);

		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CLevel_Loading*			pLevel_Loading = CLevel_Loading::Create(m_pGraphic_Device, eLevel);
	if (nullptr == pLevel_Loading)
		return E_FAIL;

	m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel_Loading);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), m_pCollider= CCollider::Create(m_pGraphic_Device))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	 //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}


CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pGameInstance);
	
	CGameInstance::Release_Engine();
}

