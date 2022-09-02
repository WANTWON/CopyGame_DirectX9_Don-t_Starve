#include "stdafx.h"
#include "Terrain.h"
#include "PickingMgr.h"
#include "GameInstance.h"
#include "Transform.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CPickingMgr)


CPickingMgr::CPickingMgr()
{
}

HRESULT CPickingMgr::Ready_PickingMgr(LEVEL TerrainLevelindex)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (m_pTerrain != nullptr)
		Clear_PickingMgr();

	CTerrain* pTerrain = dynamic_cast<CTerrain*>(pGameInstance->Get_Object(TerrainLevelindex, TEXT("Layer_Terrain")));

	if (nullptr == pTerrain)
		return E_FAIL;
	m_GameObjects.push_back(pTerrain);

	return S_OK;
}

void CPickingMgr::Clear_PickingMgr()
{
	Safe_Release(m_pTerrain);

	for (auto& iter : m_GameObjects)
		Safe_Release(iter);

	m_GameObjects.clear();
}

void CPickingMgr::Add_PickingGroup(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return;
	m_GameObjects.push_back(pGameObject);
}

void CPickingMgr::Out_PickingGroup(CGameObject * pGameObject)
{
	auto& iter = m_GameObjects.begin();
	while (iter != m_GameObjects.end())
	{
		if (*iter == pGameObject)
			iter = m_GameObjects.erase(iter);
		else
			++iter;
	}
}

_bool CPickingMgr::Picking()
{
	if (m_bMouseInUI)
		return false;

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



	Safe_Release(pGameInstance);
	vector<CGameObject*> vecPicked;
	vector<_float3>		vecPos;
	_float3 vPos;

	for (auto& pGameObject : m_GameObjects)
	{
		if ( pGameObject->Picking(&vPos) == true)
		{
			vecPicked.push_back(pGameObject);
			vecPos.push_back(vPos);
		}
	}

	if (!vecPicked.empty()) //가장 z값이 작은 것을 구한다.
	{

		_float3 vecNearPos(0, 0, 999);
		int NearNum = 0;
		for (_uint i = 0; i < vecPos.size(); ++i)
		{
			if (vecPos[i].z <= vecNearPos.z)
			{
				vecNearPos = vecPos[i];
				NearNum = i;
			}

		}

		vecPicked[NearNum]->PickingTrue();
		return true;
	}



	return false;
}




void CPickingMgr::Free()
{
	Safe_Release(m_pTerrain);

	//for (auto& iter : m_GameObjects)
		//Safe_Release(iter);

	m_GameObjects.clear();
}
