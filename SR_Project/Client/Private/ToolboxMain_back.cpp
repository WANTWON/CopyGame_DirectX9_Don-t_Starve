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

	/*if (iNum == 0)
	{
		m_bfirstclick_W == false;
	}
	else if (iNum == 1)
	{
		m_bfirstclick_G == false;
	}*/

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
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&rcRect, ptMouse))
	{
		m_fSizeX = 55.f;
		m_fSizeY = 55.f;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
		

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
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (!PtInRect(&rcRect, ptMouse))
	{
		m_fSizeX = 40;
		m_fSizeY = 40;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);

	}


	if (PtInRect(&rcRect, ptMouse)&& CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
	{
		CGameInstance* pInstance = CGameInstance::Get_Instance();
		pInstance->PlaySounds(TEXT("clickitem.wav"), SOUND_UIOPEN, 0.9f);
		
				

		if (iNum == 0)
		{
			if (m_bfirstclick_G == true)
			{
				CInventory_Manager::Get_Instance()->gathertool_on();
				m_bfirstclick_G = false;

				CInventory_Manager::Get_Instance()->weapontool_off();
				
				CInventory_Manager::Get_Instance()->constructtool_off();
				CInventory_Manager::Get_Instance()->materialtool_off();
				CInventory_Manager::Get_Instance()->craft_off();

				/*m_bfirstclick_W = true;
				m_bfirstclick_C = true;
				m_bfirstclick_M = true;*/

				CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
				auto k = pinv->CInventory_Manager::Get_Instance()->Get_toolboxmainback_list();
				for (auto i : *k)
				{
					i->set_c(true);
					i->set_w(true);
					i->set_m(true);
				}
					
					
					
			}
			else
			{
				CInventory_Manager::Get_Instance()->gathertool_off();

				CInventory_Manager::Get_Instance()->craft_off();
				m_bfirstclick_G = true;
			}
	    }
		else if (iNum == 1)
		{
			if (m_bfirstclick_W == true)
			{
				CInventory_Manager::Get_Instance()->weapontool_on();
				m_bfirstclick_W = false;

				CInventory_Manager::Get_Instance()->gathertool_off();

				CInventory_Manager::Get_Instance()->constructtool_off();
				CInventory_Manager::Get_Instance()->materialtool_off();
				CInventory_Manager::Get_Instance()->craft_off();
				/*m_bfirstclick_G = true;
				m_bfirstclick_C = true;
				m_bfirstclick_M = true;*/
				CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
				auto k = pinv->CInventory_Manager::Get_Instance()->Get_toolboxmainback_list();
				for (auto i : *k)
				{
					i->set_c(true);
					i->set_g(true);
					i->set_m(true);
				}
			}
			else
			{
				CInventory_Manager::Get_Instance()->weapontool_off();
				CInventory_Manager::Get_Instance()->craft_off();
				m_bfirstclick_W = true;
			}
		}
		else if (iNum == 2)
		{
			if (m_bfirstclick_C == true)
			{
				CInventory_Manager::Get_Instance()->constructtool_on();
				m_bfirstclick_C = false;

				CInventory_Manager::Get_Instance()->gathertool_off();

				CInventory_Manager::Get_Instance()->weapontool_off();
				CInventory_Manager::Get_Instance()->materialtool_off();
				CInventory_Manager::Get_Instance()->craft_off();
				/*m_bfirstclick_G = true;
				m_bfirstclick_W = true;
				m_bfirstclick_M = true;*/
				CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
				auto k = pinv->CInventory_Manager::Get_Instance()->Get_toolboxmainback_list();
				for (auto i : *k)
				{
					i->set_g(true);
					i->set_w(true);
					i->set_m(true);
				}
			}
			else
			{
				CInventory_Manager::Get_Instance()->constructtool_off();
				CInventory_Manager::Get_Instance()->craft_off();
				m_bfirstclick_C = true;
			}
		}
		else if (iNum == 3)
		{
			if (m_bfirstclick_M == true)
			{
				CInventory_Manager::Get_Instance()->materialtool_on();
				m_bfirstclick_M = false;

				CInventory_Manager::Get_Instance()->gathertool_off();

				CInventory_Manager::Get_Instance()->weapontool_off();
				CInventory_Manager::Get_Instance()->constructtool_off();
				CInventory_Manager::Get_Instance()->craft_off();
				/*m_bfirstclick_G = true;
				m_bfirstclick_C = true;
				m_bfirstclick_W = true;*/
				CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
				auto k = pinv->CInventory_Manager::Get_Instance()->Get_toolboxmainback_list();
				for (auto i : *k)
				{
					i->set_c(true);
					i->set_w(true);
					i->set_g(true);
				}
			}
			else
			{
				CInventory_Manager::Get_Instance()->materialtool_off();
				CInventory_Manager::Get_Instance()->craft_off();
				m_bfirstclick_M = true;
			}
		}
		
			
	}

	/*if (iNum == 2 && PtInRect(&rcRect, ptMouse) && CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
	{
		

	}*/

	
	if (iNum == 0 && CInventory_Manager::Get_Instance()->Get_Toolboxgather_list()->front()->get_onof() == false)
	{
		backtexnum = 0;
	}
	else if (iNum == 0 && CInventory_Manager::Get_Instance()->Get_Toolboxgather_list()->front()->get_onof() == true)
	{
		backtexnum = 1;
	}


	if (iNum == 1 && CInventory_Manager::Get_Instance()->Get_Toolboxweapon_list()->front()->get_onof() == false)
	{
		backtexnum = 0;
	}
	else if (iNum == 1 && CInventory_Manager::Get_Instance()->Get_Toolboxweapon_list()->front()->get_onof() == true)
	{
		backtexnum = 1;
	}


	if (iNum == 2 && CInventory_Manager::Get_Instance()-> Get_ToolboxConstruct_list()->front()->get_onof() == false)
	{
		backtexnum = 0;
	}
	else if (iNum == 2 && CInventory_Manager::Get_Instance()->Get_ToolboxConstruct_list()->front()->get_onof() == true)
	{
		backtexnum = 1;
	}

	if (iNum == 3 && CInventory_Manager::Get_Instance()->Get_ToolboxMaterial_list()->front()->get_onof() == false)
	{
		backtexnum = 0;
	}
	else if (iNum == 3 && CInventory_Manager::Get_Instance()->Get_ToolboxMaterial_list()->front()->get_onof() == true)
	{
		backtexnum = 1;
	}

	

	/*if (m_bfirstclick_G == true || m_bfirstclick_W == true || m_bfirstclick_C == true)
		

	else if(m_bfirstclick_G == false || m_bfirstclick_W == false || m_bfirstclick_C == false )
		backtexnum = 1;*/


	/*if (m_bfirstclick_W == true && iNum == 1)
		backtexnum = 0;

	else
		backtexnum = 1;*/


	if (nullptr != m_pRendererCom)
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainToolbox_back"), (CComponent**)&m_pTextureCom)))
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

	CInventory_Manager::Get_Instance()->Get_toolboxmainback_list()->push_back(pInstance);

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

