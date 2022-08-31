#include "stdafx.h"
#include "..\Public\ToolboxMain_back.h"
#include "GameInstance.h"
#include "Inventory.h"


CToolboxMain_back::CToolboxMain_back(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CToolboxMain_back::CToolboxMain_back(const CToolboxMain_back & rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolboxMain_back::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolboxMain_back::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 40.0f;
	m_fSizeY = 40.0f;
	m_fX = 30.f;
	m_fY = 210.f + (iNum * 50.f);


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	

	return S_OK;
}

int CToolboxMain_back::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&rcRect, ptMouse))
	{
		m_fSizeX = 55.f;
		m_fSizeY = 55.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		backtexnum = 1;

		/*if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
		m_bItem = true;
		}*/
	}

	/*if (m_bItem)
	{
	m_fX = ptMouse.x;
	m_fY = ptMouse.y;
	}*/

	return OBJ_NOEVENT;
}

void CToolboxMain_back::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (!PtInRect(&rcRect, ptMouse))
	{
		m_fSizeX = 40;
		m_fSizeY = 40;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		backtexnum = 0;


		//ERR_MSG(L"Ãæµ¹");
	}

	/*if (m_bItem)
	{

	}*/

	if (nullptr != m_pRendererCom && iNum < 10)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	else if (nullptr != m_pRendererCom&& iNum >= 10 && m_bcheck_bag)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CToolboxMain_back::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(backtexnum)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CToolboxMain_back::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MainToolbox_back"), (CComponent**)&m_pTextureCom)))
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

HRESULT CToolboxMain_back::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CToolboxMain_back::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}

CToolboxMain_back * CToolboxMain_back::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CToolboxMain_back*	pInstance = new CToolboxMain_back(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CToolboxMain_back"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CToolboxMain_back::Clone(void* pArg)
{
	CToolboxMain_back*	pInstance = new CToolboxMain_back(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CToolboxMain_back"));
		Safe_Release(pInstance);
	}

	//CInventory_Manager::Get_Instance()->Get_back_Inven_list()->push_back(pInstance);

	return pInstance;
}


void CToolboxMain_back::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

