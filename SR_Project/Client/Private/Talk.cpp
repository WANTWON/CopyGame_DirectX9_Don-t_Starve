#include "stdafx.h"
#include "..\Public\Talk.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "CameraManager.h"
#include "Bearger.h"
#include "Level_GamePlay.h"
CTalk::CTalk(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTalk::CTalk(const CTalk & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTalk::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTalk::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 800.f;
	m_fSizeY = 250.f;
	m_fX = 640.f;
	m_fY = 500.f;



	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	texnum = 0;

	return S_OK;
}

int CTalk::Tick(_float fTimeDelta)
{


	if (m_bcheck == true)
	{
		CInventory_Manager* pinv = CInventory_Manager::Get_Instance();

		__super::Tick(fTimeDelta);


		////////////////////////////// Bearger 등장 카메라 테스트 //////////////////////////

		/*if (CGameInstance::Get_Instance()->Key_Up(VK_F3))
		{
			if (!m_SetTargetBearger)
			{
				CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_TARGET);
				CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
				pCamera->Set_TalkingMode(true);
				CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
				pCamera->Set_Target(pGameObject);
				m_SetTargetBearger = true;
			}
			else
			{
				CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
				pCamera->Set_TalkingMode(false);
				CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
				m_SetTargetBearger = false;
			}
		}*/
		///////////////////////////////////////////////////////////////////////////////////////////
		Check_Quest();

		//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		//CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
		//Safe_AddRef(pGameInstance);
		//Safe_AddRef(pinv);

		//pinv->sethp((dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Player_Stat().fCurrentHealth));

		//texnum = pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->get_
		//texnum = (dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Player_Stat().fCurrentHealth) / 2;


		//Safe_Release(pGameInstance);
		//Safe_Release(pinv);

	}


	return OBJ_NOEVENT;
}

void CTalk::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{
		__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}

}

HRESULT CTalk::Render()
{
	if (m_bcheck == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4		ViewMatrix;
		D3DXMatrixIdentity(&ViewMatrix);

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

		


		if (m_bforwendy == true)
		{
			if (FAILED(m_pTextureCom1->Bind_OnGraphicDev(texnum1)))
				return E_FAIL;

			if (FAILED(SetUp_RenderState()))
				return E_FAIL;

			m_pVIBufferCom->Render();

			if (FAILED(Release_RenderState()))
				return E_FAIL;
		}
		else if (m_bforWinona)
		{
			if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(texnum1)))
				return E_FAIL;

			if (FAILED(SetUp_RenderState()))
				return E_FAIL;

			m_pVIBufferCom->Render();

			if (FAILED(Release_RenderState()))
				return E_FAIL;
		}
		else if (m_bforWoody)
		{
			if (FAILED(m_pTextureCom3->Bind_OnGraphicDev(texnum1)))
				return E_FAIL;

			if (FAILED(SetUp_RenderState()))
				return E_FAIL;

			m_pVIBufferCom->Render();

			if (FAILED(Release_RenderState()))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
		}


	}
	return S_OK;
}

void CTalk::Check_Quest(void)
{
	CInventory_Manager* pinv = CInventory_Manager::Get_Instance();

	if (m_bActivated == true)
	{
		if (texnum == 6)
		{
			if (m_bquest1 == false)
			{
				m_bquest1 = true;


			}
			auto pinven = pinv->Get_Inven_list();
			//Safe_Release(pinv);//üũ
			//	Safe_AddRef(pinven);

			//여기서끄자
			pinv->Get_Quest_list()->front()->set_onoff(false);
			for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
			{

				if ((*iter)->get_texnum() == ITEMNAME_BERRY)
				{
					(*iter)->minus_material(3);
				}

				if ((*iter)->get_texnum() == ITEMNAME_GRASS)
				{
					(*iter)->minus_material(2);
				}

				if ((*iter)->get_texnum() == ITEMNAME_WOOD)
				{
					(*iter)->minus_material(1);
				}
			}
			for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
			{

				if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
				{
					(*iter)->set_texnum(ITEMNAME_QUEST1);

					(*iter)->set_check(true);

					break;
				}

			}
		}
		else if (texnum == 8)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(1);
			
		}
		else if (texnum == 9)
		{
			if (m_bquest2 == false)
			{
				m_bquest2 = true;

			}
		}
		else if (texnum == 12)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(2);
			
		}
		else if (texnum == 15)
		{
			if (m_bquest3 == false)
			{
				m_bquest3 = true;

			}

		}
		else if (texnum == 16)
		{


		}
		else if (texnum == 18)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(3);
			

		}
		else if (texnum == 19)
		{


			if (m_bquest4 == false)
			{
				m_bquest4 = true;

			}
			

		}
		else if (texnum == 25)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(4);
			

		}

		else if (texnum == 26)
		{
			if (m_bquest5 == false)
			{
				m_bquest5 = true;

			}
		}

		else if (texnum == 29)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(5);
			

		}

		m_bActivated = false;
	}


}

HRESULT CTalk::Excute(void)
{

	CInventory_Manager* pinv = CInventory_Manager::Get_Instance();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	if (m_bforwendy)
	{
		/*if (texnum1 == 2 || texnum1 == 3 || texnum1 == 6 || texnum1 == 7)
		{
			m_bcheck = false;
		}*/
	}

	else if (texnum == 33)
	{
		auto pinven = pinv->Get_Inven_list();

		for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
		{

			if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
			{
				(*iter)->set_texnum(ITEMNAME_QUEST2);

				(*iter)->set_check(true);
				break;

				
			}

		}
		//pinv->Get_Quest_list()->front()->set_onoff(true);
		//pinv->Get_Quest_list()->front()->set_texnum(5);
		CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
		pCamera->Set_TalkingMode(false);
		m_bcheck = false;
		m_bTalkEnd = true;
		pPlayer->Set_TalkMode(false);
		pPlayer->Set_bOnlyActionKey(false);

		CInventory_Manager::Get_Instance()->Get_Ending_list()->front()->set_ending(true);
	}
	else
	{

		if(texnum == 2)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(0);
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			m_bcheck = false;
			//m_bActivated = false;
			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);
			m_bTalkEnd = true;
			//Safe_Release(pinv);	
		}
		else if (texnum == 6)
		{

			pinv->Get_Quest_list()->front()->set_onoff(false);
			m_bcheck = false;
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			m_bcheck = false;
			//m_bActivated = false;

			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);
			m_bTalkEnd = true;
			//Safe_Release(pinv);

		}
		else if (texnum == 8)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(1);
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			m_bcheck = false;
			//m_bActivated = false;
			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);
			m_bTalkEnd = true;
			//	Safe_Release(pinv);
		}
		else if (texnum == 12)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(2);
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			m_bcheck = false;
			//m_bActivated = false;
			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);
			m_bTalkEnd = true;
			//Safe_Release(pinv);			
		}
		else if (texnum == 15)
		{
			auto pinven = pinv->Get_Inven_list();


			for (auto iter = pinven->begin(); iter != pinven->end(); ++iter)
			{
				if ((*iter)->get_texnum() == (ITEMNAME_ROCK2) && (*iter)->get_check() == true && (*iter)->get_item_number() <= 99)
				{
					(*iter)->plus_itemcount2(20);   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가
					break;
				}
				else if ((*iter)->get_check() == false || (*iter)->get_texnum() == ITEMNAME_END)
				{
					(*iter)->set_texnum(ITEMNAME_ROCK2);
					(*iter)->plus_itemcount2(20);

					(*iter)->set_check(true);

					break;
				}

			}
			pinv->Get_Quest_list()->front()->set_onoff(false);
			m_bcheck = false;

			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			//m_bcheck = false;
			//m_bActivated = false;
			m_bTalkEnd = true;
			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);
		}
		else if (texnum == 16)
		{
			if (!m_SetTargetBearger)
			{
				CGameInstance::Get_Instance()->StopSound(SOUND_UI);
				if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_Bearger"), LEVEL_GAMEPLAY, TEXT("Layer_Monster"), _float3(10.f, 0.f, 40.f))))
					return E_FAIL;

				// Play Sound
				pGameInstance->PlaySounds(TEXT("bearger_distant_1.wav"), SOUND_ID::SOUND_MONSTER_VOICE, .8f);

				CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_TARGET);
				CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
				pCamera->Set_TalkingMode(true);
				CGameObject* pGameObject = Find_Boss();

				//CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
				pCamera->Set_Target(pGameObject);
				m_SetTargetBearger = true;

				_bool bearger = true;

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BossName"), LEVEL_STATIC, TEXT("Layer_UI"), (bool*)&bearger)))
					return E_FAIL;

			}
			else if (m_SetTargetBearger)
			{
				texnum++;
				//m_bActivated = false;
			}

		}
		else if (texnum == 18)
		{

			if (m_SetTargetBearger)
			{
				CGameInstance::Get_Instance()->StopSound(SOUND_GROUND);
				CCameraTarget* pCamera = (CCameraTarget*)CCameraManager::Get_Instance()->Get_CurrentCamera();
				pCamera->Set_TalkingMode(false);
				CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
				m_SetTargetBearger = false;
			}
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(3);
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			m_bcheck = false;
			//m_bActivated = false;
			m_bTalkEnd = true;
			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);

		}
		else if (texnum == 25)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(4);
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			m_bcheck = false;
			m_bTalkEnd = true;
			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);
		}

		else if (texnum == 29)
		{
			pinv->Get_Quest_list()->front()->set_onoff(true);
			pinv->Get_Quest_list()->front()->set_texnum(5);
			CCameraDynamic* pCamera = (CCameraDynamic*)CCameraManager::Get_Instance()->Get_CurrentCamera();
			pCamera->Set_TalkingMode(false);
			m_bcheck = false;
			m_bTalkEnd = true;
			pPlayer->Set_TalkMode(false);
			pPlayer->Set_bOnlyActionKey(false);
		}
		else
		{
			if (m_bIsStartText)
			{
				m_bIsStartText = false;
			}
			else {
				++texnum;
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();

				pGameInstance->PlaySounds(TEXT("talkclick.wav"), SOUND_UI, 0.8f);
			}
			//m_bActivated = false;
			//m_bTalkEnd = false;
		}
	}


	
	//m_bActivated = false;

	return S_OK;
}

HRESULT CTalk::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_talk"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture1"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_talkwendy"), (CComponent**)&m_pTextureCom1)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_talkwinona"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_talkwoody"), (CComponent**)&m_pTextureCom3)))
		return E_FAIL;
	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTalk::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CTalk::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CGameObject * CTalk::Find_Boss(void)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> list_Obj = *(pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Monster")));
	CGameObject* m_pTarget = nullptr;

	for (auto& iter = list_Obj.begin(); iter != list_Obj.end();)
	{
		if (*iter == nullptr)
		{
			iter++;
			continue;
		}
			

		if (static_cast<CMonster*>(*iter)->Get_MonsterID() == MONSTERID::MONSTER_BEARGER)
		{
			Safe_Release(pGameInstance);
			return *iter;
		}
		else
		{
			iter++;
		}
	}



	Safe_Release(pGameInstance);
	return nullptr;
}

CTalk * CTalk::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTalk*	pInstance = new CTalk(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTalk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalk::Clone(void* pArg)
{

	CTalk*	pInstance = new CTalk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTalk"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Talk_list()->push_back(pInstance);

	return pInstance;
}


void CTalk::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom1);
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
}
