#include "stdafx.h"
#include "..\Public\Skill.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Level_Manager.h"
#include "Interactive_Object.h"
#include "Player.h"


CSkill::CSkill(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CPawn(pGraphic_Device)
{
}

CSkill::CSkill(const CSkill & rhs)
	:CPawn(rhs)
{
}

HRESULT CSkill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_TimerTag = TEXT("Timer_Skill");

	if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSkill::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tSkillDesc = *(SKILL_DESC*)(pArg);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	m_eShaderID = SHADER_IDLE_ALPHABLEND;
	return S_OK;
}

int CSkill::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead == true)
		return OBJ_DEAD;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_OBJECT, this);

	Excute(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CSkill::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Compute_CamDistance(Get_Position());

	SetUp_BillBoard();
	if (nullptr != m_pRendererCom )
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	Activate_Check(fTimeDelta);

	/*if (Compare_Terrain())
		m_bDead = true;*/
}

HRESULT CSkill::Render()
{
	if (m_bRender)
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
		//Render_TextureState();

		m_pShaderCom->Begin(m_eShaderID);

		m_pVIBufferCom->Render();

		m_pShaderCom->End();
	}
	
#ifdef _DEBUG
	if (g_ColliderRender)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}
#endif // _DEBUG
	m_pTextureCom->MoveFrame(m_TimerTag);

	return S_OK;
}

HRESULT CSkill::SetUp_Components(void * pArg)
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
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = m_tSkillDesc.vPosition;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Init_Data();
	
	return S_OK;
}

HRESULT CSkill::SetUp_DebugComponents(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_DebugBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIDebugBufferCom)))
		return E_FAIL;

	return S_OK;
}

void CSkill::SetUp_BillBoard()
{
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

}

void CSkill::WalkingTerrain()
{
}

HRESULT CSkill::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

_bool CSkill::Compare_Terrain(void)
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

	_float3 vScale = m_pTransformCom->Get_Scale();
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (vScale.y*0.5f));

	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vMyPos.y = vPosition.y;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMyPos);

	return true;
}

HRESULT CSkill::Init_Data(void)
{
	switch (m_tSkillDesc.eSkill)
	{
	case SKILL_TYPE::HEAL:
		break;
	case SKILL_TYPE::ICE_BLAST:
		Change_Texture(TEXT("Com_Texture_IceBlast_Pre"));
		m_pTransformCom->Set_Scale(m_tSkillDesc.vScale.x, m_tSkillDesc.vScale.y, m_tSkillDesc.vScale.z);
		m_fDamage = 30.f;
		break;
	case SKILL_TYPE::ELEC:
		m_pTransformCom->Set_Scale(3.f, 1.f, 3.f);
		m_bRender = false;
		break;
	case SKILL_TYPE::SPARK:
		m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
		m_fDamage = 0.f;
		break;
	case SKILL_TYPE::BURST:
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
		m_fDamage = 0.f;
		m_iReversTex = m_pTextureCom->Get_Frame().m_iEndTex - 1;
		m_pTextureCom->Get_Frame().m_iCurrentTex = m_iReversTex;
		break;
	}


	return S_OK;
}

HRESULT CSkill::Texture_Clone(void)
{
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_iStartTex = 0;

	/*Ice_Blast*/
	switch (m_tSkillDesc.eSkill)
	{
	case SKILL_TYPE::ELEC:
	case SKILL_TYPE::HEAL:
		TextureDesc.m_iEndTex = 38;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Heal"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Heal"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case SKILL_TYPE::ICE_BLAST:
		TextureDesc.m_iEndTex = 19;
		TextureDesc.m_fSpeed = 40;

		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IceBlast_Pre"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ice_Blast_Pre"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 19;
		TextureDesc.m_fSpeed = 60;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IceBlast"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ice_Blast"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case SKILL_TYPE::SPARK:
		TextureDesc.m_iEndTex = 16;
		TextureDesc.m_fSpeed = 40;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Spark"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spark_Effect"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case SKILL_TYPE::BURST:
		TextureDesc.m_iEndTex = 19;
		TextureDesc.m_fSpeed = 40;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Burst"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Effect"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	}

	return S_OK;
}

void CSkill::Ice_Blast(_float _fTimeDelta)
{
	if (!m_bTrigger
		&& m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bTrigger = true;
		Change_Texture(TEXT("Com_Texture_IceBlast"));
	}

	//Activate
	if (m_bTrigger &&m_pTextureCom->Get_Frame().m_iCurrentTex == 2
		/*&& m_pTextureCom->Get_Frame().m_iCurrentTex <= 6*/)
	{
		m_bActivated = true;
	}
	else
	{
		m_bActivated = false;
	}

	//Dead
	if ((m_bTrigger && m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1))
	{
		m_bDead = true;
	}
}

void CSkill::Heal(_float _fTimeDelta)
{
	//_float4x4 WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float3 CurPos = m_tSkillDesc.pTarget->Get_Position();

	D3DXVec3Normalize(&vLook, &vLook);
	CurPos -= vLook * 0.1f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CurPos);

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		static_cast<CPlayer*>(m_tSkillDesc.pTarget)->Set_HP(30.f);
		m_bDead = true;
	}
}

void CSkill::Elec(_float _fTimeDelta)
{
	//Test
	m_fAccDeadTimer += _fTimeDelta;
	if (m_fAccDeadTimer > 1.f)
	{
		m_bActivated = true;
	}

}

void CSkill::Spark(_float _fTimeDelta)
{
	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bDead = true;
	}
}

void CSkill::Burst(_float _fTimeDelta)
{
	if (m_pTextureCom->Get_Frame().m_iCurrentTex <= m_pTextureCom->Get_Frame().m_iStartTex + 1)
	{
		m_bDead = true;
		m_pTextureCom->Get_Frame().m_iCurrentTex = 0;
	}
	else
	{
		m_pTextureCom->Get_Frame().m_iCurrentTex = --m_iReversTex;
	}
}

CSkill * CSkill::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkill*	pInstance = new CSkill(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkill::Clone(void * pArg)
{
	CSkill*	pInstance = new CSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSkill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill::Free()
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

void CSkill::Excute(_float _fTimeDelta)
{
	switch (m_tSkillDesc.eSkill)
	{
	case SKILL_TYPE::HEAL:
		Heal(_fTimeDelta);
		break;
	case SKILL_TYPE::ICE_BLAST:
		Ice_Blast(_fTimeDelta);
		break;
	case SKILL_TYPE::ELEC:
		Elec(_fTimeDelta);
		break;
	case SKILL_TYPE::BURST:
		Burst(_fTimeDelta);
		break;
	case SKILL_TYPE::SPARK:
		Spark(_fTimeDelta);
		break;
	}
}

void CSkill::Activate_Check(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	vector<CGameObject*> vecDamagedActor;

	if (pGameInstance->Collision_Check_Group_Multi(CCollider_Manager::COLLISION_MONSTER, vecDamagedActor, this, CCollider_Manager::COLLSIION_BOX))
	{
		switch (m_tSkillDesc.eSkill)
		{
		case SKILL_TYPE::HEAL:

			break;
		case SKILL_TYPE::ICE_BLAST:
			if (m_bActivated)
			{
				m_fDamage = 30.f;
				goto Attack;
			}
			break;
		case SKILL_TYPE::ELEC:
			if (m_bActivated)
			{
				m_fDamage = 10.f;
				m_bDead = true;
				goto Attack;

			}
			break;
		case SKILL_TYPE::SPARK:

			break;
		}

		return;

		//goto
		Attack:
		CGameInstance::Apply_Damage_Multi(m_fDamage, vecDamagedActor, this, nullptr);

	}
	vecDamagedActor.clear();
	
	
}

void CSkill::Dead_Check(_float _fTimeDelta)
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	m_fAccDeadTimer += _fTimeDelta;
	//ForA
	switch (m_tSkillDesc.eSkill)
	{
	case SKILL_TYPE::HEAL:
		break;
	case SKILL_TYPE::ICE_BLAST:
		break;

	}
}
