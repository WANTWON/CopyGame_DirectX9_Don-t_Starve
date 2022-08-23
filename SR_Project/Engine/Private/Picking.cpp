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

}

void CPicking::Compute_LocalRayInfo(_float3 * pRayDir, _float3 * pRayPos, CTransform * pTransform)
{
	_float4x4	WorldMatrixInverse;
	D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &pTransform->Get_WorldMatrix());

	D3DXVec3TransformCoord(pRayPos, &m_vRayPos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(pRayDir, &m_vRayDir, &WorldMatrixInverse);
}

void CPicking::Free()
{
	Safe_Release(m_pGraphic_Device);
}
