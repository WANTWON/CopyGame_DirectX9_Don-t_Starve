#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	typedef struct TerrainDesc
	{
		_uint			m_iPosVerticesX = 0;
		_uint			m_iPosVerticesZ = 0;
		_uint			m_iNumVerticesX = 0;
		_uint			m_iNumVerticesZ = 0;
		_float			m_fSizeX = 1.f;
		_float			m_fSizeZ = 1.f;
		_float			m_fTextureSize = 30.f;
		_int			m_iTextureNum = 0;

	}TERRAINDESC;

private:
	CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	TERRAINDESC Get_TerrainDesc() { return m_TerrainDesc; }

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	TERRAINDESC				m_TerrainDesc;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT SetUp_SamplerState();
	HRESULT Release_RenderState();

public:
	void SetUp_TerrainY();
	void Picking();

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END