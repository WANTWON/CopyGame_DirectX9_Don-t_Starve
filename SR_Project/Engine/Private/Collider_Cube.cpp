#include "..\Public\Collider_Cube.h"


_tchar* CCollider_Cube::m_pTransformTag = TEXT("Com_Transform");

_bool m_bRender = true;

CCollider_Cube::CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CCollider_Cube::CCollider_Cube(const CCollider_Cube & rhs)
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

HRESULT CCollider_Cube::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_StateDesc.StateMatrix);

	m_iNumVertices = 8;
	m_iStride = sizeof(VTXCUBETEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = 12;

	/* 정점들을 할당했다. */
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	VTXCUBETEX*			pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = m_vPoint[0] = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexture = pVertices[0].vPosition;

	pVertices[1].vPosition = m_vPoint[1] = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexture = pVertices[1].vPosition;

	pVertices[2].vPosition = m_vPoint[2] = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexture = pVertices[2].vPosition;

	pVertices[3].vPosition = m_vPoint[3] = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexture = pVertices[3].vPosition;

	pVertices[4].vPosition = m_vPoint[4] = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexture = pVertices[4].vPosition;

	pVertices[5].vPosition = m_vPoint[5] = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexture = pVertices[5].vPosition;

	pVertices[6].vPosition = m_vPoint[6] = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexture = pVertices[6].vPosition;

	pVertices[7].vPosition = m_vPoint[7] = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexture = pVertices[7].vPosition;

	m_pVB->Unlock();

	m_iIndicesByte = sizeof(FACEINDICES16);
	m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iNumPrimitive * m_iIndicesByte, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	FACEINDICES16*			pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/* +X */
	pIndices[0]._0 = 1; pIndices[0]._1 = 5; pIndices[0]._2 = 6;
	pIndices[1]._0 = 1; pIndices[1]._1 = 6; pIndices[1]._2 = 2;

	/* -X */
	pIndices[2]._0 = 4; pIndices[2]._1 = 0; pIndices[2]._2 = 3;
	pIndices[3]._0 = 4; pIndices[3]._1 = 3; pIndices[3]._2 = 7;

	/* +Y */
	pIndices[4]._0 = 4; pIndices[4]._1 = 5; pIndices[4]._2 = 1;
	pIndices[5]._0 = 4; pIndices[5]._1 = 1; pIndices[5]._2 = 0;

	/* -Y */
	pIndices[6]._0 = 3; pIndices[6]._1 = 2; pIndices[6]._2 = 6;
	pIndices[7]._0 = 3; pIndices[7]._1 = 6; pIndices[7]._2 = 7;

	/* +Z */
	pIndices[8]._0 = 5; pIndices[8]._1 = 4; pIndices[8]._2 = 7;
	pIndices[9]._0 = 5; pIndices[9]._1 = 7; pIndices[9]._2 = 6;

	/* -Z */
	pIndices[10]._0 = 0; pIndices[10]._1 = 1; pIndices[10]._2 = 2;
	pIndices[11]._0 = 0; pIndices[11]._1 = 2; pIndices[11]._2 = 3;

	m_pIB->Unlock();
	
	return S_OK;
}

HRESULT CCollider_Cube::Initialize(void * pArg)
{
	m_pTransform = CTransform::Create(m_pGraphic_Device);
	if (m_pTransform == nullptr)
		return E_FAIL;

	if (pArg != nullptr)
	{
		memcpy(&m_StateDesc, pArg, sizeof(COLLRECTDESC));

		D3DXMatrixIdentity(&m_StateDesc.StateMatrix);

		m_iNumVertices = 8;
		m_iStride = sizeof(VTXCUBETEX);
		m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
		m_ePrimitiveType = D3DPT_TRIANGLELIST;
		m_iNumPrimitive = 12;

		/* 정점들을 할당했다. */
		if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iStride, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
			return E_FAIL;

		VTXCUBETEX*			pVertices = nullptr;

		m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);


		pVertices[0].vPosition = m_vPoint[0] = _float3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);// + InitPos;
		pVertices[0].vTexture = pVertices[0].vPosition;

		pVertices[1].vPosition = m_vPoint[1] = _float3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);// +InitPos;
		pVertices[1].vTexture = pVertices[1].vPosition;

		pVertices[2].vPosition = m_vPoint[2] = _float3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);// +InitPos;
		pVertices[2].vTexture = pVertices[2].vPosition;

		pVertices[3].vPosition = m_vPoint[3] = _float3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);// +InitPos;
		pVertices[3].vTexture = pVertices[3].vPosition;

		pVertices[4].vPosition = m_vPoint[4] = _float3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);// + InitPos;
		pVertices[4].vTexture = pVertices[4].vPosition;

		pVertices[5].vPosition = m_vPoint[5] = _float3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);// +InitPos;
		pVertices[5].vTexture = pVertices[5].vPosition;

		pVertices[6].vPosition = m_vPoint[6] = _float3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);// +InitPos;
		pVertices[6].vTexture = pVertices[6].vPosition;

		pVertices[7].vPosition = m_vPoint[7] = _float3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);// +InitPos;
		pVertices[7].vTexture = pVertices[7].vPosition;
		m_pVB->Unlock();

		m_iIndicesByte = sizeof(FACEINDICES16);
		m_eIndexFormat = D3DFMT_INDEX16;

		if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iNumPrimitive * m_iIndicesByte, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
			return E_FAIL;

		FACEINDICES16*	pIndices = nullptr;

		m_pIB->Lock(0, 0, (void**)&pIndices, 0);


		/* +X */
		pIndices[0]._0 = 1; pIndices[0]._1 = 5; pIndices[0]._2 = 6;
		pIndices[1]._0 = 1; pIndices[1]._1 = 6; pIndices[1]._2 = 2;

		/* -X */
		pIndices[2]._0 = 4; pIndices[2]._1 = 0; pIndices[2]._2 = 3;
		pIndices[3]._0 = 4; pIndices[3]._1 = 3; pIndices[3]._2 = 7;

		/* +Y */
		pIndices[4]._0 = 4; pIndices[4]._1 = 5; pIndices[4]._2 = 1;
		pIndices[5]._0 = 4; pIndices[5]._1 = 1; pIndices[5]._2 = 0;

		/* -Y */
		pIndices[6]._0 = 3; pIndices[6]._1 = 2; pIndices[6]._2 = 6;
		pIndices[7]._0 = 3; pIndices[7]._1 = 6; pIndices[7]._2 = 7;

		/* +Z */
		pIndices[8]._0 = 5; pIndices[8]._1 = 4; pIndices[8]._2 = 7;
		pIndices[9]._0 = 5; pIndices[9]._1 = 7; pIndices[9]._2 = 6;

		/* -Z */
		pIndices[10]._0 = 0; pIndices[10]._1 = 1; pIndices[10]._2 = 2;
		pIndices[11]._0 = 0; pIndices[11]._1 = 2; pIndices[11]._2 = 3;


		m_pIB->Unlock();
	}


	return S_OK;
}

HRESULT CCollider_Cube::Update_ColliderBox(_float4x4 WorldMatrix)
{

	m_StateDesc.StateMatrix = WorldMatrix;

	m_vPoint[0] = _float3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);
	m_vPoint[1] = _float3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);
	m_vPoint[2] = _float3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);
	m_vPoint[3] = _float3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, -m_StateDesc.fRadiusZ);
	m_vPoint[4] = _float3(-m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);
	m_vPoint[5] = _float3(m_StateDesc.fRadiusX, m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);
	m_vPoint[6] = _float3(m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);
	m_vPoint[7] = _float3(-m_StateDesc.fRadiusX, -m_StateDesc.fRadiusY, m_StateDesc.fRadiusZ);

	_float3 vecOffsetPos = *(_float3*)&(m_StateDesc.StateMatrix.m[3][0]);
	vecOffsetPos.x += m_StateDesc.fOffSetX;
	vecOffsetPos.y += m_StateDesc.fOffSetY;
	vecOffsetPos.z += m_StateDesc.fOffsetZ;

	m_StateDesc.StateMatrix.m[3][0] = vecOffsetPos.x;
	m_StateDesc.StateMatrix.m[3][1] = vecOffsetPos.y;
	m_StateDesc.StateMatrix.m[3][2] = vecOffsetPos.z;

	for (int i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_StateDesc.StateMatrix);
	}
	

	return S_OK;
}

HRESULT CCollider_Cube::Render_ColliderBox()
{
	if (GetKeyState(VK_F11))
	{
		m_bRender == true ? m_bRender = false : m_bRender = true;
	}

	if (m_bRender)
	{
		m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_StateDesc.StateMatrix);

		m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iStride);
		m_pGraphic_Device->SetFVF(m_dwFVF);
		m_pGraphic_Device->SetIndices(m_pIB);

		m_pGraphic_Device->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iNumVertices, 0, m_iNumPrimitive);

		m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	


	return S_OK;
}

_bool CCollider_Cube::Collision_Check(CCollider_Cube * pTarget, _float3* pOutDistance)
{
	//AABB Collision Check

	CCollider_Cube* pOther = pTarget;
	if (nullptr == pOther)
		return false;

	_float3		vSourMin, vSourMax, vSourCenter;
	_float3		vDestMin, vDestMax, vDestCenter;
	_float3		vDistance = _float3(0, 0, 0);


	vSourMin = m_vPoint[3];
	vSourMax = m_vPoint[5];
	vSourCenter = (vSourMax + vSourMin)*0.5f;


	vDestMin = pOther->m_vPoint[3];
	vDestMax = pOther->m_vPoint[5];
	
	vDestCenter = (vDestMax + vDestMin)*0.5f;


	if (min(vSourMax.x, vDestMax.x) < max(vSourMin.x, vDestMin.x))
		return false;
	else
	{
		if (vSourCenter.x > vDestCenter.x)
		{
			vDistance.x = -((min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x)));
		}
		else
			vDistance.x = (min(vSourMax.x, vDestMax.x) - max(vSourMin.x, vDestMin.x));
	}

	//if (min(vSourMax.y, vDestMax.y) < max(vSourMin.y, vDestMin.y))
	/*	return false;
	else
	{
		if (vSourCenter.y > vDestCenter.y)
		{
			vDistance.y = -((min(vSourMax.y, vDestMax.y) - max(vSourMin.y, vDestMin.y)));
		}
		else
			vDistance.y = min(vSourMax.y, vDestMax.y) - max(vSourMin.y, vDestMin.y);
	}*/

	if (min(vSourMax.z, vDestMax.z) < max(vSourMin.z, vDestMin.z))
		return false;
	else
	{
		if (vSourCenter.z > vDestCenter.z)
		{
			vDistance.z = -((min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z)));
		}
		else
			vDistance.z = min(vSourMax.z, vDestMax.z) - max(vSourMin.z, vDestMin.z);
	}

	if (pOutDistance != nullptr)
		*pOutDistance = vDistance;

	return true;
}



CCollider_Cube * CCollider_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Cube*	pInstance = new CCollider_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider_Cube::Clone(void * pArg)
{
	CCollider_Cube*	pInstance = new CCollider_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Cube::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
