#include "stdafx.h"
#include "..\Public\Portal.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Manager.h"
#include "Level_GamePlay.h"
#include "Level_Hunt.h"

CPortal::CPortal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
}

CPortal::CPortal(const CPortal & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortal::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		memcpy(&m_ePortalDesc, pArg, sizeof(PORTALDESC));
		if (FAILED(__super::Initialize(&m_ePortalDesc.vPosition)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}


	m_eInteract_OBJ_ID = INTERACTOBJ_ID::PORTAL;

	switch (m_ePortalDesc.m_eType)
	{
	case PORTAL_NORMAL:
	{
		m_eState = IDLE_CLOSE;
		m_fRadius = 0.5f;
		m_fOpenRadius = 3.f;

		m_pTransformCom->Set_Scale(2.f, 1.f, 1.f);
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1);
		break;
	}
	case PORTAL_BOSS:
	{
		m_eState = IDLE_CLOSE;
		m_fRadius = 1.f;
		m_fOpenRadius = 1.f;

		m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
		//m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1);
		break;
	}
	}

	return S_OK;
}

int CPortal::Tick(_float fTimeDelta)
{

	LEVEL eLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	if (eLevel == LEVEL_LOADING)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);

	if (m_bShouldTeleport)
	{

		switch (eLevel)
		{
		case Client::LEVEL_GAMEPLAY:
		{
			CLevel_GamePlay* pLevel = (CLevel_GamePlay*)CLevel_Manager::Get_Instance()->Get_CurrentLevel();
			pLevel->Set_NextLevel(true);
			break;
		}
		case Client::LEVEL_HUNT:
		{
			if (m_ePortalDesc.m_eType == PORTAL_NORMAL)
			{
				CLevel_Hunt* pLevel = (CLevel_Hunt*)CLevel_Manager::Get_Instance()->Get_CurrentLevel();
				pLevel->Set_PastLevel(true);
			}
			else if (m_ePortalDesc.m_eType == PORTAL_BOSS)
			{
				CLevel_Hunt* pLevel = (CLevel_Hunt*)CLevel_Manager::Get_Instance()->Get_CurrentLevel();
				pLevel->Set_NextLevel(true);
			}
			break;
		}
		default:
			break;
		}


	}


	AI_Behaviour();

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CPortal::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	Change_Motion();
	Change_Frame();

	_float3 vPosition = Get_Position();
	vPosition.y -= m_fRadius - 0.1f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPortal::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortal::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_TimerTag = TEXT("Timer_Portal");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	Safe_Release(pGameInstance);

	/* For.Com_Collider*/
	//if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
	//	return E_FAIL;

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

	return S_OK;
}

void CPortal::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);

	if (m_ePortalDesc.m_eType == PORTAL_BOSS)
	{
		m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
	}	
}

HRESULT CPortal::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_CLOSE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Idle_Close"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_OPENING"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Open"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 49;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_OPEN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Idle_Open"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_CLOSING"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal_Close"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 39;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_BOSS_IDLE_OPEN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossPortal_Idle_Open"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 18;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_BOSS_IDLE_CLOSE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossPortal_Idle_Close"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CPortal::Change_Frame()
{
	switch (m_eState)
	{
	case STATE::IDLE_CLOSE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::OPENING:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
				m_eState = STATE::IDLE_OPEN;
		break;
	case STATE::IDLE_OPEN:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case STATE::CLOSING:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
				m_eState = STATE::IDLE_CLOSE;

			if (!m_bInteract)
				m_bShouldTeleport = true;
		}
		break;
	}
}

void CPortal::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case STATE::IDLE_CLOSE:
			m_ePortalDesc.m_eType == PORTAL_NORMAL ? Change_Texture(TEXT("Com_Texture_IDLE_CLOSE")) : Change_Texture(TEXT("Com_Texture_BOSS_IDLE_CLOSE"));
			break;
		case STATE::OPENING:
			m_ePortalDesc.m_eType == PORTAL_NORMAL ? Change_Texture(TEXT("Com_Texture_OPENING")) : Change_Texture(TEXT("Com_Texture_BOSS_IDLE_CLOSE"));;
			break;
		case STATE::IDLE_OPEN:
			m_ePortalDesc.m_eType == PORTAL_NORMAL ? Change_Texture(TEXT("Com_Texture_IDLE_OPEN")) : Change_Texture(TEXT("Com_Texture_BOSS_IDLE_CLOSE"));;
			break;
		case STATE::CLOSING:
			m_ePortalDesc.m_eType == PORTAL_NORMAL ? Change_Texture(TEXT("Com_Texture_CLOSING")) : Change_Texture(TEXT("Com_Texture_BOSS_IDLE_OPEN"));;
			break;
		}

		m_ePreState = m_eState;
	}
}

void CPortal::AI_Behaviour()
{
	switch (m_ePortalDesc.m_eType)
	{
	case PORTAL_NORMAL:
	{
		if (m_bShouldClosePortal)
			m_eState = STATE::CLOSING;
		else
		{
			if (Check_Target() && m_eState != STATE::IDLE_OPEN && m_eState != STATE::OPENING)
				m_eState = STATE::OPENING;
			else if (!Check_Target() && (m_eState == STATE::IDLE_OPEN || m_eState == STATE::OPENING))
				m_eState = STATE::CLOSING;
		}
		break;
	}
	case PORTAL_BOSS:
	{
		if (m_bShouldClosePortal)
			m_eState = STATE::CLOSING;
		else
		{
			if (Check_Target() && m_eState != STATE::IDLE_OPEN && m_eState != STATE::OPENING)
				m_eState = STATE::OPENING;
			else if (!Check_Target() && (m_eState == STATE::IDLE_OPEN || m_eState == STATE::OPENING))
				m_eState = STATE::CLOSING;
		}
		break;
	}
	default:
		break;
	}

}

_bool CPortal::Check_Target()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	Safe_Release(pGameInstance);

	if (pTarget)
	{
		_float3 vTargetPos = pTarget->Get_Position();
		_float fDistanceToTarget = sqrt(pow(Get_Position().x - vTargetPos.x, 2) + pow(Get_Position().y - vTargetPos.y, 2) + pow(Get_Position().z - vTargetPos.z, 2));

		if (fDistanceToTarget < m_fOpenRadius)
			return true;
		else
			return false;
	}
	else
		return false;
}

void CPortal::Interact(_uint Damage)
{
	m_bInteract = false;
	m_bShouldClosePortal = true;
}

HRESULT CPortal::Drop_Items()
{
	return E_NOTIMPL;
}

CPortal* CPortal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPortal* pInstance = new CPortal(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPortal::Clone(void* pArg)
{
	CPortal* pInstance = new CPortal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPortal::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

}