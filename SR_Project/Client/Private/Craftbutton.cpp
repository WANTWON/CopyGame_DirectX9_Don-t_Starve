#include "stdafx.h"
#include "..\Public\Craftbutton.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Mouse.h"
#include "KeyMgr.h"
#include "Player.h"




CCraftbutton::CCraftbutton(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCraftbutton::CCraftbutton(const CCraftbutton & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCraftbutton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CCraftbutton::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fSizeX = 130.f;
	m_fSizeY = 55.f;
	m_fX = 260;
	m_fY = 300.f;

	m_firstx = m_fX;
	m_firsty = m_fY;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));


	//CInventory_Manager::Get_Instance()->Get_Inven_list()->front.push_back(this);
	//CInventory_Manager::Get_Instance()->Get_Inven_list().push_back(this);
	//INVENLIST
//	m_tooltype = (TOOLTYPE)iNum;

	return S_OK;
}

int CCraftbutton::Tick(_float fTimeDelta)
{

	if (m_bonof == false)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);


	/*if (m_fY <= 300.f)
		Open_Craft(fTimeDelta);*/


	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);


	if (PtInRect(&rcRect, ptMouse))
	{

		m_fSizeY = 75.f;
		m_fSizeX = 155.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

		if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
		{
			craft();
		}

	}




	//if()
	return OBJ_NOEVENT;
}

void CCraftbutton::Late_Tick(_float fTimeDelta)
{

	if (m_bonof == false)
		return;

	__super::Late_Tick(fTimeDelta);

	CMouse*			pMouse = CMouse::Get_Instance();
	Safe_AddRef(pMouse);

	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
	Safe_AddRef(pinv);

	auto mouse = pinv->Get_Mouse_item_list()->begin();


	if (!PtInRect(&rcRect, ptMouse))
	{

		m_fSizeX = 130.f;
		m_fSizeY = 55.f;

		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		
	}






	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	Safe_Release(pMouse);
	Safe_Release(pinv);
	//set_check(false);
}

HRESULT CCraftbutton::Render()
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

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;





	return S_OK;
}

HRESULT CCraftbutton::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Craftbutton"), (CComponent**)&m_pTextureCom)))
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

HRESULT CCraftbutton::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CCraftbutton::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CCraftbutton * CCraftbutton::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCraftbutton*	pInstance = new CCraftbutton(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCraftbutton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCraftbutton::Clone(void* pArg)
{
	CCraftbutton*	pInstance = new CCraftbutton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCraftbutton"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Craftbutton_list()->push_back(pInstance);
	return pInstance;

}
void CCraftbutton::craft()
{

	CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();
	Safe_AddRef(pInventory_Manager);
	_uint checkcount = 0;
	bool stop1 = false;
	bool stop2 = false;

	auto pinven = pInventory_Manager->Get_Inven_list();

	auto pcraftback = pInventory_Manager->Get_Craftmainback_list();

	for (auto iter = pcraftback->begin(); iter != pcraftback->end(); ++iter)
	{
		if ((*iter)->get_texnum() == 0)
			++checkcount;
	}

	if (checkcount == 2)
	{
		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{
			if ((*iter)->get_texnum() == ITEMNAME_WOOD && (*iter)->get_item_number() >= 1 && stop1 == false)  //재료가 있는지 검사 있다면 재료차감..(가방처리는난중에하까)
			{
				(*iter)->minus_material(1);
				stop1 = true;

			}

			if ((*iter)->get_texnum() == ITEMNAME_ROCK2 && (*iter)->get_item_number() >= 1 && stop2 == false)  //재료가 있는지 검사 있다면 재료차감..(가방처리는난중에하까)
			{
				(*iter)->minus_material(1);
				stop2 = true;
				
			}

			if (stop1 == true && stop2 == true)
			{
				break;
			}
		}
	}
	else
		return;



	for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
	{
		if ((*iter)->get_check() == false)
		{
			(*iter)->set_texnum(ITEMNAME_AXE);

			(*iter)->set_check(true);

			break;
		}

	}

	Safe_Release(pInventory_Manager);
}




void CCraftbutton::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

