#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

int CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (IsDead())
		return OBJ_DEAD;

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_MONSTER, this);

	// Match Terrain-Y
	WalkingTerrain();

	return OBJ_NOEVENT;
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMonster::Render()
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

HRESULT CMonster::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMonster::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CMonster::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CMonster::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CMonster::WalkingTerrain()
{
	
}

_float CMonster::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage <= 0 || m_bDead)
		return 0.f;

	m_tInfo.iCurrentHp -= fDamage;

	if (m_tInfo.iCurrentHp <= 0)
	{
		m_tInfo.iCurrentHp = 0.f;
		m_bDead = true;
	}

	return fDamage;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}