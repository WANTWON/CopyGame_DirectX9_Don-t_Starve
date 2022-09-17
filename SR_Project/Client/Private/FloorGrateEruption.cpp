#include "stdafx.h"
#include "FloorGrateEruption.h"
#include "GameInstance.h"
#include "PickingMgr.h"

CFloorGrateEruption::CFloorGrateEruption(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CFloorGrateEruption::CFloorGrateEruption(const CFloorGrateEruption & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFloorGrateEruption::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloorGrateEruption::Initialize(void* pArg)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
	m_tInfo.fDamage = 5.f;

	ZeroMemory(&m_tEruptionDesc, sizeof(ERUPTIONDESC));
	memcpy(&m_tEruptionDesc, (ERUPTIONDESC*)pArg, sizeof(ERUPTIONDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(m_tEruptionDesc.vInitPosition)))
		return E_FAIL;

	switch (m_tEruptionDesc.eState)
	{
	case ERUPTION_STATE::WARNING:
		m_pTransformCom->Set_Scale(.4f, .4f, 1.f);
		break;
	case ERUPTION_STATE::ERUPTION:
		m_pTransformCom->Set_Scale(1.7f, 1.7f, 1.f);
		break;
	}

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}

int CFloorGrateEruption::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	WalkingTerrain();
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_pColliderCom)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);
	}

	return OBJ_NOEVENT;
}

void CFloorGrateEruption::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Compute_CamDistance(Get_Position());
	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
		Check_Collision();
	}

	Change_Motion();
	Change_Frame();
}

HRESULT CFloorGrateEruption::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (m_pColliderCom && g_ColliderRender)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}


HRESULT CFloorGrateEruption::SetUp_Components(void* pArg)
{
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
	if (m_tEruptionDesc.eState == ERUPTION_STATE::ERUPTION)
	{
		CCollider_Cube::COLLRECTDESC CollRectDesc;
		ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
		CollRectDesc.fRadiusX = .15f;
		CollRectDesc.fRadiusY = .4f;
		CollRectDesc.fRadiusZ = .25f;
		CollRectDesc.fOffSetX = 0.f;
		CollRectDesc.fOffSetY = .7f;
		CollRectDesc.fOffsetZ = 0.f;
		if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFloorGrateEruption::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CFloorGrateEruption::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CFloorGrateEruption::SetUp_BillBoard()
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

void CFloorGrateEruption::WalkingTerrain()
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

HRESULT CFloorGrateEruption::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CFloorGrateEruption::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	switch (m_tEruptionDesc.eState)
	{
	case ERUPTION_STATE::WARNING:
		TextureDesc.m_iEndTex = 31;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Eruption_Warning"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Eruption_Warning"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case ERUPTION_STATE::ERUPTION:
		TextureDesc.m_iEndTex = 40;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Eruption"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Eruption"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	}	

	return S_OK;
}

void CFloorGrateEruption::Change_Motion()
{
	// Change Texture based on State
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case ERUPTION_STATE::WARNING:
			Change_Texture(TEXT("Com_Texture_Eruption_Warning"));
			break;
		case ERUPTION_STATE::ERUPTION:
			Change_Texture(TEXT("Com_Texture_Eruption"));
			break;
		}

		m_ePreState = m_eState;
	}
}

void CFloorGrateEruption::Change_Frame()
{
	if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		m_bDead = true;
}

void CFloorGrateEruption::Check_Collision()
{
	if (m_bDidDamage)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	vector<CGameObject*> vecDamagedActor;

	if (pGameInstance->Collision_Check_Group_Multi(CCollider_Manager::COLLISION_GROUP::COLLISION_PLAYER, vecDamagedActor, this, CCollider_Manager::COLLISION_TYPE::COLLSIION_BOX))
	{
		CGameInstance::Apply_Damage_Multi(m_tInfo.fDamage, vecDamagedActor, this);
		m_bDidDamage = true;
	}
}

CFloorGrateEruption* CFloorGrateEruption::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFloorGrateEruption* pInstance = new CFloorGrateEruption(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFloorGrateEruption"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFloorGrateEruption::Clone(void* pArg)
{
	CFloorGrateEruption* pInstance = new CFloorGrateEruption(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFloorGrateEruption "));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFloorGrateEruption::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}