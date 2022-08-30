#include "stdafx.h"
#include "..\Public\Bullet.h"
#include "GameInstance.h"
#include "Transform.h"


CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CBullet::CBullet(const CBullet & rhs)
	:CGameObject(rhs)
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

	AttackCheck();

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

	if(m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_HAND && m_tBulletData.eWeaponType != WEAPON_TYPE::WEAPON_SWORD)
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
	또한 화살 불마법은 지속적인 위치 업데이트가 필요하다.
	*/
	

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
	}

}

void CBullet::AttackCheck()
{
	if (m_pColliderCom->Collision_with_Group(CCollider::COLLISION_MONSTER, this))
	{
		switch (m_tBulletData.eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_HAND:
		case WEAPON_TYPE::WEAPON_SWORD:
			m_bDead = OBJ_DEAD;
			break;
		case WEAPON_TYPE::WEAPON_STAFF:
			m_bDead = OBJ_DEAD;
			break;
		case WEAPON_TYPE::WEAPON_DART:
			m_bDead = OBJ_DEAD;
			break;
		}
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
	}

	return S_OK;
}

HRESULT CBullet::Init_Data(void)
{

	if (m_tBulletData.eWeaponType == WEAPON_TYPE::WEAPON_DART)
	{
		if (m_tBulletData.eDirState == DIR_STATE::DIR_LEFT)
		{
			m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
		}
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
