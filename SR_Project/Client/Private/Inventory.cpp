#pragma once
#include "stdafx.h"
#include "..\Public\Inventory.h"
#include "Mouse.h"
#include "Player.h"
#include "GameInstance.h"
#include "Portal.h"
#include "CameraManager.h"

IMPLEMENT_SINGLETON(CInventory_Manager)
CInventory_Manager::CInventory_Manager()
{
}

HRESULT CInventory_Manager::Reserve_Container(_uint iNumLevels)
{


	return S_OK;
}

HRESULT CInventory_Manager::Add_GameObject(_uint iLevelIndex, void* pArg)
{



	return S_OK;
}

void CInventory_Manager::Tick(_float fTimeDelta)
{
	_uint count = 0;
	auto iter = m_MainInventorylist.begin();
	auto iterfont = m_Pontlist.begin();
	bool bcheck = true;

	while (bcheck && !m_MainInventorylist.empty() && !m_Pontlist.empty())
	{





		if ((*iter)->get_pontcheck() == false)  //�κ��丮�� ��Ʈ�� false��
		{
			(*iterfont)->set_check(false);//��Ʈâ�� �ȶ߰��ұ���
		}
		else if ((*iter)->get_pontcheck() == true)
		{
			(*iterfont)->set_check(true);
		}

		if ((*iter)->get_big() == false)  //�κ��丮�� ��Ʈ�� false��
		{
			(*iterfont)->set_big(false);//��Ʈâ�� �ȶ߰��ұ���
		}
		else if ((*iter)->get_big() == true)
		{
			(*iterfont)->set_big(true);
		}

		if ((*iter)->get_bbig() == true)  //�κ��丮�� ��Ʈ�� false��
		{
			(*iterfont)->set_bbig(true);//��Ʈâ�� �ȶ߰��ұ���
		}

		if ((*iter)->get_small() == true)  //�κ��丮�� ��Ʈ�� false��
		{
			(*iterfont)->set_small(true);//��Ʈâ�� �ȶ߰��ұ���
		}
		

		if ((*iterfont)->get_pontnum() % 2 == 0) //짝수번쨰폰트 (10의자리)
		{
			(*iterfont)->set_pont_num(((*iter)->get_item_number() % 100) / 10);      //10�� �ڸ����� 0���� �־� (0)

			if ((*iterfont)->get_pontex() == 0)
			{
				(*iterfont)->set_check(false);
			}


		}
		else if ((*iterfont)->get_pontnum() % 2 != 0) // 홀수번쨰폰트 (1의자리)                
		{
			(*iterfont)->set_pont_num(((*iter)->get_item_number() % 10));

			//++iterfont;
		}
		//��Ʈ1����Ŭ ��
		++count;                                                                   //ī��Ʈ���� ��Ʈó�� �������ϱ� ��Ʈ�� 1�������γѱ���
		++iterfont;

		if (count == 2)                                                           //��������Ŭ�� ��Ʈó�� �� ������ �κ��丮 �ѱ��
		{
			++iter;
			count = 0;
		}





		//++iter;


		if (iter == m_MainInventorylist.end() || iterfont == m_Pontlist.end())
		{
			bcheck = false;
		}
	}


	
	auto iterparty = m_Partylist.begin();
	auto iterpartyhp = m_Partyhplist.begin();
	bool bcheck1 = true;

	while (bcheck1 && !m_MainInventorylist.empty() && !m_Pontlist.empty())
	{





		if ((*iterparty)->Get_check() == false)  //�κ��丮�� ��Ʈ�� false��
		{
			(*iterpartyhp)->Set_check(false);//��Ʈâ�� �ȶ߰��ұ���
		}
		else if ((*iterparty)->Get_check() == true)
		{
			(*iterpartyhp)->Set_check(true);
			(*iterpartyhp)->Set_whatnpc((*iterparty)->Get_texnum());
		}

		
		
		++iterpartyhp;
		++iterparty;
			
	



		if (iterparty == m_Partylist.end() || iterpartyhp == m_Partyhplist.end())
		{
			bcheck1 = false;
		}
	}

}

void CInventory_Manager::Late_Tick(_float fTimeDelta)
{


	CMouse*			pMouse = CMouse::Get_Instance();
	



	for (auto& i : m_MainInventorylist)
	{
		if (i->get_iNum() == pMouse->Get_index() && pMouse->Get_picked() == false)
		{
			if (pMouse->Get_Prev_Item_name() != ITEMNAME_END)
			{
				i->set_texnum(pMouse->Get_Prev_Item_name());
			}

			if (pMouse->Get_Same() == false)
			{
				i->set_itemcount(pMouse->Get_Item_prev_count()); //���⼭ �����ε��� Ŭ���ƴϸ�..
			}

			pMouse->Set_index(20);
			break;
		}
	}

	for (auto& p : m_Equipmentlist)
	{
		if (pMouse->Get_Item_type() == ITEM_HAND && p->get_iNum() == 0)
		{
			p->set_texnum(pMouse->Get_Equipment_name());
			pMouse->Set_Item_type(ITEM_END);
			break;
		}

		if (pMouse->Get_Item_type() == ITEM_BAG && p->get_iNum() == 1)
		{
			p->set_texnum(pMouse->Get_Equipment_name());
			pMouse->Set_Item_type(ITEM_END);
			break;
		}

		if (pMouse->Get_Item_type() == ITEM_ARMOR && p->get_iNum() == 2)
		{
			p->set_texnum(pMouse->Get_Equipment_name());
			pMouse->Set_Item_type(ITEM_END);
			break;
		}

		if (pMouse->Get_Item_type() == ITEM_HAT && p->get_iNum() == 3)
		{
			p->set_texnum(pMouse->Get_Equipment_name());
			pMouse->Set_Item_type(ITEM_END);
			break;
		}

	}



	for (auto& p : m_Hppontlist)
	{
		if (p->get_iNum() == 0)
		{

			p->set_pont_num(m_ihp / 100);
		}

		if (p->get_iNum() == 1)
		{
			p->set_pont_num((m_ihp % 100) / 10);
		}

		if (p->get_iNum() == 2)
		{
			p->set_pont_num(m_ihp % 10);
		}

	}



	for (auto& i : m_Hungerpontlist)
	{

		if (i->get_iNum() == 0)
		{

			i->set_pont_num(m_ihunger / 100);


		}

		if (i->get_iNum() == 1)
		{
			i->set_pont_num((m_ihunger % 100) / 10);


		}

		if (i->get_iNum() == 2)
		{
			i->set_pont_num(m_ihunger % 10);
		}

	}


	for (auto& k : m_Mentalitiypontlist)
	{

		if (k->get_iNum() == 0)
		{

			k->set_pont_num(m_mentality / 100);


		}

		if (k->get_iNum() == 1)
		{
			k->set_pont_num((m_mentality % 100) / 10);


		}

		if (k->get_iNum() == 2)
		{
			k->set_pont_num(m_mentality % 10);
		}


	}

	//	if (m_Questpont.front() != nullptr && m_Questpont.front()->get_check() == true)
	update_questpont();

	if (m_bcardgameon == true)
		Cardgame();

	
	if (icardgamecount == 9)
	{
		

		if (m_first)
		{
			for (auto& k : m_Cardgamelist)
				k->set_check(false);
			m_bcardgameon = false;

			CPortal::PORTALDESC PortalDesc;


			PortalDesc.m_eType = CPortal::PORTAL_BOSS;
			PortalDesc.vPosition = _float3(25.f, 2.f, 25.f);
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_MAZE, TEXT("Layer_Object"), &PortalDesc);

			CGameInstance::Get_Instance()->StopSound( SOUND_BGM);
			CCameraManager* pCameraManager = CCameraManager::Get_Instance();

			if (pCameraManager->Get_CamState() == CCameraManager::CAM_TARGET)
			{
				_tchar	szFullPath[MAX_PATH] = TEXT("Bell_Win_Carnivalgame_Herding_Station_DST_%02d.wav");
				_uint i = rand() % 7 + 1;
				wsprintf(szFullPath, szFullPath, i);
				CGameInstance::Get_Instance()->PlaySounds(szFullPath, SOUND_OBJECT, 0.5f);

				CCamera* pCamera = pCameraManager->Get_CurrentCamera();
				dynamic_cast<CCameraTarget*>(pCamera)->Set_TalkingMode(false);
			}

		}


		m_first = false;
		
			
	}


	


}

void CInventory_Manager::Clear(_uint iLevelIndex)
{


}

void CInventory_Manager::Use_bag()
{

	auto p = m_Equipmentlist.begin();

	++p;

	if ((*p)->get_texnum() == ITEMNAME_BAG)
	{


		for (auto& k : m_MainInventorylist)
		{
			if (k->get_iNum() >= 10)
			{
				k->set_check_bag(true);
			}
		}

		for (auto& j : m_MainInventorybacklist)
		{

			if (j->get_iNum() >= 10)
			{
				j->set_check_bag(true);
			}
		}

		m_BagInventorylist.front()->set_check_bag(true);
		m_BagInventorylist.front()->set_openbag();



	}


}

void CInventory_Manager::Off_bag()
{

	for (auto& k : m_MainInventorylist)
	{
		if (k->get_iNum() >= 10)
		{
			k->set_check_bag(false);
		}
	}

	for (auto& j : m_MainInventorybacklist)
	{

		if (j->get_iNum() >= 10)
		{
			j->set_check_bag(false);
		}
	}

	m_BagInventorylist.front()->set_closebag();      //set_check_bag(false);
}

void CInventory_Manager::Use_pot(CCookPot* pCookPot)
{

	m_Pot.front()->set_check_pot(true);

	for (auto& k : m_Potback)
		k->set_check_pot(true);
	for (auto& k : m_Potfront)
		k->set_check_pot(true);
	m_Potbutton.front()->set_check_pot(true, pCookPot);

	m_Pot.front()->set_openpot();

}

void CInventory_Manager::Off_pot(CCookPot* pCookPot)
{
	//m_Pot.front()->set_check_pot(false);

	for (auto& k : m_Potback)
		k->set_check_pot(false);
	for (auto& k : m_Potfront)
	{
		k->set_check_pot(false);
		k->set_texnum(ITEMNAME_END);
	}

	m_Potbutton.front()->set_check_pot(false, pCookPot);
	m_Pot.front()->set_closepot();
}

void CInventory_Manager::weapontool_on()
{


	for (auto& k : m_Toolboxweapon)
		k->set_onof(true);
	for (auto& k : m_Toolboxweapon_back)
		k->set_onof(true);
	for (auto& k : m_Toolboxweapon_front)
		k->set_onof(true);




}

void CInventory_Manager::weapontool_off()
{


	for (auto& k : m_Toolboxweapon)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxweapon_back)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxweapon_front)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

}

void CInventory_Manager::gathertool_on()
{


	for (auto& k : m_Toolboxgather)
		k->set_onof(true);
	for (auto& k : m_Toolboxgather_back)
		k->set_onof(true);
	for (auto& k : m_Toolboxgather_front)
		k->set_onof(true);




}

void CInventory_Manager::gathertool_off()
{


	for (auto& k : m_Toolboxgather)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxgather_back)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxgather_front)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

}

void CInventory_Manager::constructtool_on()
{
	for (auto& k : m_Toolboxconstruct)
		k->set_onof(true);
	for (auto& k : m_Toolboxconstruct_back)
		k->set_onof(true);
	for (auto& k : m_Toolboxconstruct_front)
		k->set_onof(true);
}

void CInventory_Manager::constructtool_off()
{
	for (auto& k : m_Toolboxconstruct)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxconstruct_back)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxconstruct_front)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

}

void CInventory_Manager::materialtool_on()
{
	for (auto& k : m_Toolboxmaterial)
		k->set_onof(true);
	for (auto& k : m_Toolboxmaterial_back)
		k->set_onof(true);
	for (auto& k : m_Toolboxmaterial_front)
		k->set_onof(true);
}

void CInventory_Manager::materialtool_off()
{
	for (auto& k : m_Toolboxmaterial)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxmaterial_back)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

	for (auto& k : m_Toolboxmaterial_front)
	{
		k->set_onof(false);
		k->gobackfirstX();
	}

}

void CInventory_Manager::Cardgame()
{
	_uint checktex = 10;
	_uint checktex2 = 10;
	_uint count = 0;
	_uint count2 = 0;
	_bool onetime = false;
		
	for (auto& k : m_Cardgamelist)
	{
		if (k->Get_frontcard() == true)
			++count;
	}

	if (count == 2)
	{
		for (auto& k : m_Cardgamelist)
		{
			if (k->Get_frontcard() == true)
			{
				if (onetime == true)
				{
					onetime = false;  //해야하나?
					checktex2 = k->Get_texnum1();
					break;

				}
				else
				{
					checktex = k->Get_texnum1();
					onetime = true;
				}
				


			}
		}
	}
	else
		return;


		
			for (auto& k : m_Cardgamelist)
			{
				if (checktex == checktex2)
				{
					if (k->Get_frontcard() == true)
					{
						k->Set_frontcard(false);
						k->set_lastdance(true);
						++count2;
						if (count2 == 2)
						{   
							_uint random = rand() % 2 + 1;


							if (random == 1)
								CGameInstance::Get_Instance()->PlaySounds(TEXT("cardcorrect.wav"), SOUND_UI, 0.8f);
							else
								CGameInstance::Get_Instance()->PlaySounds(TEXT("cardcorrect2.wav"), SOUND_UI, 0.8f);
							//++icardgamecount;//함수로뺴
							break;
						}
							
					}
						
				}
				else
				{
					if (k->Get_frontcard() == true)
					{
						k->Set_frontcard(false);
						k->set_wrong(true);


						//foreffect		effectdesc;
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						/*ZeroMemory(&effectdesc, sizeof(foreffect));
						
						effectdesc.pos = (dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Position());
						effectdesc.pos.y += 0.5f;
						effectdesc.whateffect = 0;
					
						pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CardEffect"), LEVEL_GAMEPLAY, TEXT("Layer_Cardeffect_thunder"), &effectdesc);*/
						++count2;
						if (count2 == 2)
						{
							_uint random = rand() % 2 + 1;


							if (random == 1)
								CGameInstance::Get_Instance()->PlaySounds(TEXT("cardwrong.wav"), SOUND_UI, 0.8f);
							else
								CGameInstance::Get_Instance()->PlaySounds(TEXT("cardwrong2.wav"), SOUND_UI, 0.8f);
							break;
							}
								

								
					}
						
				}

			}
				

		
		
           


	

	
	



}

void CInventory_Manager::Start_Cardgame()
{
	CGameInstance::Get_Instance()->PlaySounds(TEXT("cardgamestart.mp3"), SOUND_UI, 0.5f);
	CGameInstance::Get_Instance()->PlayBGM(TEXT("cardgamebgm.wav"), 0.5f);

	m_bcardgameon = true;


	CCameraManager* pCameraManager = CCameraManager::Get_Instance();
	//pCameraManager->Set_CamState(CCameraManager::CAM_TARGET);
	//CGameObject* pShooter = CGameInstance::Get_Instance()->Get_Object(LEVEL_MAZE, TEXT("Layer_Shooter"));
	//_float3 vShooterPos = pShooter->Get_Position();
	//vShooterPos.z += 2.f;
	//CCamera* pCamera = pCameraManager->Get_CurrentCamera();
	//dynamic_cast<CCameraTarget*>(pCamera)->Set_OffSetDistance(_float3(0.f, 7.f, -6.f));
	//dynamic_cast<CCameraTarget*>(pCamera)->Set_Position(vShooterPos);
	//dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(true);


	pCameraManager->Set_CamState(CCameraManager::CAM_TARGET);
	_float3 vCameraLookPosition = _float3(26.f, 0.f, 25.f);
	CCamera* pCamera = pCameraManager->Get_CurrentCamera();
	dynamic_cast<CCameraTarget*>(pCamera)->Set_OffSetDistance(_float3(0.f, 20.f, -5.f));
	dynamic_cast<CCameraTarget*>(pCamera)->Set_Position(vCameraLookPosition);
	dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(true);


	for (auto& k : m_Cardgamelist)
	{
		k->set_check(true);
		k->set_goback(true);
	}
		
}

void CInventory_Manager::Dead_on()
{
	//m_Deadmain.front()->gobackfirstY();
	m_Deadmain.front()->set_check(true);


	for (auto& k : m_Deadcountpont)
	{
		k->set_check(true);
		k->timeron();
		//k->gobackfirstY();
	}

	for (auto& k : m_Daypont)
	{
		k->set_check(true);
		k->set_daycount(m_Daycountpont.front()->get_daycount());
		//k->gobackfirstX();
	}

}

void CInventory_Manager::Dead_off()
{

	m_Deadmain.front()->set_check(false);
	//m_Deadmain.front()->gobackfirstY();

	for (auto& k : m_Deadcountpont)
	{
		k->set_check(false);
		//k->gobackfirstY();
	}

	for (auto& k : m_Daypont)
	{
		k->set_check(false);
		//k->gobackfirstX();
	}


}

void CInventory_Manager::craft_on(MAKEWHAT item)
{
	for (auto& k : m_Craftmain)
	{
		k->set_onof(true);
		k->set_makewhat(item);
	}

	for (auto& k : m_Craftmainback)
	{
		k->set_onof(true);
		k->set_makewhat(item);
	}

	for (auto& k : m_Craftmainfront)
	{
		k->set_onof(true);
		k->set_makewhat(item);
	}

	for (auto& k : m_Craftbutton)
	{
		k->set_onof(true);
		k->set_makewhat(item);
	}

	for (auto& k : m_Craftpont)
	{
		k->set_check(true);
		k->set_makewhat(item);
	}
	update_craftpont();

}

void CInventory_Manager::craft_off()
{

	for (auto& k : m_Craftmain)
	{
		k->set_onof(false);
		k->gobackfirstY();
		k->gobackfirstX();
	}

	for (auto& k : m_Craftmainback)
	{
		k->set_onof(false);
		k->gobackfirstY();
		k->gobackfirstX();
	}
	for (auto& k : m_Craftmainfront)
	{
		k->set_onof(false);
		k->gobackfirstY();
		k->gobackfirstX();
	}
	for (auto& k : m_Craftbutton)
	{
		k->set_onof(false);
		k->gobackfirstY();
		k->gobackfirstX();
	}

	for (auto& k : m_Craftpont)
	{
		k->set_check(false);
		k->gobackfirstY();
		k->gobackfirstX();
	}
}

void CInventory_Manager::update_craftpont()
{
	_uint wood = 0;
	_uint rock2 = 0;
	_uint meat = 0;
	_uint grass = 0;
	_uint gold = 0;
	_uint rope = 0;
	_uint pigtail = 0;

	for (auto& k : m_MainInventorylist)
	{
		if (k->get_texnum() == ITEMNAME_WOOD)
			wood += k->get_item_number();
		else if (k->get_texnum() == ITEMNAME_ROCK2)
			rock2 += k->get_item_number();
		else if (k->get_texnum() == ITEMNAME_MEAT)
			meat += k->get_item_number();
		else if (k->get_texnum() == ITEMNAME_GRASS)
			grass += k->get_item_number();
		else if (k->get_texnum() == ITEMNAME_GOLD)
			gold += k->get_item_number();
		else if (k->get_texnum() == ITEMNAME_ROPE)
			rope += k->get_item_number();
		else if (k->get_texnum() == ITEMNAME_PIGTAIL)
			pigtail += k->get_item_number();
	}


	for (auto& k : m_Craftpont)
	{
		if (m_Craftmain.front()->get_makewhat() == MAKE_AXE || m_Craftmain.front()->get_makewhat() == MAKE_PICK)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((wood % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(wood % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((rock2 % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(rock2 % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(1);
				return;
			}

		}
		else if (m_Craftmain.front()->get_makewhat() == MAKE_HAMBAT)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((wood % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(wood % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((meat % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(meat % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(3);
				return;
			}
		}
		else if (m_Craftmain.front()->get_makewhat() == MAKE_ARMOR || m_Craftmain.front()->get_makewhat() == MAKE_HELMET)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((wood % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(wood % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((rock2 % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(rock2 % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(3);
				return;
			}
		}
		else if (m_Craftmain.front()->get_makewhat() == MAKE_SHOTTER)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((grass % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(grass % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(2);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((rope % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(rope % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(2);
				return;
			}
		}
		else if (m_Craftmain.front()->get_makewhat() == MAKE_STAFF)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((wood % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(wood % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(2);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((gold % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(gold % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(2);
				return;
			}
		}

		else if (m_Craftmain.front()->get_makewhat() == MAKE_FENCE)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((wood % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(wood % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((rope % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(rope % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(1);
				return;
			}
		}

		else if (m_Craftmain.front()->get_makewhat() == MAKE_POT)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((rock2 % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(rock2 % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((pigtail % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(pigtail % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(1);
				return;
			}
		}

		else if (m_Craftmain.front()->get_makewhat() == MAKE_TENT)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((grass % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(grass % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(2);

			}

			else if (k->get_pontnum() == 4)
			{
				k->set_pont_num((rope % 100) / 10);
			}
			else if (k->get_pontnum() == 5)
			{
				k->set_pont_num(rope % 10);
			}

			else if (k->get_pontnum() == 7)
			{
				k->set_pont_num(1);
				return;
			}
		}

		else if (m_Craftmain.front()->get_makewhat() == MAKE_ROPE)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((grass % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(grass % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);
				return;
			}


		}

		else if (m_Craftmain.front()->get_makewhat() == MAKE_COAL)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((rock2 % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(rock2 % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);
				return;
			}


		}
		else if (m_Craftmain.front()->get_makewhat() == MAKE_TORCH)
		{
			if (k->get_pontnum() == 0)
			{
				k->set_pont_num((wood % 100) / 10);
			}
			else if (k->get_pontnum() == 1)
			{
				k->set_pont_num(wood % 10);
			}


			else if (k->get_pontnum() == 3)
			{
				k->set_pont_num(1);
				return;
			}


		}

	}






}

void CInventory_Manager::update_questpont() //for questpont
{
	
	for (auto& k : m_Quest)
	{
		if (k->get_texnum() == 0)
		{

			_uint berry = 0;

			_uint grass = 0;
			_uint wood = 0;

			//_uint rope = 0;

			for (auto& k : m_MainInventorylist)
			{
				if (k->get_texnum() == ITEMNAME_WOOD)
					wood += k->get_item_number();
				else if (k->get_texnum() == ITEMNAME_GRASS)
					grass += k->get_item_number();

				else if (k->get_texnum() == ITEMNAME_BERRY)
					berry += k->get_item_number();

			}


			for (auto& k : m_Questpont)
			{


				if (k->get_pontnum() == 0)
				{
					k->set_pont_num(berry % 10);
				}


				else if (k->get_pontnum() == 1)
				{
					k->set_pont_num(grass % 10);

				}


				else if (k->get_pontnum() == 2)
				{
					k->set_pont_num(wood % 10);
				}
			}
		}
		
		else if (k->get_texnum() == 4)
		{


				for (auto& k : m_Questpont)
				{
					if (k->get_pontnum() == 0)
					{
						k->set_pont_num((m_Quest.front()->get_spidercount() % 100) / 10);

						//if(m_Quest.front()->get_spidercount() >= 10)
						k->set_check(true);

						
					
					}

					if (k->get_pontnum() == 1)
					{
						k->set_pont_num(m_Quest.front()->get_spidercount() % 10);
						k->set_check(true);
					}
				}

			}

		}

		
	
	
}

void CInventory_Manager::Free()
{
	//__super::Free();
	//unsigned long refcnt = get_refcnt();

	//for (auto& iter : m_MainInventorylist)
	//	Safe_Release(iter);
	m_MainInventorylist.clear();
	//for (auto& iter : m_MainInventorybacklist)
	//	Safe_Release(iter);
	m_MainInventorybacklist.clear();
	//for (auto& iter : m_Equipmentlist)
	//	Safe_Release(iter);
	m_Equipmentlist.clear();
	//for (auto& iter : m_Pontlist)
		//Safe_Release(iter);
	m_Pontlist.clear();
	//for (auto& iter : m_BagInventorylist)
		//Safe_Release(iter);
	m_BagInventorylist.clear();
	//for (auto& iter : m_Mouseitemlist)
		//Safe_Release(iter);
	m_Mouseitemlist.clear();
	//for (auto& iter : m_Mouseiteminfolist)
		//Safe_Release(iter);
	m_Mouseiteminfolist.clear();
	
	//for (auto& iter : m_Toolboxweapon)
		//Safe_Release(iter);
	m_Toolboxweapon.clear();
	//for (auto& iter : m_Toolboxweapon_back)
		//Safe_Release(iter);
	m_Toolboxweapon_back.clear();
	//for (auto& iter : m_Toolboxweapon_front)
		//Safe_Release(iter);
	m_Toolboxweapon_front.clear();


	//for (auto& iter : m_Toolboxgather)
		//Safe_Release(iter);
	m_Toolboxgather.clear();
	//for (auto& iter : m_Toolboxgather_front)
		//Safe_Release(iter);
	m_Toolboxgather_front.clear();
	//for (auto& iter : m_Toolboxgather_back)
	//	Safe_Release(iter);
	m_Toolboxgather_back.clear();


	//for (auto& iter : m_Hppontlist)
	//	Safe_Release(iter);
	m_Hppontlist.clear();
	//for (auto& iter : m_Hungerpontlist)
	//	Safe_Release(iter);
	m_Hungerpontlist.clear();
	//for (auto& iter : m_Mentalitiypontlist)
	//	Safe_Release(iter);
	m_Mentalitiypontlist.clear();


	//for (auto& iter : m_Toolboxconstruct)
	//	Safe_Release(iter);
	m_Toolboxconstruct.clear();
	//for (auto& iter : m_Toolboxconstruct_back)
	//	Safe_Release(iter);
	m_Toolboxconstruct_back.clear();
	//for (auto& iter : m_Toolboxconstruct_front)
	//	Safe_Release(iter);
	m_Toolboxconstruct_front.clear();

	//for (auto& iter : m_Toolboxmaterial)
	//	Safe_Release(iter);
	m_Toolboxmaterial.clear();
	//for (auto& iter : m_Toolboxmaterial_back)
	//	Safe_Release(iter);
	m_Toolboxmaterial_back.clear();
	//for (auto& iter : m_Toolboxmaterial_front)
	//	Safe_Release(iter);
	m_Toolboxmaterial_front.clear();

    //for (auto& iter : m_Craftmain)
	//	Safe_Release(iter);
	m_Craftmain.clear();
	//for (auto& iter : m_Craftmainback)
	//	Safe_Release(iter);
	m_Craftmainback.clear();
	//for (auto& iter : m_Craftmainfront)
		//Safe_Release(iter);
	m_Craftmainfront.clear();


	//for (auto& iter : m_Craftbutton)
		//Safe_Release(iter);
	m_Craftbutton.clear();
	//for (auto& iter : m_Craftpont)
	//	Safe_Release(iter);
	m_Craftpont.clear();

	//for (auto& iter : m_Quest)
	//	Safe_Release(iter);
	m_Quest.clear();
	//for (auto& iter : m_Questpont)
	//	Safe_Release(iter);
	m_Questpont.clear();
	//for (auto& iter : m_Line)
	//	Safe_Release(iter);
	m_Line.clear();
	//for (auto& iter : m_Talk)
	//	Safe_Release(iter);
	m_Talk.clear();


	//for (auto& iter : m_Pot)
		//Safe_Release(iter);
	m_Pot.clear();
	//for (auto& iter : m_Potback)
	//	Safe_Release(iter);
	m_Potback.clear();
	//for (auto& iter : m_Potfront)
	//	Safe_Release(iter);
	m_Potfront.clear();
	//for (auto& iter : m_Potbutton)
	//	Safe_Release(iter);
	m_Potbutton.clear();


	//for (auto& iter : m_Eateffect)
	//	Safe_Release(iter);
	m_Eateffect.clear();


	//for (auto& iter : m_Daypont)
	//	Safe_Release(iter);
	m_Daypont.clear();
	//for (auto& iter : m_Deadmain)
	//	Safe_Release(iter);
	m_Deadmain.clear();
	//for (auto& iter : m_Deadcountpont)
	//	Safe_Release(iter);
	m_Deadcountpont.clear();


	//for (auto& iter : m_Daycountpont)
	//	Safe_Release(iter);
	m_Daycountpont.clear();

	//for (auto& iter : m_Mousemonsterinfolist)
	//	Safe_Release(iter);
	m_Mousemonsterinfolist.clear();
//	for (auto& iter : m_Loadingscenelist)
//		Safe_Release(iter);
	m_Loadingscenelist.clear();
	//for (auto& iter : m_Monsterhppontlist)
	//	Safe_Release(iter);
	m_Monsterhppontlist.clear();


	//for (auto& iter : m_Skill)
	//	Safe_Release(iter);
	m_Skill.clear();
	//for (auto& iter : m_Minimaplist)
	//	Safe_Release(iter);
	m_Minimaplist.clear();
	//for (auto& iter : m_Minimapiconlist)
	//	Safe_Release(iter);
	m_Minimapiconlist.clear();



}