#include "stdafx.h"
#include "..\Public\Inventory.h"
#include "Mouse.h"

IMPLEMENT_SINGLETON(CInventory_Manager)
CInventory_Manager::CInventory_Manager()
{
}

HRESULT CInventory_Manager::Reserve_Container(_uint iNumLevels)
{
	/*if (nullptr != m_MainInventorylist)
		return E_FAIL;

	m_MainInventorylist = new INVENLIST;

	m_iNumLevels = iNumLevels;*/

	return S_OK;
}

HRESULT CInventory_Manager::Add_GameObject( _uint iLevelIndex, void* pArg)
{
	

	/*CGameObject*		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*			pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		pLayer->Add_GameObject(pGameObject);
	}*/



	return S_OK;
}

void CInventory_Manager::Tick(_float fTimeDelta)
{
	/*for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& iter : m_MainInventorylist[i])
		{
			iter->Tick(fTimeDelta);
		}
	}*/
	
	

	


	auto iter = m_MainInventorylist.begin();
	auto iterfont = m_Pontlist.begin();
	bool bcheck = true;
	
	while (bcheck&&!m_MainInventorylist.empty() && !m_Pontlist.empty())
	{
		
		(*iterfont)->set_pont_num((*iter)->get_item_number());              //->set_pont_num(iter->get_item_number());

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
	/*for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& iter : m_MainInventorylist[i])
		{
			iter->Late_Tick(fTimeDelta);
		}
	}
*/
	CMouse*			pMouse = CMouse::Get_Instance();
	Safe_AddRef(pMouse);

	

	for (auto& i : m_MainInventorylist)
	{
		if (i->get_iNum() == pMouse->Get_index() && pMouse->Get_picked() == false)
		{
			i->set_texnum(pMouse->Get_Prev_Item_name());
			pMouse->Set_index(20);
			return;
		}
	}
	
	Safe_Release(pMouse);


}

void CInventory_Manager::Clear(_uint iLevelIndex)
{
	/*if (iLevelIndex >= m_iNumLevels ||
		nullptr == m_MainInventorylist)
		return;

	for (auto& iter : m_MainInventorylist[iLevelIndex])
		Safe_Release(iter);

	m_MainInventorylist[iLevelIndex].clear();*/

}


void CInventory_Manager::Free()
{
		
	for (auto& iter : m_MainInventorylist)
		Safe_Release(iter);

	m_MainInventorylist.clear();

	for (auto& iter : m_Equipmentlist)
		Safe_Release(iter);

	m_Equipmentlist.clear();

	for (auto& iter : m_Pontlist)
		Safe_Release(iter);

	m_Pontlist.clear();

}
