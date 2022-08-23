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

HRESULT CPickingMgr::Ready_PickingMgr(PDIRECT3DDEVICE9 pGraphic_Device)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(pGameInstance->Get_Object(LEVEL_GAMEPLAY, L"Layer_Terrain"));
	
	if (nullptr == pTerrain)
		return E_FAIL;
	m_pTerrain = pTerrain;

	return S_OK;
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

CGameObject * CPickingMgr::Picking(POINT & pt)
{
	vector<CGameObject*> vecPicked;

	for (auto& pGameObject : m_GameObjects)
	{
		/*if (pGameObject->Picking(pt))
		{
			vecPicked.push_back(pGameObject);
		}*/

	}

	if (!vecPicked.empty())
	{
		//가장 z값이 작은 것을 구한다.
		sort(vecPicked.begin(), vecPicked.end(), [](CGameObject* pA, CGameObject* pB) {
			CTransform* pTransformA = (CTransform*)pA->Find_Component(L"Com_Transform");
			CTransform* pTransformB = (CTransform*)pB->Find_Component(L"Com_Transform");
			if (nullptr == pTransformA || nullptr == pTransformB)
				return false;
			return pTransformA->Get_State(CTransform::STATE_POSITION).z > pTransformB->Get_State(CTransform::STATE_POSITION).z;
		});

	}

	if (vecPicked.empty())
		return nullptr;

	return vecPicked.front();
}

CGameObject * CPickingMgr::Get_Closest_Object(_float3 vOrigin, CGameObject* pSelf)
{
	if (nullptr == m_pTerrain)
		return nullptr;


	_float fMinDist = FLT_MAX;
	CGameObject* pClosest = nullptr;
	//해당 위치에 가장 가까운 오브젝트를 찾는다.
	for (auto& pObj : m_GameObjects)
	{
		if (pObj == pSelf)
			continue;

		CTransform* pTransform = (CTransform*)pObj->Find_Component(L"Com_Transform");
		if (nullptr == pTransform)
			return nullptr;

		_float fDist = D3DXVec3Length(&(vOrigin - pTransform->Get_State(CTransform::STATE_POSITION)));
		if (fMinDist > fDist)
		{
			fMinDist = fDist;
			pClosest = pObj;
		}

		return pClosest;
	}

	return nullptr;
}

_bool CPickingMgr::TerrainPicking()
{
	return true;
}


void CPickingMgr::Free()
{
	Safe_Release(m_pTerrain);
	for (auto& iter : m_GameObjects)
		Safe_Release(iter);

	m_GameObjects.clear();
}
