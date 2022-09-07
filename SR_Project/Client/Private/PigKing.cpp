#include "stdafx.h"
#include "PigKing.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "CameraManager.h"

CPigKing::CPigKing(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
}

CPigKing::CPigKing(const CPigKing & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CPigKing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPigKing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eInteract_OBJ_ID = INTERACTOBJ_ID::NPC;
	m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);

	return S_OK;
}

int CPigKing::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CPigKing::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	_float3 vPosition = Get_Position();
	vPosition.y -= 0.4f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	Change_Motion();
	Change_Frame();
}

HRESULT CPigKing::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPigKing::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Pig_King"))))
		return E_FAIL;
	m_TimerTag = TEXT("Timer_Pig_King");

	Safe_Release(pGameInstance);

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

	/* For.Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPigKing::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 69;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_King_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 47;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_COINTOSS"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_King_Cointoss"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 56;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_UNIMPRESSED"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_King_Unimpressed"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CPigKing::Change_Frame()
{
	switch (m_eState)
	{
	case STATE::IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::COINTOSS:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			m_bInteract = true;
			m_eState = STATE::IDLE;
			Drop_Items();
		}
		break;
	case STATE::UNIMPRESSED:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		{
			m_bInteract = true;
			m_eState = STATE::IDLE;
		}
	}
}

void CPigKing::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case STATE::COINTOSS:
			Change_Texture(TEXT("Com_Texture_COINTOSS"));
			break;
		case STATE::UNIMPRESSED:
			Change_Texture(TEXT("Com_Texture_UNIMPRESSED"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
	}
}

void CPigKing::Interact(_uint Damage)
{
	_uint count = 0;
	bool success = false;
	//_uint count1 = 0;
	if (m_bInteract)
	{
		CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		m_bInteract = false;
		CInventory_Manager*         pinven = CInventory_Manager::Get_Instance();
		Safe_AddRef(pinven);

		pCamera->Set_TalkingMode(true);
		pCamera->Set_Target(this);
		pinven->Get_Talk_list()->front()->setcheck(true); //first talking
		auto line = pinven->Get_Line_list();
		

		for (auto iter = line->begin(); iter != line->end(); ++iter)
		{
			if ((*iter)->get_check() == true)
			{
				if((*iter)->get_pontnum() == 1)
			   {
					success = true;
			    }
				/*pCamera->Set_TalkingMode(true);
				pCamera->Set_Target(this);*/
				if(m_iQuestnum ==0)
				++count;

				
			}
		}
		if (count >= 3 && m_iQuestnum == 0)
		{
			++m_iQuestnum;
			pinven->Get_Talk_list()->front()->settexnum(3);

			
		}
		else if (m_iQuestnum == 1 )
		{
			pinven->Get_Talk_list()->front()->settexnum(7);
			
			m_iQuestnum = 2;

		}
		else if (success && m_iQuestnum == 2 )
		{
			
			pinven->Get_Talk_list()->front()->settexnum(9);
		}
		/*else if (m_iQuestnum == 1 && count1 >= 1)
		{
			
		}*/
			
		
		

		Safe_Release(pinven);
		
		m_eState = rand() % 2 ? COINTOSS : UNIMPRESSED;
	}


}

void CPigKing::Find_Target()
{
	// TODO: Find Target is used to find the Player Position in order to throw the reward at the correct location.
}

HRESULT CPigKing::Drop_Items()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

	// Random Position Drop based on Object Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignX = rand() % 2;
	_float fOffsetZ = (((_float)rand() / (float)(RAND_MAX)) * .5f) + 1.f;
	/*_int bSignZ = rand() % 2;*/
	_float fPosX = bSignX ? (Get_Position().x + fOffsetX) : (Get_Position().x - fOffsetX);
	_float fPosZ = /*bSignZ ? (Get_Position().z + fOffsetZ) : (*/Get_Position().z - fOffsetZ/*)*/;

	ItemDesc.fPosition = _float3(fPosX, Get_Position().y, fPosZ);
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Gold");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_GOLD;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_GAMEPLAY, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPigKing* CPigKing::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPigKing* pInstance = new CPigKing(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPigKing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPigKing::Clone(void* pArg)
{
	CPigKing* pInstance = new CPigKing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPigKing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPigKing::Free()
{
	__super::Free();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
