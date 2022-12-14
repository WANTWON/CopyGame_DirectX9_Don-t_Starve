#include "..\Public\VIBuffer_Terrain.h"
#include "Picking.h"
#include "Transform.h"






CVIBuffer_Terrain::CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_TerrainDesc(rhs.m_TerrainDesc)
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

	VTXTEX*			m_pVertices = nullptr;
	m_pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&m_pVertices, 0);

	for (_uint i = 0; i < m_TerrainDesc.m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_TerrainDesc.m_iNumVerticesX; ++j)
		{
			_uint	iIndex = i * m_TerrainDesc.m_iNumVerticesX + j;

			m_pVertices[iIndex].vPosition = _float3(m_TerrainDesc.m_iPosVerticesX + j*m_TerrainDesc.m_fSizeX, 0.0f, m_TerrainDesc.m_iPosVerticesZ + i*m_TerrainDesc.m_fSizeZ);
			m_pVertices[iIndex].vTexture = _float2(j / (m_TerrainDesc.m_iNumVerticesX - 1.0f)*m_TerrainDesc.m_fTextureSize, i / (m_TerrainDesc.m_iNumVerticesZ - 1.0f)*m_TerrainDesc.m_fTextureSize);
		}
	}

	m_pVB->Unlock();

	m_iIndicesByte = sizeof(FACEINDICES32);
	m_eIndexFormat = D3DFMT_INDEX32;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	FACEINDICES32* m_pIndices = nullptr;
	m_pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&m_pIndices, 0);

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

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[1];
			m_pIndices[iNumFaces]._2 = iIndices[2];
			++iNumFaces;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[2];
			m_pIndices[iNumFaces]._2 = iIndices[3];
			++iNumFaces;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	ZeroMemory(&m_TerrainDesc, sizeof(TERRAINDESC));
	memcpy(&m_TerrainDesc, (TERRAINDESC*)pArg, sizeof(TERRAINDESC));

	m_iNumVertices = m_TerrainDesc.m_iNumVerticesX * m_TerrainDesc.m_iNumVerticesZ;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = (m_TerrainDesc.m_iNumVerticesX - 1) * (m_TerrainDesc.m_iNumVerticesZ - 1) * 2;

	/* 정점들을 할당했다. */
	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	VTXTEX*			pVertices = nullptr;
	pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	for (_uint i = 0; i < m_TerrainDesc.m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_TerrainDesc.m_iNumVerticesX; ++j)
		{
			_uint	iIndex = i * m_TerrainDesc.m_iNumVerticesX + j;
			pVertices[iIndex].vPosition = _float3(m_TerrainDesc.m_iPosVerticesX + j*m_TerrainDesc.m_fSizeX, 0.0f, m_TerrainDesc.m_iPosVerticesZ + i*m_TerrainDesc.m_fSizeZ);
			pVertices[iIndex].vTexture = _float2(j / (m_TerrainDesc.m_iNumVerticesX - 1.0f)*m_TerrainDesc.m_fTextureSize, i / (m_TerrainDesc.m_iNumVerticesZ - 1.0f)*m_TerrainDesc.m_fTextureSize);
		}
	}

	m_pVB->Unlock();

	m_iIndicesByte = sizeof(FACEINDICES32);
	m_eIndexFormat = D3DFMT_INDEX32;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	FACEINDICES32* m_pIndices = nullptr;
	m_pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&m_pIndices, 0);

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

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[1];
			m_pIndices[iNumFaces]._2 = iIndices[2];
			++iNumFaces;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[2];
			m_pIndices[iNumFaces]._2 = iIndices[3];
			++iNumFaces;
		}
	}

	m_pIB->Unlock();

	return S_OK;

	return S_OK;
}



_float CVIBuffer_Terrain::Get_TerrainY(_float Posx, _float Posz)
{
	_float fHeight = 0;
	_uint index = Posz *  m_TerrainDesc.m_iNumVerticesX + Posx;
	if (index > m_TerrainDesc.m_iNumVerticesX*m_TerrainDesc.m_iNumVerticesZ || index < 0)
		return 0;

	int OneTilePosX = Posx; /// m_TerrainDesc.m_iNumVerticesX;
	int OneTilePosZ = Posz; /// m_TerrainDesc.m_iNumVerticesZ;

	VTXTEX* m_pVertices = nullptr;
	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&m_pVertices, 0);
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

	if(fHeight != 0)
		cout << "Plane : " << fHeight << endl;

	m_pVB->Unlock();

	return fHeight + 1;
}

bool CVIBuffer_Terrain::Picking(class CTransform * pTransform, _float3 * pOut)
{
	CPicking*		pPicking = CPicking::Get_Instance();

	Safe_AddRef(pPicking);

	_float4x4   WorldMatrix = pTransform->Get_WorldMatrix();
	_float4x4	WorldMatrixInverse;
	D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);


	_float3			vMouseRay, vMousePoint;
	pPicking->Compute_LocalRayInfo(&vMouseRay, &vMousePoint, pTransform);

	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	FACEINDICES32* m_pIndices = nullptr;
	VTXTEX* m_pVertices = nullptr;
	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&m_pVertices, 0);
	m_pIB->Lock(0, 0, (void**)&m_pIndices, 0);


	Safe_Release(pPicking);

	for (int iIndex = 0; iIndex < m_iNumPrimitive; ++iIndex)
	{
		_float		fU, fV, fDist;

		if (true == D3DXIntersectTri(&m_pVertices[m_pIndices[iIndex]._0].vPosition, &m_pVertices[m_pIndices[iIndex]._1].vPosition,
			&m_pVertices[m_pIndices[iIndex]._2].vPosition, &vMousePoint, &vMouseRay, &fU, &fV, &fDist))
		{
			_float3 vLocalMouse = vMousePoint + *D3DXVec3Normalize(&vMouseRay, &vMouseRay) * fDist;
			D3DXVec3TransformCoord(pOut, &vLocalMouse, &WorldMatrix);
			m_pVB->Unlock();
			m_pIB->Unlock();
			return true;
		}
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return false;
}

void CVIBuffer_Terrain::UpTerrain(CTransform * pTransform, _float3 * pOut)
{
	CPicking*		pPicking = CPicking::Get_Instance();

	Safe_AddRef(pPicking);

	_float4x4   WorldMatrix = pTransform->Get_WorldMatrix();
	_float4x4	WorldMatrixInverse;
	D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);


	_float3			vMouseRay, vMousePoint;
	pPicking->Compute_LocalRayInfo(&vMouseRay, &vMousePoint, pTransform);

	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	FACEINDICES32* m_pIndices = nullptr;
	VTXTEX* m_pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&m_pVertices, 0);
	m_pIB->Lock(0, 0, (void**)&m_pIndices, 0);


	Safe_Release(pPicking);

	for (int iIndex = 0; iIndex < m_iNumPrimitive; ++iIndex)
	{
		_float		fU, fV, fDist;

		if (true == D3DXIntersectTri(&m_pVertices[m_pIndices[iIndex]._0].vPosition, &m_pVertices[m_pIndices[iIndex]._1].vPosition,
			&m_pVertices[m_pIndices[iIndex]._2].vPosition, &vMousePoint, &vMouseRay, &fU, &fV, &fDist))
		{
			m_pVertices[m_pIndices[iIndex]._0].vPosition.y += 0.001f;
			m_pVertices[m_pIndices[iIndex]._1].vPosition.y += 0.001f;
			m_pVertices[m_pIndices[iIndex]._2].vPosition.y += 0.001f;

			m_pVB->Unlock();
			m_pIB->Unlock();
			return;
		}
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return;
}

void CVIBuffer_Terrain::DownTerrain(CTransform * pTransform, _float3 * pOut)
{
	CPicking*		pPicking = CPicking::Get_Instance();

	Safe_AddRef(pPicking);

	_float4x4   WorldMatrix = pTransform->Get_WorldMatrix();
	_float4x4	WorldMatrixInverse;
	D3DXMatrixInverse(&WorldMatrixInverse, nullptr, &WorldMatrix);


	_float3			vMouseRay, vMousePoint;
	pPicking->Compute_LocalRayInfo(&vMouseRay, &vMousePoint, pTransform);

	D3DXVec3Normalize(&vMouseRay, &vMouseRay);

	FACEINDICES32* m_pIndices = nullptr;
	VTXTEX* m_pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&m_pVertices, 0);
	m_pIB->Lock(0, 0, (void**)&m_pIndices, 0);


	Safe_Release(pPicking);

	for (int iIndex = 0; iIndex < m_iNumPrimitive; ++iIndex)
	{
		_float		fU, fV, fDist;

		if (true == D3DXIntersectTri(&m_pVertices[m_pIndices[iIndex]._0].vPosition, &m_pVertices[m_pIndices[iIndex]._1].vPosition,
			&m_pVertices[m_pIndices[iIndex]._2].vPosition, &vMousePoint, &vMouseRay, &fU, &fV, &fDist))
		{
			m_pVertices[m_pIndices[iIndex]._0].vPosition.y -= 0.001f;
			m_pVertices[m_pIndices[iIndex]._1].vPosition.y -= 0.001f;
			m_pVertices[m_pIndices[iIndex]._2].vPosition.y -= 0.001f;

			m_pVB->Unlock();
			m_pIB->Unlock();
			return;
		}
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return;
}


HRESULT CVIBuffer_Terrain::Save_VertexPos(HANDLE hFile, _ulong& dwByte)
{

	/* 한번 TerrainDesc 정보 읽기 */
	WriteFile(hFile, &m_TerrainDesc, sizeof(TERRAINDESC), &dwByte, nullptr);


	/* 그 후에 정점 정보 읽기 */
	VTXTEX* m_pVertices = nullptr;
	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&m_pVertices, 0);

	for (_uint i = 0; i < m_TerrainDesc.m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_TerrainDesc.m_iNumVerticesX; ++j)
		{
			_uint	iIndex = i * m_TerrainDesc.m_iNumVerticesX + j;

			WriteFile(hFile, &m_pVertices[iIndex], sizeof(VTXTEX), &dwByte, nullptr);
		}
	}

	m_pVB->Unlock();

	return S_OK;

}


HRESULT CVIBuffer_Terrain::Load_Prototype(HANDLE hFile, _ulong& dwByte)
{
	
	VTXTEX VertexTag;

	/* 한번 TerrainDesc 정보 읽기 */
	ReadFile(hFile, &m_TerrainDesc, sizeof(TERRAINDESC), &dwByte, nullptr);

	/* 그리고 버텍스 정점 정보 읽기 */

	for (_uint i = 0; i < m_TerrainDesc.m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_TerrainDesc.m_iNumVerticesX; ++j)
		{
			_uint	iIndex = i * m_TerrainDesc.m_iNumVerticesX + j;

			ReadFile(hFile, &VertexTag, sizeof(VTXTEX), &dwByte, nullptr);

			vecVertex.push_back(VertexTag);
		}
	}


	m_iNumVertices = m_TerrainDesc.m_iNumVerticesX * m_TerrainDesc.m_iNumVerticesZ;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = (m_TerrainDesc.m_iNumVerticesX - 1) * (m_TerrainDesc.m_iNumVerticesZ - 1) * 2;

	/* 정점들을 할당했다. */
	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	VTXTEX*			pVertices = nullptr;
	pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	for (_uint i = 0; i < m_TerrainDesc.m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_TerrainDesc.m_iNumVerticesX; ++j)
		{
			_uint	iIndex = i * m_TerrainDesc.m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = vecVertex[iIndex].vPosition;
			pVertices[iIndex].vTexture = vecVertex[iIndex].vTexture;
		}
	}

	m_pVB->Unlock();

	m_iIndicesByte = sizeof(FACEINDICES32);
	m_eIndexFormat = D3DFMT_INDEX32;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	FACEINDICES32* m_pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&m_pIndices, 0);

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

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[1];
			m_pIndices[iNumFaces]._2 = iIndices[2];
			++iNumFaces;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[2];
			m_pIndices[iNumFaces]._2 = iIndices[3];
			++iNumFaces;
		}
	}

	m_pIB->Unlock();

	return S_OK;
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

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device, HANDLE hFile, _ulong& dwByte)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pGraphic_Device);

	if (FAILED(pInstance->Load_Prototype(hFile, dwByte)))
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
