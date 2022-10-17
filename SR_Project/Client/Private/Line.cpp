#include "stdafx.h"
#include "..\Public\Line.h"
#include "GameInstance.h"
#include "Inventory.h"


CLine::CLine(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CLine::CLine(const CLine & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLine::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 250.0f;
	m_fSizeY = 2.0f;
	m_fX = 990.f;
	m_fY = 360.f + (iNum * 48.f);
	/*



	if (iNum >= 4)
	m_fX += 25.f;

	if (iNum == 3 || iNum == 7)
	m_fX += 2.5f;

	if (iNum == 2 || iNum == 6)
	{
	texnum = 10;
	}


	m_firsty = m_fY;

	m_firstx = m_fX;*/


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CLine::Tick(_float fTimeDelta)
{
	CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();
	//Safe_AddRef(pInventory_Manager);
	set_line();
	
	if (m_bcheck == true && pInventory_Manager->Get_Quest_list()->front()->get_onoff() == true)
	{
		
		__super::Tick(fTimeDelta);
	
	
	RECT		rcRect;
	SetRect(&rcRect, (_int)(m_fX - m_fSizeX * 0.5f), (_int)(m_fY - m_fSizeY * 0.5f), (_int)(m_fX + m_fSizeX * 0.5f), (_int)(m_fY + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	

	/*if (m_makewhat == MAKE_AXE || m_makewhat == MAKE_HAMBAT)
	m_fY = 185.f;
	else if (m_makewhat == MAKE_PICK || m_makewhat == MAKE_SHOTTER)
	m_fY = 235.f;
	else if (m_makewhat == MAKE_STAFF)
	m_fY = 285.f;
	else if (m_makewhat == MAKE_ARMOR)
	m_fY = 335.f;
	else if (m_makewhat == MAKE_HELMET)
	m_fY = 385.f;*/
	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	}
	//Safe_Release(pInventory_Manager);
	return OBJ_NOEVENT;
}

void CLine::Late_Tick(_float fTimeDelta)
{
	CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();
//	Safe_AddRef(pInventory_Manager);
	
	if (m_bcheck == true && pInventory_Manager->Get_Quest_list()->front()->get_onoff() == true)
	{
		__super::Late_Tick(fTimeDelta);

		RECT		rcRect;
		SetRect(&rcRect, (_int)(m_fX - m_fSizeX * 0.5f), (_int)(m_fY - m_fSizeY * 0.5f), (_int)(m_fX + m_fSizeX * 0.5f), (_int)(m_fY + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		//if (!PtInRect(&rcRect, ptMouse))
		//{
		//	m_fSizeX = 20;
		//	m_fSizeY = 20;
		//	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);


		//	//ERR_MSG(L"충돌");
		//}
		if (nullptr != m_pRendererCom&&m_bcheck)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

		//set_check(false);
	}
//	Safe_Release(pInventory_Manager);
	
}

HRESULT CLine::Render()
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

HRESULT CLine::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Line"), (CComponent**)&m_pTextureCom)))
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

HRESULT CLine::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 20);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CLine::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CLine * CLine::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLine*	pInstance = new CLine(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : Prototype_Component_Texture_Line"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLine::Clone(void* pArg)
{
	CLine*	pInstance = new CLine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : Prototype_Component_Texture_Line"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Line_list()->push_back(pInstance);
	return pInstance;
}


void CLine::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

void CLine::set_line()
{

	CInventory_Manager*         pInventory_Manager = CInventory_Manager::Get_Instance();
	
	if (pInventory_Manager->Get_Quest_list()->front()->get_texnum() == 0)  // quest 0
	{
		auto questpont = pInventory_Manager->Get_Questpont_list();
   
			for (auto iter = questpont->begin(); iter != questpont->end(); ++iter)
			{
				if (iNum == 0)
				{
					if ((*iter)->get_pontnum() == 0 && (*iter)->get_pontex() >= 3) // 있으면 초록! 업승면 빨강! 근데.. 계속돌아서 
					{
						m_bcheck = true;
						break;
					}
					else
						m_bcheck = false;
				}

				else if (iNum == 1)
				{
					if ((*iter)->get_pontnum() == 1 && (*iter)->get_pontex() >= 2) // 있으면 라인 없으면 노라인
					{
						m_bcheck = true;
						break;
					}
					else
						m_bcheck = false;
				}

				else if (iNum == 2)
				{
					if ((*iter)->get_pontnum() == 2 && (*iter)->get_pontex() >= 1) // 있으면 라인 없으면 노라인
					{
						m_bcheck = true;
						break;
					}
					else
						m_bcheck = false;
				}

				

			}
		}

	else if (pInventory_Manager->Get_Quest_list()->front()->get_texnum() == 1)  // quest 1
	{

		
			
			if (iNum == 0 || iNum == 2)
				m_bcheck = false;

			else if (iNum == 1)
			{  
				if (quest2count >= 3)
				{
					m_bcheck = true;
				}
				else
					m_bcheck = false;
				
					
					
			}
				
			

			

			

		}

	else if (pInventory_Manager->Get_Quest_list()->front()->get_texnum() == 2)  // quest 1
	{

		if (m_bquest2 == true)
		{
			m_bcheck = true;
		}
		else
			m_bcheck = false;


		
		
		
		

		

	}

	else if (pInventory_Manager->Get_Quest_list()->front()->get_texnum() == 3)  // quest 1
	{

		if (m_bquest3 == true)
		{
			m_bcheck = true;
		}
		else
			m_bcheck = false;


		if (iNum == 0 || iNum == 2)
			m_bcheck = false;
		

	}

	else if (pInventory_Manager->Get_Quest_list()->front()->get_texnum() == 4)  // quest 1
	{

		if (m_bquest4 == true)
		{
			m_bcheck = true;
		}
		else
			m_bcheck = false;


		if (iNum == 0 || iNum == 2)
			m_bcheck = false;


	}

	else if (pInventory_Manager->Get_Quest_list()->front()->get_texnum() == 5)  // quest 1
	{

		/*if (m_bquest4 == true)
		{
			m_bcheck = true;
		}
		else
			m_bcheck = false;


		if (iNum == 0 || iNum == 2)*/
		if (m_bquest5 == true)
		{
			m_bcheck = true;
		}
		else
			m_bcheck = false;


		if (iNum == 0 || iNum == 2)
			m_bcheck = false;


	}

}
	

	

