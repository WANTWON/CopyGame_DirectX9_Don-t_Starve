#include "stdafx.h"
#include "ShockEffect.h"
#include "GameInstance.h"
#include "PickingMgr.h"

CShockEffect::CShockEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CShockEffect::CShockEffect(const CShockEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CShockEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShockEffect::Initialize(void* pArg)
{
	ZeroMemory(&m_tShockDesc, sizeof(SHOCKDESC));
	memcpy(&m_tShockDesc, (SHOCKDESC*)pArg, sizeof(SHOCKDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(m_tShockDesc.vInitPosition)))
		return E_FAIL;

	if (m_tShockDesc.eShockType == SHOCKTYPE::SHOCK)
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
	
	if (m_tShockDesc.eShockType == SHOCKTYPE::HIT)
		m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

int CShockEffect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	// If Effect has Target
	Stick_ToTarget();

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CShockEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Compute_CamDistance(Get_Position());
	SetUp_BillBoard();
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	if (m_tShockDesc.eShockType == SHOCKTYPE::SHOCK)
	{
		if (m_fShockTime > m_tShockDesc.fShockTimeLimit)
			m_bDead = true;
		else
			m_fShockTime += fTimeDelta;
	}

	Change_Frame();
}

HRESULT CShockEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}


HRESULT CShockEffect::SetUp_Components(void* pArg)
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
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShockEffect::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CShockEffect::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CShockEffect::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CShockEffect::WalkingTerrain()
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (!pGameInstance)
		return;
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (!pVIBuffer_Terrain)
		return;
	CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (!pTransform_Terrain)
		return;

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScale = m_pTransformCom->Get_Scale();
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y *0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CShockEffect::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CShockEffect::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	if (m_tShockDesc.eShockType == SHOCKTYPE::SHOCK)
	{
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Shock_Effect"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Shock_Effect"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}
	
	if (m_tShockDesc.eShockType == SHOCKTYPE::HIT)
	{
		TextureDesc.m_iEndTex = 18;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Shock_Effect"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Effect"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}

	return S_OK;
}

void CShockEffect::Change_Frame()
{
	if (m_tShockDesc.eShockType == SHOCKTYPE::SHOCK)
		m_pTextureCom->MoveFrame(m_TimerTag);

	if (m_tShockDesc.eShockType == SHOCKTYPE::HIT)
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_bDead = true;
}

void CShockEffect::Stick_ToTarget()
{
	if (m_tShockDesc.pEffectTarget)
	{
		CComponent* pComponentColl = m_tShockDesc.pEffectTarget->Find_Component(TEXT("Com_Collider_Cube"));
		if (!pComponentColl)
			return;
		CCollider_Cube* pColliderCube = dynamic_cast<CCollider_Cube*>(pComponentColl);
		if (!pColliderCube)
			return;
		CComponent* pComponentTrans = m_tShockDesc.pEffectTarget->Find_Component(TEXT("Com_Transform"));
		if (!pComponentTrans)
			return;
		CTransform* pTransform = dynamic_cast<CTransform*>(pComponentTrans);
		if (!pTransform)
			return;

		_float3 vTargetPosition;
		_float3 vLook;
		D3DXVec3Normalize(&vLook, &pTransform->Get_State(CTransform::STATE::STATE_LOOK));
		vTargetPosition = (_float3)pColliderCube->Get_CollRectDesc().StateMatrix.m[3] - vLook;

		m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vTargetPosition);
	}
}

CShockEffect* CShockEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShockEffect* pInstance = new CShockEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CShockEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShockEffect::Clone(void* pArg)
{
	CShockEffect* pInstance = new CShockEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShockEffect "));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CShockEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}