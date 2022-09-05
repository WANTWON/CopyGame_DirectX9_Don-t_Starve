#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CBase
{
	DECLARE_SINGLETON(CCollider)

public:
	enum COLLISON_GROUP { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_OBJECT, COLLISION_END };

private:
	CCollider(/*LPDIRECT3DDEVICE9 pGraphic_Device*/);
	virtual ~CCollider() = default;

public:
	HRESULT Add_CollisionGroup(COLLISON_GROUP eCollisionGroup, class CGameObject* pGameObject);
	HRESULT Update_ColliderGroup();

	_bool Collision_with_Group(COLLISON_GROUP eGroup, class CGameObject* pGameObject);
	_bool Collision_Check_Group_Multi(COLLISON_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, class CGameObject* pDamageCauser);

private:
	list<class CGameObject*> m_GameObjects[COLLISION_END];
	typedef list<class CGameObject*> GAMEOBJECTS;

public:
	virtual void Free() override;
};
END