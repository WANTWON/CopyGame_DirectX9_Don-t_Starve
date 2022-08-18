#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_float Get_TerrainY(_float Posx, _float Posz);

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;
	VTXTEX*			m_pVertices = nullptr;

public:
	static CVIBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iNumVerticesX, _uint iNumVerticesZ);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END