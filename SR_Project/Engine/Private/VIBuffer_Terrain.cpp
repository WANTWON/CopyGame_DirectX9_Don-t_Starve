#include "..\Public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_TerrainDesc(rhs.m_TerrainDesc)
	, m_pVertices(rhs.m_pVertices)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(TERRAINDESC TerrainDesc)
{
	ZeroMemory(&m_TerrainDesc, sizeof(TERRAINDESC));
	memcpy(&m_TerrainDesc, &TerrainDesc, sizeof(TERRAINDESC));

	m_iNumVertices = m_TerrainDesc.m_iNumVerticesX * m_TerrainDesc.m_iNumVerticesZ;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = (m_TerrainDesc.m_iNumVerticesX - 1) * (m_TerrainDesc.m_iNumVerticesZ - 1) * 2;

	/* 정점들을 할당했다. */
	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	//VTXTEX*			pVertices = nullptr;
	m_pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&m_pVertices, 0);

	for (_uint i = 0; i < m_TerrainDesc.m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_TerrainDesc.m_iNumVerticesX; ++j)
		{
			_uint	iIndex = i * m_TerrainDesc.m_iNumVerticesX + j;

			m_pVertices[iIndex].vPosition = _float3(j*m_TerrainDesc.m_fSizeX, 0.0f, i*m_TerrainDesc.m_fSizeZ);
			m_pVertices[iIndex].vTexture = _float2(j / (m_TerrainDesc.m_iNumVerticesX - 1.0f)*m_TerrainDesc.m_fTextureSize, i / (m_TerrainDesc.m_iNumVerticesZ - 1.0f)*m_TerrainDesc.m_fTextureSize);
			int a = 0;
		}
	}

	m_pVB->Unlock();

	m_iIndicesByte = sizeof(FACEINDICES32);
	m_eIndexFormat = D3DFMT_INDEX32;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	FACEINDICES32*			pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_TerrainDesc.m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_TerrainDesc.m_iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = i * m_TerrainDesc.m_iNumVerticesX + j;

			_uint	iIndices[4] = {
				iIndex + m_TerrainDesc.m_iNumVerticesX,
				iIndex + m_TerrainDesc.m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];
			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];
			++iNumFaces;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

_float CVIBuffer_Terrain::Get_TerrainY(_float Posx, _float Posz)
{
	_float fHeight = 0;
	int index = int(Posz / (m_TerrainDesc.m_iNumVerticesZ)+(Posx / m_TerrainDesc.m_iNumVerticesX));

	if (index > m_TerrainDesc.m_iNumVerticesX*m_TerrainDesc.m_iNumVerticesZ || index < 0)
		return 0;

	int OneTilePosX = Posx / m_TerrainDesc.m_iNumVerticesX;
	int OneTilePosZ = Posx / m_TerrainDesc.m_iNumVerticesZ;

	//a,b,c = 평면의 법선벡터  / x,y,z 평면위의 점
	D3DXPLANE Plane;

	// ax + by + cz + d = 0;
	if ((OneTilePosX - 0.5 >= 0) && (0.5 - OneTilePosZ) < 0)
	{
		D3DXPlaneFromPoints(&Plane, &m_pVertices[index + m_TerrainDesc.m_iNumVerticesX].vPosition, &m_pVertices[index + m_TerrainDesc.m_iNumVerticesX + 1].vPosition, &m_pVertices[index + 1].vPosition);
		fHeight = -(Plane.a*Posx + Plane.c*Posz + Plane.a*Plane.b*Plane.c*Plane.d) / Plane.b;
	}
	else
	{
		D3DXPlaneFromPoints(&Plane, &m_pVertices[index + m_TerrainDesc.m_iNumVerticesX].vPosition, &m_pVertices[index + 1].vPosition, &m_pVertices[index].vPosition);
		fHeight = -(Plane.a*Posx + Plane.c*Posz + Plane.a*Plane.b*Plane.c*Plane.d) / Plane.b;
	}

	return fHeight + 1;
}

bool CVIBuffer_Terrain::Picking(POINT & ptMouse, _float4x4 WorldMatrix, _float3 * pOutPos)
{
	D3DVIEWPORT9		ViewPort;
	m_pGraphic_Device->GetViewport(&ViewPort);

	_float4		vMousePos = _float4((_float)ptMouse.x, (_float)ptMouse.y, 0.0f, 0.0f);

	// 2차원 투영 스페이스로 변환한다.(0, 0, g_iWinCX, g_iWinCY -> -1, 1, 1, -1)
	vMousePos.x = vMousePos.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = vMousePos.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;
	vMousePos.w = 1.f;

	//기존 투영행렬을 얻어온다
	_float4x4 ProjMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
	D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

	D3DXVec4Transform(&vMousePos, &vMousePos, &ProjMatrix);

	// 뷰스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	_float3		vMousePoint = _float3(0.f, 0.f, 0.f);
	_float3		vMouseRay = _float3(vMousePos.x, vMousePos.y, vMousePos.z) - vMousePoint;

	//기존 뷰 행렬을 어떻게 얻어온다 
	_float4x4 ViewMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	// 월드스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec3TransformCoord(&vMousePoint, &vMousePoint, &ViewMatrix);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &ViewMatrix);

	// 로컬로 변환 (픽킹 대상 객체의 월드 행렬을 가져온다)
	_float4x4	WorldMatrixInverse;
	D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);

	// 로컬스페이스 상의 마우스 레이, 마우스의 시작점을 구한다.
	D3DXVec3TransformCoord(&vMousePoint, &vMousePoint, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&vMouseRay, &vMouseRay, &WorldMatrixInverse);

	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int iIndex = 0; iIndex < m_iNumVertices ; ++iIndex)
	{
		_uint iIndices[4] = {
			{ iIndex + m_TerrainDesc.m_iNumVerticesX },
			{ iIndex + m_TerrainDesc.m_iNumVerticesX + 1 },
			{ iIndex + 1 },
			{ iIndex },
		};
		_float		fU, fV, fDist;

		// 우 상단 삼각형.z
		if (true == D3DXIntersectTri(&m_pVertices[iIndices[1]].vPosition, &m_pVertices[iIndices[0]].vPosition, &m_pVertices[iIndices[2]].vPosition, &vMousePoint, &vMouseRay, &fU, &fV, &fDist))
		{
			_float3 vLocalMouse = vMousePoint + *D3DXVec3Normalize(&vMouseRay, &vMouseRay) * fDist;
			D3DXVec3TransformCoord(pOutPos, &vLocalMouse, &WorldMatrix);
			return true;
		}

		// 좌 하단 삼각형.
		if (true == D3DXIntersectTri(&m_pVertices[iIndices[3]].vPosition, &m_pVertices[iIndices[2]].vPosition, &m_pVertices[iIndices[0]].vPosition, &vMousePoint, &vMouseRay, &fU, &fV, &fDist))
		{
			_float3 vLocalMouse = vMousePoint + *D3DXVec3Normalize(&vMouseRay, &vMouseRay) * fDist;
			D3DXVec3TransformCoord(pOutPos, &vLocalMouse, &WorldMatrix);
			return true;

		}
	}

	return false;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TERRAINDESC TerrainDesc)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(TerrainDesc)))
	{
		ERR_MSG(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

}
