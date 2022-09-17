#include "stdafx.h"
#include "Boaron.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "PickingMgr.h"

CBoaron::CBoaron(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CBoaron::CBoaron(const CBoaron & rhs)
	: CMonster(rhs)
{
}

HRESULT CBoaron::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoaron::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 100;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
	m_tInfo.fDamage = 5.f;

	m_fAttackRadius = .75f;

	m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	return S_OK;
}

int CBoaron::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta) && m_bDeadAnimExpired)
	{
		CPickingMgr::Get_Instance()->Out_PickingGroup(this);
		return OBJ_DEAD;
	}
		

	

	// A.I.
	AI_Behaviour(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CBoaron::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	if (!m_bPicking)
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}
}

HRESULT CBoaron::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	if (g_ColliderRender)
		m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CBoaron::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

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

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));

	CollRectDesc.fRadiusY = .3f;
	CollRectDesc.fRadiusX = .3f;
	CollRectDesc.fRadiusZ = .3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -0.5f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	SetUp_DebugComponents(pArg);

	return S_OK;
}

HRESULT CBoaron::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Attack_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Attack_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Attack_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 23;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DEATH"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Death"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 29;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 29;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 29;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 12;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_RUN_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Run_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 12;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_RUN_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Run_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 12;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_RUN_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boaron_Run_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CBoaron::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE::IDLE:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-1.5f, 1.5f, 1.f);
		else
			m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::RUN:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-1.5f, 1.5f, 1.f);
		else
			m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::ATTACK:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-1.5f, 1.5f, 1.f);
		else
			m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);

		Attack(fTimeDelta);
		break;
	case STATE::HIT:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-1.5f, 1.5f, 1.f);
		else
			m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);

		if ((m_pTextureCom->MoveFrame(m_TimerTag, false) == true))
			m_bHit = false;
		break;
	case STATE::DIE:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 13)
			Drop_Items();
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 23)
			m_bDeadAnimExpired = true;

		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

void CBoaron::Change_Motion()
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
		case STATE::RUN:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_RUN_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_RUN_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_RUN_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::ATTACK:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_ATTACK_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_ATTACK_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_ATTACK_SIDE"));
				break;
			}

			if (m_eDir != m_ePreDir)
				m_ePreDir = m_eDir;
			break;
		case STATE::HIT:
			Change_Texture(TEXT("Com_Texture_HIT"));
			break;
		case STATE::DIE:
			Change_Texture(TEXT("Com_Texture_DEATH"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
	}
}

_bool CBoaron::Picking(_float3 * PickingPoint)
{
	if (CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
		return false;

	if (true == m_pVIBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		m_vecOutPos = *PickingPoint;



		return true;
	}
	else
	{
		CInventory_Manager* pInvenManager = CInventory_Manager::Get_Instance(); Safe_AddRef(pInvenManager);

		auto i = pInvenManager->Get_Monsterinfo_list()->front();
		auto k = pInvenManager->Get_Monsterhp_list();

		i->set_monstername(MONSTER_END);
		i->set_check(false);


		for (auto j : *k)
			j->set_check(false);
		return false;
	}

	return true;
}

void CBoaron::PickingTrue()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance(); Safe_AddRef(pGameInstance);
	CInventory_Manager* pInvenManager = CInventory_Manager::Get_Instance(); Safe_AddRef(pInvenManager);

	auto i = pInvenManager->Get_Monsterinfo_list()->front();
	auto k = pInvenManager->Get_Monsterhp_list();

	i->set_monstername(MONSTER_BOARON);
	i->set_check(true);

	for (auto j : *k)
	{
		j->set_check(true);
		j->set_hp((_uint)m_tInfo.iCurrentHp);
	}

	Safe_Release(pGameInstance);
	Safe_Release(pInvenManager);
}

void CBoaron::AI_Behaviour(_float fTimeDelta)
{
	if (m_bDead || m_bHit || m_bIsAttacking)
	{
		if (m_bHit)
			m_eState = STATE::HIT;

		return;
	}

	Find_Target();  // Check: pTarget, bAggro, fDistanceToTarget

	if (m_pTarget && m_bAggro)
	{
		Calculate_Direction(m_pTarget->Get_Position());

		// If in AttackRadius: Attack
		if (m_fDistanceToTarget < m_fAttackRadius)
		{
			if (GetTickCount() > m_dwAttackTime + 1500)
			{
				m_eState = STATE::ATTACK;
				m_bIsAttacking = true;
			}
			else
				m_eState = STATE::IDLE;
		}
		// If NOT in AttackRadius: Follow Target
		else
			Follow_Target(fTimeDelta);
	}
	else
		Patrol(fTimeDelta);
}

void CBoaron::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	if (m_eState == STATE::IDLE)
	{
		if (GetTickCount() > m_dwIdleTime + 3000 + (rand() % 3000)*(rand() % 2 + 1))
		{
			m_eState = STATE::RUN;
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
	else if (m_eState == STATE::RUN)
	{
		if (GetTickCount() > m_dwWalkTime + 1500 + (rand() % 3000)*(rand() % 2 + 1))
		{
			m_eState = STATE::IDLE;
			m_dwIdleTime = GetTickCount();
		}
	}

	// Movement
	if (m_eState == STATE::RUN)
	{
		// Adjust PatrolPosition Y
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (!pGameInstance)
			return;
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_BOSS, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		if (!pVIBuffer_Terrain)
			return;
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_BOSS, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
		if (!pTransform_Terrain)
			return;

		_float3 vPatrolPosition = { m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ };
		_float3 vScale = m_pTransformCom->Get_Scale();

		vPatrolPosition.y = pVIBuffer_Terrain->Compute_Height(vPatrolPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y / 2));

		Calculate_Direction(vPatrolPosition);

		m_pTransformCom->Go_PosTarget(fTimeDelta * .1f, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
	}
}

void CBoaron::Find_Target()
{
	if (m_bDead)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	// If Player is Dead: Reset Aggro and Patrol
	if (pTarget->Get_Dead())
	{
		if (m_bAggro)
		{
			m_bAggro = false;
			m_pTarget = nullptr;

			m_eState = STATE::IDLE;
		}
	}
	else
	{
		_float3 vTargetPos = pTarget->Get_Position();
		m_fDistanceToTarget = D3DXVec3Length(&(vTargetPos - Get_Position()));

		// Set Target and Aggro
		m_pTarget = pTarget;
		m_bAggro = true;
	}
}

void CBoaron::Follow_Target(_float fTimeDelta)
{
	// Set State 
	m_eState = STATE::RUN;

	_float3 fTargetPos = m_pTarget->Get_Position();
	m_pTransformCom->Go_PosTarget(fTimeDelta * .2f, fTargetPos, _float3(0, 0, 0));

	m_bIsAttacking = false;
}

void CBoaron::Attack(_float fTimeDelta)
{
	if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
	{
		// Resetting Variables
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();
		m_bDidDamage = false;
	}
	else
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		// Normal Attack
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 14 && !m_bDidDamage)
		{
			// Create Standard Bullet
			BULLETDATA BulletData;
			ZeroMemory(&BulletData, sizeof(BulletData));

			BulletData.bIsPlayerBullet = false;
			BulletData.vPosition = m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
			BulletData.eDirState = Get_Unprocessed_Dir(m_eDir);
			BulletData.fOffsetSide = .5f;
			BulletData.fOffsetUp = .5f;
			BulletData.fOffsetDown = .5f;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bDidDamage = true;
		}
	}
}

_float CBoaron::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	_float fDmg = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (fDmg > 0)
	{
		foreffect		effectdesc;
		ZeroMemory(&effectdesc, sizeof(foreffect));
		effectdesc.dmg = fDmg;
		effectdesc.pos = Get_Position();
		effectdesc.pos.z -= 0.01f;
		//effectdesc.pos.y += 1.25f;
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dmg_pont"), LEVEL_GAMEPLAY, TEXT("Layer_dmgp"), &effectdesc)))
			return OBJ_NOEVENT;
		if (!m_bDead)
			m_bHit = true;

		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();
		m_bDidDamage = false;
	}

	return fDmg;
}

HRESULT CBoaron::Drop_Items()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

	// Random Position Drop based on Object Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignZ = rand() % 2;
	_float fPosX = bSignX ? (Get_Position().x + fOffsetX) : (Get_Position().x - fOffsetX);
	_float fPosZ = bSignZ ? (Get_Position().z + fOffsetZ) : (Get_Position().z - fOffsetZ);

	ItemDesc.fPosition = _float3(fPosX, Get_Position().y, fPosZ);
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Spider_Meat");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_SPIDERMEAT;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_BOSS, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

_bool CBoaron::IsDead()
{
	if (m_bDead && m_eState == STATE::DIE && GetTickCount() > m_dwDeathTime + 1500)
		return true;
	else if (m_bDead && m_eState != STATE::DIE)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DIE;
	}

	return false;
}

CBoaron* CBoaron::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoaron* pInstance = new CBoaron(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBoaron"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoaron::Clone(void* pArg)
{
	CBoaron* pInstance = new CBoaron(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBoaron"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoaron::Free()
{
	__super::Free();
}