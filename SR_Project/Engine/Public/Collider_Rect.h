#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Rect final : public CComponent
{
public:
	CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Rect(const CCollider_Rect& rhs);
	virtual ~CCollider_Rect() = default;

public:
	HRESULT Initialize_Prototype(LPDIRECT3DDEVICE9 pGraphic_Device);
	HRESULT Initialize(void* pArg) override;

public:
	_float3 Get_CenterPoint() { return m_fCenterPoint; }

protected:
	array<_float3, 4> m_aCollisionBoxVerteces;
	_float3 m_fCenterPoint;

public:
	static CCollider_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};