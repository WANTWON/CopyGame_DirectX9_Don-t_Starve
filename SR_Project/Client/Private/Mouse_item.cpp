#include "stdafx.h"
#include "..\Public\Mouse_item.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Mouse.h"


CMouse_item::CMouse_item(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMouse_item::CMouse_item(const CMouse_item & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMouse_item::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMouse_item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/*iNumber = (int*)pArg;

	iNum = *iNumber;*/

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 40.0f;
	m_fSizeY = 40.0f;
	

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CMouse_item::Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{
		__super::Tick(fTimeDelta);

		RECT		rcRect;
		SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);



		m_fX = (float)ptMouse.x;
		m_fY = (float)ptMouse.y;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	}
	
	
		

	return OBJ_NOEVENT;
}

void CMouse_item::Late_Tick(_float fTimeDelta)
{
	CMouse* pMouse = CMouse::Get_Instance();


	if (texnum == ITEMNAME_ARMOR)
	{
		pMouse->Set_Item_type(ITEM_ARMOR);
	}
	else if (texnum == ITEMNAME_AXE || texnum == ITEMNAME_SHOTTER || texnum == ITEMNAME_TORCH || texnum == ITEMNAME_STAFF || texnum == ITEMNAME_PICK || texnum == ITEMNAME_HAMBAT)
	{
		pMouse->Set_Item_type(ITEM_HAND);
	}

	else if (texnum == ITEMNAME_HELMET)
	{
		pMouse->Set_Item_type(ITEM_HAT);
	}

	else if (texnum == ITEMNAME_BAG)
	{
		pMouse->Set_Item_type(ITEM_BAG);
	}

	if (m_bcheck == true)
	{
		__super::Late_Tick(fTimeDelta);


		

		if (pMouse->Get_picked() == true && pMouse->Get_Item_count() <= 0 )
		{
			if (pMouse->Get_Item_type() != ITEM_HAT && pMouse->Get_Item_type() != ITEM_BAG && pMouse->Get_Item_type() != ITEM_HAND && pMouse->Get_Item_type() != ITEM_ARMOR
				&& pMouse->Get_Item_name() != ITEMNAME_QUEST1)
			{
				m_bcheck = false;
				pMouse->Set_picked(false);
			}
			
			
		}
		else
			m_bcheck = true;

		if (nullptr != m_pRendererCom && m_bcheck == true)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
	
	
	
	/*if (m_bItem)
	{

	}*/

	

}

HRESULT CMouse_item::Render()
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



	return S_OK;
}

HRESULT CMouse_item::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainInventory_front"), (CComponent**)&m_pTextureCom)))
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

HRESULT CMouse_item::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMouse_item::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CMouse_item * CMouse_item::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMouse_item*	pInstance = new CMouse_item(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMouse_item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMouse_item::Clone(void* pArg)
{
	CMouse_item*	pInstance = new CMouse_item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMouse_item"));
		Safe_Release(pInstance);
	} 

	CInventory_Manager::Get_Instance()->Get_Mouse_item_list()->push_back(pInstance);

	return pInstance;
}


void CMouse_item::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

