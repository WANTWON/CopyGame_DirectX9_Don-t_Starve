#include "stdafx.h"
#include "TotemEffect.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "Collider_Cube.h"
#include "Transform.h"

CTotemEffect::CTotemEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTotemEffect::CTotemEffect(const CTotemEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTotemEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTotemEffect::Initialize(void* pArg)
{
	ZeroMemory(&m_tTotemEffectDesc, sizeof(TOTEMEFFECTDESC));
	memcpy(&m_tTotemEffectDesc, (TOTEMEFFECTDESC*)pArg, sizeof(TOTEMEFFECTDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(m_tTotemEffectDesc.vInitPosition)))
		return E_FAIL;

	switch (m_tTotemEffectDesc.eType)
	{
	case TOTEM_EFFECT_TYPE::DEFENSE:
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);
		break;
	case TOTEM_EFFECT_TYPE::HEAL:
		m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);
		break;
	case TOTEM_EFFECT_TYPE::PARTICLES:
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
		break;
	case TOTEM_EFFECT_TYPE::SHIELD:
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
		break;
	}

	return S_OK;
}

int CTotemEffect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	// If Effect has Target
	Stick_ToTarget();

	//WalkingTerrain(); // No need
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CTotemEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Compute_CamDistance(Get_Position());
	if (m_tTotemEffectDesc.eType == TOTEM_EFFECT_TYPE::SHIELD || m_tTotemEffectDesc.eType == TOTEM_EFFECT_TYPE::PARTICLES)
		SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	Change_Frame();
}

HRESULT CTotemEffect::Render()
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

	return S_OK;
}


HRESULT CTotemEffect::SetUp_Components(void* pArg)
{
	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static_Blend"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

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

	return S_OK;
}

HRESULT CTotemEffect::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CTotemEffect::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CTotemEffect::SetUp_BillBoard()
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

void CTotemEffect::WalkingTerrain()
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
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y *0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CTotemEffect::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CTotemEffect::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	switch (m_tTotemEffectDesc.eType)
	{
	case TOTEM_EFFECT_TYPE::DEFENSE:
		TextureDesc.m_iEndTex = 17;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Defend_Effect"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Effect_Defense"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case TOTEM_EFFECT_TYPE::HEAL:
		TextureDesc.m_iEndTex = 34;
		TextureDesc.m_fSpeed = 20;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Heal_Effect"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Effect_Heal"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case TOTEM_EFFECT_TYPE::PARTICLES:
		TextureDesc.m_iEndTex = 45;
		TextureDesc.m_fSpeed = 20;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Heal_Particles"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Effect_Heal_Particles"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case TOTEM_EFFECT_TYPE::SHIELD:
		TextureDesc.m_iEndTex = 15;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Shield_Effect"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Totem_Effect_Shield"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	}
	
	return S_OK;
}

void CTotemEffect::Change_Frame()
{
	if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		m_bDead = true;
}

void CTotemEffect::Stick_ToTarget()
{
	if (m_tTotemEffectDesc.pEffectTarget)
	{
		CComponent* pComponent = m_tTotemEffectDesc.pEffectTarget->Find_Component(TEXT("Com_Collider_Cube"));
		if (!pComponent)
			return;

		CCollider_Cube* pColliderCube = dynamic_cast<CCollider_Cube*>(pComponent);
		if (!pColliderCube)
			return;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);

		_float3 vTargetPosition;

		if (m_tTotemEffectDesc.eType == TOTEM_EFFECT_TYPE::DEFENSE || m_tTotemEffectDesc.eType == TOTEM_EFFECT_TYPE::HEAL)
		{
			vTargetPosition = (_float3)pColliderCube->Get_CollRectDesc().StateMatrix.m[3];
			vTargetPosition.y = pVIBuffer_Terrain->Compute_Height(vTargetPosition, pTransform_Terrain->Get_WorldMatrix(), .01f);

			m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vTargetPosition);
		}
		else if (m_tTotemEffectDesc.eType == TOTEM_EFFECT_TYPE::SHIELD || m_tTotemEffectDesc.eType == TOTEM_EFFECT_TYPE::PARTICLES)
		{
			CComponent* pComponent = m_tTotemEffectDesc.pEffectTarget->Find_Component(TEXT("Com_Transform"));
			if (!pComponent)
				return;

			CTransform* pTransform = dynamic_cast<CTransform*>(pComponent);
			if (!pTransform)
				return;

			_float3 vLook;
			D3DXVec3Normalize(&vLook, &pTransform->Get_State(CTransform::STATE::STATE_LOOK));
			vTargetPosition = (_float3)pColliderCube->Get_CollRectDesc().StateMatrix.m[3] - vLook;

			m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vTargetPosition);
		}
	}
}

CTotemEffect* CTotemEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTotemEffect* pInstance = new CTotemEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTotemEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTotemEffect::Clone(void* pArg)
{
	CTotemEffect* pInstance = new CTotemEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTotemEffect "));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTotemEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}