#include "stdafx.h"
#include "../Public/Carrot.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "Level_Manager.h"
#include "CameraManager.h"
#include "Level_Maze.h"

CCarrot::CCarrot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarrot::CCarrot(const CCarrot & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCarrot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarrot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::CARROT;

	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
	if(iLevel == LEVEL_MAZE)
		m_pTransformCom->Set_Scale(1.f, 2.5f, 1.f);
	else
		m_pTransformCom->Set_Scale(.5f, .5f, .5f);

	return S_OK;
}

int CCarrot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarrot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	
	Change_Motion();
	Change_Frame();
}

HRESULT CCarrot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCarrot::Interact(_uint Damage)
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	if (iLevel != LEVEL_MAZE)
		Drop_Items();

	m_bInteract = false;
	m_bDead = true;
	if (m_bDead)
	{
		CGameInstance* pInstance = CGameInstance::Get_Instance();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"), 0));

		if (iLevel == LEVEL_MAZE)
		{
			dynamic_cast<CPlayer*>(pPlayer)->Set_FPSMode(false);
			CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
			CLevel* pLevel =  CLevel_Manager::Get_Instance()->Get_CurrentLevel();
			dynamic_cast<CLevel_Maze*>(pLevel)->Set_Flowerpicked(true);
		}
		
		pPlayer->Check_Target(m_bDead);
	}
}

HRESULT CCarrot::Drop_Items()
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
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Carrot");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_CARROT;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), iLevelIndex, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CCarrot::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	

	//	m_TimerTag = TEXT("Timer_Carrot");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	Safe_Release(pGameInstance);
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

HRESULT CCarrot::Texture_Clone()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_DestinationLevelIndex();
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	if (iLevel == LEVEL_MAZE)
	{
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_fSpeed = 45;
		TextureDesc.m_iEndTex = 24;
	}
	else
	{
		TextureDesc.m_iStartTex = 0;
		TextureDesc.m_fSpeed = 60;
		TextureDesc.m_iEndTex = 0;
		iLevel = LEVEL_GAMEPLAY;
	}
	
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Carrot_IDLE"), iLevel, TEXT("Prototype_Component_Texture_Carrot"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCarrot::Change_Frame()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	switch (m_eState)
	{
	case IDLE:
		if(iLevel == LEVEL_GAMEPLAY)
			m_pTextureCom->MoveFrame(m_TimerTag, false);
		else
			m_pTextureCom->MoveFrame(m_TimerTag, true);
		break;
	}
}

void CCarrot::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CCarrot::IDLE:
			Change_Texture(TEXT("Com_Texture_Carrot_IDLE"));
			break;
		}
		m_ePreState = m_eState;
	}
}

CCarrot* CCarrot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarrot* pInstance = new CCarrot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarrot::Clone(void* pArg)
{
	CCarrot* pInstance = new CCarrot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarrot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarrot::Free()
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