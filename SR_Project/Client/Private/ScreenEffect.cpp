#include "stdafx.h"
#include "..\Public\ScreenEffect.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

CScreenEffect::CScreenEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CScreenEffect::CScreenEffect(const CScreenEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CScreenEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CScreenEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 1280;
	m_fSizeY = 720.f;
	m_fX = 640.f;
	m_fY = 360.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	m_bforboss = *(_bool*)pArg;

	if (m_bforboss == true)
	{
		texnum = 1;
		alpha = 1.f;
	}

	return S_OK;
}

int CScreenEffect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bforboss == false)
	{
		alpha += 0.06f;

		if (GetTickCount() > m_dwDeadtime + 5000)
		{
			m_dwDeadtime = GetTickCount();
			return OBJ_DEAD;
		}
	}
	else
	{

		if (GetTickCount() > m_dwDeadtime + 7000)
		{
			m_dwDeadtime = GetTickCount();
			return OBJ_DEAD;
		}

		if (alpha >= 0 && m_bfirst == true)
			alpha -= (_float)0.1f;

		if (alpha <= 0)
		{
			m_bfirst = false;
		}

		if (m_bfirst == false)
		{
			alpha +=(_float) 0.1f;
		}

		

	}
	

	



	return OBJ_NOEVENT;
}

void CScreenEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CScreenEffect::Render()
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

HRESULT CScreenEffect::SetUp_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ScreenEffect"), (CComponent**)&m_pTextureCom)))
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

HRESULT CScreenEffect::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CScreenEffect::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CScreenEffect * CScreenEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScreenEffect*	pInstance = new CScreenEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CScreenEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CScreenEffect::Clone(void* pArg)
{
	CScreenEffect*	pInstance = new CScreenEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CScreenEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CScreenEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
