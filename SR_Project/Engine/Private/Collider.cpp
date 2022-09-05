#include "..\Public\Collider.h"
#include "Component.h"
#include "Transform.h"
#include "Collider_Rect.h"

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

HRESULT CCollider::Update_CollisionBox()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
	{
		for (auto& pGameObject : m_GameObjects[i])
		{
			if (pGameObject)
			{
				CTransform* pTransform = dynamic_cast<CTransform*>(pGameObject->Find_Component(TEXT("Com_Transform")));
				CCollider_Rect* pColliderRect = dynamic_cast<CCollider_Rect*>(pGameObject->Find_Component(TEXT("Com_Collider_Rect")));
				if (pTransform && pColliderRect)
				{
					CCollider_Rect::COLLRECTDESC tCollRectDesc = pColliderRect->Get_CollRectDesc();
					_float3 pOrigin = pTransform->Get_State(CTransform::STATE_POSITION) + _float3(0.f, tCollRectDesc.fOffsetOrigin, 0.f);

					_float3 pPoint;
					pPoint = _float3(pOrigin.x - tCollRectDesc.fOffsetX, pOrigin.y + tCollRectDesc.fOffsetY, pOrigin.z);
					pColliderRect->Set_CollisionBoxVertex(pPoint, 0);
					pPoint = _float3(pOrigin.x + tCollRectDesc.fOffsetX, pOrigin.y + tCollRectDesc.fOffsetY, pOrigin.z);
					pColliderRect->Set_CollisionBoxVertex(pPoint, 1);
					pPoint = _float3(pOrigin.x - tCollRectDesc.fOffsetX, pOrigin.y - tCollRectDesc.fOffsetY, pOrigin.z);
					pColliderRect->Set_CollisionBoxVertex(pPoint, 2);
					pPoint = _float3(pOrigin.x + tCollRectDesc.fOffsetX, pOrigin.y - tCollRectDesc.fOffsetY, pOrigin.z);
					pColliderRect->Set_CollisionBoxVertex(pPoint, 3);
				}
			}
		}
	}

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

_bool CCollider::Collision_with_Group(COLLISION_GROUP eGroup, class CGameObject* pGameObject)
{
	for (auto& iter : m_GameObjects[eGroup])
	{
		if (nullptr != iter)
		{
			_float3 vecDir = pGameObject->Get_Position() - iter->Get_Position();
			_float fDistance = D3DXVec3Length(&vecDir);
			
			if (fDistance <= pGameObject->Get_Radius() + iter->Get_Radius())
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
			_float3 vecDir = pDamageCauser->Get_Position() - iter->Get_Position();
			_float fDistance = D3DXVec3Length(&vecDir);

			if (fDistance <= pDamageCauser->Get_Radius() + iter->Get_Radius())
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

//CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
//{
//	CCollider*	pInstance = new CCollider(pGraphic_Device);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		ERR_MSG(TEXT("Failed to Created : CCollider"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

//CComponent * CCollider::Clone(void * pArg)
//{
//	AddRef();
//
//	return this;
//}

void CCollider::Free()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
		m_GameObjects[i].clear();
}