#include "stdafx.h"
#include "Carnival_Egg.h"
#include "GameInstance.h"

CCarnival_Egg::CCarnival_Egg(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarnival_Egg::CCarnival_Egg(const CCarnival_Egg & rhs)
	: CMonster(rhs)
{
}

HRESULT CCarnival_Egg::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnival_Egg::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

	m_tInfo.iMaxHp = 150;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_fAttackRadius = 1.f;
	m_eState = IDLE;
	
	return S_OK;
}

int CCarnival_Egg::Tick(_float fTimeDelta)
{

	if (__super::Tick(fTimeDelta))
	{
		
		return OBJ_DEAD;
	}

	// A.I.
	if(m_eState != COMPLETE && m_eState != PLACE)
		AI_Behaviour(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarnival_Egg::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	
}

HRESULT CCarnival_Egg::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CCarnival_Egg::SetUp_Components(void* pArg)
{

	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = *(_float3*)pArg;
	

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCarnival_Egg::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 25;

	TextureDesc.m_iEndTex = 5;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_DOWN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
}


void CCarnival_Egg::Change_Motion()
{
	
}


void CCarnival_Egg::AI_Behaviour(_float fTimeDelta)
{
	Find_Target();
	if (m_fDistanceToTarget < m_fAttackRadius)
	{
		m_bAggro = true;
		m_eState = STATE::WALK;
		
	}
	else
	{
		m_bAggro = false;
	}

	if (m_bAggro)
	{
		if (m_pTarget)
		{
			Follow_Target(fTimeDelta);
		}
	}
		
}


_float CCarnival_Egg::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{

	return 0.f;
}

HRESULT CCarnival_Egg::Drop_Items()
{

	return S_OK;
}

_bool CCarnival_Egg::IsDead()
{
	if (m_bDead)
		return true;

	return false;
}

CCarnival_Egg* CCarnival_Egg::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarnival_Egg* pInstance = new CCarnival_Egg(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCarnival_Egg"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarnival_Egg::Clone(void* pArg)
{
	CCarnival_Egg* pInstance = new CCarnival_Egg(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarnival_Egg"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCarnival_Egg::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}