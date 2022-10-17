#include "..\Public\Collider.h"
#include "GameObject.h"



CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}


HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CCollider::Add_CollisionGroup(COLLISON_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[eCollisionGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CCollider::Update_ColliderGroup()
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

bool CCollider::Collision_with_Group(COLLISON_GROUP eGroup, class CGameObject* pGameObject)
{
	for (auto& iter : m_GameObjects[eGroup])
	{
		if (nullptr != iter)
		{
			//distance <= rb0.radius + rb1.radius
			_float3 vecDir = pGameObject->Get_Position() - iter->Get_Position();
			_float fDistance = D3DXVec3Length(&vecDir);
			
			if (fDistance <= pGameObject->Get_Radius() + iter->Get_Radius())
				return true;
		}
	}

	return false;
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider*	pInstance = new CCollider(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CCollider::Free()
{
	__super::Free();

}
