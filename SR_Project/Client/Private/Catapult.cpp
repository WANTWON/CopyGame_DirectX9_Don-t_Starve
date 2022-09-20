#include "stdafx.h"
#include "..\Public\Catapult.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Bullet.h"
#include "Winona.h"
CCatapult::CCatapult(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CPawn(pGraphic_Device)
{
}

CCatapult::CCatapult(const CCatapult & rhs)
	: CPawn(rhs)
{
}

HRESULT CCatapult::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCatapult::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&m_tDesc, sizeof(m_tDesc));
	m_tDesc = *(CATAPULTDESC*)pArg;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	static_cast<CWinona*>(m_tDesc.pOwner)->Add_Catapult(this);
	Safe_AddRef(this);

	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
	m_tInfo.iMaxHp = 100.f;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

	Change_Texture(TEXT("Com_Texture_Place"));

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	//받아올 것 좌표. 주인 포인터.

	return S_OK;
}

int CCatapult::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	if (m_bDead)
		return OBJ_DEAD;

	Update_Delay(fTimeDelta);

	Behavior(fTimeDelta);

	WalkingTerrain();
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_GROUP::COLLISION_MONSTER, this);

	return OBJ_NOEVENT;
}

void CCatapult::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	m_pTextureCom->MoveFrame(m_TimerTag);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
}

HRESULT CCatapult::Render()
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

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex));

	m_pShaderCom->Begin(m_eShaderID);

	m_pVIBufferCom->Render();
	m_pShaderCom->End();

#ifdef _DEBUG
	if (g_ColliderRender && m_pColliderCom != nullptr)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CCatapult::SetUp_Components(void * pArg)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

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
	TransformDesc.InitPos = m_tDesc.vInitPos;
	//TransformDesc.InitPos = _float3(45.f, 0.5f, 27.f); // Test
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusX = 0.2f;
	CollRectDesc.fRadiusY = 0.3f;
	CollRectDesc.fRadiusZ = 0.1f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.0f;
	CollRectDesc.fOffsetZ = 0.f;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	return S_OK;
}

void CCatapult::SetUp_BillBoard()
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

void CCatapult::WalkingTerrain()
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
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y * .5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CCatapult::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CCatapult::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 40.f;

	/*Place*/
	TextureDesc.m_iEndTex = 44;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Place"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Catapult_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	/*Idle*/
	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Catapult_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	/*Attack*/
	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Catapult_Attack"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	/*Destroy*/
	TextureDesc.m_iEndTex = 44;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Destroy"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Catapult_Death"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCatapult::Update_Delay(_float _fTimeDelta)
{
	if (!m_bDestroyed)
	{
		m_fLifeTime -= _fTimeDelta;
		if (m_fLifeTime <= 0.f)
		{
			m_bDestroyed = true;
			m_eState = DESTROY;
		}
	}
	

	if (!m_bCanAttack)
	{
		m_fAttackDelay -= _fTimeDelta;
		if (m_fAttackDelay <= 0.f)
		{
			m_fAttackDelay = m_fAttackMaxDelay;
			m_bCanAttack = true;
		}
	}

}

void CCatapult::Behavior(_float _fTimeDelta)
{
	switch (m_eState)
	{
	case STATE::PLACE:
		Place(_fTimeDelta);
		break;
	case STATE::IDLE:
		Idle(_fTimeDelta);
		break;

	case STATE::ATTACK:
		Attack(_fTimeDelta);
		break;

	case STATE::DESTROY:
		Destroy(_fTimeDelta);
		break;
	}
}

void CCatapult::Attack(_float _fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		Change_Texture(TEXT("Com_Texture_Attack"));
		m_ePreState = m_eState;
		m_fSavePos = m_pTarget->Get_Position();
	}

	if (!Check_Distance(m_pTarget))
	{
		m_pTarget == nullptr;
		m_eState = CCatapult::IDLE;
	}

	

	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 32)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		BulletData.bIsPlayerBullet = true;
		BulletData.eDirState = DIR_STATE::DIR_DOWN;
		BulletData.eWeaponType = WEAPON_TYPE::WEAPON_ROCK;
		BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_UP);
		
		BulletData.vPosition = Get_Position();
		BulletData.vPosition.y += 0.3f;
		_float3 temp = { m_fSavePos.x - Get_Position().x, 0.f, m_fSavePos.z - Get_Position().z };
		BulletData.vTargetPos = temp;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
			return;
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_eState = IDLE;
		m_bCanAttack = false;
	}
}

void CCatapult::Idle(_float _fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		Change_Texture(TEXT("Com_Texture_Idle"));
		m_ePreState = m_eState;
	}

	if (m_bCanAttack&& Find_Enemy(_fTimeDelta))
	{
		m_eState = ATTACK;
	}

}

void CCatapult::Destroy(_float _fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		Change_Texture(TEXT("Com_Texture_Destroy"));
		m_ePreState = m_eState;
		static_cast<CWinona*>(m_tDesc.pOwner)->Decreas_Catapult(this);
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bDead = true;
	}
}

void CCatapult::Place(_float _fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		Change_Texture(TEXT("Com_Texture_Place"));
		m_ePreState = m_eState;
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bDestroyed = false;
		m_eState = IDLE;
	}

}

_bool CCatapult::Find_Enemy(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Monster"));

	if (list_Obj == nullptr)
		return false;

	_uint iIndex = 0;

	m_pTarget = nullptr;
	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if ((*iter_Obj) != nullptr && dynamic_cast<CMonster*>(*iter_Obj)->Get_Aggro())
		{			
			if (!Check_Distance(*iter_Obj))
			{
				++iIndex;
				iter_Obj++;
				continue;
			}
			else
			{
				m_pTarget = *iter_Obj;
				return true;
			}
		}
		else
		{
			++iIndex;
			iter_Obj++;
			continue;
		}
	}

	Safe_Release(pGameInstance);

	return false;
}

_bool CCatapult::Check_Distance(CGameObject* _pObj)
{
	_float fCmpDir = (Get_Position().x - (_pObj)->Get_Position().x)*(Get_Position().x - (_pObj)->Get_Position().x)
		+ (Get_Position().y - (_pObj)->Get_Position().y)*(Get_Position().y - (_pObj)->Get_Position().y)
		+ (Get_Position().z - (_pObj)->Get_Position().z)*(Get_Position().z - (_pObj)->Get_Position().z);

	if (fCmpDir > m_fMaxRange || fCmpDir < m_fMinRange)
	{
		return false;
	}
	else
	{
		return true;
	}
	
}

void CCatapult::Interact(_uint Damage)
{
}

CCatapult * CCatapult::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCatapult* pInstance = new CCatapult(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCatapult"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCatapult::Clone(void * pArg)
{
	CCatapult* pInstance = new CCatapult(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCatapult "));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCatapult::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
