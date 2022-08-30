#include "..\Public\Picking.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}


HRESULT CPicking::Initialize(HWND hWnd, _uint iWinCX, _uint iWinCY, LPDIRECT3DDEVICE9 pGraphic_Device)
{
	m_pGraphic_Device = pGraphic_Device;

	Safe_AddRef(m_pGraphic_Device);

	m_hWnd = hWnd;

	m_iWinCX = iWinCX;

	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Tick()
{
	POINT			ptMouse;

	GetCursorPos(&ptMouse);

	ScreenToClient(m_hWnd, &ptMouse);

	_float3			vMousePos;

	/* 투영공간상의 마우스 좌표를 구한다. */
	vMousePos.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	vMousePos.z = 0.f;


	/* 뷰스페이스 상의 좌표를 구한다. */
	_float4x4		ProjMatrixInv;
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

	/* 뷰스페이스 상의 마우스 레이와 레이포스를 구하자. */
	m_vRayPos = _float3(0.f, 0.f, 0.f);
	m_vRayDir = vMousePos - m_vRayPos;

	/* 월드스페이스 상의 마우스 레이와 레이포스를 구하자. */
	_float4x4		ViewMatrixInv;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);

}

void CPicking::Transform_ToLocalSpace(_float4x4 WorldMatrixInverse)
{
	D3DXVec3TransformCoord(&m_vRayPos_Local, &m_vRayPos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&m_vRayDir_Local, &m_vRayDir, &WorldMatrixInverse);

	D3DXVec3Normalize(&m_vRayDir_Local, &m_vRayDir_Local);
}

_bool CPicking::Intersect_InWorldSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut)
{
	_float		fU, fV, fDist;

	if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos, &m_vRayDir, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos + m_vRayDir* fDist;

		return true;
	}

	return false;
}

_bool CPicking::Intersect_InLocalSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut)
{
	_float		fU, fV, fDist;

	if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos_Local, &m_vRayDir_Local, &fU, &fV, &fDist))
	{
		*pOut = m_vRayPos_Local + m_vRayDir_Local * fDist;

		return true;
	}

	return false;
}

void CPicking::Free()
{
	Safe_Release(m_pGraphic_Device);
}
