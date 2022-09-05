#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider final : public CBase
{
	DECLARE_SINGLETON(CCollider)

public:
	enum COLLISION_GROUP { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_OBJECT, COLLISION_END };

private:
	CCollider();
	virtual ~CCollider() = default;

public:
	HRESULT Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);
	HRESULT Update_CollisionBox();
	HRESULT Reset_ColliderGroup();

	_bool Collision_with_Group(COLLISION_GROUP eGroup, class CGameObject* pGameObject);
	_bool Collision_Check_Group_Multi(COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, class CGameObject* pDamageCauser);

private:
	list<class CGameObject*> m_GameObjects[COLLISION_END];
	typedef list<class CGameObject*> GAMEOBJECTS;

public:
	virtual void Free() override;
};
END