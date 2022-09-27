#include "stdafx.h"
#include "..\Public\Monster.h"
#include "Player.h"
#include "Inventory.h"
#include "Level_Manager.h"
#include "GameInstance.h"
#include "CameraManager.h"
#include "NPC.h"
CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPawn(pGraphic_Device)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CPawn(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_MONSTER;


	return S_OK;
}

int CMonster::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_MONSTER, this);

	m_iDirOffset = CCameraManager::Get_Instance()->Get_CamDir();

	__super::Tick(fTimeDelta);

	if (IsDead())
		return OBJ_DEAD;
	
	// Match Terrain-Y
	WalkingTerrain();

	return OBJ_NOEVENT;
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	Set_ShaderID();
	if (m_bPickingTrue)
		m_eShaderID = SHADER_PICKING;
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;


	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	//CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	//m_pShaderCom->Set_RawValue("g_PlayerPosition", &pTarget->Get_Position(), sizeof(_float3));

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
//	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	//m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex));

	m_pShaderCom->Begin(m_eShaderID);

	m_pVIBufferCom->Render();

	m_pShaderCom->End();

	return S_OK;
}

_float3 CMonster::Get_Scale(void)
{
	return m_pTransformCom->Get_Scale();
}

HRESULT CMonster::SetUp_DebugComponents(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	/* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Components(TEXT("Com_DebugBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIDebugBufferCom)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMonster::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMonster::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CMonster::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CMonster::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	_float3 vLook = *(_float3*)&ViewMatrix.m[2][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * m_pTransformCom->Get_Scale().z);
}

void CMonster::WalkingTerrain()
{
	_uint LevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (!pGameInstance)
		return;
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component((LEVEL)LevelIndex, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (!pVIBuffer_Terrain)
		return;
	CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component((LEVEL)LevelIndex, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (!pTransform_Terrain)
		return;

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScale = m_pTransformCom->Get_Scale();
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (vScale.y*0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

DIR_STATE CMonster::Get_Unprocessed_Dir(DIR_STATE eDir)
{
	DIR_STATE eOldDir = (DIR_STATE)((int)eDir - m_iDirOffset);
	if (eOldDir < 0)
	{
		eDir = (DIR_STATE)((int)eOldDir + 4);
		return eDir;
	}

	return eOldDir;
}

DIR_STATE CMonster::Get_Processed_Dir(DIR_STATE eDir)
{
	DIR_STATE eNewDir = (DIR_STATE)((int)eDir + m_iDirOffset);
	if (eNewDir > 3)
	{
		eDir = (DIR_STATE)((int)eNewDir - 4);
		return eDir;
	}
	
	return eNewDir;
}

void CMonster::Calculate_Direction(_float3 vTargetPos)
{
	// Set Direction
	_float fX = vTargetPos.x - Get_Position().x;
	_float fZ = vTargetPos.z - Get_Position().z;

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
}

_float CMonster::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage <= 0 || m_bDead)
		return 0.f;

	m_tInfo.iCurrentHp -= fDamage;

	if (m_tInfo.iCurrentHp <= 0)
	{
		m_tInfo.iCurrentHp = 0.f;
		m_bDead = true;
	}

	return fDamage;
}


void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}