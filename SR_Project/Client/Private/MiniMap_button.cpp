#include "stdafx.h"
#include "..\Public\MiniMap_button.h"
#include "GameInstance.h"
#include "Inventory.h"

CMiniMap_button::CMiniMap_button(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMiniMap_button::CMiniMap_button(const CMiniMap_button & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniMap_button::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniMap_button::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 46.f;
	m_fSizeY = 46.f;
	m_fX = 1230.f;
	m_fY = 340.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CMiniMap_button::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	CInventory_Manager*         pInv = CInventory_Manager::Get_Instance();
	
	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();


	if (m_iCurrentLevelndex == LEVEL_LOADING && onetime == true)
	{
		pInv->Iconreset();
		onetime = false;
	}
		
	else if (m_iCurrentLevelndex != LEVEL_LOADING)
		onetime = true;
		
	


	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

	if (PtInRect(&rcRect, ptMouse))
	{

		m_fSizeX = 70.f;
		m_fSizeY = 70.f;

		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);

		if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
		{
			
			//Safe_AddRef(pInv);
			if (pInv->Get_Minimap_list()->front()->get_check() == true)
			{
				
				CGameInstance* pInstance = CGameInstance::Get_Instance();
				pInstance->PlaySounds(TEXT("Map_close1.wav"), SOUND_UICLOSE, 0.9f);

				pInv->Get_Minimap_list()->front()->set_check(false);
				auto p = pInv->Get_Minimapicon_list();
				for (auto k : *p)
				{
					if(k != nullptr)
						k->set_check(false);
				}
					
						
			}
			else if (pInv->Get_Minimap_list()->front()->get_check() == false)
			{
				CGameInstance* pInstance = CGameInstance::Get_Instance();
				pInstance->PlaySounds(TEXT("Map_open.wav"), SOUND_UIOPEN, 0.9f);

				pInv->Get_Minimap_list()->front()->set_check(true);
				auto p = pInv->Get_Minimapicon_list();
				for (auto k : *p)
				{
					if(k != nullptr)
						k->set_check(true);
				}
					
			}
			//Safe_Release(pInv);


		}

	}
	else
	{
		m_fSizeX = 46.f;
		m_fSizeY = 46.f;

		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	}



	return OBJ_NOEVENT;
}

void CMiniMap_button::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMiniMap_button::Render()
{
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

HRESULT CMiniMap_button::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMapbutton"), (CComponent**)&m_pTextureCom)))
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

HRESULT CMiniMap_button::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMiniMap_button::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CMiniMap_button * CMiniMap_button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMiniMap_button*	pInstance = new CMiniMap_button(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMiniMap_button"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniMap_button::Clone(void* pArg)
{
	CMiniMap_button*	pInstance = new CMiniMap_button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMiniMap_button"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMiniMap_button::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
