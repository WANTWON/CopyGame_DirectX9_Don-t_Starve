#include "stdafx.h"
#include "..\Public\MiniMap.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

CMiniMap::CMiniMap(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMiniMap::CMiniMap(const CMiniMap & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniMap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniMap::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 640.f;
	m_fSizeY = 400.f;
	m_fX = 640.f;
	m_fY = 360.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CMiniMap::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return OBJ_NOEVENT;

	if (m_iCurrentLevelndex != m_iPreLevelIndex)
	{
		if (m_iCurrentLevelndex == LEVEL_GAMEPLAY) //*8
		{
			m_fSizeX = 640.f;
			m_fSizeY = 400.f;
			m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			texnum = 0;

			m_iPreLevelIndex = m_iCurrentLevelndex;
		}


		else if (m_iCurrentLevelndex == LEVEL_HUNT) // *10
		{
			m_fSizeX = 600.f;
			m_fSizeY = 400.f;
			m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			texnum = 1;

			m_iPreLevelIndex = m_iCurrentLevelndex;
		}

		else if (m_iCurrentLevelndex == LEVEL_MAZE) // *12
		{
			m_fSizeX = 600.f;
			m_fSizeY = 600.f;
			m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			texnum = 2;

			m_iPreLevelIndex = m_iCurrentLevelndex;
		}

		else if (m_iCurrentLevelndex == LEVEL_BOSS) // *10
		{
			m_fSizeX = 300.f;
			m_fSizeY = 300.f;
			m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			texnum = 3;

			m_iPreLevelIndex = m_iCurrentLevelndex;
		}
	}
	
	

	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	//CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
	//Safe_AddRef(pGameInstance);
	//Safe_AddRef(pinv);

	//pinv->sethp((_uint)(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Player_Stat().fCurrentHealth));

	////texnum = pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->get_
	//texnum = (_uint)(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Player_Stat().fCurrentHealth) / 20;

	//Safe_Release(pGameInstance);
	//Safe_Release(pinv);


	return OBJ_NOEVENT;
}

void CMiniMap::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{
		__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
	
}

HRESULT CMiniMap::Render()
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

		m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(texnum));

		m_pShaderCom->Begin(m_eShaderID);

		m_pVIBufferCom->Render();
		m_pShaderCom->End();

		return S_OK;
	}

	return E_FAIL;
}

HRESULT CMiniMap::SetUp_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMap"), (CComponent**)&m_pTextureCom)))
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

HRESULT CMiniMap::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMiniMap::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CMiniMap * CMiniMap::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMiniMap*	pInstance = new CMiniMap(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMiniMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniMap::Clone(void* pArg)
{
	CMiniMap*	pInstance = new CMiniMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMiniMap"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Minimap_list()->push_back(pInstance);

	return pInstance;
}


void CMiniMap::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
