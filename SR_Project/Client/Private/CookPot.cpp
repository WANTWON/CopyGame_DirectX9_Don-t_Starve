#include "stdafx.h"
#include "CookPot.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "PickingMgr.h"
#include "ParticleSystem.h"
#include "Particle.h"

CCookPot::CCookPot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCookPot::CCookPot(const CCookPot & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCookPot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookPot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eInteract_OBJ_ID = INTERACTOBJ_ID::COOKPOT;

	return S_OK;
}

int CCookPot::Tick(_float fTimeDelta)
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

void CCookPot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/*if (!m_bPicking)
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}*/

	Change_Motion();
	Change_Frame();
}

HRESULT CCookPot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCookPot::Interact(_uint Damage)
{
	m_bInteract = false;

	m_eState = COOKING;
	// TODO: ..
	// Open Craft Window Here.
}

HRESULT CCookPot::Drop_Items()
{
	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	//Safe_AddRef(pGameInstance);

	//CItem::ITEMDESC ItemDesc;
	//ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

	//// Random Position Drop based on Object Position
	//_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * 1;
	//_int bSignX = rand() % 2;
	//_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * 1;
	//_int bSignZ = rand() % 2;
	//_float fPosX = bSignX ? (Get_Position().x + fOffsetX) : (Get_Position().x - fOffsetX);
	//_float fPosZ = bSignZ ? (Get_Position().z + fOffsetZ) : (Get_Position().z - fOffsetZ);

	//ItemDesc.fPosition = _float3(fPosX, Get_Position().y, fPosZ);
	//ItemDesc.pTextureComponent = TEXT("Com_Texture_Rocks");
	//ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	//ItemDesc.eItemName = ITEMNAME::ITEMNAME_ROCK2;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_GAMEPLAY, TEXT("Layer_Object"), &ItemDesc)))
	//	return E_FAIL;

	//Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CCookPot::SetUp_Components(void* pArg)
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

HRESULT CCookPot::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 22;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PLACE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_COOKING"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Cooking"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCookPot::Change_Frame()
{
	switch (m_eState)
	{
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case PLACE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
			break;
	case COOKING:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	}
}

void CCookPot::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CCookPot::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case CCookPot::PLACE:
			Change_Texture(TEXT("Com_Texture_PLACE"));
			break;
		case CCookPot::COOKING:
			Change_Texture(TEXT("Com_Texture_COOKING"));
			break;
		}

		m_ePreState = m_eState;
	}
}

_bool CCookPot::Picking(_float3 * PickingPoint)
{
	/*if (true == m_pVIBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		m_vecOutPos = *PickingPoint;
		return true;
	}
	else
		return false;*/
	return true;
}

void CCookPot::PickingTrue()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	//pPlayer->Set_PickingPoint(_float3(m_vecOutPos.x, m_vecOutPos.y, m_vecOutPos.z));

	cout << "Collision CookPot : " << m_vecOutPos.x << " " << m_vecOutPos.y << " " << m_vecOutPos.z << endl;

	Safe_Release(pGameInstance);
}

CCookPot* CCookPot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCookPot* pInstance = new CCookPot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCookPot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCookPot::Clone(void* pArg)
{
	CCookPot* pInstance = new CCookPot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCookPot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCookPot::Free()
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