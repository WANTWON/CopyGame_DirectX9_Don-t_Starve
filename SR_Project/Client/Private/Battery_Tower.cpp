#include "stdafx.h"
#include "..\Public\Battery_Tower.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Bullet.h"
#include "Winona.h"
#include "Skill.h"
#include "Battery.h"
CBattery_Tower::CBattery_Tower(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CPawn(pGraphic_Device)
{
}

CBattery_Tower::CBattery_Tower(const CBattery_Tower & rhs)
	: CPawn(rhs)
{
}

HRESULT CBattery_Tower::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattery_Tower::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;

	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
	m_tInfo.iMaxHp = 100.f;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

	Change_Texture(TEXT("Com_Texture_Place"));

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	//받아올 것 좌표. 주인 포인터.

	return S_OK;
}

int CBattery_Tower::Tick(_float fTimeDelta)
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

void CBattery_Tower::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
}

HRESULT CBattery_Tower::Render()
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

	m_pTextureCom->MoveFrame(m_TimerTag);

#ifdef _DEBUG
	if (g_ColliderRender && m_pColliderCom != nullptr)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CBattery_Tower::SetUp_Components(void * pArg)
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
	TransformDesc.InitPos = *(_float3*)pArg;
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

void CBattery_Tower::SetUp_BillBoard()
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

void CBattery_Tower::WalkingTerrain()
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

HRESULT CBattery_Tower::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CBattery_Tower::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 40.f;

	/*Place*/
	TextureDesc.m_iEndTex = 74;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Place"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battery_Tower_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	/*Attack*/
	TextureDesc.m_iEndTex = 49;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battery_Tower_Attack"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CBattery_Tower::Update_Delay(_float _fTimeDelta)
{
	if (!m_bDestroyed)
	{
		m_fLifeTime -= _fTimeDelta;		

		if (m_fLifeTime <= 0.f)
		{
			m_bDestroyed = true;
			m_eState = DESTROY;
			static_cast<CBattery*>(m_pTarget)->Interact(7);
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;
		}
		else {
			if (m_pTarget != nullptr)
			{
				if (m_fLifeTime < (10.f / m_iCountMax) *m_iCount)
				{
					static_cast<CBattery*>(m_pTarget)->Interact(m_iCount--);
				}

			}
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

void CBattery_Tower::Behavior(_float _fTimeDelta)
{

	switch (m_eState)
	{
	case STATE::PLACE:
		Place(_fTimeDelta);
		break;

	case STATE::ATTACK:
		Attack(_fTimeDelta);
		break;

	case STATE::DESTROY:
		Destroy(_fTimeDelta);
		break;
	}
}

void CBattery_Tower::Attack(_float _fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		Change_Texture(TEXT("Com_Texture_Attack"));
		m_ePreState = m_eState;
	}

	Give_Burst(_fTimeDelta);

	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
	{
		Play_Sound(_fTimeDelta);
	}
	if (m_bCanAttack)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		CSkill::SKILL_DESC SkillDesc;

		SkillDesc.eDirState = DIR_END;
		SkillDesc.eSkill = CSkill::SKILL_TYPE::ELEC;
		SkillDesc.vTargetPos = Get_Position();
		SkillDesc.vPosition = Get_Position();
		SkillDesc.vScale = _float3(3.f, 1.f, 3.f);
		SkillDesc.pTarget = nullptr;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skill"), m_iCurrentLevelndex, TEXT("Skill"), &SkillDesc)))
			return;
		m_bCanAttack = false;

		Create_Spark(_fTimeDelta);
	}

}

void CBattery_Tower::Destroy(_float _fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		Play_Sound(_fTimeDelta);
		Change_Texture(TEXT("Com_Texture_Place"));
		m_ePreState = m_eState;
		m_iReversTex = m_pTextureCom->Get_Frame().m_iEndTex - 1;
		m_pTextureCom->Get_Frame().m_iCurrentTex = m_iReversTex--;
	}

	

	if (m_pTextureCom->Get_Frame().m_iCurrentTex <= m_pTextureCom->Get_Frame().m_iStartTex + 1)
	{
		m_bDead = true;
		m_pTextureCom->Get_Frame().m_iCurrentTex = 0;
	}
	else
	{
		m_pTextureCom->Get_Frame().m_iCurrentTex = m_iReversTex--;
	}
}

void CBattery_Tower::Place(_float _fTimeDelta)
{
	if (m_ePreState != m_eState)
	{
		Play_Sound(_fTimeDelta);
		Change_Texture(TEXT("Com_Texture_Place"));
		m_ePreState = m_eState;
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bDestroyed = false;
		m_eState = ATTACK;

		CBattery::BatteryDesc BatteryDesc;
		BatteryDesc.pOwner = this;
		Safe_AddRef(this);
		BatteryDesc.vInitPos = Get_Position();

		_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		D3DXVec3Normalize(&vLook, &vLook);

		BatteryDesc.vInitPos -= vLook * .1f;
		
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Battery"), m_iCurrentLevelndex, TEXT("Layer_Battery"), &BatteryDesc)))
			return;
	}

}

void CBattery_Tower::Create_Spark(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CSkill::SKILL_DESC SkillDesc;

	SkillDesc.eDirState = DIR_END;
	SkillDesc.eSkill = CSkill::SKILL_TYPE::SPARK;
	SkillDesc.vTargetPos = Get_Position();
	SkillDesc.vPosition = Get_Position();
	SkillDesc.vScale = _float3(2.f, 2.f, 1.f);
	SkillDesc.pTarget = nullptr;

	_float3 m_vPos;
	for (_Uint32t i = 0; i < 4; ++i)
	{
		_float fOffsetX = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;
		_float fOffsetZ = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;

		SkillDesc.vPosition = _float3(Get_Position().x + fOffsetX * -1.f, Get_Position().y, Get_Position().z + fOffsetZ * -1.f);
	
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skill"), m_iCurrentLevelndex, TEXT("Skill"), &SkillDesc)))
			return;
	}
	
	for (_Uint32t i = 0; i < 4; ++i)
	{
		_float fOffsetX = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;
		_float fOffsetZ = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;

		SkillDesc.vPosition = _float3(Get_Position().x + fOffsetX * 1.f, Get_Position().y, Get_Position().z + fOffsetZ * 1.f);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skill"), m_iCurrentLevelndex, TEXT("Skill"), &SkillDesc)))
			return;
	}
	
	for (_Uint32t i = 0; i < 4; ++i)
	{
		_float fOffsetX = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;
		_float fOffsetZ = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;

		SkillDesc.vPosition = _float3(Get_Position().x + fOffsetX * -1.f, Get_Position().y, Get_Position().z + fOffsetZ * 1.f);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skill"), m_iCurrentLevelndex, TEXT("Skill"), &SkillDesc)))
			return;
	}

	for (_Uint32t i = 0; i < 4; ++i)
	{
		_float fOffsetX = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;
		_float fOffsetZ = _float(rand() % (int)m_fAttakRange) *1.f + _float(rand() % 10) *0.1f;

		SkillDesc.vPosition = _float3(Get_Position().x + fOffsetX * 1.f, Get_Position().y, Get_Position().z + fOffsetZ * -1.f);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skill"), m_iCurrentLevelndex, TEXT("Skill"), &SkillDesc)))
			return;
	}

	

	
}

void CBattery_Tower::Give_Burst(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Catapults"));

	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if (*iter_Obj == nullptr)
		{
			iter_Obj++;
			continue;
		}

		_float fCmpDistance = (Get_Position().x - (*iter_Obj)->Get_Position().x)*(Get_Position().x - (*iter_Obj)->Get_Position().x)
			+ (Get_Position().y - (*iter_Obj)->Get_Position().y)*(Get_Position().y - (*iter_Obj)->Get_Position().y)
			+ (Get_Position().z - (*iter_Obj)->Get_Position().z)*(Get_Position().z - (*iter_Obj)->Get_Position().z);

		if (m_fMaxRange >= fCmpDistance)
		{
			static_cast<CCatapult*>(*iter_Obj)->Interact((_uint)m_fLifeTime);
		}
		iter_Obj++;
	}
	
	Safe_Release(pGameInstance);
}

void CBattery_Tower::Play_Sound(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	_int iNum = rand() % 4;
	_tchar	szFullPath[MAX_PATH];
	_float fVolume = 0.5f;
	switch (m_eState)
	{
	case PLACE:
		wcscpy_s(szFullPath, TEXT("place_battery_DST.wav"));
		break;
	case ATTACK:
		fVolume = 0.2f;
		wcscpy_s(szFullPath, TEXT("on_battery_DST.wav"));
		break;
	case DESTROY:
		wcscpy_s(szFullPath, TEXT("place_battery_DST.wav"));
		break;
	}
	pGameInstance->PlaySounds(szFullPath, SOUND_BATTERY, fVolume);

	Safe_Release(pGameInstance);
}

_bool CBattery_Tower::Check_Distance(CGameObject* _pObj)
{
	_float fCmpDir = (Get_Position().x - (_pObj)->Get_Position().x)*(Get_Position().x - (_pObj)->Get_Position().x)
		+ (Get_Position().y - (_pObj)->Get_Position().y)*(Get_Position().y - (_pObj)->Get_Position().y)
		+ (Get_Position().z - (_pObj)->Get_Position().z)*(Get_Position().z - (_pObj)->Get_Position().z);

	if (fCmpDir > m_fMaxRange)
	{
		return false;
	}
	else
	{
		return true;
	}

}

void CBattery_Tower::Interact(_uint Damage)
{
}

CBattery_Tower * CBattery_Tower::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBattery_Tower* pInstance = new CBattery_Tower(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBattery_Tower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBattery_Tower::Clone(void * pArg)
{
	CBattery_Tower* pInstance = new CBattery_Tower(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBattery_Tower "));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattery_Tower::Free()
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
