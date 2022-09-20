#include "stdafx.h"
#include "../Public/Grass.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"

CGrass::CGrass(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
}

CGrass::CGrass(const CGrass & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CGrass::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	//Test
	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::GRASS;

	m_pTransformCom->Set_Scale(0.6f, 1.f, 1.f);

	return S_OK;
}

int CGrass::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CGrass::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame();
}

HRESULT CGrass::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CGrass::Interact(_uint Damage)
{
	if (m_eState < PICK)
	{
		m_bInteract = false;
		m_eState = PICK;
		Drop_Items();
	}
}

HRESULT CGrass::Drop_Items()
{
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
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Cut_Grass");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_GRASS;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_GAMEPLAY, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGrass::Destroy()
{
	__super::Destroy();

	m_eState = STATE::PICK;
}

HRESULT CGrass::SetUp_Components(void* pArg)
{
	

	//m_TimerTag = TEXT("Timer_Grass");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
		//return E_FAIL;

	
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(Texture_Clone()))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	/*if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;*/

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
	minidesc.name = MIN_GRASS;
	minidesc.pointer = this;

	LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
	//LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
	switch (CurrentLevelndex)
	{
	case LEVEL_HUNT:
		break;
	case LEVEL_MAZE:
		break;
	case LEVEL_BOSS:
		break;
	default:
		CurrentLevelndex = LEVEL_GAMEPLAY;
		break;
	}
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), CurrentLevelndex, TEXT("MiniMap_Icon"), &minidesc);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.25f;
	CollRectDesc.fRadiusX = 0.25f;
	CollRectDesc.fRadiusZ = 0.5f;
	CollRectDesc.fOffSetX = 0.0f;
	CollRectDesc.fOffSetY = -.5f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Texture_Clone()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
	if (iLevel == LEVEL_HUNT)
		iLevel = LEVEL_HUNT;
	else
		iLevel = LEVEL_GAMEPLAY;

	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 20;

	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), iLevel, TEXT("Prototype_Component_Texture_Grass_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PICK"), iLevel, TEXT("Prototype_Component_Texture_Grass_PICK"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PICKED"), iLevel, TEXT("Prototype_Component_Texture_Grass_PICKED"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CGrass::Change_Frame()
{
	switch (m_eState)
	{
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case PICK:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			break;
	case PICKED:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

void CGrass::Change_Motion()
{
	// Change Texture based on State
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CGrass::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case CGrass::PICK:
			Change_Texture(TEXT("Com_Texture_PICK"));
			break;
		case CGrass::PICKED:
			Change_Texture(TEXT("Com_Texture_PICKED"));
			break;
		}

		m_ePreState = m_eState;
	}
}

CGrass* CGrass::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGrass* pInstance = new CGrass(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CGrass"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrass::Clone(void* pArg)
{
	CGrass* pInstance = new CGrass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CGrass"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CGrass::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}