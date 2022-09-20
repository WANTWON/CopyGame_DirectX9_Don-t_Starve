#include "..\Public\Collider_Manager.h"
#include "Component.h"
#include "Transform.h"
#include "Collider_Rect.h"
#include "Collider_Cube.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCollider_Manager)

CCollider_Manager::CCollider_Manager(/*LPDIRECT3DDEVICE9 pGraphic_Device*/)
{
}

HRESULT CCollider_Manager::Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[eCollisionGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

void CCollider_Manager::Out_CollisiomGroup(COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
		auto& iter = m_GameObjects[eCollisionGroup].begin();
		while (iter != m_GameObjects[eCollisionGroup].end())
		{
			if (*iter == pGameObject)
				iter = m_GameObjects[eCollisionGroup].erase(iter);
			else
				++iter;
		}
}


HRESULT CCollider_Manager::Reset_ColliderGroup()
{
	for (_uint i = 0; i < COLLISION_GROUPEND; ++i)
	{
		for (auto& pGameObject : m_GameObjects[i])
		{
			if (nullptr != pGameObject)
			{
				Safe_Release(pGameObject);
			}
		}
		m_GameObjects[i].clear();
	}

	return S_OK;
}
_bool  CCollider_Manager::Collision_with_Group(COLLISION_GROUP eGroup, class CGameObject* pGameObject, COLLISION_TYPE eCollisionType, _float3* pOutDistance )
{
	CComponent* Target = nullptr;
	CComponent* DamageOwner = nullptr;

	for (auto& iter : m_GameObjects[eGroup])
	{
		if (nullptr != iter)
		{
			switch (eCollisionType)
			{
			case Engine::CCollider_Manager::COLLISION_RECT:
				 DamageOwner = (CCollider_Rect*)pGameObject->Find_Component(TEXT("Com_Collider_Rect"));
				 Target = (CCollider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
				if (Target == nullptr)
					continue;
				if (true == (dynamic_cast<CCollider_Rect*>(DamageOwner)->Collision_Check((CCollider_Rect*)Target, pOutDistance)))
					return true;
				break;
			case Engine::CCollider_Manager::COLLSIION_BOX:
				 DamageOwner = (CCollider_Cube*)pGameObject->Find_Component(TEXT("Com_Collider_Cube"));
				 Target = (CCollider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
				if (Target == nullptr)
					continue;
				if (true == ((dynamic_cast<CCollider_Cube*>(DamageOwner)->Collision_Check((CCollider_Cube*)Target, pOutDistance))))
					return true;
				break;
			default:
				break;
			}
			
		}
	}

	return false;
}

_bool CCollider_Manager::Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, CGameObject * pDamageCauser, COLLISION_TYPE eCollisionType)
{
	CComponent* Target = nullptr;
	CComponent* DamageOwner = nullptr;

	for (auto& iter : m_GameObjects[eGroup])
	{
		if (nullptr != iter)
		{
			switch (eCollisionType)
			{
			case Engine::CCollider_Manager::COLLISION_RECT:
				DamageOwner = (CCollider_Rect*)pDamageCauser->Find_Component(TEXT("Com_Collider_Rect"));
				Target = (CCollider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
				if (Target == nullptr)
					continue;

				if (true == (dynamic_cast<CCollider_Rect*>(DamageOwner)->Collision_Check((CCollider_Rect*)Target)))
					vecDamagedObj.push_back(iter);
				break;
			case Engine::CCollider_Manager::COLLSIION_BOX:
				DamageOwner = (CCollider_Cube*)pDamageCauser->Find_Component(TEXT("Com_Collider_Cube"));
				Target = (CCollider_Cube*)iter->Find_Component(TEXT("Com_Collider_Cube"));
				if (Target == nullptr)
					continue;

				if (true == (dynamic_cast<CCollider_Cube*>(DamageOwner)->Collision_Check((CCollider_Cube*)Target)))
					vecDamagedObj.push_back(iter);
				break;
			default:
				break;
			}
			
		}
	}
	
	if (vecDamagedObj.size() > 0)
	{
		return true;
	}
	else {
		return false;
	}
}


void CCollider_Manager::Free()
{
	for (_uint i = 0; i < COLLISION_GROUPEND; ++i)
		m_GameObjects[i].clear();
}