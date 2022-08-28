#include "stdafx.h"
#include "..\Public\Playerhp_pont.h"
#include "GameInstance.h"
#include "Inventory.h"


CPlayerhp_pont::CPlayerhp_pont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayerhp_pont::CPlayerhp_pont(const CPlayerhp_pont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerhp_pont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerhp_pont::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 20.0f;
	m_fSizeY = 20.0f;
	m_fX = 1140.f + (iNum * 20.f);
	m_fY = 200.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CPlayerhp_pont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&rcRect, ptMouse))
	{

		m_fSizeX = 35.f;
		m_fSizeY = 35.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		//set_check(true);
	}




	//if()
	return OBJ_NOEVENT;
}

void CPlayerhp_pont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (!PtInRect(&rcRect, ptMouse))
	{
		m_fSizeX = 20;
		m_fSizeY = 20;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);


		//ERR_MSG(L"Ãæµ¹");
	}
	if (nullptr != m_pRendererCom&&m_bcheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	//set_check(false);
}

HRESULT CPlayerhp_pont::Render()
{
	//if (m_bcheck)
	//{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4		ViewMatrix;
		D3DXMatrixIdentity(&ViewMatrix);

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if(FAILED(Release_RenderState()))
			return E_FAIL;
//	}




	return S_OK;
}

HRESULT CPlayerhp_pont::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pont"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayerhp_pont::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPlayerhp_pont::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPlayerhp_pont * CPlayerhp_pont::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayerhp_pont*	pInstance = new CPlayerhp_pont(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : Prototype_Component_Texture_Layer_HpPont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerhp_pont::Clone(void* pArg)
{
	CPlayerhp_pont*	pInstance = new CPlayerhp_pont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : Prototype_Component_Texture_Layer_HpPont"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_playerhp_Pont_list()->push_back(pInstance);
	return pInstance;
}


CGameObject * CPlayerhp_pont::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CPlayerhp_pont::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}