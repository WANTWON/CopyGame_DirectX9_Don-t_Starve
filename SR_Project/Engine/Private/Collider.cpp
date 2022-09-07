#include "..\Public\Collider.h"
#include "Component.h"
#include "Transform.h"
#include "Collider_Rect.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCollider)

CCollider::CCollider(/*LPDIRECT3DDEVICE9 pGraphic_Device*/)
{
}

HRESULT CCollider::Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[eCollisionGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}


HRESULT CCollider::Reset_ColliderGroup()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
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
_bool  CCollider::Collision_with_Group(COLLISION_GROUP eGroup, class CGameObject* pGameObject, _float3* pOutDistance )
{
	for (auto& iter : m_GameObjects[eGroup])
	{
		if (nullptr != iter)
		{
			CCollider_Rect*  DamageOwner = (CCollider_Rect*)pGameObject->Find_Component(TEXT("Com_Collider_Rect"));
			CCollider_Rect*	 Target = (CCollider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
			if (Target == nullptr)
				continue;

			if (true == (DamageOwner->Collision_Check(Target, pOutDistance)))
				return true;
		}
	}

	return false;
}

_bool CCollider::Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, CGameObject * pDamageCauser)
{
	for (auto& iter : m_GameObjects[eGroup])
	{
		if (nullptr != iter)
		{
			CCollider_Rect*  DamageOwner = (CCollider_Rect*)pDamageCauser->Find_Component(TEXT("Com_Collider_Rect"));
			CCollider_Rect*	 Target = (CCollider_Rect*)iter->Find_Component(TEXT("Com_Collider_Rect"));
			if (Target == nullptr)
				continue;

			if (true == (DamageOwner->Collision_Check(Target)))
			{
			vecDamagedObj.push_back(iter);
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



void CCollider::Free()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
		m_GameObjects[i].clear();
}