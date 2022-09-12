#include "stdafx.h"
#include "..\Public\Inventory.h"
#include "Mouse.h"
#include "Player.h"
#include "GameInstance.h"

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

		if ((*iterfont)->get_pontnum() % 2 == 0) //10�ڸ�
		{
			(*iterfont)->set_pont_num(((*iter)->get_item_number() % 100) / 10);      //10�� �ڸ����� 0���� �־� (0)

			if ((*iterfont)->get_pontex() == 0)
			{
				(*iterfont)->set_check(false);
			}


		}
		else if ((*iterfont)->get_pontnum() % 2 != 0) // 1���ڸ�                 
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

}

void CInventory_Manager::Late_Tick(_float fTimeDelta)
{


	CMouse*			pMouse = CMouse::Get_Instance();
	Safe_AddRef(pMouse);



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


	Safe_Release(pMouse);


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

void CInventory_Manager::Off_pot()
{
	//m_Pot.front()->set_check_pot(false);

	for (auto& k : m_Potback)
		k->set_check_pot(false);
	for (auto& k : m_Potfront)
	{
		k->set_check_pot(false);
		k->set_texnum(ITEMNAME_END);
	}

	m_Potbutton.front()->set_check_pot(false);
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
	_uint berry = 0;

	_uint grass = 0;
	_uint wood = 0;

	//_uint rope = 0;

	for (auto& k : m_MainInventorylist)
	{
		if (k->get_texnum() == ITEMNAME_WOOD)
			wood += k->get_item_number();
		//	else if (k->get_texnum() == ITEMNAME_ROCK2)
		//		rock2 += k->get_item_number();
		//	else if (k->get_texnum() == ITEMNAME_MEAT)
		//		meat += k->get_item_number();
		else if (k->get_texnum() == ITEMNAME_GRASS)
			grass += k->get_item_number();

		else if (k->get_texnum() == ITEMNAME_BERRY)
			berry += k->get_item_number();
		//	else if (k->get_texnum() == ITEMNAME_GOLD)
		//		gold += k->get_item_number();
		//	else if (k->get_texnum() == ITEMNAME_ROPE)
		//		rope += k->get_item_number();
	}


	for (auto& k : m_Questpont)
	{
		if (m_Quest.front()->get_texnum() == 0)
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
}

void CInventory_Manager::Free()
{
	//__super::Free();

	m_MainInventorylist.clear();
	m_MainInventorybacklist.clear();
	m_Equipmentlist.clear();
	m_Pontlist.clear();
	m_BagInventorylist.clear();
	m_Mouseitemlist.clear();
	m_Mouseiteminfolist.clear();
	m_Toolboxweapon.clear();
	m_Toolboxweapon_back.clear();
	m_Toolboxweapon_front.clear();
	m_Toolboxgather.clear();
	
	m_Hppontlist.clear();
	m_Hungerpontlist.clear();
	m_Mentalitiypontlist.clear();

	m_Toolboxconstruct.clear();
	m_Toolboxconstruct_back.clear();
	m_Toolboxconstruct_front.clear();

	m_Toolboxmaterial.clear();
	m_Toolboxmaterial_back.clear();
	m_Toolboxmaterial_front.clear();


	m_Craftmain.clear();
	m_Craftmainback.clear();
	m_Craftmainfront.clear();

	m_Craftbutton.clear();
	m_Craftpont.clear();

	m_Quest.clear();
	m_Questpont.clear();
	m_Line.clear();
	m_Talk.clear();

	m_Pot.clear();
	m_Potback.clear();
	m_Potfront.clear();
	m_Potbutton.clear();

	m_Eateffect.clear();

	m_Daypont.clear();
	m_Deadmain.clear();
	m_Deadcountpont.clear();

	m_Daycountpont.clear();

}