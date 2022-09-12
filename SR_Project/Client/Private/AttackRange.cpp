#include "stdafx.h"
#include "..\Public\AttackRange.h"
#include "GameInstance.h"


CAttackRange::CAttackRange(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CAttackRange::CAttackRange(const CAttackRange & rhs)
	:CGameObject(rhs)
{
}

HRESULT CAttackRange::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAttackRange::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bIsPicker = *(_bool*)pArg;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);
	return S_OK;
}

int CAttackRange::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return OBJ_NOEVENT;
}

void CAttackRange::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//if (Compare_Terrain())

	//SetUp_BillBoard();
	if (m_bIsPicker)
	{
		if (nullptr != m_pRendererCom && m_bShow)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
	else
	{
		if (nullptr != m_pRendererCom && m_bShow)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CAttackRange::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(Check_Picker()))
		return E_FAIL;
	
	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;

}

HRESULT CAttackRange::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	/*if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;*/

	/* For.Com_Texture */
	if (FAILED(Texture_Clone()))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = { 0.f, 0.f, 0.f };
	TransformDesc.InitPos.y += 0.2f;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CAttackRange::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CAttackRange::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CAttackRange::Init_Data(void)
{
	return S_OK;
}

HRESULT CAttackRange::Texture_Clone(void)
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 1;
	TextureDesc.m_fSpeed = 60;

	if (m_bIsPicker)
	{
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Picker"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Picker"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Range"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_AttackRange"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
	}
	//m_vecTexture.push_back(m_pTextureCom);
	
	return S_OK;
}

void CAttackRange::Set_Scale(_float3 _vScale)
{
	m_pTransformCom->Set_Scale(_vScale.x, _vScale.y, _vScale.z);
}

void CAttackRange::Set_Pos(_float3 _vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos); 

}

HRESULT CAttackRange::Check_Picker()
{
	if (m_bIsCorrect)
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
			return E_FAIL;
	}

	return S_OK;
}

void CAttackRange::WalkingTerrain()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.5f);

}

void CAttackRange::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CAttackRange * CAttackRange::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAttackRange*	pInstance = new CAttackRange(pGraphic_Device);

 
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CAttackRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAttackRange::Clone(void * pArg)
{
	CAttackRange*	pInstance = new CAttackRange(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CAttackRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CAttackRange::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	//Safe_Release(m_pColliderCom);
}
