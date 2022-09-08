#pragma once
#include "Base.h"


BEGIN(Engine)

class CCullingMgr : public CBase
{
	DECLARE_SINGLETON(CCullingMgr)
private:
	explicit CCullingMgr();
	virtual ~CCullingMgr() = default;

public:
	HRESULT	Ready_CullingMgr(LPDIRECT3DDEVICE9 pGraphicDevice);
	HRESULT Update_CullingMgr();
	_bool Is_In_Frustum(_float3 pGameObjectPos, _float fRadius);

private:
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	_float3 m_WorldPoint[8];
	D3DXPLANE m_FrustumPlane[6];

public:
	virtual void Free() override;
};

END
