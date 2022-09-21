#include "stdafx.h"
#include "CarnivalCard.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "PickingMgr.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Inventory.h"
#include "CarnivalMemory.h"

CCarnivalCard::CCarnivalCard(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarnivalCard::CCarnivalCard(const CCarnivalCard & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCarnivalCard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnivalCard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::COOKPOT;
	
	m_bInteract = false;

	return S_OK;
}

int CCarnivalCard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bConstruct)
	{
		_float3 vPickingPos = CPickingMgr::Get_Instance()->Get_PickingPos();
		vPickingPos.y += m_fRadius;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPickingPos);
	}

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarnivalCard::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (!m_bPicking && !CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}

	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CCarnivalCard::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCarnivalCard::Interact(_uint Damage)
{
	m_bInteract = false;

	m_eState = m_bIsGood ? STATECARD::REVEAL_GOOD_PRE : STATECARD::REVEAL_BAD_PRE;
	m_pMemory->Make_Guess(m_bIsGood ? true : false);
}

HRESULT CCarnivalCard::Drop_Items()
{
	return S_OK;
}

HRESULT CCarnivalCard::SetUp_Components(void* pArg)
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
	TransformDesc.fRotationPerSec = D3DXToRadian(0.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnivalCard::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	TextureDesc.m_iEndTex = 32;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hint_Bad"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Hint_Bad"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 32;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hint_Good"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Hint_Good"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 18;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reveal_Bad"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Bad"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 12;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reveal_Bad_Post"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Bad_Post"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 4;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reveal_Bad_Pre"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Bad_Pre"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 18;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reveal_Good"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Good"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 12;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reveal_Good_Post"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Good_Post"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 4;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Reveal_Good_Pre"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Good_Pre"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 13;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Turn_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Turn_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCarnivalCard::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case HINT_BAD:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::ON;
		break;
	case HINT_GOOD:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::ON;
		break;
	case OFF:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case ON:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case REVEAL_BAD:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::REVEAL_BAD_POST;
		break;
	case REVEAL_BAD_POST:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::ON;
		break;
	case REVEAL_BAD_PRE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::REVEAL_BAD;
		break;
	case REVEAL_GOOD:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::REVEAL_GOOD_POST;
		break;
	case REVEAL_GOOD_POST:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::ON;
		break;
	case REVEAL_GOOD_PRE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::HINT_GOOD;
		break;
	case TURN_OFF:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::OFF;
		break;
	case TURN_ON:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATECARD::ON;
		break;
	}
}

void CCarnivalCard::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case HINT_BAD:
			Change_Texture(TEXT("Com_Texture_Hint_Bad"));
			break;
		case HINT_GOOD:
			Change_Texture(TEXT("Com_Texture_Hint_Good"));
			break;
		case OFF:
			Change_Texture(TEXT("Com_Texture_Off"));
			break;
		case ON:
			Change_Texture(TEXT("Com_Texture_On"));
			break;
		case REVEAL_BAD:
			Change_Texture(TEXT("Com_Texture_Reveal_Bad"));
			break;
		case REVEAL_BAD_POST:
			Change_Texture(TEXT("Com_Texture_Reveal_Bad_Post"));
			break;
		case REVEAL_BAD_PRE:
			Change_Texture(TEXT("Com_Texture_Reveal_Bad_Pre"));
			break;
		case REVEAL_GOOD:
			Change_Texture(TEXT("Com_Texture_Reveal_Good"));
			break;
		case REVEAL_GOOD_POST:
			Change_Texture(TEXT("Com_Texture_Reveal_Good_Post"));
			break;
		case REVEAL_GOOD_PRE:
			Change_Texture(TEXT("Com_Texture_Reveal_Good_Pre"));
			break;
		case TURN_OFF:
			Change_Texture(TEXT("Com_Texture_Turn_Off"));
			break;
		case TURN_ON:
			Change_Texture(TEXT("Com_Texture_Turn_On"));
			break;
		}

		m_ePreState = m_eState;
	}
}

_bool CCarnivalCard::Picking(_float3 * PickingPoint)
{
	if (CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
		return false;

	if (true == m_pVIBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		m_vecOutPos = *PickingPoint;
		return true;
	}
	else
		return false;
	return true;
}

void CCarnivalCard::PickingTrue()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Up(VK_LBUTTON))
	{
	}

	cout << "Collision Carnival Card : " << m_vecOutPos.x << " " << m_vecOutPos.y << " " << m_vecOutPos.z << endl;

	Safe_Release(pGameInstance);
}

CCarnivalCard* CCarnivalCard::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarnivalCard* pInstance = new CCarnivalCard(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCarnivalCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarnivalCard::Clone(void* pArg)
{
	CCarnivalCard* pInstance = new CCarnivalCard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarnivalCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarnivalCard::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}