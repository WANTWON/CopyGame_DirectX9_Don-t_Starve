#include "stdafx.h"
#include "..\Public\Pot_front.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Mouse.h"
#include "KeyMgr.h"
#include "Player.h"
#include "PickingMgr.h"


CPot_front::CPot_front(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPot_front::CPot_front(const CPot_front & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPot_front::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CPot_front::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 40.0f;
	m_fSizeY = 40.0f;
	m_fX = 740.0f;
	m_fY = 320.0f + (iNum * 43.f);

	

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	/*if (iNum == 0)
	{
		texnum = ITEMNAME_BERRY;
		
	}

	if (iNum == 1)
	{
		texnum = ITEMNAME_BERRY;
		
	}
   if (iNum == 2)
	{
		texnum == ITEMNAME_BERRY;
	}
	if (iNum == 3)
	{
		texnum == ITEMNAME_BERRY;
	}*/
	//texnum = ITEMNAME_BERRY;
	

	

	return S_OK;
}

int CPot_front::Tick(_float fTimeDelta)
{
	if (m_bcheck_pot == true)
	{
		__super::Tick(fTimeDelta);

		RECT		rcRect;
		SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);


	}
	

	

	




	//if (PtInRect(&rcRect, ptMouse))
	//{

	//	m_fSizeX = 55.f;
	//	m_fSizeY = 55.f;
	//	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	//	//set_check(true);
	//	//plus_itemcount();
	//}




	//if()
	return OBJ_NOEVENT;
}

void CPot_front::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck_pot == true)
	{
		__super::Late_Tick(fTimeDelta);

		
		CMouse*			pMouse = CMouse::Get_Instance();
		//Safe_AddRef(pMouse);

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

			collided = false;
		}
		else
			collided = true;


		if (PtInRect(&rcRect, ptMouse) && CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
		{
			if (true == pMouse->Get_picked())
			{
				ITEMNAME iname;
				iname = texnum;
				
				(pMouse->minus_Item_count());
				texnum = (pMouse->Get_Item_name());
				if (texnum != ITEMNAME_END && iname != pMouse->Get_Item_name())  // 마우스에들고있는 아이템이 팟창의 아이템과 다를떄 + 팟창의 아이템이 빈칸이 아닐때 ( 플러스해주면 안될때!)
				{
					
					CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();
				//	Safe_AddRef(pInventory_Manager);

					auto pinven = pInventory_Manager->Get_Inven_list();

//Safe_Release(pInventory_Manager);


					
					

					for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
					{
						if ((*iter)->get_texnum() == iname && (*iter)->get_check() == true)
						{
							(*iter)->plus_itemcount();   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가 증가시켰다면 리턴으로 탈출! 못찾았으면? 탐색해야지 새로
							texnum = (pMouse->Get_Item_name());							 //check = true;
							return;
						}
					}

					for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
					{
						if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END ||(*iter)->get_item_number() == 0)
						{
							(*iter)->set_texnum(iname);
							(*iter)->plus_itemcount();

							(*iter)->set_check(true);
							texnum = (pMouse->Get_Item_name());
							return;
						}

					}

				}
			}

			
			//pMouse->Set_Prev_Item_name(texnum); //

			//if (pMouse->Get_Item_type() == ITEM_STRUCT && m_itemtype == ITEMNAME_END)
			//	pMouse->Set_Item_prev_count(pMouse->Get_Item_prev_count());
			//else
			//pMouse->Set_Item_prev_count(item_number);
			//
			//set_itemcount(pMouse->Get_Item_count());
			//pMouse->Set_picked(false);
			//m_bcheck = true;
			//pMouse->Set_Item_type(ITEM_END);
			//(*mouse)->set_texnum(ITEMNAME_END);
			//(*mouse)->set_check(false);//마우스이미지


		}

		//else if (false == pMouse->Get_picked())
		//{
		//	pMouse->Set_Item_type(ITEM_END);
		//	pMouse->Set_Item_name(texnum);   //첫피킹
		//	pMouse->Set_index(iNum);
		//	pMouse->Set_picked(true);
		//	pMouse->Set_Item_count(item_number);

		//	(*mouse)->set_check(true);
		//	(*mouse)->set_texnum(texnum);//마우스이미지

		//	set_texnum(ITEMNAME_END);
		//}



	}


	//	Safe_Release(pMouse);
	//	Safe_Release(pinv);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}




HRESULT CPot_front::Render()
{
	if (m_bcheck_pot == true)
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

HRESULT CPot_front::SetUp_Components()
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

HRESULT CPot_front::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPot_front::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPot_front * CPot_front::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPot_front*	pInstance = new CPot_front(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPot_front"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPot_front::Clone(void* pArg)
{
	CPot_front*	pInstance = new CPot_front(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPot_front"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Potfront_list()->push_back(pInstance);//Get_Inven_list().push_back(pInstance);
	return pInstance;

}


void CPot_front::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}


