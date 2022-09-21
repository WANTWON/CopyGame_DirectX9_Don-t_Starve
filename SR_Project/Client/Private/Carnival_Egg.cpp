#include "stdafx.h"
#include "..\Public\Carnival_Egg.h"
#include "GameInstance.h"
#include "Player.h"

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

	m_pTransformCom->Set_Scale(0.3f, 0.3f, 1.f);

	m_tInfo.iMaxHp = 150;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_fAttackRadius = 1.f;
	m_eMonsterID = PIG;
	m_eState = IDLE;
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

int CCarnival_Egg::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	// A.I.
	AI_Behaviour(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarnival_Egg::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

}

HRESULT CCarnival_Egg::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	if (g_ColliderRender &&  m_pColliderCom != nullptr)
		m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CCarnival_Egg::SetUp_Components(void* pArg)
{

	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.3f;
	CollRectDesc.fRadiusX = 0.3f;
	CollRectDesc.fRadiusZ = 0.3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.f;
	CollRectDesc.fOffsetZ = 0.0f;

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
	TransformDesc.InitPos = *(_float3*)pArg;
	

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	SetUp_DebugComponents(pArg);

	return S_OK;
}

HRESULT CCarnival_Egg::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 45;

	TextureDesc.m_iEndTex = 5;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_DOWN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 5;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_UP"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 5;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_SIDE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 13;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_UP"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Run_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 13;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_DOWN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Run_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 13;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_SIDE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Run_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 2;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PLACE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_COMPLETE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Complete"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCarnival_Egg::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE::IDLE:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-2.f, 2.f, 1.f);
		else
			m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::WALK:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-2.f, 2.f, 1.f);
		else
			m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::COMPLETE:
	{
		if (m_pTextureCom->MoveFrame(m_TimerTag, false) == true)
			m_bDead = true;
		break;
	}	
	}
}

void CCarnival_Egg::Change_Motion()
{
	if (m_eState != m_ePreState || m_eDir != m_ePreDir)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_IDLE_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_IDLE_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_IDLE_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::WALK:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_WALK_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_WALK_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_WALK_SIDE"));
				break;
			}
			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::COMPLETE:
			Change_Texture(TEXT("Com_Texture_COMPLETE"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
	}
}


void CCarnival_Egg::AI_Behaviour(_float fTimeDelta)
{
	
		Find_Target();
		if (m_pTarget)
		{
			// If in AttackRadius > Attack
			if (m_fDistanceToTarget < m_fAttackRadius)
			{
				m_eState = STATE::WALK;
				Follow_Target(fTimeDelta);
			}
			else
			{
				m_eState = STATE::IDLE;
				Patrol(fTimeDelta);
			}
		}
		
}

void CCarnival_Egg::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	if (m_eState == STATE::IDLE)
	{
		if (GetTickCount() > m_dwIdleTime + (rand() % 3000)*(rand() % 2 + 1) + 3000)
		{
			m_eState = STATE::WALK;
			m_dwWalkTime = GetTickCount();

			// Find Random Patroling Position
			_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
			_int bSignX = rand() % 2;
			_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
			_int bSignZ = rand() % 2;
			m_fPatrolPosX = bSignX ? (m_pTransformCom->Get_TransformDesc().InitPos.x + fOffsetX) : (m_pTransformCom->Get_TransformDesc().InitPos.x - fOffsetX);
			m_fPatrolPosZ = bSignZ ? (m_pTransformCom->Get_TransformDesc().InitPos.z + fOffsetZ) : (m_pTransformCom->Get_TransformDesc().InitPos.z - fOffsetZ);
		}
	}
	else if (m_eState == STATE::WALK)
	{
		if (GetTickCount() > m_dwWalkTime + (rand() % 3000)*(rand() % 2 + 1) + 1500)
		{
			m_eState = STATE::IDLE;
			m_dwIdleTime = GetTickCount();
		}
	}

	// Movement
	if (m_eState == STATE::WALK)
	{
		// Adjust PatrolPosition Y
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (!pGameInstance)
			return;
		CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
		if (!pLevelManager)
			return;
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		if (!pVIBuffer_Terrain)
			return;
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
		if (!pTransform_Terrain)
			return;

		_float3 vPatrolPosition = { m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ };
		_float3 vScale = m_pTransformCom->Get_Scale();

		vPatrolPosition.y = pVIBuffer_Terrain->Compute_Height(vPatrolPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y / 2));

		// Change Direction
		_float fX = vPatrolPosition.x - Get_Position().x;
		_float fZ = vPatrolPosition.z - Get_Position().z;

		// Move Horizontally
		if (abs(fX) > abs(fZ))
			if (fX > 0)
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_RIGHT);
			else
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_LEFT);
		// Move Vertically
		else
			if (fZ > 0)
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_UP);
			else
				m_eDir = Get_Processed_Dir(DIR_STATE::DIR_DOWN);

		m_pTransformCom->Go_PosTarget(fTimeDelta * .1f, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
	}
}

void CCarnival_Egg::Find_Target()
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);

	if (pPlayer)
	{

		_float3 vTargetPos = pTarget->Get_Position();
		m_fDistanceToTarget = D3DXVec3Length(&(Get_Position()- vTargetPos));
		m_pTarget = pTarget;

	}
	else
		m_pTarget = nullptr;

}

void CCarnival_Egg::Follow_Target(_float fTimeDelta)
{
	// Set State 
	m_eState = STATE::WALK;

	_float3 fTargetPos = m_pTarget->Get_Position();

	// Set Direction
	_float fX = fTargetPos.x - Get_Position().x;
	_float fZ = fTargetPos.z - Get_Position().z;

	// Move Horizontally
	if (abs(fX) > abs(fZ))
		if (fX > 0)
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_LEFT);
		else
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_RIGHT);
	// Move Vertically
	else
		if (fZ > 0)
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_DOWN);
		else
			m_eDir = Get_Processed_Dir(DIR_STATE::DIR_UP);

	m_pTransformCom->Go_PosTarget(-fTimeDelta * .2f, fTargetPos, _float3(0, 0, 0));
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