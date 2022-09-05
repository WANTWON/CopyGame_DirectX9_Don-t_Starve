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
	void	Add_PickingObject(ITEMID type);
	void	Add_PickingGroup(CGameObject* pGameObject);
	void	Out_PickingGroup(CGameObject* pGameObject);
	void	Mouse_Intersect_UI(_bool Data) {m_bMouseInUI = Data;}

public: /*Get*/
	_float3 Get_PickingPos() { return m_vPickingPos; }
	_bool Get_Mouse_Has_Construct() { return m_bMouseHasConstruction; }
	void Set_PickingItemType(ITEMID type) { m_PickingItemType = type; Add_PickingObject(type); }
	ITEMID Get_PickingItemType() { return m_PickingItemType; }


public:
	_bool	Picking();
	void	Release_PickingObject();

private:
	CTerrain*			m_pTerrain = nullptr;
	list<CGameObject*>	m_GameObjects;
	_bool				m_bMouseInUI = false;
	_float3				m_vPickingPos;

private:
	CGameObject*		m_pPickingObject = nullptr;
	_bool				m_bMouseHasConstruction = false;
	ITEMID				m_PickingItemType = ITEM_END;

public:
	virtual void Free() override;
};
END
