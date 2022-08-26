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
	HRESULT	Ready_PickingMgr(PDIRECT3DDEVICE9 pGraphic_Device);

public:
	void	Add_PickingGroup(CGameObject* pGameObject);
	void	Out_PickingGroup(CGameObject* pGameObject);

public:
	//게임오브젝트를 피킹한다.
	CGameObject*	Picking(POINT& pt);
	//vOrigin 으로부터 가장 가까운 오브젝트를 반환한다. Self는 무시한다.
	CGameObject*	Get_Closest_Object(_float3 vOrigin, CGameObject* pSelf = nullptr);
public:
	_bool TerrainPicking();

	void			Clear_PickingMgr();

private:
	CTerrain*			m_pTerrain = nullptr;
	list<CGameObject*>	m_GameObjects;
	
public:
	virtual void Free() override;
};
END
