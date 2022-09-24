#include "stdafx.h"
#include "Terrorbeak.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "PickingMgr.h"

CTerrorbeak::CTerrorbeak(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CTerrorbeak::CTerrorbeak(const CTerrorbeak & rhs)
	: CMonster(rhs)
{
}

HRESULT CTerrorbeak::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrorbeak::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 40;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
	m_tInfo.fDamage = 10.f;

	m_fAttackRadius = 1.f;

	m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

int CTerrorbeak::Tick(_float fTimeDelta)
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

void CTerrorbeak::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta); // Monsters are inserted in the RENDER_NOTALPHABLEND group.

	SetUp_BillBoard();
	Set_ShaderID(true);

	// Alphablend
	Compute_CamDistance(Get_Position());
	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	// Collider
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	// Picking
	if (!m_bPicking)
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}

	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CTerrorbeak::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	if (g_ColliderRender &&  m_pColliderCom != nullptr)
		m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CTerrorbeak::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

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

	MINIMAP		minidesc;
	ZeroMemory(&minidesc, sizeof(MINIMAP));
	minidesc.name = MIN_TERRORBEAK;
	minidesc.pointer = this;

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), LEVEL_BOSS, TEXT("MiniMap_Icon"), &minidesc);
	Safe_Release(pGameInstance);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));

	CollRectDesc.fRadiusY = .4f;
	CollRectDesc.fRadiusX = .3f;
	CollRectDesc.fRadiusZ = .5f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static_Blend"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	SetUp_DebugComponents(pArg);

	return S_OK;
}

HRESULT CTerrorbeak::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	TextureDesc.m_iEndTex = 15;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_APPEAR"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Appear"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_DOWN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Attack_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_SIDE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Attack_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_UP"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Attack_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 13;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DISAPPEAR"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Disappear"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 42;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_DOWN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 42;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_SIDE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 42;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_UP"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_DOWN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Walk_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 33;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_SIDE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Walk_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_UP"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrorbeak_Walk_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CTerrorbeak::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE::APPEAR:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATE::IDLE;
		break;
	case STATE::ATTACK:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-2.f, 2.f, 1.f);
		else
			m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

		Attack(fTimeDelta);
		break;
	case STATE::DISAPPEAR:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			// When Hit
			if (!m_bDead)
			{
				m_eState = STATE::IDLE;
				m_bHit = false;
			}
			// When Dead
			else
				m_bDeadAnimExpired = true;
		}	
		else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 8 && m_bDead)
		{
			if (!m_bItemDropped)
				Drop_Items();
		}
		break;
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
	}
}

void CTerrorbeak::Change_Motion()
{
	if (m_eState != m_ePreState || m_eDir != m_ePreDir)
	{
		switch (m_eState)
		{
		case STATE::APPEAR:
			Change_Texture(TEXT("Com_Texture_APPEAR"));
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
			break;
		case STATE::DISAPPEAR:
			Change_Texture(TEXT("Com_Texture_DISAPPEAR"));
			break;
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
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
		if (m_eDir != m_ePreDir)
			m_ePreDir = m_eDir;
	}
}

_bool CTerrorbeak::Picking(_float3 * PickingPoint)
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
		m_bPickingTrue = false;
		CInventory_Manager* pInvenManager = CInventory_Manager::Get_Instance(); Safe_AddRef(pInvenManager);

		auto i = pInvenManager->Get_Monsterinfo_list()->front();
		auto k = pInvenManager->Get_Monsterhp_list();

		Safe_Release(pInvenManager);

		i->set_monstername(MONSTER_END);
		i->set_check(false);

		m_eShaderID = SHADER_IDLE;
		for (auto j : *k)
			j->set_check(false);
		return false;
	}

	return true;
}

void CTerrorbeak::PickingTrue()
{
	m_bPickingTrue = true;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance(); Safe_AddRef(pGameInstance);
	CInventory_Manager* pInvenManager = CInventory_Manager::Get_Instance(); Safe_AddRef(pInvenManager);

	auto i = pInvenManager->Get_Monsterinfo_list()->front();
	auto k = pInvenManager->Get_Monsterhp_list();
	m_eShaderID = SHADER_PICKING;
	i->set_monstername(MONSTER_TERRORBEAK);
	i->set_check(true);

	for (auto j : *k)
	{
		j->set_check(true);
		j->set_hp((_uint)m_tInfo.iCurrentHp);
	}

	Safe_Release(pGameInstance);
	Safe_Release(pInvenManager);
}

void CTerrorbeak::AI_Behaviour(_float fTimeDelta)
{
	if (m_bDead || m_bHit || m_bIsAttacking)
	{
		if (m_bHit)
			m_eState = STATE::DISAPPEAR;

		return;
	}

	Find_Target();  // Check: pTarget, bAggro, fDistanceToTarget

	if (m_pTarget && m_bAggro)
	{
		Calculate_Direction(m_pTarget->Get_Position());

		// If in AttackRadius: Attack
		if (m_fDistanceToTarget < m_fAttackRadius)
		{
			if (GetTickCount() > m_dwAttackTime + 2000)
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

void CTerrorbeak::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	if (m_eState == STATE::IDLE)
	{
		if (GetTickCount() > m_dwIdleTime + 3000 + (rand() % 3000)*(rand() % 2 + 1))
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
		if (GetTickCount() > m_dwWalkTime + 1500 + (rand() % 3000)*(rand() % 2 + 1))
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
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		if (!pVIBuffer_Terrain)
			return;
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
		if (!pTransform_Terrain)
			return;

		_float3 vPatrolPosition = { m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ };
		_float3 vScale = m_pTransformCom->Get_Scale();

		vPatrolPosition.y = pVIBuffer_Terrain->Compute_Height(vPatrolPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y / 2));

		Calculate_Direction(vPatrolPosition);

		m_pTransformCom->Go_PosTarget(fTimeDelta * .1f, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
	}
}

void CTerrorbeak::Find_Target()
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

void CTerrorbeak::Follow_Target(_float fTimeDelta)
{
	// Set State 
	m_eState = STATE::WALK;

	_float3 fTargetPos = m_pTarget->Get_Position();
	m_pTransformCom->Go_PosTarget(fTimeDelta * .2f, fTargetPos, _float3(0, 0, 0));

	m_bIsAttacking = false;
}

void CTerrorbeak::Attack(_float fTimeDelta)
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
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 4 && !m_bDidDamage)
		{
			// Create Standard Bullet
			BULLETDATA BulletData;
			ZeroMemory(&BulletData, sizeof(BulletData));

			BulletData.bIsPlayerBullet = false;
			BulletData.vPosition = m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
			BulletData.eDirState = Get_Unprocessed_Dir(m_eDir);
			BulletData.fOffsetSide = .3f;
			BulletData.fOffsetUp = .3f;
			BulletData.fOffsetDown = .3f;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bDidDamage = true;
		}
	}
}

_float CTerrorbeak::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	_float fDmg = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (fDmg > 0)
	{
		foreffect effectdesc;
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

HRESULT CTerrorbeak::Drop_Items()
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

	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	m_bItemDropped = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

_bool CTerrorbeak::IsDead()
{
	if (m_bDead && m_eState == STATE::DISAPPEAR && GetTickCount() > m_dwDeathTime + 1500)
		return true;
	else if (m_bDead && m_eState != STATE::DISAPPEAR)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DISAPPEAR;
	}

	return false;
}

CTerrorbeak* CTerrorbeak::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrorbeak* pInstance = new CTerrorbeak(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTerrorbeak"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrorbeak::Clone(void* pArg)
{
	CTerrorbeak* pInstance = new CTerrorbeak(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrorbeak"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrorbeak::Free()
{
	__super::Free();
}