#include "stdafx.h"
#include "..\Public\ToolboxGather_front.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Mouse.h"
#include "KeyMgr.h"
#include "Player.h"




CToolboxGather_front::CToolboxGather_front(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CToolboxGather_front::CToolboxGather_front(const CToolboxGather_front & rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolboxGather_front::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CToolboxGather_front::Initialize(void* pArg)
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


	//CInventory_Manager::Get_Instance()->Get_Inven_list()->front.push_back(this);
	//CInventory_Manager::Get_Instance()->Get_Inven_list().push_back(this);
	//INVENLIST
	m_tooltype = (TOOLTYPE)iNum;

	return S_OK;
}

int CToolboxGather_front::Tick(_float fTimeDelta)
{

	if (m_bonof == false)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

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
		//set_check(true);
		//plus_itemcount();
	}




	//if()
	return OBJ_NOEVENT;
}

void CToolboxGather_front::Late_Tick(_float fTimeDelta)
{

	if (m_bonof == false)
		return;

	__super::Late_Tick(fTimeDelta);

	

	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
	//Safe_AddRef(pinv);

	auto mouse = pinv->Get_Mouse_item_list()->begin();


	if (!PtInRect(&rcRect, ptMouse))
	{

		m_fSizeX = 40;
		m_fSizeY = 40;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	}



	//	if (PtInRect(&rcRect, ptMouse) && CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
	//
	//	{
	//
	//
	//	}
	//
	//if (PtInRect(&rcRect, ptMouse) && CKeyMgr::Get_Instance()->Key_Up(VK_RBUTTON)) //付快胶 快努腐贸府
	//	{
	//		
	//	}




	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);


//	Safe_Release(pinv);
	//set_check(false);
}

HRESULT CToolboxGather_front::Render()
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

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(iNum)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;





	return S_OK;
}

HRESULT CToolboxGather_front::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_GatherToolbox_front"), (CComponent**)&m_pTextureCom)))
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

HRESULT CToolboxGather_front::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CToolboxGather_front::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CToolboxGather_front * CToolboxGather_front::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CToolboxGather_front*	pInstance = new CToolboxGather_front(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CToolboxGather_front"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CToolboxGather_front::Clone(void* pArg)
{
	CToolboxGather_front*	pInstance = new CToolboxGather_front(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CToolboxGather_front"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Toolboxgather_front_list()->push_back(pInstance);
	return pInstance;

}


void CToolboxGather_front::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

//void CMainInventory_front::Use_item(ITEMNAME item)
//{
//	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
//	Safe_AddRef(pGameInstance);
//	switch (item)
//	{
//	case ITEMNAME_CARROT:
//
//
//
//		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Set_HP(10));
//		item_number -= 1;
//		//minus_itemcount();
//		break;
//
//
//	case ITEMNAME_BERRY:
//
//
//
//		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Set_Hungry(10));
//		minus_itemcount();
//		break;
//
//
//
//	}
//
//
//	Safe_Release(pGameInstance);
//}
