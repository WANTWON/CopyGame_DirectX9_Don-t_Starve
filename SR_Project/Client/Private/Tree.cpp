#include "stdafx.h"
#include "../Public/Tree.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "Level_Manager.h"

CTree::CTree(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CTree::CTree(const CTree & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CTree::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::TREE;
	m_tInfo.iMaxHp = 60;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	if (CLevel_Manager::Get_Instance()->Get_PastLevelIndex() == LEVEL_GAMEPLAY)
	{
		_float fSize = 2.f;
		m_pTransformCom->Set_Scale(1.5, fSize, 1.f);
		//m_fRadius *= fSize;

	}
	else
	{
		_float fSize = (rand() % 200)*0.01f + 1.5f;
		m_pTransformCom->Set_Scale(fSize, fSize, 1.f);
		//m_fRadius *= fSize;
	}


	return S_OK;
}

int CTree::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// If Hp <= 0 : Fall and Drop Items
	if (m_tInfo.iCurrentHp <= 0 && m_eState < FALL_RIGHT)
	{
		m_bInteract = false;
		_int bLeftRight = rand() % 2;
		STATE eFall = bLeftRight ? FALL_RIGHT : FALL_LEFT;
		m_eState = eFall;

		Drop_Items();
	}
	
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CTree::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	_float3 vPosition = Get_Position();
	vPosition.y -= 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	Change_Motion();
	Change_Frame();
}

HRESULT CTree::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CTree::Interact(_uint Damage)
{
	if (m_tInfo.iCurrentHp == 0)
		return;

	if (Damage > m_tInfo.iCurrentHp)
	{
		m_tInfo.iCurrentHp = 0;

	}	
	else
		m_tInfo.iCurrentHp -= Damage;

	m_eState = CHOP;
}

HRESULT CTree::Drop_Items()
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

	// Random Position Drop based on Object Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignZ = rand() % 2;
	_float fPosX = bSignX ? (Get_Position().x + fOffsetX) : (Get_Position().x - fOffsetX);
	_float fPosZ = bSignZ ? (Get_Position().z + fOffsetZ) : (Get_Position().z - fOffsetZ);

	ItemDesc.fPosition = _float3(fPosX, Get_Position().y, fPosZ);
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Log");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_WOOD;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), iLevelIndex, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTree::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

//	m_TimerTag = TEXT("Timer_Tree");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
		//return E_FAIL;

	Safe_Release(pGameInstance);

	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
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

HRESULT CTree::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	_uint iPastLevelIndex = CLevel_Manager::Get_Instance()->Get_PastLevelIndex();

	if (iPastLevelIndex == LEVEL_GAMEPLAY)
	{
		// Tall
		TextureDesc.m_iEndTex = 79;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_IDLE"), LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 14;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_CHOP"), LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_CHOP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 37;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_FALL_RIGHT"), LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_RIGHT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 37;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_FALL_LEFT"), LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_LEFT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_STUMP"), LEVEL_HUNT, TEXT("Prototype_Component_Texture_Tree_Tall_STUMP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}
	else
	{
		// Tall
		TextureDesc.m_iEndTex = 79;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 14;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_CHOP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_CHOP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 37;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_FALL_RIGHT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_RIGHT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 37;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_FALL_LEFT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_LEFT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);

		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_STUMP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_STUMP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}

	

	return S_OK;
}

void CTree::Change_Frame()
{
	switch (m_eState)
	{
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case CHOP:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case GROW:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case FALL_LEFT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			break;
	case FALL_RIGHT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			break;
	case STUMP:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

void CTree::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CTree::IDLE:
			Change_Texture(TEXT("Com_Texture_Tall_IDLE"));
			break;
		case CTree::CHOP:
			Change_Texture(TEXT("Com_Texture_Tall_CHOP"));
			break;
		case CTree::GROW:
			Change_Texture(TEXT("Com_Texture_Tall_GROW"));
			break;
		case CTree::FALL_RIGHT:
			Change_Texture(TEXT("Com_Texture_Tall_FALL_RIGHT"));
			break;
		case CTree::FALL_LEFT:
			Change_Texture(TEXT("Com_Texture_Tall_FALL_LEFT"));
			break;
		case CTree::STUMP:
			Change_Texture(TEXT("Com_Texture_Tall_STUMP"));
			break;
		}

		m_ePreState = m_eState;
	}
}

CTree* CTree::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTree* pInstance = new CTree(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTree::Clone(void* pArg)
{
	CTree* pInstance = new CTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTree::Free()
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
