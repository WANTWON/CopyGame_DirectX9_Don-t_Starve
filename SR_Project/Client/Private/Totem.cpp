#include "stdafx.h"
#include "Totem.h"
#include "GameInstance.h"
#include "PickingMgr.h"

CTotem::CTotem(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTotem::CTotem(const CTotem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTotem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTotem::Initialize(void* pArg)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
	m_tInfo.iMaxHp = 75.f;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	ZeroMemory(&m_tTotemDesc, sizeof(TOTEMDESC));
	memcpy(&m_tTotemDesc, (TOTEMDESC*)pArg, sizeof(TOTEMDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(m_tTotemDesc.vInitPosition)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

int CTotem::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bShouldDestroy)
		return OBJ_DEAD;

	AI_Behaviour(fTimeDelta);

	WalkingTerrain();
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_GROUP::COLLISION_MONSTER, this);

	return OBJ_NOEVENT;
}

void CTotem::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	Change_Motion();
	Change_Frame();
}

HRESULT CTotem::Render()
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

HRESULT CTotem::SetUp_Components(void* pArg)
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

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusX = .1f;
	CollRectDesc.fRadiusY = .2f;
	CollRectDesc.fRadiusZ = .3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = .8f;
	CollRectDesc.fOffsetZ = 0.f;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTotem::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CTotem::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CTotem::SetUp_BillBoard()
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

void CTotem::WalkingTerrain()
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

HRESULT CTotem::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CTotem::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	switch (m_tTotemDesc.eType)
	{
	case TOTEM_TYPE::DEFENSE:
	{
		TextureDesc.m_iEndTex = 27;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_BREAK"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Defense_Break"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 18;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Defense_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 39;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Defense_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 15;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_PLACE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Defense_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 10;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_ACTIVE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Defense_Active"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	}
	case TOTEM_TYPE::HEAL:
	{
		TextureDesc.m_iEndTex = 27;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_BREAK"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Heal_Break"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 18;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Heal_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 39;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Heal_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 15;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_PLACE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Heal_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 10;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_ACTIVE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Heal_Active"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	}
	}

	return S_OK;
}

void CTotem::Change_Motion()
{
	// Change Texture based on State
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case TOTEM_STATE::PLACE:
			Change_Texture(TEXT("Com_Texture_PLACE"));
			break;
		case TOTEM_STATE::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case TOTEM_STATE::HIT:
			Change_Texture(TEXT("Com_Texture_HIT"));
			break;
		case TOTEM_STATE::BREAK:
			Change_Texture(TEXT("Com_Texture_BREAK"));
			break;
		case TOTEM_STATE::ACTIVE:
			Change_Texture(TEXT("Com_Texture_ACTIVE"));
			break;
		}

		m_ePreState = m_eState;
	}
}

void CTotem::Change_Frame()
{
	switch (m_eState)
	{
	case TOTEM_STATE::PLACE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = TOTEM_STATE::IDLE;
		break;
	case TOTEM_STATE::IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case TOTEM_STATE::HIT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_bIsHit = false;
		break;
	case TOTEM_STATE::BREAK:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_bShouldDestroy = true;
		break;
	case TOTEM_STATE::ACTIVE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_bIsActive = false;
		break;
	}
}

_float CTotem::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage <= 0 || m_bDead)
		return 0.f;

	m_tInfo.iCurrentHp -= fDamage;
	m_bIsHit = true;

	// Play Sound
	_tchar szFileName[MAX_PATH] = TEXT("");
	wsprintf(szFileName, TEXT("totem_hit_%d.wav"), rand() % 3 + 1);
	CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_OBJECT, .8f);

	if (m_tInfo.iCurrentHp <= 0)
	{
		m_tInfo.iCurrentHp = 0.f;
		m_bDead = true;
	}

	return fDamage;
}

void CTotem::AI_Behaviour(_float fTimeDelta)
{
	if (m_bDead)
		m_eState = TOTEM_STATE::BREAK;
	else if (m_bIsHit)
		m_eState = TOTEM_STATE::HIT;
	else if (m_bIsActive)
		m_eState = TOTEM_STATE::ACTIVE;
	else if (m_eState != TOTEM_STATE::PLACE || m_eState != TOTEM_STATE::ACTIVE)
		m_eState = TOTEM_STATE::IDLE;
}

CTotem* CTotem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTotem* pInstance = new CTotem(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTotem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTotem::Clone(void* pArg)
{
	CTotem* pInstance = new CTotem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTotem "));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTotem::Free()
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