#include "stdafx.h"
#include "..\Public\Bullet.h"
#include "GameInstance.h"
#include "Transform.h"


CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CBullet::CBullet(const CBullet & rhs)
	: CGameObject(rhs)
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
	//무기 이넘값 받기, 생성 위치값 받기.

	m_tBulletData = *(BULLETDATA*)(pArg);

	if (FAILED(SetUp_Components()))
		return E_FAIL;



	return S_OK;
}

int CBullet::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead == true)
		return OBJ_DEAD;

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_OBJECT, this);

	Excute(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return OBJ_NOEVENT;
}

void CBullet::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	AttackCheck(fTimeDelta);

	if (Compare_Terrain())
		m_bDead = true;


	SetUp_BillBoard();
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBullet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	Render_TextureState();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_HAND && m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_SWORD)
		m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;
	m_pTextureCom->MoveFrame(m_TimerTag);
	return S_OK;
}

HRESULT CBullet::SetUp_Components()
{


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
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
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = m_tBulletData.vPosition;
	TransformDesc.InitPos.y += 0.2f;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	//귀찮아 ㅎㅎ
	Init_Data();

	return S_OK;
}

HRESULT CBullet::SetUp_RenderState()
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

HRESULT CBullet::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

void CBullet::Excute(_float fTimeDelta)
{	/*공격이나 칼은 맞는 객체가 몬스터만 있으면되지만
	화살, 불마법은 누가 맞았는지에 따른 처리도 필요,
	또한 화살 불마법은 지속적인 위치 업데이트가 필요하다.*/

	//Monster
	if (!m_tBulletData.bIsPlayerBullet)
	{
		m_fAccDeadTimer += fTimeDelta;
		if (m_fAccDeadTimer > 1.f)
		{
			m_bDead = OBJ_DEAD;
		}
	}
	else //Player
	{
		switch (m_tBulletData.eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_HAND:
		case WEAPON_TYPE::WEAPON_SWORD:
			m_fAccDeadTimer += fTimeDelta;
			if (m_fAccDeadTimer > 1.f)
			{
				m_bDead = OBJ_DEAD;
			}
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
				m_pTransformCom->Go_Left(fTimeDelta);
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
			m_fAccDeadTimer += fTimeDelta;
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex-1 )
			{
				m_bDead = OBJ_DEAD;
			}
			break;

		}
	}
}

void CBullet::AttackCheck(_float _fTimeDelta)
{
	vector<CGameObject*> vecDamagedActor;


	if (m_pColliderCom->Collision_Check_Group_Multi(m_tBulletData.bIsPlayerBullet ? CCollider::COLLISION_MONSTER : CCollider::COLLISION_PLAYER, vecDamagedActor, this))
	{
		switch (m_tBulletData.eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_HAND:
		case WEAPON_TYPE::WEAPON_SWORD:
			m_bDead = OBJ_DEAD;
			goto Attack;
			break;
		case WEAPON_TYPE::WEAPON_STAFF:
			m_bDead = OBJ_DEAD;
			goto Attack;
			break;
		case WEAPON_TYPE::WEAPON_DART:
			m_bDead = OBJ_DEAD;
			goto Attack;
			break;
		case WEAPON_TYPE::WEAPON_SMOKE:
			m_fDamageTimger += _fTimeDelta;
			if (m_fDamageTimger > 0.5f)
			{
				m_fDamage = 0.f;
				goto Attack;
			}
			break;

		}
		Attack:
		Apply_Damage_Multi(m_fDamage, vecDamagedActor, nullptr);
	}


	vecDamagedActor.clear();
}

void CBullet::Red_Smoke(_float _fTimeDelta)
{
	m_fAccDeadTimer += _fTimeDelta;
	if (m_fAccDeadTimer > 3.f)
	{
		if (m_pTextureCom->Get_Frame().m_iCurrentTex >= 14)
		{
			m_bDead = OBJ_DEAD;
		}
		return;
	}
	else {
		if (m_pTextureCom->Get_Frame().m_iCurrentTex >= 7)
		{
			m_pTextureCom->Set_ZeroFrame();
		}
	}

	
}

void CBullet::Bomb(void)
{	//y = time* time -power*time ;
	m_fTime += 0.1f;//m_tBulletData.fAdd_Z;
					//if (m_fTime >= m_MaxTime) // 시간값을 크게??	
	m_MaxTime = 2.f * 8.2f;

	float fSpeed = (-0.3f *m_fTime*m_fTime + 0.3f);//m_tBulletData.fAdd_Z); // ((3.f / 2.f)*m_fPower *0.03f); //0.1f 보간
	m_tBulletData.vPosition.y += fSpeed;
	m_tBulletData.vPosition.x += m_tBulletData.vTargetPos.x / m_MaxTime;
	m_tBulletData.vPosition.z += m_tBulletData.vTargetPos.z / m_MaxTime;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tBulletData.vPosition);

	//회전
	//m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), fTimeDelta);

	if (Compare_Terrain())
	{
		//스모크 생성
		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		BulletData.bIsPlayerBullet = true;
		BulletData.eDirState = DIR_STATE::DIR_DOWN;
		BulletData.eWeaponType = WEAPON_TYPE::WEAPON_SMOKE;
		BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_UP);
		BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		CGameInstance*pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), LEVEL_GAMEPLAY, TEXT("Bullet"), &BulletData)))
			return;
		Safe_Release(pGameInstance);
		m_bDead = true;

	}
}

HRESULT CBullet::Render_TextureState()
{
	switch (m_tBulletData.eWeaponType)
	{
	case WEAPON_TYPE::WEAPON_HAND:
	case WEAPON_TYPE::WEAPON_SWORD:
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_STAFF:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case WEAPON_TYPE::WEAPON_DART:
		switch (m_tBulletData.eDirState)
		{
		case DIR_STATE::DIR_UP:
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
				return E_FAIL;
			break;
		case DIR_STATE::DIR_DOWN:
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
				return E_FAIL;
			break;
		case DIR_STATE::DIR_RIGHT:
		case DIR_STATE::DIR_LEFT:
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(2)))
				return E_FAIL;
			break;
		case DIR_STATE::DIR_END:
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
				return E_FAIL;
			break;
		default:
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
				return E_FAIL;
			break;
		}
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

void CBullet::Apply_Damage(_float Damage, CGameObject * DamagedObj, void * AttackType)
{
	DamagedObj->Take_Damage(Damage, nullptr, this);
}

void CBullet::Apply_Damage_Multi(_float fDamage, vector<CGameObject*>& vecDamagedObj, void * AttackType)
{
	//Damge_Type 추후 넣기.

	for (auto& iter = vecDamagedObj.begin(); iter != vecDamagedObj.end();)
	{
		(*iter)->Take_Damage(fDamage, nullptr, this);
		iter++;
	}

}

_bool CBullet::Compare_Terrain(void)
{

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return false;

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return false;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return false;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	switch (m_tBulletData.eWeaponType)
	{
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

	}

	return _bool();
}

void CBullet::SetUp_BillBoard()
{

	if (m_tBulletData.eDirState == DIR_STATE::DIR_END)
		return;

	_float4x4		ViewMatrix;

	if (m_tBulletData.eDirState == DIR_STATE::DIR_LEFT)
		return;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
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
		TextureDesc.m_iEndTex = 16;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_RedSmoke"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_RedSmoke"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case WEAPON_TYPE::WEAPON_LIGHTNING:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_iEndTex = 9;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Lightning"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
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
		m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.f);
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

CGameObject * CBullet::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);

	//for (auto& iter : m_vecTexture)
	//	Safe_Release(iter);

	//m_vecTexture.clear();
}
