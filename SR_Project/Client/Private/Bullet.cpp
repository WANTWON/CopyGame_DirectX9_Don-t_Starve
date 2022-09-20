#include "stdafx.h"
#include "..\Public\Bullet.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Level_Manager.h"
#include "Interactive_Object.h"
#include "Carnival_Shooter.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CPawn(pGraphic_Device)
{

}

CBullet::CBullet(const CBullet & rhs)
	: CPawn(rhs)
{

}

HRESULT CBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_TimerTag = TEXT("Timer_Bullet");

	if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Bullet"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CBullet::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tBulletData = *(BULLETDATA*)(pArg);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	m_eShaderID = SHADER_IDLE_ALPHABLEND;
	return S_OK;
}

int CBullet::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	SetUp_BillBoard();

	if (m_bDead == true)
		return OBJ_DEAD;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_OBJECT, this);

	Excute(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CBullet::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	SetUp_BillBoard();
	if (nullptr != m_pRendererCom && m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_MINES)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	AttackCheck(fTimeDelta);
	DeadCheck(fTimeDelta);

	if (Compare_Terrain()&& m_tBulletData.eWeaponType == WEAPON_BOMB)
		m_bDead = true;

	Compute_CamDistance(Get_Position());
}

HRESULT CBullet::Render()
{


	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	Render_TextureState();

	m_pShaderCom->Begin(m_eShaderID);

	if (m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_HAND && m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_SWORD)
		m_pVIBufferCom->Render();
	m_pShaderCom->End();

#ifdef _DEBUG
	if (g_ColliderRender &&  m_pColliderCom != nullptr)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}
#endif // _DEBUG
	m_pTextureCom->MoveFrame(m_TimerTag);

	return S_OK;
}

HRESULT CBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Rect::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Rect::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.5f;
	CollRectDesc.fRadiusX = 0.5f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.f;

	/* For.Com_Collider_Rect*/
	/*if (FAILED(__super::Add_Components(TEXT("Com_Collider_Rect"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(Texture_Clone()))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;

	if(m_tBulletData.eWeaponType == CARNIVAL_ARROW)
		TransformDesc.fRotationPerSec = D3DXToRadian(1.0f);
	else
		TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	switch (m_tBulletData.eDirState)
	{
	case DIR_STATE::DIR_LEFT:
		m_tBulletData.vPosition -= _float3(m_tBulletData.fOffsetSide, 0.f, 0.f);
		break;
	case DIR_STATE::DIR_RIGHT:
		m_tBulletData.vPosition += _float3(m_tBulletData.fOffsetSide, 0.f, 0.f);
		break;
	case DIR_STATE::DIR_UP:
		m_tBulletData.vPosition += _float3(0.f, 0.f, m_tBulletData.fOffsetUp);
		break;
	case DIR_STATE::DIR_DOWN:
		m_tBulletData.vPosition -= _float3(0.f, 0.f, m_tBulletData.fOffsetDown);
		break;
	}

	TransformDesc.InitPos = m_tBulletData.vPosition;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Init_Data();

	//SetUp_DebugComponents();

	return S_OK;
}

HRESULT CBullet::SetUp_DebugComponents(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_DebugBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIDebugBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}

HRESULT CBullet::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	return S_OK;
}

void CBullet::Excute(_float fTimeDelta)
{
	//Monster
	if (!m_tBulletData.bIsPlayerBullet)
	{
		switch (m_tBulletData.eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_HAND:
			break;
		}
	}
	//Player
	else
	{
		switch (m_tBulletData.eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_HAND:
		case WEAPON_TYPE::WEAPON_SWORD:

			break;
		case WEAPON_TYPE::WEAPON_STAFF:
			switch (m_tBulletData.eDirState)
			{
			case DIR_STATE::DIR_UP:
				m_pTransformCom->Go_Straight(fTimeDelta);
				break;
			case DIR_STATE::DIR_DOWN:
				m_pTransformCom->Go_Backward(fTimeDelta);
				break;
			case DIR_STATE::DIR_RIGHT:
				m_pTransformCom->Go_Right(fTimeDelta);
				break;
			case DIR_STATE::DIR_LEFT:
				m_pTransformCom->Go_Right(fTimeDelta);
				break;
			case DIR_STATE::DIR_END:
				m_pTransformCom->Go_PosDir(fTimeDelta, m_tBulletData.vLook);
				//m_pTransformCom->Go_Straight(fTimeDelta);
				break;
			default:
				break;
			}
			break;
		case WEAPON_TYPE::WEAPON_DART:
			switch (m_tBulletData.eDirState)
			{
			case DIR_STATE::DIR_UP:
				m_pTransformCom->Go_Straight(fTimeDelta);
				break;
			case DIR_STATE::DIR_DOWN:
				m_pTransformCom->Go_Backward(fTimeDelta);
				break;
			case DIR_STATE::DIR_RIGHT:
				m_pTransformCom->Go_Right(fTimeDelta);
				break;
			case DIR_STATE::DIR_LEFT:
				m_pTransformCom->Go_Right(fTimeDelta);
				break;
			case DIR_STATE::DIR_END:
				m_pTransformCom->Go_PosDir(fTimeDelta, m_tBulletData.vLook);
				break;
			default:
				break;
			}
			break;
		case WEAPON_TYPE::WEAPON_BOMB:
			Bomb();
			break;
		case WEAPON_TYPE::WEAPON_SMOKE:
			Red_Smoke(fTimeDelta);
			break;
		case WEAPON_TYPE::WEAPON_LIGHTNING:
			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE1:
		case WEAPON_TYPE::WEAPON_ICESPIKE2:
		case WEAPON_TYPE::WEAPON_ICESPIKE3:
		case WEAPON_TYPE::WEAPON_ICESPIKE4:
			IceSpikes(fTimeDelta);
			break;
		case WEAPON_TYPE::WEAPON_MINE:
			IceMine(fTimeDelta);
			break;
		case WEAPON_TYPE::WEAPON_MINES:
			IceMines(fTimeDelta);
			break;
		case WEAPON_TYPE::WEAPON_ICESMOKE:
			Ice_Smoke(fTimeDelta);
			break;
		case WEAPON_TYPE::WEAPON_ROCK:
			Rock(fTimeDelta);
			break;
		case WEAPON_TYPE::CARNIVAL_ARROW:
			if (!m_bShoot)
				Carnival_Arrow(fTimeDelta);
			else
				Shoot_Carnival_Arrow(fTimeDelta);
			break;
		}
	}
}

void CBullet::AttackCheck(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	vector<CGameObject*> vecDamagedActor;

	if (pGameInstance->Collision_Check_Group_Multi(m_tBulletData.bIsPlayerBullet ? CCollider_Manager::COLLISION_MONSTER : CCollider_Manager::COLLISION_PLAYER, vecDamagedActor, this, CCollider_Manager::COLLSIION_BOX))
	{
		switch (m_tBulletData.eWeaponType)
		{
		case WEAPON_TYPE::CARNIVAL_ARROW:
		case WEAPON_TYPE::WEAPON_HAND:
		case WEAPON_TYPE::WEAPON_SWORD:
			m_bDead = OBJ_DEAD;
			goto AttackMulti;
			break;
		case WEAPON_TYPE::WEAPON_STAFF:
			m_bDead = OBJ_DEAD;
			goto AttackMulti;
			break;
		case WEAPON_TYPE::WEAPON_DART:
			m_bDead = OBJ_DEAD;
			goto AttackMulti;
			break;
		case WEAPON_TYPE::WEAPON_SMOKE:
			if (m_bIsAttacked)
			{
				m_bIsAttacked = false;
				m_fDamage = 30.f;
				goto AttackMulti;
			}
			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE1:
			m_fDamage = 1.f;
			goto AttackMulti;
			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE2:
			m_fDamage = 1.f;
			goto AttackMulti;
			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE3:
			m_bIsAttacked = false;
			m_fDamage = 1.f;
			goto AttackMulti;
			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE4:
			m_bIsAttacked = false;
			m_fDamage = 1.f;
			goto AttackMulti;
			break;
		case WEAPON_TYPE::WEAPON_MINE:
			if (!m_bIsAttacked)
			{
				m_bIsAttacked = true; // �����ѹ�����
				m_bActivated = true; // 
				m_fDamage = 10.f;
				goto AttackMulti;
			}
			break;
		case WEAPON_TYPE::WEAPON_ICESMOKE:
			if (m_bIsAttacked)
			{
				m_fDamage = 0.6f;
				goto AttackMulti;
			}
			break;
		case WEAPON_TYPE::WEAPON_ROCK:
			if (m_bIsAttacked)
			{
				m_fDamage = 30.f;
				goto AttackMulti;
			}
			break;

		case WEAPON_TYPE::BEARGER_SPECIAL:
		case WEAPON_TYPE::BOARRIOR_SPECIAL:
			m_bDead = OBJ_DEAD;
			goto AttackMulti;
			break;
		}
		return;

	AttackMulti:
		CGameInstance::Apply_Damage_Multi(m_fDamage, vecDamagedActor, this, nullptr);
	}
	vecDamagedActor.clear();

	// Collision with Object (if Bearger Special Attack)
	if (m_tBulletData.eWeaponType == WEAPON_TYPE::BEARGER_SPECIAL)
	{
		if (pGameInstance->Collision_Check_Group_Multi(CCollider_Manager::COLLISION_OBJECT, vecDamagedActor, this, CCollider_Manager::COLLSIION_BOX))
		{
			for (auto& iter = vecDamagedActor.begin(); iter != vecDamagedActor.end(); ++iter)
			{
				CInteractive_Object* pInteractiveObj = dynamic_cast<CInteractive_Object*>((*iter));
				if (pInteractiveObj)
				{
					if (!pInteractiveObj->Get_Destroyed())
					{
						pInteractiveObj->Drop_Items();
						pInteractiveObj->Destroy();
						m_bDead = OBJ_DEAD;
					}
				}
			}
		}
	}
	vecDamagedActor.clear();

}

void CBullet::DeadCheck(_float _fTimeDelta)
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	m_fAccDeadTimer += _fTimeDelta;

	switch (m_tBulletData.eWeaponType)
	{
	case WEAPON_TYPE::WEAPON_HAND:
	case WEAPON_TYPE::WEAPON_SWORD:
		m_fAccDeadTimer += _fTimeDelta;
		if (m_fAccDeadTimer > 1.f)
		{
			m_bDead = OBJ_DEAD;
		}
		break;
	case WEAPON_TYPE::WEAPON_STAFF:
		break;
	case WEAPON_TYPE::WEAPON_DART:
		break;
	case WEAPON_TYPE::WEAPON_BOMB:
		if (Compare_Terrain())
		{
			BULLETDATA BulletData;
			ZeroMemory(&BulletData, sizeof(BulletData));
			BulletData.bIsPlayerBullet = true;
			BulletData.eDirState = DIR_STATE::DIR_DOWN;
			BulletData.eWeaponType = WEAPON_TYPE::WEAPON_SMOKE;
			BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_UP);
			BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			CGameInstance*pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;
			Safe_Release(pGameInstance);
			m_bDead = true;

		}
		break;

	case WEAPON_TYPE::WEAPON_SMOKE:
		/*if (m_fAccDeadTimer > 3.f && !m_bActivated)
		{
			m_bActivated = true;

		}*/
		if (m_pTextureCom->Get_Frame().m_iCurrentTex >= 10)
		{
			m_bDead = OBJ_DEAD;
		}
		break;
	case WEAPON_TYPE::WEAPON_LIGHTNING:
	case WEAPON_TYPE::WEAPON_ICESPIKE1:
	case WEAPON_TYPE::WEAPON_ICESPIKE2:
	case WEAPON_TYPE::WEAPON_ICESPIKE3:
	case WEAPON_TYPE::WEAPON_ICESPIKE4:
		if ((m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1))
		{
			m_bDead = OBJ_DEAD;
		}
		break;

	case WEAPON_TYPE::WEAPON_MINE:
		if ((m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1 && m_bActivated3)
			|| m_fAccDeadTimer > 3.f)
		{
			m_bDead = OBJ_DEAD;
		}
		break;

	case WEAPON_TYPE::WEAPON_MINES:
		if (m_bActivated3)
			m_bDead = OBJ_DEAD;
		break;

	case WEAPON_TYPE::WEAPON_PUFF:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 2)
			m_bDead = OBJ_DEAD;
		break;

	case WEAPON_TYPE::BEARGER_SPECIAL:
	case WEAPON_TYPE::BOARRIOR_SPECIAL:
	case WEAPON_TYPE::WEAPON_ICESMOKE:
		if ((m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1))
			m_bDead = true;
		break;
	}
}

void CBullet::Red_Smoke(_float _fTimeDelta)
{
	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 7)
	{
		m_bIsAttacked = true;
	}
}

void CBullet::Bomb(void)
{	//y = time* time -power*time ;



	m_fTime += 0.1f;//m_tBulletData.fAdd_Z;
					//if (m_fTime >= m_MaxTime) // �ð����� ũ��??	
	m_MaxTime = 2.f * 8.2f;

	float fSpeed = (-0.3f *m_fTime*m_fTime + 0.3f);//m_tBulletData.fAdd_Z); // ((3.f / 2.f)*m_fPower *0.03f); //0.1f ����
	m_tBulletData.vPosition.y += fSpeed;
	m_tBulletData.vPosition.x += m_tBulletData.vTargetPos.x / m_MaxTime;
	m_tBulletData.vPosition.z += m_tBulletData.vTargetPos.z / m_MaxTime;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tBulletData.vPosition);

	//ȸ��
	//m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), fTimeDelta);

}

void CBullet::IceSpikes(_float _fTimeDelta)
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	_float fRemainTime = 3.f;

	m_fTime += _fTimeDelta;

	_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float3 vTempLook = m_tBulletData.vLook;

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vTempLook, &vTempLook);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	switch (m_tBulletData.eDirState)
	{

	case DIR_STATE::DIR_END:
		switch (m_tBulletData.eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_ICESPIKE1:
			if (m_fTime < fRemainTime && m_pTextureCom->Get_Frame().m_iCurrentTex == 14)
			{
				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}
			else if (m_fTime > fRemainTime && !m_bActivated2)
			{
				m_bActivated2 = true;
				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}

			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE2:
			if (m_fTime < fRemainTime && m_pTextureCom->Get_Frame().m_iCurrentTex == 14)
			{
				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}
			else if (m_fTime > fRemainTime && !m_bActivated2)
			{
				m_bActivated2 = true;
				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}

			if (!m_bActivated &&m_fTime > 0.2f)
			{
				m_bActivated = true;

				BULLETDATA BulletData;
				ZeroMemory(&BulletData, sizeof(BulletData));
				BulletData.bIsPlayerBullet = true;
				BulletData.eDirState = m_tBulletData.eDirState;
				BulletData.eWeaponType = WEAPON_TYPE::WEAPON_ICESPIKE1;
				//BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				BulletData.vLook = m_tBulletData.vLook;//m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				BulletData.vPosition += vTempLook*m_fAttackRange;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE3:
			if (m_fTime < fRemainTime && m_pTextureCom->Get_Frame().m_iCurrentTex == 14)
			{
				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}
			else if (m_fTime > fRemainTime && !m_bActivated2)
			{
				m_bActivated2 = true;
				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}

			if (!m_bActivated &&m_fTime > 0.2f)
			{
				m_bActivated = true;

				BULLETDATA BulletData;
				ZeroMemory(&BulletData, sizeof(BulletData));
				BulletData.bIsPlayerBullet = true;
				BulletData.eDirState = m_tBulletData.eDirState;
				BulletData.eWeaponType = WEAPON_TYPE::WEAPON_ICESPIKE2;
				//BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				BulletData.vLook = m_tBulletData.vLook;//m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				BulletData.vPosition += vTempLook*m_fAttackRange;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		case WEAPON_TYPE::WEAPON_ICESPIKE4:
			if (m_fTime < fRemainTime && m_pTextureCom->Get_Frame().m_iCurrentTex == 14)
			{

				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}
			else if (m_fTime > fRemainTime && !m_bActivated2)
			{
				m_bActivated2 = true;
				m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
			}

			if (!m_bActivated &&m_fTime > 0.2f)
			{
				m_bActivated = true;

				BULLETDATA BulletData;
				ZeroMemory(&BulletData, sizeof(BulletData));
				BulletData.bIsPlayerBullet = true;
				BulletData.eDirState = m_tBulletData.eDirState;
				BulletData.eWeaponType = WEAPON_TYPE::WEAPON_ICESPIKE3;
				//BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				BulletData.vLook = m_tBulletData.vLook;//m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				BulletData.vPosition += vTempLook*m_fAttackRange;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		}
		break;
	default:
		break;
	}

	Safe_Release(pGameInstance);
}

void CBullet::IceMine(_float _fTimeDelta)
{
	if (!m_bActivated)
	{
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 14)
		{
			m_pTextureCom->Get_Frame().m_iCurrentTex = 13;
		}
	}
	else {
		if (!m_bActivated2)
		{
			m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

			m_bActivated2 = true;

			Change_Texture(TEXT("Prototype_Component_Texture_SandSpike_Tall_Create"));

			if (!m_bActivated3)
			{
				m_bActivated3 = true;
				Change_Texture(TEXT("Prototype_Component_Texture_SandSpike_Tall_Break"));
			}
		}
	}
}

void CBullet::IceMines(_float _fTimeDelta)
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	m_fTime += _fTimeDelta;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	BULLETDATA BulletData;
	ZeroMemory(&BulletData, sizeof(BulletData));
	BulletData.bIsPlayerBullet = true;
	BulletData.eDirState = m_tBulletData.eDirState;
	BulletData.eWeaponType = WEAPON_TYPE::WEAPON_MINE;
	BulletData.vLook = m_tBulletData.vLook;

	_float3 vPositiontemp = m_tBulletData.vPosition;

	//BulletData.vPosition = m_vTargetPicking;
	//for FPS Mode
	_float3 vLooktemp = m_tBulletData.vLook;
	D3DXVec3Normalize(&vLooktemp, &vLooktemp);
	_float3 vPos = m_tBulletData.vPosition;
	_float3 vRightTemp = m_tBulletData.vRight; // = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	D3DXVec3Normalize(&vRightTemp, &vRightTemp);
	switch (m_tBulletData.eDirState)
	{
	case DIR_STATE::DIR_UP:
		if (!m_bActivated)
		{
			vPos += vLooktemp * 2.f;
			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated = true;
		}

		if (m_fTime > 0.3f && !m_bActivated2)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vLooktemp * 3.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated2 = true;
		}
		if (m_fTime > 0.5f && !m_bActivated3)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vLooktemp * 4.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated3 = true;
		}
		break;
	case DIR_STATE::DIR_DOWN:
		if (!m_bActivated)
		{
			vPos -= vLooktemp * 2.f;
			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated = true;
		}

		if (m_fTime > 0.3f && !m_bActivated2)
		{
			vPos = m_tBulletData.vPosition;

			vPos -= vLooktemp * 3.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated2 = true;
		}
		if (m_fTime > 0.5f && !m_bActivated3)
		{
			vPos = m_tBulletData.vPosition;

			vPos -= vLooktemp * 4.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated3 = true;
		}
		break;
	case DIR_STATE::DIR_RIGHT:
		if (!m_bActivated)
		{
			vPos = m_tBulletData.vPosition;
			vPos += vRightTemp * 2.f;
			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated = true;
		}

		if (m_fTime > 0.3f && !m_bActivated2)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vRightTemp * 3.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated2 = true;
		}
		if (m_fTime > 0.5f && !m_bActivated3)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vRightTemp * 4.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated3 = true;
		}
		break;
	case DIR_STATE::DIR_LEFT:
		if (!m_bActivated)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vRightTemp * 2.f;
			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated = true;
		}

		if (m_fTime > 0.3f && !m_bActivated2)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vRightTemp * 3.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated2 = true;
		}
		if (m_fTime > 0.5f && !m_bActivated3)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vRightTemp * 4.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated3 = true;
		}
		break;
	case DIR_STATE::DIR_END:
		if (!m_bActivated)
		{
			vPos += vLooktemp * 4.f;
			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated = true;
		}

		if (m_fTime > 0.3f && !m_bActivated2)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vLooktemp * 5.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated2 = true;
		}
		if (m_fTime > 0.5f && !m_bActivated3)
		{
			vPos = m_tBulletData.vPosition;

			vPos += vLooktemp * 6.f;

			BulletData.vPosition = vPos;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
				return;

			m_bActivated3 = true;
		}
		break;
	}


	Safe_Release(pGameInstance);
}

void CBullet::Puff(_float _fTimeDelta)
{


}

void CBullet::Ice_Smoke(_float _fTimeDelta)
{
	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= 2
		&& m_pTextureCom->Get_Frame().m_iCurrentTex <= 24)
	{
		m_bIsAttacked = true;
	}
	else
	{
		m_bIsAttacked = false;
	}
}

void CBullet::Ice_Blast(_float _fTimeDelta)
{
	//m_fTime 시간 봐보기

	if (!m_bActivated
		&& m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bActivated = true;
		Change_Texture(TEXT("Com_Texture_IceBlast"));

	}

	if (m_bActivated &&m_pTextureCom->Get_Frame().m_iCurrentTex == 2
		/*&& m_pTextureCom->Get_Frame().m_iCurrentTex <= 6*/)
	{
		m_bIsAttacked = true;
	}
	else
	{
		m_bIsAttacked = false;
	}

}

void CBullet::Rock(_float _fTimeDelta)
{
	if (!m_bActivated&&Compare_Terrain())
	{
		m_bActivated = true;
		
		m_pTextureCom->Get_Frame().m_iCurrentTex = 0;
	}
	else if(!m_bActivated&&!Compare_Terrain())
	{
		m_fTime += 0.1f;//m_tBulletData.fAdd_Z;
						//if (m_fTime >= m_MaxTime) // �ð����� ũ��??	
		m_MaxTime = 2.f * 8.2f;

		float fSpeed = (-0.3f *m_fTime*m_fTime + 0.3f);//m_tBulletData.fAdd_Z); // ((3.f / 2.f)*m_fPower *0.03f); //0.1f ����
		m_tBulletData.vPosition.y += fSpeed;
		m_tBulletData.vPosition.x += m_tBulletData.vTargetPos.x / m_MaxTime;
		m_tBulletData.vPosition.z += m_tBulletData.vTargetPos.z / m_MaxTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tBulletData.vPosition);
	
		m_pTextureCom->Get_Frame().m_iCurrentTex = m_pTextureCom->Get_Frame().m_iEndTex;
	}
	//Attack
	if (m_bActivated2)
	{
		m_bIsAttacked = false;
	}

	if (!m_bActivated2 &&m_bActivated &&m_pTextureCom->Get_Frame().m_iCurrentTex ==1)
	{
		m_bIsAttacked = true;
		m_bActivated2 = true;
	}
	
	

	//Dead
	if (m_bActivated &&m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bDead = OBJ_DEAD;
	}
}

void CBullet::Carnival_Arrow(_float fTimeDelta)
{
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_MAZE, TEXT("Layer_Shooter"));
	_float3 vCenterPos = pGameObject->Get_Position();
	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	if (dynamic_cast<CCarnival_Shooter*>(pGameObject)->Get_bShoot() && !m_bShoot)
	{
		m_bShoot = true;
		
		_float3 vDir =  vPosition - vCenterPos;
		D3DXVec3Normalize(&vDir, &vDir);
		m_vShootDir = vDir;
		dynamic_cast<CCarnival_Shooter*>(pGameObject)->Set_bShoot(false);
		return;
	}
		

	_float vDistance = 0.5f;

	m_fAngle++;
	if (m_fAngle >= 360)
		m_fAngle = 0;

	vPosition.x = vCenterPos.x + cosf(D3DXToRadian(m_fAngle))*vDistance - sin(D3DXToRadian(m_fAngle))*vDistance;
	vPosition.z = vCenterPos.z + sin(D3DXToRadian(m_fAngle))*vDistance + cos(D3DXToRadian(m_fAngle))*vDistance;

	_float fDegree =  D3DXVec3Dot(&vCenterPos, &vPosition);
	acos(fDegree);

	if (fDegree > 180)
		fDegree = 360 - fDegree;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fDegree);
}

void CBullet::Shoot_Carnival_Arrow(_float fTimeDelta)
{
	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition += m_vShootDir;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}


HRESULT CBullet::Render_TextureState()
{
	switch (m_tBulletData.eWeaponType)
	{
	case WEAPON_TYPE::WEAPON_HAND:
	case WEAPON_TYPE::WEAPON_SWORD:
		m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
		break;
	case WEAPON_TYPE::WEAPON_STAFF:
		m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex));
		break;
	case WEAPON_TYPE::WEAPON_DART:
		switch (m_tBulletData.eDirState)
		{
		case DIR_STATE::DIR_UP:
			m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(1));
			break;
		case DIR_STATE::DIR_DOWN:
			m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
			break;
		case DIR_STATE::DIR_RIGHT:
		case DIR_STATE::DIR_LEFT:
			m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(2));
			break;
		case DIR_STATE::DIR_END:
			m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(1));
			break;
		default:
			m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex));
			break;
		}
	}
	if (m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_DART
		&& m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_SWORD
		&& m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_HAND)
	{
		m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex));
	}

	return S_OK;
}

HRESULT CBullet::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

//void CBullet::Apply_Damage(_float Damage, CGameObject * DamagedObj, void * AttackType)
//{
//	DamagedObj->Take_Damage(Damage, nullptr, this);
//}
//
//void CBullet::Apply_Damage_Multi(_float fDamage, vector<CGameObject*>& vecDamagedObj, void * AttackType)
//{
//	Damge_Type ���� �ֱ�.
//
//	for (auto& iter = vecDamagedObj.begin(); iter != vecDamagedObj.end();)
//	{
//		(*iter)->Take_Damage(fDamage, nullptr, this);
//		iter++;
//	}
//
//}

_bool CBullet::Compare_Terrain(void)
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return false;

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return false;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return false;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	switch (m_tBulletData.eWeaponType)
	{
	case WEAPON_TYPE::WEAPON_ROCK:
	case WEAPON_TYPE::WEAPON_BOMB:
		vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.5f);

		if (vPosition.y > m_pTransformCom->Get_State(CTransform::STATE_POSITION).y)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	case WEAPON_TYPE::WEAPON_DART:
	case WEAPON_TYPE::WEAPON_STAFF:
		break;
	case WEAPON_TYPE::WEAPON_HAND:
		break;
	case WEAPON_TYPE::WEAPON_SWORD:
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE1:
	case WEAPON_TYPE::WEAPON_ICESPIKE2:
	case WEAPON_TYPE::WEAPON_ICESPIKE3:
	case WEAPON_TYPE::WEAPON_ICESPIKE4:
	case WEAPON_TYPE::WEAPON_MINE:
	case WEAPON_TYPE::BEARGER_SPECIAL:
	case WEAPON_TYPE::BOARRIOR_SPECIAL:
	case WEAPON_TYPE::WEAPON_ICESMOKE:
		_float3 vScale = m_pTransformCom->Get_Scale();
		vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (vScale.y*0.5f));

		_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vMyPos.y = vPosition.y;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMyPos);
		break;
	}

	return _bool();
}

void CBullet::SetUp_BillBoard()
{

	if (m_tBulletData.eDirState == DIR_STATE::DIR_END
		&& m_tBulletData.eWeaponType == WEAPON_TYPE::WEAPON_MINE)
		return;

	if (m_tBulletData.eWeaponType == WEAPON_TYPE::CARNIVAL_ARROW)
		return;

	_float4x4		ViewMatrix;

	/*if (m_tBulletData.eDirState == DIR_STATE::DIR_LEFT)
		return;*/

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	if (m_tBulletData.eDirState == DIR_STATE::DIR_LEFT)
	{
		if (m_tBulletData.eWeaponType == WEAPON_DART)
		{
			m_pTransformCom->Set_Scale(-0.3f, 0.3f, 1.f);
		}
		else
		{
			m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
		}
	}
}

HRESULT CBullet::Texture_Clone(void)
{
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	switch (m_tBulletData.eWeaponType)
	{
	case WEAPON_TYPE::WEAPON_HAND:
#ifdef _DEBUG
		/*Debug*/
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 1;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Debug"), LEVEL_STATIC, TEXT("Prototype_Component_DebugLine"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
#endif
		break;
	case WEAPON_TYPE::WEAPON_SWORD:
#ifdef _DEBUG
		/*Debug*/
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 1;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Debug"), LEVEL_STATIC, TEXT("Prototype_Component_DebugLine"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
#endif
		break;
	case WEAPON_TYPE::WEAPON_STAFF:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 9;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Fireball"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fireball"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_DART:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 2;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dart"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dart"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_BOMB:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 1;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_BathBomb"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BathBomb"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_SMOKE:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 11;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_LaserHit"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Laser_Hit1"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_LIGHTNING:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 9;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Lightning"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE1:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 27;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IceSpike1"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike1"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE2:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 27;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IceSpike2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike2"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE3:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 27;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IceSpike3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike3"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE4:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 25;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IceSpike4"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_IceSpike4"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_MINE:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 16;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Texture_SandSpike_Small_Create"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Small_Create"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 15;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Texture_SandSpike_Small_Break"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Small_Break"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 8;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Texture_SandSpike_Small_PreBreak"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Small_PreBreak"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 16;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Texture_SandSpike_Tall_Create"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Tall_Create"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 15;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Texture_SandSpike_Tall_Break"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SandSpike_Tall_Break"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		break;
	case WEAPON_TYPE::BEARGER_SPECIAL:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 34;
		TextureDesc.m_fSpeed = 30;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Bearger_Special"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Rocks"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		break;

	case WEAPON_TYPE::BOARRIOR_SPECIAL:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 31;
		TextureDesc.m_fSpeed = 20;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Boarrior_Special"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Eruption"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		break;

	case WEAPON_TYPE::WEAPON_PUFF:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 16;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Small_Puff"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Small_Puff"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		break;
	case WEAPON_TYPE::WEAPON_ICESMOKE:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 26;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IceSmoke"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ice_Smoke"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

	case WEAPON_TYPE::WEAPON_ROCK:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 28;
		TextureDesc.m_fSpeed = 20;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Rock"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Catapult_Projectile_Impact"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;

	case WEAPON_TYPE::CARNIVAL_ARROW:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 0;
		TextureDesc.m_fSpeed = 20;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Carnival"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Carnival_Arrow"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	}


	return S_OK;
}

HRESULT CBullet::Init_Data(void)
{
	//refactory Func soon
	switch (m_tBulletData.eWeaponType)
	{
	case WEAPON_TYPE::WEAPON_DART:
		m_pTransformCom->Set_Scale(0.3f, 0.3f, 1.f);
		if (m_tBulletData.eDirState == DIR_STATE::DIR_LEFT)
		{
			m_pTransformCom->Set_Scale(-0.3f, 0.3f, 1.f);
		}
		break;
	case WEAPON_TYPE::WEAPON_SMOKE:
		m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
		m_fDamage = 3.f;
		break;
	case WEAPON_TYPE::WEAPON_BOMB:
		m_pTransformCom->Set_Scale(0.7f, 0.7f, 1.f);
		m_fDamage = 0.f;
		break;
	case WEAPON_TYPE::WEAPON_LIGHTNING:
		m_pTransformCom->Set_Scale(1.f, 7.f, 1.f);
		m_fDamage = 0.f;
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE1:
		m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
		m_fDamage = 0.f;
		Compare_Terrain();
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE2:
		m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.f);
		m_fDamage = 0.f;
		Compare_Terrain();
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE3:
		m_pTransformCom->Set_Scale(0.9f, 0.9f, 1.f);
		m_fDamage = 0.f;
		Compare_Terrain();
		break;
	case WEAPON_TYPE::WEAPON_ICESPIKE4:
		m_pTransformCom->Set_Scale(0.6f, 0.6f, 1.f);
		m_fDamage = 0.f;
		Compare_Terrain();
		break;
	case WEAPON_TYPE::WEAPON_MINE:
		m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
		Change_Texture(TEXT("Com_Texture_Texture_SandSpike_Small_Create"));
		m_fDamage = 10.f;
		Compare_Terrain();
		break;

	case WEAPON_TYPE::WEAPON_ICESMOKE:
		m_pTransformCom->Set_Scale(1.f, 2.f, 1.f);
		m_fDamage = 20.f;
		Compare_Terrain();
		break;

	case WEAPON_TYPE::WEAPON_ROCK:
		m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
		//m_pTransformCom->Set_Scale(m_tBulletData.vScale.x, m_tBulletData.vScale.y, m_tBulletData.vScale.z);
		m_fDamage = 10.f;
		Compare_Terrain();
		break;
	case WEAPON_TYPE::BOARRIOR_SPECIAL:
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
		break;
	case WEAPON_TYPE::CARNIVAL_ARROW:
		m_pTransformCom->Set_Scale(0.2f, 0.5f, 1.f);
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 90.f);
		break;
	default:
		break;

	}


	if (m_tBulletData.eDirState == DIR_STATE::DIR_END)
	{
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.0f);

		//m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_tBulletData.vLook);
	}

	return S_OK;
}

CBullet * CBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBullet*	pInstance = new CBullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBullet::Clone(void * pArg)
{
	CBullet*	pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIDebugBufferCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);


	m_vecTexture.clear();
}
