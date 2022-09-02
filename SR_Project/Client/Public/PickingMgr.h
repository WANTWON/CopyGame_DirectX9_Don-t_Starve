#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CTerrain;
class CPickingMgr : public CBase
{
	DECLARE_SINGLETON(CPickingMgr)
private:
	explicit CPickingMgr();
	virtual ~CPickingMgr() = default;

public:
	HRESULT	Ready_PickingMgr(LEVEL TerrainLevelindex);
	void	Clear_PickingMgr();

public:
	void	Add_PickingGroup(CGameObject* pGameObject);
	void	Out_PickingGroup(CGameObject* pGameObject);
	void	Mouse_Intersect_UI(_bool Data) {m_bMouseInUI = Data;}

public:
	_bool	Picking();

private:
	CTerrain*			m_pTerrain = nullptr;
	list<CGameObject*>	m_GameObjects;
	_bool				m_bMouseInUI = false;

public:
	virtual void Free() override;
};
END
