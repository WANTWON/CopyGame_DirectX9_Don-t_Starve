#include "..\Public\CullingMgr.h"

IMPLEMENT_SINGLETON(CCullingMgr)

CCullingMgr::CCullingMgr()
{
}


HRESULT CCullingMgr::Ready_CullingMgr(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	m_pGraphicDevice = pGraphicDevice;
	Safe_AddRef(m_pGraphicDevice);

	m_WorldPoint[0] = _float3(-1.f, 1.f, 0.f);
	m_WorldPoint[1] = _float3(1.f, 1.f, 0.f);
	m_WorldPoint[2] = _float3(1.f, -1.f, 0.f);
	m_WorldPoint[3] = _float3(-1.f, -1.f, 0.f);
	m_WorldPoint[4] = _float3(-1.f, 1.f, 1.f);
	m_WorldPoint[5] = _float3(1.f, 1.f, 1.f);
	m_WorldPoint[6] = _float3(1.f, -1.f, 1.f);
	m_WorldPoint[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

HRESULT CCullingMgr::Update_CullingMgr()
{
	m_WorldPoint[0] = _float3(-1.f, 1.f, 0.f);
	m_WorldPoint[1] = _float3(1.f, 1.f, 0.f);
	m_WorldPoint[2] = _float3(1.f, -1.f, 0.f);
	m_WorldPoint[3] = _float3(-1.f, -1.f, 0.f);
	m_WorldPoint[4] = _float3(-1.f, 1.f, 1.f);
	m_WorldPoint[5] = _float3(1.f, 1.f, 1.f);
	m_WorldPoint[6] = _float3(1.f, -1.f, 1.f);
	m_WorldPoint[7] = _float3(-1.f, -1.f, 1.f);

	_float4x4 ProjMatrixInv;
	m_pGraphicDevice->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
	D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);

	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&m_WorldPoint[i], &m_WorldPoint[i], &ProjMatrixInv);

	/* 월드스페이스 상의 마우스 레이와 레이포스를 구하자. */
	_float4x4		ViewMatrixInv;
	m_pGraphicDevice->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
	D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);

	for (int i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&m_WorldPoint[i], &m_WorldPoint[i], &ViewMatrixInv);

	return S_OK;
}

_bool CCullingMgr::Is_In_Frustum(_float3 pGameObjectPos, _float fRadius)
{
	D3DXPlaneFromPoints(&m_FrustumPlane[0] ,&m_WorldPoint[0], &m_WorldPoint[1], &m_WorldPoint[2]);
	D3DXPlaneFromPoints(&m_FrustumPlane[1], &m_WorldPoint[1], &m_WorldPoint[5], &m_WorldPoint[6]);
	D3DXPlaneFromPoints(&m_FrustumPlane[2], &m_WorldPoint[4], &m_WorldPoint[5], &m_WorldPoint[1]);
	D3DXPlaneFromPoints(&m_FrustumPlane[3], &m_WorldPoint[4], &m_WorldPoint[0], &m_WorldPoint[3]);
	D3DXPlaneFromPoints(&m_FrustumPlane[4], &m_WorldPoint[5], &m_WorldPoint[4], &m_WorldPoint[7]);
	D3DXPlaneFromPoints(&m_FrustumPlane[5], &m_WorldPoint[3], &m_WorldPoint[2], &m_WorldPoint[6]);

	for (int i = 0; i < 6; ++i)
	{
		_float distance = m_FrustumPlane[i].a*pGameObjectPos.x + m_FrustumPlane[i].b*pGameObjectPos.y + m_FrustumPlane[i].c*pGameObjectPos.z + m_FrustumPlane[i].d;
		if (distance > fRadius + 1)
			return false;
	}

	return true;
}

void CCullingMgr::Free()
{

	Safe_Release(m_pGraphicDevice);
}
