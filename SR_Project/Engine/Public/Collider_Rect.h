#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Rect final : public CComponent
{
public:
	typedef struct tagCollisionRectDesc
	{
		_float fOffsetX;
		_float fOffsetY;
		_float fOffsetOrigin;
		_float3 vCenterPoint;
	}COLLRECTDESC;

public:
	CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Rect(const CCollider_Rect& rhs);
	virtual ~CCollider_Rect() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;

public:
	COLLRECTDESC Get_CollRectDesc() { return m_tCollRectDesc; }
	void Set_CollisionBoxVertex(_float3 pVertex, int iIndex) { m_aCollisionBoxVerteces[iIndex] = pVertex; }
	array<_float3, 4> Get_CollisionVerteces() { return m_aCollisionBoxVerteces; }
	_float3 Get_CenterPoint() { return m_fCenterPoint; }

protected:
	COLLRECTDESC m_tCollRectDesc;
	_float3 m_fCenterPoint;
	array<_float3, 4> m_aCollisionBoxVerteces;

public:
	static CCollider_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END