#include "stdafx.h"
#include "..\Public\Shooting_Target.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "PickingMgr.h"
#include "Statue.h"
#include "DecoObject.h"

CShooting_Target::CShooting_Target(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CShooting_Target::CShooting_Target(const CShooting_Target & rhs)
	: CMonster(rhs)
{
}

HRESULT CShooting_Target::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShooting_Target::Initialize(void* pArg)
{
	memcpy(&m_ShootingTargetDesc, pArg, sizeof(TARGETDESC)); 

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1.f, 1.5f, 1.f);
	m_eState = IDLE;

	m_tInfo.iMaxHp = 75;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	m_dwIdleTime = GetTickCount();
	return S_OK;
}

int CShooting_Target::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// A.I.
	
	/*if (  m_eState == IDLE  && m_dwIdleTime + rand() % 5000 + 3000 < GetTickCount())
	{
		m_dwAttackTime = GetTickCount();
		m_eState = STOP;
	}*/
	

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CShooting_Target::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	_float3 vDistance = _float3(0, 0, 0);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Collision_with_Group(CCollider_Manager::COLLISION_BLOCK, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		_float3 vPosition = Get_Position();

		if (fabsf(vDistance.x) < fabsf(vDistance.z))
			vPosition.x -= vDistance.x;
		else
			vPosition.z -= vDistance.z;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

}

HRESULT CShooting_Target::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


#ifdef _DEBUG
	if (g_ColliderRender &&  m_pColliderCom != nullptr)
		m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CShooting_Target::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.5f;
	CollRectDesc.fRadiusX = 0.5f;
	CollRectDesc.fRadiusZ = 0.5f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = *(_float3*)&m_ShootingTargetDesc.vPosition;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShooting_Target::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 20;

	
		TextureDesc.m_iEndTex = 17;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_BAD"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooting_Target_Bad"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 8;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_BAD"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooting_Target_Bad_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 9;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_STOP_BAD"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooting_Target_Bad_Stop"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	
		TextureDesc.m_iEndTex = 17;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_GOOD"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooting_Target_Good"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 41;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_GOOD"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooting_Target_Good_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 9;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_STOP_GOOD"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooting_Target_Good_Stop"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		
	
	return S_OK;
}

void CShooting_Target::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE::IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		{
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 16)
			{
				int iNum = rand() % 2;
				iNum == 0 ? m_ShootingTargetDesc.eType = TARGET_BAD : m_ShootingTargetDesc.eType = TARGET_GOOD;
				if (m_ShootingTargetDesc.eType == TARGET_BAD)
					Change_Texture(TEXT("Com_Texture_IDLE_BAD"));
				else 
					Change_Texture(TEXT("Com_Texture_IDLE_GOOD"));
			}
		}
		break;
	case STATE::HIT:
	{
		m_pTextureCom->MoveFrame(m_TimerTag, false);

		if (m_ShootingTargetDesc.eType == TARGET_BAD && m_dwDeathTime + 3000 < GetTickCount())
		{
			m_dwIdleTime = GetTickCount();
			m_bHit = false;
			m_eState = IDLE;
		}
		break;
	}
	case STATE::STOP:
		m_pTextureCom->MoveFrame(m_TimerTag);
		if(m_dwAttackTime + rand()%3000 < GetTickCount())
		{
			m_dwIdleTime = GetTickCount();
			m_bHit = false;
			m_eState = IDLE;
		}
		break;
	}
}

void CShooting_Target::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			if(m_ShootingTargetDesc.eType == TARGET_GOOD)
				Change_Texture(TEXT("Com_Texture_IDLE_GOOD"));
			else
				Change_Texture(TEXT("Com_Texture_IDLE_BAD"));
			break;
		case STATE::HIT:
		{
			if (m_ShootingTargetDesc.eType == TARGET_GOOD)
			{
				Change_Texture(TEXT("Com_Texture_HIT_GOOD"));
				CDecoObject::DECODECS DecoDesc;
				DecoDesc.m_eState = CDecoObject::PARTY;
				DecoDesc.vInitPosition = Get_Position();
				DecoDesc.vInitPosition.y += 1.5f;
				CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);
			}
			else
				Change_Texture(TEXT("Com_Texture_HIT_BAD"));
			break;
		}
			
			
		case STATE::STOP:
			if (m_ShootingTargetDesc.eType == TARGET_GOOD)
				Change_Texture(TEXT("Com_Texture_STOP_GOOD"));
			else
				Change_Texture(TEXT("Com_Texture_STOP_BAD"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;

	}
}






_float CShooting_Target::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	//_float fDmg = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (m_eState == IDLE && m_pTextureCom->Get_Frame().m_iCurrentTex > 12)
		return 0.f;

	m_dwDeathTime = GetTickCount();
	m_bHit = true;
	m_eState = HIT;
	m_pTransformCom->Set_Scale(1.2f, 1.5f, 1.f);
	return 0.f;
}


_bool CShooting_Target::IsDead()
{
	
	return false;
}

CShooting_Target* CShooting_Target::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShooting_Target* pInstance = new CShooting_Target(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CShooting_Target"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShooting_Target::Clone(void* pArg)
{
	CShooting_Target* pInstance = new CShooting_Target(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShooting_Target"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CShooting_Target::Free()
{
	__super::Free();
}