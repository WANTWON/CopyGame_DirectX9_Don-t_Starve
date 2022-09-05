#pragma once

#include "Component.h"
#include "VIBuffer_Rect.h"
BEGIN(Engine)

class ENGINE_DLL CCollider_Rect final : public CComponent
{
public:
	typedef struct tagCollisionRectDesc
	{
		_float	fWidth = 0.f, fHeight = 0.f;
		_float	fOffSetX = 0.f, fOffSetY = 0.f;
		_float4x4		StateMatrix;
	}COLLRECTDESC;

public:
	CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Rect(const CCollider_Rect& rhs);
	virtual ~CCollider_Rect() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	HRESULT Update_ColliderBox(_float4x4 WorldMatrix);
	HRESULT Render_ColliderBox();

public:
	COLLRECTDESC Get_CollRectDesc() { return m_StateDesc; }

protected:
	COLLRECTDESC		m_StateDesc;
	

protected:
	LPDIRECT3DVERTEXBUFFER9  m_pVB = nullptr;
	_uint						m_iNumVertices = 0;
	_uint						m_iStride = 0; /* 정점의 크기(byte) */
	_ulong						m_dwFVF = 0;
	D3DPRIMITIVETYPE			m_ePrimitiveType;
	_uint						m_iNumPrimitive = 0;

	_float3				m_vPoint[4];
	_float3				m_vOriginalPoint[4];
	_float4x4			m_ColliderMatrix;
	LPD3DXLINE			m_pLine;

protected:
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
	_uint						m_iIndicesByte = 0;
	D3DFORMAT					m_eIndexFormat;
public:
	static CCollider_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END