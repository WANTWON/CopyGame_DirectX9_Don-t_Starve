#include "stdafx.h"
#include "Tent.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "PickingMgr.h"
#include "ParticleSystem.h"
#include "Particle.h"

CTent::CTent(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CTent::CTent(const CTent & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CTent::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTent::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::TENT;

	m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);

	return S_OK;
}

int CTent::Tick(_float fTimeDelta)
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

void CTent::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CTent::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CTent::Interact(_uint Damage)
{
	if (m_eState == STATE::IDLE || m_eState == STATE::SLEEP)
	{
		m_bInteract = false;
		m_bIsInsideTent = m_eState == STATE::IDLE ? true : false;

		if (m_eState == STATE::IDLE)
			CGameInstance::Get_Instance()->PlaySounds(TEXT("tent_open.wav"), SOUND_ID::SOUND_OBJECT, .8f);
		else if (m_eState == STATE::SLEEP)
			CGameInstance::Get_Instance()->PlaySounds(TEXT("tent_close.wav"), SOUND_ID::SOUND_OBJECT, .8f);

		m_eState = ENTER;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		pPlayer->Set_Sleeping(m_bIsInsideTent ? true : false);	
	}
}

HRESULT CTent::Drop_Items()
{
	return S_OK;
}

HRESULT CTent::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_TimerTag = TEXT("Timer_Tent");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;



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

	MINIMAP		minidesc;
	ZeroMemory(&minidesc, sizeof(MINIMAP));
	minidesc.name = MIN_TENT;
	minidesc.pointer = this;

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), LEVEL_GAMEPLAY, TEXT("MiniMap_Icon"), &minidesc);
	Safe_Release(pGameInstance);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTent::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 55;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 21;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PLACE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 22;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ENTER"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Enter"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 64;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_SLEEP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Sleep"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 12;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 67;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DESTROY"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tent_Destroy"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CTent::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case PLACE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case ENTER:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			if (m_bIsInsideTent)
				m_eState = SLEEP;
			else
				m_eState = IDLE;

			m_bInteract = true;
		}
			
		break;
	case SLEEP:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 35 
			|| m_pTextureCom->Get_Frame().m_iCurrentTex == 41
			|| m_pTextureCom->Get_Frame().m_iCurrentTex == 49)
		{
			if (m_bFirstFrame)
			{
				// Play Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("tent_sleep_%d.wav"), rand() % 11 + 1);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_OBJECT, .4f);

				m_bFirstFrame = false;
			}
		}
		else
			m_bFirstFrame = true;

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case HIT:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case DESTROY:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

void CTent::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case STATE::PLACE:
			Change_Texture(TEXT("Com_Texture_PLACE"));
			break;
		case STATE::ENTER:
			Change_Texture(TEXT("Com_Texture_ENTER"));
			break;
		case STATE::SLEEP:
			Change_Texture(TEXT("Com_Texture_SLEEP"));
			break;
		case STATE::HIT:
			Change_Texture(TEXT("Com_Texture_HIT"));
			break;
		case STATE::DESTROY:
			Change_Texture(TEXT("Com_Texture_DESTROY"));
			break;
		}

		m_ePreState = m_eState;
	}
}

_bool CTent::Picking(_float3 * PickingPoint)
{
	return true;
}

void CTent::PickingTrue()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	//pPlayer->Set_PickingPoint(_float3(m_vecOutPos.x, m_vecOutPos.y, m_vecOutPos.z));

	cout << "Collision CTent : " << m_vecOutPos.x << " " << m_vecOutPos.y << " " << m_vecOutPos.z << endl;

	Safe_Release(pGameInstance);
}

CTent* CTent::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTent* pInstance = new CTent(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTent"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTent::Clone(void* pArg)
{
	CTent* pInstance = new CTent(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTent"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTent::Free()
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