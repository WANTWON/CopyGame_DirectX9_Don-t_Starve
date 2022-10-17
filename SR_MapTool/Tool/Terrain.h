#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Terrain.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CTerrain final : public CGameObject
{
private:
	CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	HRESULT Initialize_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	CVIBuffer_Terrain::TERRAINDESC Get_TerrainDesc() { return m_pVIBufferCom->GetTerrainDesc(); }
	HRESULT Save_Data(HANDLE hFile, _ulong& dwByte);
	bool Picking(_float3* OutPos);


private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_Components(const _tchar* VIBufferTag, void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT SetUp_SamplerState();
	HRESULT Release_RenderState();

private:
	void Picking();
	

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr) override;
	virtual void Free() override;
};

END