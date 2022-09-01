#include "stdafx.h"
#include "..\Public\ToolboxGather_back.h"
#include "GameInstance.h"
#include "Inventory.h"

CToolboxGather_back::CToolboxGather_back(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CToolboxGather_back::CToolboxGather_back(const CToolboxGather_back & rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolboxGather_back::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolboxGather_back::Initialize(void* pArg)
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

	m_firstx = m_fX;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));



	return S_OK;
}

int CToolboxGather_back::Tick(_float fTimeDelta)
{
	if (m_bonof == false)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);



	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_fX <= 100)
		Open_Weapontool(fTimeDelta);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));


	if (PtInRect(&rcRect, ptMouse))
	{
		m_fSizeX = 55.f;
		m_fSizeY = 55.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		backtexnum = 0;


	}

	if (PtInRect(&rcRect, ptMouse) && CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON) && iNum == 0)
	{
		bool bok = false;
		CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();
		Safe_AddRef(pInventory_Manager);

		pInventory_Manager->craft_on(MAKE_AXE);
		auto pinven = pInventory_Manager->Get_Inven_list();

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_texnum() == ITEMNAME_WOOD && (*iter)->get_item_number() >= 3)  //재료가 있는지 검사 있다면 재료차감..(가방처리는난중에하까)
			{
				(*iter)->minus_material(3);
				bok = true;
				break;
			}


		}
		if (bok == false)
			return 0;

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_check() == false)
			{
				(*iter)->set_texnum(ITEMNAME_AXE);

				(*iter)->set_check(true);

				break;
			}

		}









	}

	/*if (m_bItem)
	{
	m_fX = ptMouse.x;
	m_fY = ptMouse.y;
	}*/

	return OBJ_NOEVENT;
}

void CToolboxGather_back::Late_Tick(_float fTimeDelta)
{

	if (m_bonof == false)
		return;

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
		backtexnum = 2;


		//ERR_MSG(L"충돌");
	}

	/*if (m_bItem)
	{

	}*/

	if (nullptr != m_pRendererCom && iNum < 10)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CToolboxGather_back::Render()
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

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(backtexnum)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CToolboxGather_back::SetUp_Components()
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

HRESULT CToolboxGather_back::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CToolboxGather_back::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}

CToolboxGather_back * CToolboxGather_back::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CToolboxGather_back*	pInstance = new CToolboxGather_back(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CToolboxGather_back"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CToolboxGather_back::Clone(void* pArg)
{
	CToolboxGather_back*	pInstance = new CToolboxGather_back(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CToolboxGather_back"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Toolboxgather_back_list()->push_back(pInstance);

	return pInstance;
}


void CToolboxGather_back::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

