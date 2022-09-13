#include "stdafx.h"
#include "..\Public\WoodWall.h"
#include "GameInstance.h"
#include "PickingMgr.h"

CWoodWall::CWoodWall(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CWoodWall::CWoodWall(const CWoodWall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWoodWall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWoodWall::Initialize(void* pArg)
{

	if (pArg != nullptr)
		memcpy(&m_eWallDesc, pArg, sizeof(WALLDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 60;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	if (m_eWallDesc.etype == WALL_WOOD)
		m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	if (m_eWallDesc.etype == WALL_ROCK)
		m_pTransformCom->Set_Scale(1.f, 3.f, 1.f);
	if (m_eWallDesc.etype == WALL_END)
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);

	if (CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex() == LEVEL_MAZE)
	{
		m_pTransformCom->Set_Scale(2.f, 3.f, 1.f);
	}


	//WalkingTerrain();
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();


	if (m_eWallDesc.eDir == SIDE)
		m_pTransformCom->Turn(_float3(0, 1, 0), 1.f);

	
	return S_OK;
}


int CWoodWall::Tick(_float fTimeDelta)
{

	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{

		if (CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct() == false)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_BLOCK, this);

		}
	}
		
	__super::Tick(fTimeDelta);

	// If Hp <= 0 : Drop Items
	if (m_tInfo.iCurrentHp > 40)
		m_eState = HEALTHY;
	else if (m_tInfo.iCurrentHp <= 40 && m_tInfo.iCurrentHp > 0)
		m_eState = DAMAGED;
	else if (m_tInfo.iCurrentHp <= 0)
	{
		if (m_eState < BROKEN)
		{
			m_eState = BROKEN;
		}
	}

	if (m_bConstruct)
	{
		_float3 vPickingPos = CPickingMgr::Get_Instance()->Get_PickingPos();
		vPickingPos.y += m_fRadius;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPickingPos);
	}
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CWoodWall::Late_Tick(_float fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	if (m_eWallDesc.eDir == WALL_DIREND)
		SetUp_BillBoard();


	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
			if (nullptr != m_pRendererCom)
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	}
		
	
	if (m_eWallDesc.etype == WALL_WOOD)
	{
		Change_Motion();
		Change_Frame();

	}
	
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
}

HRESULT CWoodWall::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (m_eWallDesc.etype == WALL_WOOD)
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
			return E_FAIL;
	}
	else if (m_eWallDesc.etype == WALL_ROCK)
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;
	}
	

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if(m_eWallDesc.etype != WALL_END)
		m_pVIBufferCom->Render();

#ifdef _DEBUG
	if (CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct() == false)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}
#endif // _DEBUG

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}


HRESULT CWoodWall::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_TimerTag = TEXT("Timer_Boulder");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	Safe_Release(pGameInstance);

	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.5f;
	CollRectDesc.fRadiusX = 0.5f;
	CollRectDesc.fRadiusZ = 0.1f;

	if(m_eWallDesc.etype == WALL_END)
		CollRectDesc.fRadiusZ = 0.5f;
	if (CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex() == LEVEL_MAZE)
	{
		CollRectDesc.fRadiusZ = 0.5f;
	}
	if (m_eWallDesc.eDir == SIDE)
	{
		CollRectDesc.fRadiusZ = 0.8f;
		CollRectDesc.fRadiusX = 0.1f;
	}
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.f;
	CollRectDesc.fOffsetZ = 0.f;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.InitPos = m_eWallDesc.vecPosition;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWoodWall::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CWoodWall::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CWoodWall::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CWoodWall::WalkingTerrain()
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

HRESULT CWoodWall::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CWoodWall::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));


	switch (m_eWallDesc.etype)
	{
	case Client::CWoodWall::WALL_WOOD:
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_fSpeed = 60;

		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_HEALTHY"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WoodWall_HEALTHY"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_DAMAGED"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WoodWall_DAMAGED"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_BROKEN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WoodWall_BROKEN"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case Client::CWoodWall::WALL_ROCK:
		TextureDesc.m_iEndTex = 2;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_RockHEALTHY"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_RockWall_HEALTHY"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case Client::CWoodWall::WALL_END:
		TextureDesc.m_iEndTex = 2;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_RockHEALTHY"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_RockWall_HEALTHY"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	default:
		break;
	}
	return S_OK;
}

void CWoodWall::Change_Frame()
{
	m_pTextureCom->MoveFrame(m_TimerTag);
}

void CWoodWall::Change_Motion()
{

	if (m_eWallDesc.etype == WALL_WOOD)
	{
		if (m_eState != m_ePreState)
		{
			switch (m_eState)
			{
			case CWoodWall::HEALTHY:
				Change_Texture(TEXT("Com_Texture_HEALTHY"));
				break;
			case CWoodWall::DAMAGED:
				Change_Texture(TEXT("Com_Texture_DAMAGED"));
				break;
			case CWoodWall::BROKEN:
				Change_Texture(TEXT("Com_Texture_BROKEN"));
				break;
			}

			m_ePreState = m_eState;
		}
	}
}


CWoodWall* CWoodWall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWoodWall* pInstance = new CWoodWall(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWoodWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWoodWall::Clone(void* pArg)
{
	CWoodWall* pInstance = new CWoodWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWoodWall "));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CWoodWall::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}