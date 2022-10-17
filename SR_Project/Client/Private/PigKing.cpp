#include "stdafx.h"
#include "PigKing.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "CameraManager.h"

_uint g_iQuestnum = 0;

CPigKing::CPigKing(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CNPC(pGraphic_Device)
{
}

CPigKing::CPigKing(const CPigKing & rhs)
	: CNPC(rhs)
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

	m_eObjID = OBJID::OBJ_NPC;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::NPC;
	
	m_eNPCID = NPCID::NPC_PIGKING;

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
	Change_Frame(fTimeDelta);
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

	/* For.Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = *(_float3*)pArg;

	MINIMAP		minidesc;
	ZeroMemory(&minidesc, sizeof(MINIMAP));
	minidesc.name = MIN_PIGKING;
	minidesc.pointer = this;

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), LEVEL_GAMEPLAY, TEXT("MiniMap_Icon"), &minidesc);
	Safe_Release(pGameInstance);

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

void CPigKing::Change_Frame(_float fTimeDelta)
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

	if (Damage == 2)
	{
		m_bInteract = true;
	}
	

	//_uint count1 = 0;
	if (m_bInteract)
	{
	
		CCameraManager::CAM_STATE eState = CCameraManager::Get_Instance()->Get_CamState();

		if (eState == CCameraManager::CAM_PLAYER)
		{
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(true);
			pCamera->Set_Target(this);
		}
		
		//m_bInteract = false;
		CInventory_Manager*         pinven = CInventory_Manager::Get_Instance();
		Safe_AddRef(pinven);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		pPlayer->Set_bOnlyActionKey(true);
		pPlayer->Set_TalkMode(true);
		

		pinven->Get_Talk_list()->front()->setcheck(true); //first talking

		pinven->Get_Talk_list()->front()->Set_Activated(true);

		auto line = pinven->Get_Line_list();
		
        for (auto iter = line->begin(); iter != line->end(); ++iter)
		{
			if ((*iter)->get_check() == true)
			{
				if((*iter)->get_pontnum() == 1)
			   {
					success = true;
			    }
				if(g_iQuestnum ==0 || g_iQuestnum == 3)
				++count;

				
			}
		}

		if (pinven->Get_Talk_list()->front()->gettexnum() == 0 && !m_bFirstTalk)
		{
			CGameInstance::Get_Instance()->PlaySounds(TEXT("DST_pose_speach_v2_wigfrid.wav"), SOUND_GROUND, 0.3f);
			m_bFirstTalk = true;
		}

		if (count >= 3 && g_iQuestnum == 0
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{
			++g_iQuestnum;
			pinven->Get_Talk_list()->front()->settexnum(3);
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
		}
		else if (g_iQuestnum == 1 
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{
			
			pinven->Get_Talk_list()->front()->settexnum(7);	
			g_iQuestnum = 2;
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
		}
		else if (success && g_iQuestnum == 2 
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{
			pinven->Get_Talk_list()->front()->settexnum(9);
			g_iQuestnum = 3;
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
		}
		else if (count >=3 && g_iQuestnum == 3
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{
			pinven->Get_Talk_list()->front()->settexnum(13);
			g_iQuestnum = 4;
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
		}
		else if (g_iQuestnum == 4
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{		
			pinven->Get_Talk_list()->front()->settexnum(16);
			g_iQuestnum = 5;
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
		}
		else if (g_iQuestnum == 5&& success 
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{
			pinven->Get_Talk_list()->front()->settexnum(19);
			g_iQuestnum = 6;
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
			
		}
		else if (g_iQuestnum == 6 && pinven->Get_Quest_list()->front()->get_spidercount() >= 30
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{
			pinven->Get_Talk_list()->front()->settexnum(26);
			g_iQuestnum = 7;
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
		}
		else if (g_iQuestnum == 7 && success
			&& pinven->Get_Talk_list()->front()->Get_TalkEnd())
		{
			pinven->Get_Talk_list()->front()->settexnum(30);
			g_iQuestnum = 7;
			pinven->Get_Talk_list()->front()->Set_TalkEnd(false);
			pinven->Get_Talk_list()->front()->Set_StartText(true);
		}
		else {
			pPlayer->Set_bOnlyActionKey(false);
		}
		pinven->Get_Talk_list()->front()->Excute();

		Safe_Release(pinven);
		
		m_eState = rand() % 2 ? COINTOSS : UNIMPRESSED;
		if(m_eState == UNIMPRESSED)
			CGameInstance::Get_Instance()->PlaySounds(TEXT("PigKing_reject.wav"), SOUND_OBJECT, 0.5f);
		else if (m_eState == COINTOSS)
			CGameInstance::Get_Instance()->PlaySounds(TEXT("PigKing_happy.wav"), SOUND_OBJECT, 0.5f);
			
	}


}

void CPigKing::Find_Target()
{
	// TODO: Find Target is used to find the Player Position in order to throw the reward at the correct location.
}

void CPigKing::Move(_float _fTimeDelta)
{
}

void CPigKing::Idle(_float _fTimeDelta)
{
}

void CPigKing::Interaction(_float _fTimedelta)
{
}

void CPigKing::Talk(_float _fTimeDelta)
{
}

void CPigKing::Dance(_float _fTimeDelta)
{
}

void CPigKing::Select_Target(_float _fTimeDelta)
{
}

void CPigKing::Set_RandPos(_float _fTimeDelta)
{
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
