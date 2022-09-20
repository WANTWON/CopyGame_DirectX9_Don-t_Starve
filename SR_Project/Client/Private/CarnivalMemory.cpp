#include "stdafx.h"
#include "CarnivalMemory.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "PickingMgr.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Inventory.h"

CCarnivalMemory::CCarnivalMemory(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarnivalMemory::CCarnivalMemory(const CCarnivalMemory & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCarnivalMemory::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnivalMemory::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::COOKPOT;

	m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

	m_eShaderID = SHADER_IDLE_ALPHABLEND;

	return S_OK;
}

int CCarnivalMemory::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bConstruct)
	{
		_float3 vPickingPos = CPickingMgr::Get_Instance()->Get_PickingPos();
		vPickingPos.y += m_fRadius;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPickingPos);
	}

	Play_Game(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarnivalMemory::Late_Tick(_float fTimeDelta)
{
	// __super::Late_Tick(fTimeDelta); // CInteractiveObject are RENDER_NONALPHABLEND, but this need to be RENDER_ALPHABLEND

	SetUp_BillBoard();

	Compute_CamDistance(Get_Position());
	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	if (m_pColliderCom)
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	if (!m_bPicking && !CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}

	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CCarnivalMemory::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCarnivalMemory::Interact(_uint Damage)
{
	m_eState = STATEMEMORY::HIT;
	
	Start_Game();

	m_bInteract = false;
}

void CCarnivalMemory::Start_Game()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();

	list<CGameObject*>* lObjects = pGameInstance->Get_ObjectList(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Object"));
	if (!lObjects)
		return;

	_uint iHintCounter = 0;

	// Populate Card Vector
	for (auto& iter = lObjects->begin(); iter != lObjects->end(); ++iter)
	{
		CCarnivalCard* pCard = dynamic_cast<CCarnivalCard*>(*iter);
		if (pCard)
		{
			pCard->Set_Memory(this);

			m_vecCards.push_back(pCard);
			m_vecHints.push_back(iHintCounter);
			iHintCounter++;
		}
	}

	// Turn On Cards
	for (auto pCard : m_vecCards)
		if (pCard)
			pCard->Turn_On();

	m_bCanPlay = true;
}

void CCarnivalMemory::Play_Game(_float fTimeDelta)
{
	if (m_bCanPlay)
	{
		if (!m_bIsRoundActive && m_fGameTimer > 3.f)
			Start_Round(fTimeDelta);
		else if (m_bIsRoundActive)
		{
			Check_Guesses();
			
			if (m_bShouldResetRound)
				Reset_Round();
		}
		else
			m_fGameTimer += fTimeDelta;
	}
}

void CCarnivalMemory::Start_Round(_float fTimeDelta)
{
	random_shuffle(m_vecHints.begin(), m_vecHints.end());
	_uint iGoodHints = Get_Round_Good_Hint();

	for (auto i = 0; i < m_vecCards.size(); ++i)
	{
		if (m_vecCards[i])
		{
			m_vecCards[i]->Give_Hint(m_vecHints[i] < iGoodHints ? true : false);
			m_vecCards[i]->Set_Interact(true);
		}
	}

	m_bIsRoundActive = true;
}

void CCarnivalMemory::Check_Guesses()
{ 
	for (auto bGuess : m_vecGuesses)
	{
		if (!bGuess)
		{
			m_bShouldResetRound = true;

			// Lose Round
			m_eState = STATEMEMORY::HIT;
			m_vecGuesses.clear();
			return;
		}
	}

	if (m_vecGuesses.size() >= Get_Round_Good_Hint())
	{
		m_bShouldResetRound = true;

		// Win Round
		m_eState = STATEMEMORY::TURN_ON;
		m_vecGuesses.clear();
		m_iTurnCount--;

		// Win Game
		if (m_iTurnCount == 0)
		{
			m_eState = STATEMEMORY::WIN;
			m_bCanPlay = false;
		}
	}
}

void CCarnivalMemory::Reset_Round()
{
	m_bIsRoundActive = false;
	m_bShouldResetRound = false;
	m_fGameTimer = 0.f;

	for (auto& pCard : m_vecCards)
		pCard->Set_Interact(false);
}

_uint CCarnivalMemory::Get_Round_Good_Hint()
{
	switch (m_iTurnCount)
	{
	case 5:
	case 4:
		return 1;
	case 3:
	case 2:
		return 2;
	case 1:
		return 3;
	}
}

HRESULT CCarnivalMemory::Drop_Items()
{
	return S_OK;
}

HRESULT CCarnivalMemory::SetUp_Components(void* pArg)
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

HRESULT CCarnivalMemory::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	TextureDesc.m_iEndTex = 8;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hit"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Idle_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 40;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Idle_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 26;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Place"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Turn_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 24;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Turn_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	TextureDesc.m_iEndTex = 20;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Win"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Win"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCarnivalMemory::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case HIT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATEMEMORY::IDLE_OFF;
		break;
	case IDLE_OFF:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case IDLE_ON:
		m_pTextureCom->MoveFrame(m_TimerTag);
		
		if (m_fIdleOnTimer > 2.f)
		{
			m_fIdleOnTimer = 0.f;
			m_eState = STATEMEMORY::TURN_OFF;
		}
		else
			m_fIdleOnTimer += fTimeDelta;
		
		break;
	case PLACE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATEMEMORY::IDLE_OFF;
		break;
	case TURN_OFF:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATEMEMORY::IDLE_OFF;
		break;
	case TURN_ON:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATEMEMORY::IDLE_ON;
		break;
	case WIN:
		m_pTextureCom->MoveFrame(m_TimerTag);

		if (m_fWinTimer > 2.f)
		{
			m_fWinTimer = 0.f;
			m_eState = STATEMEMORY::TURN_OFF;
		}
		else
			m_fWinTimer += fTimeDelta;

		break;
	}
}

void CCarnivalMemory::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case HIT:
			Change_Texture(TEXT("Com_Texture_Hit"));
			break;
		case IDLE_OFF:
			Change_Texture(TEXT("Com_Texture_Idle_Off"));
			break;
		case IDLE_ON:
			Change_Texture(TEXT("Com_Texture_Idle_On"));
			break;
		case PLACE:
			Change_Texture(TEXT("Com_Texture_Place"));
			break;
		case TURN_OFF:
			Change_Texture(TEXT("Com_Texture_Turn_Off"));
			break;
		case TURN_ON:
			Change_Texture(TEXT("Com_Texture_Turn_On"));
			break;
		case WIN:
			Change_Texture(TEXT("Com_Texture_Win"));
			break;
		}

		m_ePreState = m_eState;
	}
}

_bool CCarnivalMemory::Picking(_float3 * PickingPoint)
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

void CCarnivalMemory::PickingTrue()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Up(VK_LBUTTON))
	{
	}

	cout << "Collision Carnival Memory : " << m_vecOutPos.x << " " << m_vecOutPos.y << " " << m_vecOutPos.z << endl;

	Safe_Release(pGameInstance);
}

CCarnivalMemory* CCarnivalMemory::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarnivalMemory* pInstance = new CCarnivalMemory(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCarnivalMemory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarnivalMemory::Clone(void* pArg)
{
	CCarnivalMemory* pInstance = new CCarnivalMemory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarnivalMemory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarnivalMemory::Free()
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