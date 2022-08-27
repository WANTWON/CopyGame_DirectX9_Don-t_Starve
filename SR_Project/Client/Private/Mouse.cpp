#include "stdafx.h"
#include "..\Public\Mouse.h"

IMPLEMENT_SINGLETON(CMouse)

CMouse::CMouse()
	: m_iboxindex(20)
{
}








void CMouse::Clear(_uint iLevelIndex)
{
	/*if (iLevelIndex >= m_iNumLevels ||
	nullptr == m_MainInventorylist)
	return;

	for (auto& iter : m_MainInventorylist[iLevelIndex])
	Safe_Release(iter);

	m_MainInventorylist[iLevelIndex].clear();*/

}

//CGameObject * CObject_Manager::Find_Objects(_uint iLevelIndex, const _tchar * pLayerTag)
//{
//	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
//	if (pLayer == nullptr)
//		return nullptr;
//
//	return pLayer->Get_Object();
//}
//
//CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
//{
//	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));
//
//	if (iter == m_Prototypes.end())
//		return nullptr;
//
//	return iter->second;
//}
//
//CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
//{
//	if (iLevelIndex >= m_iNumLevels)
//		return nullptr;
//
//	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(pLayerTag));
//	if (iter == m_pLayers[iLevelIndex].end())
//		return nullptr;
//
//	return iter->second;
//}

void CMouse::Free()
{
	/*for (_uint i = 0; i < m_iNumLevels; ++i)
	{
	for (auto& iter : m_MainInventorylist[i])
	Safe_Release(m_MainInventorylist);

	m_MainInventorylist[i].clear();
	}
	*/
	//for (auto& iter : m_MainInventorylist)
	//Safe_Release(m_MainInventorylist);

	/*for (auto& iter : m_MainInventorylist)
	{

	Safe_Release(iter);

	iter.clear();
	}
	*/


	//Safe_Delete_Array(m_MainInventorylist);

}
