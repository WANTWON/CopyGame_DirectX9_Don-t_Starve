#include "stdafx.h"
#include "..\Public\Trap.h"
#include "GameInstance.h"
#include "ShockEffect.h"

CTrap::CTrap(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTrap::CTrap(const CTrap & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrap::Initialize(void* pArg)
{
	ZeroMemory(&m_tTrapDesc, sizeof(TRAPDESC));
	memcpy(&m_tTrapDesc, (TRAPDESC*)pArg, sizeof(TRAPDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(&m_tTrapDesc.vInitPosition)))
		return E_FAIL;

	switch (m_tTrapDesc.eType)
	{
	case TRAPTYPE::TRAP_STAR:
		m_fRadius = .4f;
		break;
	case TRAPTYPE::TRAP_PLANT:
		m_fRadius = .5f; 
		break;
	}

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

int CTrap::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	__super::Tick(fTimeDelta);

	WalkingTerrain();

	if (m_pColliderCom)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);
	}
	
	Check_Collision();

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CTrap::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	SetUp_BillBoard();

	if (m_pColliderCom)
	{
		memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	}
	
	Change_Motion();
	Change_Frame(fTimeDelta);

	Set_ShaderID();
}

HRESULT CTrap::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	// Shader Setup
	_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;

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

HRESULT CTrap::SetUp_Components(void* pArg)
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
	TransformDesc.fRotationPerSec = D3DXToRadian(1.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusX = 0.3f;
	CollRectDesc.fRadiusY = 0.3f;
	CollRectDesc.fRadiusZ = 0.3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -m_fRadius / 2;
	CollRectDesc.fOffsetZ = 0.f;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	return S_OK;
}

void CTrap::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);	// Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);		// Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CTrap::WalkingTerrain()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(iLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;
	CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(iLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);

	if (nullptr == pTransform_Terrain)
		return;

	_float3	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), m_fRadius);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CTrap::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_fSpeed = 25.f;
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iCurrentTex = 0;

	switch (m_tTrapDesc.eType)
	{
	case TRAPTYPE::TRAP_STAR:
	{
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Ready"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Idle_Ready"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Idle_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 48;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reset"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Reset"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}
	break;
	case TRAPTYPE::TRAP_PLANT:
	{
		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Ready"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Idle_Ready"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 2;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Idle_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 23;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Trap"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Trap"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 15;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reset"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Reset"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}
	break;
	}
}

HRESULT CTrap::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CTrap::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case TRAPSTATE::STATE_IDLE_READY:
			Change_Texture(TEXT("Com_Texture_Idle_Ready"));
			break;
		case TRAPSTATE::STATE_TRAP:
			Change_Texture(TEXT("Com_Texture_Trap"));
			break;
		case TRAPSTATE::STATE_IDLE_TRAP:
			Change_Texture(TEXT("Com_Texture_Idle_Trap"));
			break;
		case TRAPSTATE::STATE_RESET:
			Change_Texture(TEXT("Com_Texture_Reset"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
	}
}

void CTrap::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case TRAPSTATE::STATE_IDLE_READY:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case TRAPSTATE::STATE_TRAP:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = TRAPSTATE::STATE_IDLE_TRAP;
		break;
	case TRAPSTATE::STATE_IDLE_TRAP:
		m_pTextureCom->MoveFrame(m_TimerTag);

		m_fTrapTimer += fTimeDelta;
		if (m_fTrapTimer > 2.f)
		{
			m_eState = TRAPSTATE::STATE_RESET;
			m_fTrapTimer = 0.f;
		}
		break;
	case TRAPSTATE::STATE_RESET:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			m_eState = TRAPSTATE::STATE_IDLE_READY;
			m_bDidDamage = false;
		}
		break;
	}
}

void CTrap::Check_Collision()
{
	if (m_bDidDamage)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	vector<CGameObject*> vCollided;

	if (pGameInstance->Collision_Check_Group_Multi(CCollider_Manager::COLLISION_GROUP::COLLISION_PLAYER, vCollided, this, CCollider_Manager::COLLISION_TYPE::COLLSIION_BOX))
	{
		if (m_eState != TRAPSTATE::STATE_TRAP)
			m_eState = TRAPSTATE::STATE_TRAP;

		if (m_eState == TRAPSTATE::STATE_TRAP && m_pTextureCom->Get_Frame().m_iCurrentTex == 6)
		{
			// Spawn Hit Effect
			CShockEffect::SHOCKDESC ShockDesc;
			ShockDesc.eShockType = CShockEffect::SHOCKTYPE::HIT;

			_float3 vLook;
			D3DXVec3Normalize(&vLook, &m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK));
			ShockDesc.vInitPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3] - vLook;

			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shock_Effect"), LEVEL_STATIC, TEXT("Layer_Effect"), &ShockDesc);

			CGameInstance::Apply_Damage_Multi(m_iDamage, vCollided, this, nullptr);
			m_bDidDamage = true;
		}
	}
}

void CTrap::Set_ShaderID()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	if (pGameObject->Get_Dead())
		m_eShaderID = SHADER_DEAD;
	else if (iLevel == LEVEL_MAZE)
		m_eShaderID = SHADER_DARK;
	else
		m_eShaderID = SHADER_IDLE_ALPHATEST;
}

CTrap* CTrap::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTrap* pInstance = new CTrap(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTrap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrap::Clone(void* pArg)
{
	CTrap* pInstance = new CTrap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTrap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrap::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}