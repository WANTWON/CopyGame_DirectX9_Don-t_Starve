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
	
	auto iter = m_MainInventorylist.begin();
	auto iterfont = m_Pontlist.begin();
	bool bcheck = true;

	while (bcheck && !m_MainInventorylist.empty() && !m_Pontlist.empty())
	{



		(*iterfont)->set_pont_num((*iter)->get_item_number());              //->set_pont_num(iter->get_item_number());
		
		if ((*iter)->get_pontcheck() == false)
		{
			(*iterfont)->set_check(false);        
		}
		else if ((*iter)->get_pontcheck() == true)
		{
			(*iterfont)->set_check(true);
		}

		++iter;
		++iterfont;

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
		if (i->get_iNum() == pMouse->Get_index() && pMouse->Get_picked() == false )
		{
			if (pMouse->Get_Prev_Item_name() != ITEMNAME_END)
			{
				i->set_texnum(pMouse->Get_Prev_Item_name());
			}
			
			i->set_itemcount(pMouse->Get_Item_prev_count());
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
		if ( p->get_iNum() == 0)
		{
			
			p->set_pont_num(m_ihp/100);
		}

		if ( p->get_iNum() == 1)
		{
			p->set_pont_num((m_ihp%100)/10);
		}

		if (p->get_iNum() == 2)
		{
			p->set_pont_num(m_ihp%10);
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
}

void CInventory_Manager::Free()
{ 
	//__super::Free();
	

}