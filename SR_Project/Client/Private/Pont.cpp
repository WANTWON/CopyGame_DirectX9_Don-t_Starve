#include "stdafx.h"
#include "..\Public\Pont.h"
#include "GameInstance.h"
#include "Inventory.h"


CPont::CPont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPont::CPont(const CPont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPont::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 13.0f;
	m_fSizeY = 15.0f;
	m_fX = 300.f + (iNum * 12.f);
	m_fY = 680.f;


   if (iNum == 0)
	   m_fX = 295.f;
   else if (iNum == 1)
	   m_fX = 308.f;

   else if (iNum == 2)
	   m_fX = 345.f;
   else if (iNum == 3)
	   m_fX = 358.f;

   else if (iNum == 4)
	   m_fX = 395.f;
   else if (iNum == 5)
	   m_fX = 408.f;

   else if (iNum == 6)
	   m_fX = 445.f;
   else if (iNum == 7)
	   m_fX = 458.f;

   else if (iNum == 8)
	   m_fX = 495.f;
   else if (iNum == 9)
	   m_fX = 508.f;

   else if (iNum == 10)
	   m_fX = 545.f;
   else if (iNum == 11)
	   m_fX = 558.f;

   else if (iNum == 12)
	   m_fX = 595.f;
   else if (iNum == 13)
	   m_fX = 608.f;

   else if (iNum == 14)
	   m_fX = 645.f;
   else if (iNum == 15)
	   m_fX = 658.f;

   else if (iNum == 16)
	   m_fX = 695.f;
   else if (iNum == 17)
	   m_fX = 708.f;

   else if (iNum == 18)
	   m_fX = 745.f;
   else if (iNum == 19)
	   m_fX = 758.f;
   
			
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CPont::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if(big && !smaller)// (PtInRect(&rcRect, ptMouse))
	{

		m_fSizeX = 16.f;
		m_fSizeY = 18.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		//set_check(true);
	}
	else if(!big && !smaller)
	{
		m_fSizeX = 13.f;
		m_fSizeY = 15.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	}


	if (bbig == true)
	{
		m_fSizeX = 20.f;
		m_fSizeY = 22.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		bbig = false;
	}


	if (smaller == true)
	{
		m_fSizeX -= 0.35f;
		m_fSizeY -= 0.35f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);

		if (m_fSizeX <= 13)
		{
			smaller = false;
		}
	}




	//if()
	return OBJ_NOEVENT;
}

void CPont::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);


	if (nullptr != m_pRendererCom&&m_bcheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	//set_check(false);
}

HRESULT CPont::Render()
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

HRESULT CPont::SetUp_Components()
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

HRESULT CPont::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPont::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPont * CPont::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPont*	pInstance = new CPont(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : Prototype_Component_Texture_Pont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPont::Clone(void* pArg)
{
	CPont*	pInstance = new CPont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : Prototype_Component_Texture_Pont"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Pont_list()->push_back(pInstance);
	return pInstance;
}


void CPont::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}