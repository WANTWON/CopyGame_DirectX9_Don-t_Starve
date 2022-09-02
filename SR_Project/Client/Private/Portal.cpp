#include "stdafx.h"
#include "..\Public\Portal.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Manager.h"
#include "Level_GamePlay.h"

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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eInteract_OBJ_ID = INTERACTOBJ_ID::PORTAL;

	/*_float fSize = 3;
	m_pTransformCom->Set_Scale(fSize, fSize, 1.f);
	m_fRadius *= fSize; */
	m_fRadius = 0.5f;
	m_fOpenRadius = 3.f;

	m_pTransformCom->Set_Scale(2.f, 1.f, 1.f);
	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f),1.0);

	return S_OK;
}

int CPortal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bShouldTeleport)
	{
		CLevel_GamePlay* pLevelGamePlay = (CLevel_GamePlay*)CLevel_Manager::Get_Instance()->Get_CurrentLevel();
		pLevelGamePlay->Set_NextLevel(true);
	}

	AI_Behaviour();

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	
	return OBJ_NOEVENT;
}

void CPortal::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame();
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
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
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

	return S_OK;
}

HRESULT CPortal::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_CLOSE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Portal_Idle_Close"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_OPENING"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Portal_Open"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 49;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_OPEN"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Portal_Idle_Open"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_CLOSING"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Portal_Close"), (CComponent**)&m_pTextureCom, &TextureDesc)))
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
			Change_Texture(TEXT("Com_Texture_IDLE_CLOSE"));
			break;
		case STATE::OPENING:
			Change_Texture(TEXT("Com_Texture_OPENING"));
			break;
		case STATE::IDLE_OPEN:
			Change_Texture(TEXT("Com_Texture_IDLE_OPEN"));
			break;
		case STATE::CLOSING:
			Change_Texture(TEXT("Com_Texture_CLOSING"));
			break;
		}

		m_ePreState = m_eState;
	}
}

void CPortal::AI_Behaviour()
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
}

_bool CPortal::Check_Target()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

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