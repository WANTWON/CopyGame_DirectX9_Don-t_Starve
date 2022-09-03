#include "stdafx.h"
#include "..\Public\CraftPont.h"
#include "GameInstance.h"
#include "Inventory.h"


CCraftPont::CCraftPont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCraftPont::CCraftPont(const CCraftPont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCraftPont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCraftPont::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 10.0f;
	m_fSizeY = 15.0f;
	m_fX = 215.f + (iNum * 10.f);
	m_fY = 185.f;


	

	if (iNum >= 4)
	  m_fX += 25.f;

	if (iNum == 3 || iNum == 7)
		m_fX += 2.5f;

	if (iNum == 2 || iNum == 6)
	{
		texnum = 10;
	}

	
	m_firsty = m_fY;

	m_firstx = m_fX;


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CCraftPont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_bcheck == false)
		return 0;
	RECT		rcRect;
	SetRect(&rcRect, (_int)(m_fX - m_fSizeX * 0.5f), (_int)(m_fY - m_fSizeY * 0.5f), (_int)(m_fX + m_fSizeX * 0.5f), (_int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_makewhat == MAKE_AXE || m_makewhat == MAKE_HAMBAT)
		m_fY = 185.f;
	else if (m_makewhat == MAKE_PICK || m_makewhat == MAKE_SHOTTER)
		m_fY = 235.f;
	else if (m_makewhat == MAKE_STAFF)
		m_fY = 285.f;
	else if (m_makewhat == MAKE_ARMOR)
		m_fY = 335.f;
	else if (m_makewhat == MAKE_HELMET)
		m_fY = 385.f;
	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	
	//if (PtInRect(&rcRect, ptMouse))
	//{

	//	m_fSizeX = 35.f;
	//	m_fSizeY = 35.f;
	//	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	//	//set_check(true);
	//}

	if (iNum == 0 || iNum == 4)
	{
		if (texnum == 0)
			m_bcheck = false;
		
	}


	//if()
	return OBJ_NOEVENT;
}

void CCraftPont::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == false)
		return ;
	__super::Late_Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, (_int)(m_fX - m_fSizeX * 0.5f), (_int)(m_fY - m_fSizeY * 0.5f), (_int)(m_fX + m_fSizeX * 0.5f), (_int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	//if (!PtInRect(&rcRect, ptMouse))
	//{
	//	m_fSizeX = 20;
	//	m_fSizeY = 20;
	//	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);


	//	//ERR_MSG(L"Ãæµ¹");
	//}
	if (nullptr != m_pRendererCom&&m_bcheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	//set_check(false);
}

HRESULT CCraftPont::Render()
{
	if (m_bcheck)
	{
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

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}




	return S_OK;
}

HRESULT CCraftPont::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pont"), (CComponent**)&m_pTextureCom)))
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

HRESULT CCraftPont::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CCraftPont::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CCraftPont * CCraftPont::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCraftPont*	pInstance = new CCraftPont(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : Prototype_Component_Texture_Pont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCraftPont::Clone(void* pArg)
{
	CCraftPont*	pInstance = new CCraftPont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : Prototype_Component_Texture_Pont"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Craftpont_list()->push_back(pInstance);
	return pInstance;
}


void CCraftPont::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}