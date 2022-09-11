#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)

public:
	enum COLLISION_GROUP { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_OBJECT, COLLISION_BLOCK, COLLISION_END };
	enum COLLISION_TYPE {COLLISION_RECT, COLLSIION_BOX, COLLSION_END };
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);
	HRESULT Reset_ColliderGroup();

	_bool Collision_with_Group(COLLISION_GROUP eGroup, class CGameObject* pGameObject, COLLISION_TYPE eCollisionType,  _float3* pOutDistance = nullptr);
	_bool Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, class CGameObject* pDamageCauser, COLLISION_TYPE eCollisionType);

private:
	list<class CGameObject*> m_GameObjects[COLLISION_END];
	typedef list<class CGameObject*> GAMEOBJECTS;

public:
	virtual void Free() override;
};
END