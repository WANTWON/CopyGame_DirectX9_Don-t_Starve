#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct TerrainDesc
	{
		_uint			m_iNumVerticesX = 0;
		_uint			m_iNumVerticesZ = 0;
		_float			m_fSizeX = 1.f;
		_float			m_fSizeZ = 1.f;
		_float			m_fTextureSize = 30.f;
	}TERRAINDESC;
private:
	CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	virtual HRESULT Initialize_Prototype(TERRAINDESC TerrainDesc);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_float Get_TerrainY(_float Posx, _float Posz);
	bool Picking(class CTransform * pTransform, _float3 * pOut);
	void UpTerrain(class CTransform * pTransform, _float3 * pOut);

private:
	TERRAINDESC		m_TerrainDesc;
	VTXTEX*			m_pVertices = nullptr;
	FACEINDICES32*	m_pIndices = nullptr;

public:
	static CVIBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TERRAINDESC TerrainDesc);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END