#include "stdafx.h"
#include "..\Public\Portable_Pot.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"

CPortable_Pot::CPortable_Pot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
}

CPortable_Pot::CPortable_Pot(const CPortable_Pot & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CPortable_Pot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortable_Pot::Initialize(void* pArg)
{
	ZeroMemory(&m_tDesc, sizeof(m_tDesc));
	m_tDesc = *(POTDESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eInteract_OBJ_ID = INTERACTOBJ_ID::PORTABLE_POT;
	m_eObjID = OBJID::OBJ_OBJECT;
	return S_OK;
}

int CPortable_Pot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_bDead)
		return OBJ_DEAD;

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CPortable_Pot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CPortable_Pot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPortable_Pot::Interact(_uint Damage)
{
	if (m_eState == IDLE && m_bInteract)
	{
		Destroy();
		m_bInteract = false;
	}

}

HRESULT CPortable_Pot::Drop_Items()
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
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Berries");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME_BERRY;




	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_GAMEPLAY, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPortable_Pot::Destroy()
{
	__super::Destroy();

	Drop_Items();
	m_bDead = true;

}

HRESULT CPortable_Pot::SetUp_Components(void* pArg)
{
	//m_TimerTag = TEXT("Timer_BerryBush");
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

	/* For.Com_Collider*/
	//if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
	//	return E_FAIL;

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

HRESULT CPortable_Pot::Texture_Clone()
{

	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), iLevel, TEXT("Prototype_Component_Texture_Berry_Bush_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 13;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PICK"), iLevel, TEXT("Prototype_Component_Texture_Berry_Bush_PICK"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PICKED"), iLevel, TEXT("Prototype_Component_Texture_Berry_Bush_PICKED"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CPortable_Pot::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case PLACE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = COOK;
		break;
	case COOK:
		m_pTextureCom->MoveFrame(m_TimerTag, true);
		break;
	case DESTROY:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case IDLE:
		m_pTextureCom->Get_Frame().m_iCurrentTex = 17;
		break;
	}
}

void CPortable_Pot::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CPortable_Pot::PLACE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case CPortable_Pot::COOK:
			Change_Texture(TEXT("Com_Texture_PICK"));
			break;
		case CPortable_Pot::DESTROY:
		case CPortable_Pot::IDLE:
			Change_Texture(TEXT("Com_Texture_PICKED"));
			break;
		}

		m_ePreState = m_eState;
	}
}

CPortable_Pot* CPortable_Pot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPortable_Pot* pInstance = new CPortable_Pot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPortable_Pot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPortable_Pot::Clone(void* pArg)
{
	CPortable_Pot* pInstance = new CPortable_Pot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPortable_Pot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPortable_Pot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);

	//for (auto& iter : m_vecTexture)
	//Safe_Release(iter);

	m_vecTexture.clear();
}