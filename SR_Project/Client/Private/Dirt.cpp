#include "stdafx.h"
#include "../Public/Dirt.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "Level_Manager.h"
#include "CameraManager.h"
#include "Level_Maze.h"

CDirt::CDirt(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CDirt::CDirt(const CDirt & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CDirt::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDirt::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::DIRT;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	return S_OK;
}

int CDirt::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CDirt::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CDirt::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CDirt::Interact(_uint Damage)
{
	m_bInteract = false;

	Drop_Items();

	m_eState = STATE::EMPTY;
}

HRESULT CDirt::Drop_Items()
{
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_uint iLootType = rand() % 3 + 1;

	// Random Position Drop based on Object Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_float fPosX = bSignX ? (Get_Position().x + fOffsetX) : (Get_Position().x - fOffsetX);
	_float fPosZ = Get_Position().z - fOffsetZ;

	_float3 vDropPos = _float3(fPosX, Get_Position().y, fPosZ);

	switch (iLootType)
	{
	// Monster
	case 1:
	{
		_uint iMonsterIndex = rand() % m_lMonstersTable.size();
		pGameInstance->Add_GameObject(m_lMonstersTable[iMonsterIndex], LEVEL_HUNT, TEXT("Layer_Monster"), vDropPos);
	}
	break;
	// Item
	case 2:
	{
		CItem::ITEMDESC ItemDesc;
		ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

		ItemDesc.fPosition = vDropPos;
		ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
		ItemDesc.pTextureComponent = TEXT("Com_Texture_Dirt_Item");
		
		_uint iItemIndex = rand() % m_lItemIdTable.size();
		ItemDesc.eItemName = (ITEMNAME)m_lItemIdTable[iItemIndex];

		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_HUNT, TEXT("Layer_Object"), &ItemDesc);
	}
	break;
	// Next Room Key
	case 3:
	{
		CItem::ITEMDESC ItemDesc;
		ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

		ItemDesc.fPosition = vDropPos;
		ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
		ItemDesc.pTextureComponent = TEXT("Com_Texture_Dirt_Key");
		ItemDesc.eItemName = ITEMNAME_KEY;

		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_HUNT, TEXT("Layer_Object"), &ItemDesc);
	}
	break;
	}

	return S_OK;
}

HRESULT CDirt::SetUp_Components(void* pArg)
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

HRESULT CDirt::Texture_Clone()
{
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 0;

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dirt_IDLE"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Prototype_Component_Texture_Dirt"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dirt_EMPTY"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Prototype_Component_Texture_Dirt_Empty"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CDirt::Change_Frame(_float fTimeDelta)
{
	m_pTextureCom->MoveFrame(m_TimerTag);
}

void CDirt::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			Change_Texture(TEXT("Com_Texture_Dirt_IDLE"));
			break;
		case STATE::EMPTY:
			Change_Texture(TEXT("Com_Texture_Dirt_EMPTY"));
			break;
		}
		m_ePreState = m_eState;
	}
}

CDirt* CDirt::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDirt* pInstance = new CDirt(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDirt"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDirt::Clone(void* pArg)
{
	CDirt* pInstance = new CDirt(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDirt"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDirt::Free()
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