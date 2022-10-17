#include "stdafx.h"
#include "Trap.h"
#include "GameInstance.h"

CTrap::CTrap(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTrap::CTrap(const CTrap & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrap::Initialize(void* pArg)
{
	ZeroMemory(&m_tTrapDesc, sizeof(TRAPDESC));
	memcpy(&m_tTrapDesc, (TRAPDESC*)pArg, sizeof(TRAPDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(&m_tTrapDesc.vInitPosition)))
		return E_FAIL;

	switch (m_tTrapDesc.eType)
	{
	case TRAPTYPE::TRAP_STAR:
		m_fRadius = .4f;
		break;
	case TRAPTYPE::TRAP_PLANT:
		m_fRadius = .5f; 
		break;
	}

	return S_OK;
}

int CTrap::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	__super::Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CTrap::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	SetUp_BillBoard();
}

HRESULT CTrap::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTrap::SetUp_Components(void* pArg)
{
	
	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(1.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

void CTrap::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);	// Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);		// Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

HRESULT CTrap::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_fSpeed = 25.f;
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iCurrentTex = 0;

	switch (m_tTrapDesc.eType)
	{
	case TRAPTYPE::TRAP_STAR:
	{
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Ready"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Idle_Ready"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Idle_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 48;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reset"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Reset"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
	}
	break;
	case TRAPTYPE::TRAP_PLANT:
	{
		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Ready"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Idle_Ready"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 2;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Idle_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 23;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 15;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reset"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Reset"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
	}
	break;
	}
}



CTrap* CTrap::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTrap* pInstance = new CTrap(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTrap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrap::Clone(void* pArg)
{
	CTrap* pInstance = new CTrap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTrap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrap::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}