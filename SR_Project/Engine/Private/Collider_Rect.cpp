#include "..\Public\Collider_Rect.h"

CCollider_Rect::CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CCollider_Rect::CCollider_Rect(const CCollider_Rect & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_dwFVF(rhs.m_dwFVF)
	, m_iStride(rhs.m_iStride)
	, m_ePrimitiveType(rhs.m_ePrimitiveType)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)

{
	memcpy(m_vPoint, rhs.m_vPoint, sizeof(_float3) * 4);
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CCollider_Rect::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_StateDesc.StateMatrix);

	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = 2;

	/* 정점들을 할당했다. */
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	VTXTEX*			pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	//_float3 InitPos = _float3(40.f, 5.f, 25.f);

	pVertices[0].vPosition = m_vPoint[0] = _float3(-0.5f, 0.5f, 0.f);// + InitPos;
	pVertices[0].vTexture = _float2(0.0f, 0.f);

	pVertices[1].vPosition = m_vPoint[1] = _float3(0.5f, 0.5f, 0.f);// +InitPos;
	pVertices[1].vTexture = _float2(1.f, 0.f);

	pVertices[2].vPosition = m_vPoint[2] = _float3(0.5f, -0.5f, 0.f);// +InitPos;
	pVertices[2].vTexture = _float2(1.f, 1.f);

	pVertices[3].vPosition = m_vPoint[3] = _float3(-0.5f, -0.5f, 0.f);// +InitPos;
	pVertices[3].vTexture = _float2(0.f, 1.f);
	m_pVB->Unlock();

	m_iIndicesByte = sizeof(FACEINDICES16);
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iNumPrimitive * m_iIndicesByte, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	FACEINDICES16*	pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CCollider_Rect::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		memcpy(&m_StateDesc, pArg, sizeof(COLLRECTDESC));

		m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = 2;

	/* 정점들을 할당했다. */
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	VTXTEX*			pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vPoint[0] = _float3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);// + InitPos;
	pVertices[0].vTexture = _float2(0.0f, 0.f);

	pVertices[1].vPosition = m_vPoint[1] = _float3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);// +InitPos;
	pVertices[1].vTexture = _float2(1.f, 0.f);

	pVertices[2].vPosition = m_vPoint[2] = _float3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);// +InitPos;
	pVertices[2].vTexture = _float2(1.f, 1.f);

	pVertices[3].vPosition = m_vPoint[3] = _float3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);// +InitPos;
	pVertices[3].vTexture = _float2(0.f, 1.f);
	m_pVB->Unlock();


	m_iIndicesByte = sizeof(FACEINDICES16);
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iNumPrimitive * m_iIndicesByte, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;


	FACEINDICES16*	pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();
	}
		

	return S_OK;
}

HRESULT CCollider_Rect::Update_ColliderBox(_float4x4 WorldMatrix)
{

	 m_vPoint[0] = _float3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);// + InitPos;

	 m_vPoint[1] = _float3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, 0.f);// +InitPos;

	 m_vPoint[2] = _float3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);// +InitPos;

	 m_vPoint[3] = _float3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, 0.f);// +InitPos;
	


	m_StateDesc.StateMatrix = WorldMatrix;
	_float3 vecOffsetPos = *(_float3*)&(m_StateDesc.StateMatrix.m[3][0]);
	vecOffsetPos.x += m_StateDesc.fOffSetX;
	vecOffsetPos.y += m_StateDesc.fOffSetY;

	m_StateDesc.StateMatrix.m[3][0] = vecOffsetPos.x;
	m_StateDesc.StateMatrix.m[3][1] = vecOffsetPos.y;


	for (size_t i = 0; i < 4; ++i)
	{
		// 1x4 * 4x4
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_StateDesc.StateMatrix);
	}

 	return S_OK;
}

HRESULT CCollider_Rect::Render_ColliderBox()
{
	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_StateDesc.StateMatrix);
	
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);

	m_pGraphic_Device->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iNumVertices, 0, m_iNumPrimitive);

	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	

	return S_OK;
}

_bool CCollider_Rect::Collision_Check(CCollider_Rect * pTarget, _float3* pOutDistance)
{
	CCollider_Rect* pOther = pTarget;
	if (nullptr == pOther)
		return false;

	_float3		vSourMin, vSourMax, vSourCenter;
	_float3		vDestMin, vDestMax, vDestCenter;
	_float3		vDistance = _float3(0, 0, 0);

	vSourMin = m_vPoint[3];
	vSourMax = m_vPoint[1];
	vSourCenter = (vSourMax + vSourMin)*0.5f;

	// Flip Min and Max if pOther is Scaled by -1 (X-Axis)
	if (pTarget->m_bIsInverse)
	{
		vDestMin = pOther->m_vPoint[2];
		vDestMax = pOther->m_vPoint[0];
	}
	else
	{
		vDestMin = pOther->m_vPoint[3];
		vDestMax = pOther->m_vPoint[1];
	}

	vDestCenter = (vDestMax + vDestMin)*0.5f;

	if (min(vSourMax.x, vDestMax.x) < max(vSourMin.x, vDestMin.x))
		return false;
	else
	{
		if (vSourCenter.x > vDestCenter.x)
		{
			vDistance.x = -(min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
		}
		else
			vDistance.x = (min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
	}
		

	if (min(vSourMax.z, vDestMax.z) < max(vSourMin.z, vDestMin.z))
		return false;
	else
	{
		if (vSourCenter.z > vDestCenter.z)
		{
			vDistance.z = -(min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z));
		}
		else
			vDistance.z = min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z);
	}
		
	if(pOutDistance != nullptr)
		*pOutDistance = vDistance;

	return true;
}

CCollider_Rect * CCollider_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider_Rect::Clone(void * pArg)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Rect::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
