#include "stdafx.h"
#include "..\Public\MainInventory_front.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Mouse.h"
#include "KeyMgr.h"
#include "Player.h"
#include "PickingMgr.h"


CMainInventory_front::CMainInventory_front(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMainInventory_front::CMainInventory_front(const CMainInventory_front & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMainInventory_front::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CMainInventory_front::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 40.0f;
	m_fSizeY = 40.0f;
	m_fX = 300.f + (iNum * 50.f);
	m_fY = 690.f;

	if (iNum == 10)
	{
		m_fX = 1185.f;
		m_fY = 435.f;

	}

	if (iNum == 11)
	{
		m_fX = 1235.f;
		m_fY = 435.f;
	}

	if (iNum == 12)
	{
		m_fX = 1185.f;
		m_fY = 485.f;
	}

	if (iNum == 13)
	{
		m_fX = 1235.f;
		m_fY = 485.f;
	}

	if (iNum == 14)
	{
		m_fX = 1185.f;
		m_fY = 535.f;
	}

	if (iNum == 15)
	{
		m_fX = 1235.f;
		m_fY = 535.f;
	}

	if (iNum == 16)
	{
		m_fX = 1185.f;
		m_fY = 585.f;
	}

	if (iNum == 17)
	{
		m_fX = 1235.f;
		m_fY = 585.f;
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	if (iNum == 0)
	{
		texnum = ITEMNAME_WOOD;
		item_number = 9;
	}

	if (iNum == 1)
	{
		texnum = ITEMNAME_ROCK2;
		item_number = 9;
	}

	if (iNum == 2)
	{
		texnum = ITEMNAME_GOLD;
		item_number = 9;
	}

	if (iNum == 3)
	{
		texnum = ITEMNAME_BAG;
	}

	if (iNum == 4)
	{
		texnum = ITEMNAME_BERRY;
		item_number = 1;
	}
	

	if (iNum == 5)
	{
		texnum = ITEMNAME_MEAT;
		item_number = 6;
	}


	if (iNum == 7)
	{
		texnum = ITEMNAME_ROPE;
		item_number = 6;
	}

	//CInventory_Manager::Get_Instance()->Get_Inven_list()->front.push_back(this);
	//CInventory_Manager::Get_Instance()->Get_Inven_list().push_back(this);
	//INVENLIST

	return S_OK;
}

int CMainInventory_front::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
	Safe_AddRef(pinv);

	auto mouse = pinv->Get_Mouse_item_list()->begin();
	auto iteminfo = pinv->Get_Mouse_iteminfo_list()->begin();
	Safe_Release(pinv);

	if (PtInRect(&rcRect, ptMouse)) // for iteminfo
	{

		m_fSizeX = 55.f;
		m_fSizeY = 55.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		if (m_itemtype == ITEM_FOOD)
		{
			(*iteminfo)->set_check(true);
			(*iteminfo)->set_test(true);
			(*iteminfo)->set_itemname(texnum);
			whatnum = iNum;
		}
	}
	else if (iNum == whatnum && !PtInRect(&rcRect, ptMouse))
	{
		m_fSizeX = 40;
		m_fSizeY = 40;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		(*iteminfo)->set_check(false);
		whatnum = 30;
	}
	/*else if (whatnum >= 18)
		(*iteminfo)->set_check(false);*/


	if (texnum == ITEMNAME_ARMOR)
	{
		m_itemtype = ITEM_ARMOR;
	}
	else if (texnum == ITEMNAME_AXE || texnum == ITEMNAME_SHOTTER || texnum == ITEMNAME_TORCH || texnum == ITEMNAME_STAFF || texnum == ITEMNAME_PICK || texnum == ITEMNAME_HAMBAT)
	{
		m_itemtype = ITEM_HAND;
	}

	else if (texnum == ITEMNAME_HELMET)
	{
		m_itemtype = ITEM_HAT;
	}

	else if (texnum == ITEMNAME_BAG)
	{
		m_itemtype = ITEM_BAG;
	}


	else if (texnum == ITEMNAME_BERRY || texnum == ITEMNAME_CARROT || texnum == ITEMNAME_MEAT || texnum == ITEMNAME_SPIDERMEAT
		|| texnum == ITEMNAME_MEATBALL || texnum == ITEMNAME_NUGGET || texnum ==ITEMNAME_LASAGNA || texnum == ITEMNAME_BERRYPIE
		|| texnum == ITEMNAME_TRASH1   || texnum == ITEMNAME_TRASH2)
	{
		m_itemtype = ITEM_FOOD;
	}

	else if (texnum == ITEMNAME_COAL || texnum == ITEMNAME_WOOD || texnum == ITEMNAME_ROCK || texnum == ITEMNAME_GOLD || texnum == ITEMNAME_WOOD2
		|| texnum == ITEMNAME_PIGTAIL || texnum == ITEMNAME_ROPE || texnum == ITEMNAME_WEB || texnum == ITEMNAME_GRASS || texnum == ITEMNAME_ROCK2)
	{
		m_itemtype = ITEM_MATERIAL;
	}
	
	else if (texnum == ITEMNAME_FENCE || texnum == ITEMNAME_POT || texnum == ITEMNAME_TENT)
	{
		m_itemtype = ITEM_STRUCT;
	}

	
	if (m_itemtype == ITEM_BAG || m_itemtype == ITEM_HAT || m_itemtype == ITEM_HAND || m_itemtype == ITEM_ARMOR || texnum == ITEMNAME_END)
	{


		m_bpontcheck = false;

	}
	else if (m_itemtype == ITEM_FOOD || m_itemtype == ITEM_MATERIAL || m_itemtype == ITEM_STRUCT)
	{
		m_bpontcheck = true;
	}



	if (texnum == ITEMNAME_END)
	{
		m_itemtype = ITEM_END;
	}

	/*if (texnum == ITEMNAME_BAG || texnum == ITEMNAME_HELMET || texnum == ITEMNAME_AXE || texnum == ITEMNAME_SHOTTER || texnum == ITEMNAME_TORCH
		|| texnum == ITEMNAME_STAFF || texnum == ITEMNAME_PICK || texnum == ITEMNAME_HAMBAT || texnum == ITEMNAME_ARMOR || texnum == ITEMNAME_END)
	{
		m_bpontcheck = false;
	}
	else if(texnum == carrot)
		m_bpontcheck = true;*/




	/*if (PtInRect(&rcRect, ptMouse))
	{

		m_fSizeX = 55.f;
		m_fSizeY = 55.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		CInventory_Manager
	}*/




	//if()
	return OBJ_NOEVENT;
}

void CMainInventory_front::Late_Tick(_float fTimeDelta)
{
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
	


	



	if (PtInRect(&rcRect, ptMouse) && CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
	{
		if (m_itemtype == ITEM_STRUCT)
		{
			if (false == pMouse->Get_picked())
			{
				pMouse->Set_Item_type(ITEM_STRUCT);
				pMouse->Set_Item_name(texnum);   //첫피킹
				pMouse->Set_index(iNum);
				pMouse->Set_picked(true);
				pMouse->Set_Item_count(item_number);
				(*mouse)->set_check(true);
				//(*mouse)->set_texnum(texnum); //마우스이미지

				CPickingMgr* pPickingMgr =  CPickingMgr::Get_Instance();
				pPickingMgr->Set_PickingItemType(ITEM_STRUCT);
				set_texnum(ITEMNAME_END);
			}
			else if (true == pMouse->Get_picked())
			{

				pMouse->Set_Prev_Item_name(texnum);
				pMouse->Set_Item_prev_count(item_number);
				texnum = (pMouse->Get_Item_name());
				set_itemcount(pMouse->Get_Item_count());
				pMouse->Set_picked(false);
				m_bcheck = true;

				(*mouse)->set_texnum(ITEMNAME_END);
				(*mouse)->set_check(false);//마우스이미지
				CPickingMgr::Get_Instance()->Release_PickingObject();
				CPickingMgr::Get_Instance()->Set_Mouse_Has_Construct(false);

			}

		}
		else
		{
			if (false == pMouse->Get_picked())
			{
				pMouse->Set_Item_type(ITEM_END);
				pMouse->Set_Item_name(texnum);   //첫피킹
				pMouse->Set_index(iNum);
				pMouse->Set_picked(true);
				pMouse->Set_Item_count(item_number);

				(*mouse)->set_check(true);
				(*mouse)->set_texnum(texnum);//마우스이미지

				set_texnum(ITEMNAME_END);
			}

			else if (true == pMouse->Get_picked())
			{
				if (pMouse->Get_index() == iNum)
				{
					pMouse->Set_Same(true);
				}
				else
					pMouse->Set_Same(false);


				pMouse->Set_Prev_Item_name(texnum);

				if(pMouse->Get_Item_type() == ITEM_STRUCT && texnum == ITEMNAME_END)
					pMouse->Set_Item_prev_count(pMouse->Get_Item_prev_count());
				else
					pMouse->Set_Item_prev_count(item_number);
				texnum = (pMouse->Get_Item_name());
				set_itemcount(pMouse->Get_Item_count());
				pMouse->Set_picked(false);
				m_bcheck = true;
				pMouse->Set_Item_type(ITEM_END);
				(*mouse)->set_texnum(ITEMNAME_END);
				(*mouse)->set_check(false);//마우스이미지
				CPickingMgr::Get_Instance()->Release_PickingObject();
				CPickingMgr::Get_Instance()->Set_Mouse_Has_Construct(false);

			}

		}


	

	}


	/*if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON)&& !PtInRect(&rcRect, ptMouse))
	{
		(*mouse)->set_texnum(ITEMNAME_END);
		(*mouse)->set_check(false);
	}*/








	if (PtInRect(&rcRect, ptMouse) && CKeyMgr::Get_Instance()->Key_Up(VK_RBUTTON)) //마우스 우클릭처리
	{
		if (m_itemtype == ITEM_ARMOR || m_itemtype == ITEM_HAND || m_itemtype == ITEM_BAG || m_itemtype == ITEM_HAT)
		{
			pMouse->Set_Item_type(m_itemtype);
			pMouse->Set_Equipment_name(texnum);
			set_texnum(ITEMNAME_END);


			if (m_itemtype == ITEM_BAG)
			{
				pinv->Late_Tick(fTimeDelta);
				pinv->Use_bag();
				Safe_Release(pinv);
			}

			m_itemtype = ITEM_END;
		}

		//minus_itemcount();

		else if (m_itemtype == ITEM_FOOD)
		{
			Use_item(texnum);
		}

	}



	Safe_Release(pMouse);
	Safe_Release(pinv);


	if (texnum == ITEMNAME_END)
	{
		//set_check(false);
	}
	if (m_itemtype == ITEM_HAT && m_itemtype == ITEM_BAG && m_itemtype == ITEM_HAND && m_itemtype == ITEM_ARMOR) // 
	{
		item_number = 0;
	}

	if (item_number <= 0 && m_itemtype != ITEM_HAT && m_itemtype !=ITEM_BAG && m_itemtype != ITEM_HAND && m_itemtype != ITEM_ARMOR ) // 카운트가 필요한 타입들이면서 
	{
		m_bpontcheck = false;
		m_bcheck = false;
		item_number = 0;
		set_texnum(ITEMNAME_END);

	}


	if (texnum == ITEMNAME_END)
	{
		m_bcheck = false;
	}
	else
		m_bcheck = true;

	if (nullptr != m_pRendererCom&&m_bcheck == true && iNum < 10)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	else if (nullptr != m_pRendererCom&&m_bcheck == true && iNum >= 10 && m_bcheck_bag)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	//set_check(false);
}

HRESULT CMainInventory_front::Render()
{
	if (m_bcheck == true)
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

HRESULT CMainInventory_front::SetUp_Components()
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

HRESULT CMainInventory_front::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMainInventory_front::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CMainInventory_front * CMainInventory_front::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMainInventory_front*	pInstance = new CMainInventory_front(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : MainInventory_front"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainInventory_front::Clone(void* pArg)
{
	CMainInventory_front*	pInstance = new CMainInventory_front(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : MainInventory_front"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Inven_list()->push_back(pInstance);//Get_Inven_list().push_back(pInstance);
	return pInstance;

}


void CMainInventory_front::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

void CMainInventory_front::Use_item(ITEMNAME item)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	switch (item)
	{
	case ITEMNAME_CARROT:



		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_HP(10));
		minus_itemcount();
		//minus_itemcount();
		break;


	case ITEMNAME_BERRY:



		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Hungry(10));
		minus_itemcount();
		break;

	case ITEMNAME_BERRYPIE:



		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Hungry(30));
		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_HP(10));
		minus_itemcount();
		//minus_itemcount();
		break;


	case ITEMNAME_LASAGNA:



		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Hungry(50));
		//(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Take_Damage(20));
		minus_itemcount();
		break;

	case ITEMNAME_NUGGET:


		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Hungry(40));
		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_HP(20));
		minus_itemcount();
		//minus_itemcount();
		break;


	case ITEMNAME_MEATBALL:



		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Hungry(30));
		minus_itemcount();
		break;

	case ITEMNAME_TRASH1:



		//(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Take_Damage());
		minus_itemcount();
		break;

	case ITEMNAME_TRASH2:



//		(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Hungry(30));
		minus_itemcount();
		break;

	



	}
	dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Add_ActStack(CPlayer::ACTION_STATE::EAT);
	dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_bMove(false);
	Safe_Release(pGameInstance);
}
