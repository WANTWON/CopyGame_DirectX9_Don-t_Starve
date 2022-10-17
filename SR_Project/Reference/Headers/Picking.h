#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
public:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY, LPDIRECT3DDEVICE9 pGraphic_Device);
	void Tick(); 
	void Transform_ToLocalSpace(_float4x4 WorldMatrixInverse);
	_bool Intersect_InWorldSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut);
	_bool Intersect_InLocalSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut);

private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
	HWND				m_hWnd;
	_uint				m_iWinCX, m_iWinCY;

	_float3					m_vRayDir, m_vRayPos;
	_float3					m_vRayDir_Local, m_vRayPos_Local;
public:
	virtual void Free() override;
};

END