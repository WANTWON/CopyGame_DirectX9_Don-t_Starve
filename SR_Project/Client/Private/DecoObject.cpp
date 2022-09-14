#include "stdafx.h"
#include "..\Public\DecoObject.h"
#include "GameInstance.h"
#include "FloorGrateEruption.h"

CDecoObject::CDecoObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDecoObject::CDecoObject(const CDecoObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDecoObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecoObject::Initialize(void* pArg)
{
	ZeroMemory(&m_DecoDesc, sizeof(DECODECS));
	memcpy(&m_DecoDesc, (DECODECS*)pArg, sizeof(DECODECS));


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(&m_DecoDesc.vInitPosition)))
		return E_FAIL;

	switch (m_DecoDesc.m_eState)
	{
	case DECOTYPE::FLOORFIRE:
		m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
		m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
		m_pTransformCom->Turn(_float3(1, 0, 0), 1.f);
		m_fRadius = 0.01f;
		break;
	case DECOTYPE::FLOOR_EFFECT:
		m_pTransformCom->Set_Scale(1.f, 1.5f, 1.f);
		break;
	}

	return S_OK;
}

int CDecoObject::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	__super::Tick(fTimeDelta);

	WalkingTerrain();
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CDecoObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_DecoDesc.m_eState != FLOORFIRE)
		SetUp_BillBoard();

	switch (m_DecoDesc.m_eState)
	{
	case DECOTYPE::FLOORFIRE:
		FloorUpdate();
		Check_Eruption(fTimeDelta);
		break;
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CDecoObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	m_pTextureCom->MoveFrame(m_TimerTag);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CDecoObject::FloorUpdate()
{
	if (m_bCreate)
		return;

	DECODECS DecoDesc;
	DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR_EFFECT;
	DecoDesc.vInitPosition = Get_Position();
	CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_BOSS, TEXT("Layer_Deco"), &DecoDesc);

	m_bCreate = true;
}

HRESULT CDecoObject::SetUp_Components(void* pArg)
{
	/* For.Com_Texture */
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_fSpeed = 60.f;
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iCurrentTex = 0;

	switch (m_DecoDesc.m_eState)
	{
	case DECOTYPE::FLOORFIRE:
		TextureDesc.m_iEndTex = 43;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_FloorDeco"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case DECOTYPE::FLOOR_EFFECT:
		TextureDesc.m_iEndTex = 32;
		TextureDesc.m_fSpeed = rand() % 20 + 10;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_FloorParticle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	}

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

HRESULT CDecoObject::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 30);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CDecoObject::Release_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CDecoObject::SetUp_BillBoard()
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

void CDecoObject::WalkingTerrain()
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

void CDecoObject::Check_Eruption(_float fTimeDelta)
{
	// Spawn Eruption (Warning already Spawned)
	if (m_bShouldErupt)
	{
		if (m_fEruptionTime < 1.5f)
			m_fEruptionTime += fTimeDelta;
		else
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			if (!pGameInstance)
				return;

			CFloorGrateEruption::ERUPTIONDESC tEruptionDesc;
			tEruptionDesc.eState = CFloorGrateEruption::ERUPTION_STATE::ERUPTION;
			tEruptionDesc.vInitPosition = Get_Position();

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Floor_Grate_Eruption"), LEVEL_BOSS, TEXT("Layer_Effect"), &tEruptionDesc)))
				return;

			m_bShouldErupt = false;
			m_fEruptionTime = 0.f;
		}
	}
	// Spawn Warning
	else
	{
		// Every 2 seconds there is a random chance of Eruption (Warning)
		if (m_fWarningTime < m_fRandomWarningLimit)
			m_fWarningTime += fTimeDelta;
		else
		{
			if (!m_bIsWarningTimeChosen)
			{
				m_fRandomWarningLimit = rand() % 3 + 1;
				m_bIsWarningTimeChosen = true;
			}
			else
			{
				m_fWarningTime = 0.f;

				_bool bShouldWarn = rand() % 5; // (0 ... 4)

				// If bShouldErupt == 0 : Erupt (25% Chance)
				if (bShouldWarn == 0)
				{
					CGameInstance* pGameInstance = CGameInstance::Get_Instance();
					if (!pGameInstance)
						return;

					CFloorGrateEruption::ERUPTIONDESC tEruptionDesc;
					tEruptionDesc.eState = CFloorGrateEruption::ERUPTION_STATE::WARNING;
					tEruptionDesc.vInitPosition = Get_Position();

					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Floor_Grate_Eruption"), LEVEL_BOSS, TEXT("Layer_Effect"), &tEruptionDesc)))
						return;

					m_bShouldErupt = true;
					m_bIsWarningTimeChosen = false;
				}
			}
		}
	}
}

CDecoObject* CDecoObject::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDecoObject* pInstance = new CDecoObject(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDecoObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecoObject::Clone(void* pArg)
{
	CDecoObject* pInstance = new CDecoObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDecoObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecoObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}