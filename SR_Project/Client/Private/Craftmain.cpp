#include "stdafx.h"
#include "..\Public\Craftmain.h"
#include "GameInstance.h"
#include "Inventory.h"


CCraftmain::CCraftmain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCraftmain::CCraftmain(const CCraftmain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCraftmain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCraftmain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 250.f;
	m_fSizeY = 325.f;
    m_fX = 238.f;
	m_fY = 0.f;


	m_firsty = m_fY;

	m_firstx = m_fX;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CCraftmain::Tick(_float fTimeDelta)
{

	if (m_bonof == false)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);

	if (m_makewhat == MAKE_AXE || m_makewhat == MAKE_HAMBAT)
		m_fY = 212.f;
	else if (m_makewhat == MAKE_PICK || m_makewhat == MAKE_SHOTTER)
		m_fY = 262.f;
	else if (m_makewhat == MAKE_STAFF)
		m_fY = 312.f;
	else if (m_makewhat == MAKE_ARMOR)
		m_fY = 362.f;
	else if (m_makewhat == MAKE_HELMET)
		m_fY = 412.f;
	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	/*if (m_fY <= 212.f)
		Open_Craft(fTimeDelta);*/
	//if (m_fX <= 100)
		//Open_Weapontool(fTimeDelta);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	/*RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&rcRect, ptMouse))
	{

	}*/

	return OBJ_NOEVENT;
}

void CCraftmain::Late_Tick(_float fTimeDelta)
{

	if (m_bonof == false)
		return;


	__super::Late_Tick(fTimeDelta);



	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCraftmain::Render()
{
	if (m_bonof == false)
		return OBJ_NOEVENT;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_makewhat)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CCraftmain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Craftmain"), (CComponent**)&m_pTextureCom)))
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

HRESULT CCraftmain::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CCraftmain::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CCraftmain * CCraftmain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCraftmain*	pInstance = new CCraftmain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCraftmain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCraftmain::Clone(void* pArg)
{
	CCraftmain*	pInstance = new CCraftmain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCraftmain"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Craftmain_list()->push_back(pInstance);
	return pInstance;
}


void CCraftmain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
