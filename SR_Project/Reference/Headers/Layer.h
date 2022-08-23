#pragma once

#include "GameObject.h"

/* 객체들을 모아 놓는다. */
BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	class CGameObject* Get_Object(_uint iIndex = 0);

private:
	list<class CGameObject*>			m_GameObjects;
	typedef list<class CGameObject*>	GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END