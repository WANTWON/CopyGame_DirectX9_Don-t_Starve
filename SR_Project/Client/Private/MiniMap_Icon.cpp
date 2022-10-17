#include "stdafx.h"
#include "..\Public\MiniMap_Icon.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

CMiniMap_Icon::CMiniMap_Icon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMiniMap_Icon::CMiniMap_Icon(const CMiniMap_Icon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniMap_Icon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniMap_Icon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		memcpy(&minidesc, pArg, sizeof(MINIMAP));

	}

	//minidesc.pointer->Get_Position()

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 20.f;
	m_fSizeY = 20.f;
	m_fX = 530.f;
	m_fY = 560.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CMiniMap_Icon::Tick(_float fTimeDelta)
{


	if (m_bcheck == true)
	{

		

		

		__super::Tick(fTimeDelta);


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		if (minidesc.pointer->Get_Dead() == true)
			return OBJ_DEAD;

		pos = minidesc.pointer->Get_Position();//(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Position()) ;

		//minidesc.pointer->get_
		m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		if (m_iCurrentLevelndex == LEVEL_LOADING)
			return OBJ_NOEVENT;

		
			if (m_iCurrentLevelndex == LEVEL_GAMEPLAY)
			{
				m_fX = 325.f + (pos.x * 8.f);
				m_fY = 560.f - (pos.z * 8.f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
				m_iPreLevelIndex = m_iCurrentLevelndex;
			}
			else if (m_iCurrentLevelndex == LEVEL_HUNT)
			{
				m_fX = 343.f + (pos.x * 10.f);
				m_fY = 555.f - (pos.z * 10.f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
				m_iPreLevelIndex = m_iCurrentLevelndex;
			}
			else if (m_iCurrentLevelndex == LEVEL_MAZE)
			{
				m_fX = 343.f + (pos.x * 12.f);
				m_fY = 655.f - (pos.z * 12.f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
				m_iPreLevelIndex = m_iCurrentLevelndex;
			}
			else if (m_iCurrentLevelndex == LEVEL_BOSS)
			{
				m_fX = 497.f + (pos.x * 10.f);
				m_fY = 500.f - (pos.z * 10.f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
				m_iPreLevelIndex = m_iCurrentLevelndex;
			}


		

		



	}
	return OBJ_NOEVENT;
}

void CMiniMap_Icon::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{
		__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
	
}

HRESULT CMiniMap_Icon::Render()
{
	if (m_bcheck == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		_float4x4		WorldMatrix, ViewMatrix;
		D3DXMatrixIdentity(&ViewMatrix);

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
		WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());

		m_pShaderCom->Set_RawValue("g_alpha", &alpha, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&m_ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));

		m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(minidesc.name));

		m_pShaderCom->Begin(m_eShaderID);

		m_pVIBufferCom->Render();
		m_pShaderCom->End();

		return S_OK;
	}
	
	return E_FAIL;
}

HRESULT CMiniMap_Icon::SetUp_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMap_Icon"), (CComponent**)&m_pTextureCom)))
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

HRESULT CMiniMap_Icon::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMiniMap_Icon::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CMiniMap_Icon * CMiniMap_Icon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMiniMap_Icon*	pInstance = new CMiniMap_Icon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMiniMap_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniMap_Icon::Clone(void* pArg)
{
	CMiniMap_Icon*	pInstance = new CMiniMap_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMiniMap_Icon"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Minimapicon_list()->push_back(pInstance);


	
	

	return pInstance;
}


void CMiniMap_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
