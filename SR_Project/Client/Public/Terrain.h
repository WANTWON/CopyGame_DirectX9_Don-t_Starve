#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
class CShader;
END

BEGIN(Client)

class CTerrain final : public CGameObject, public CObserver
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
	HRESULT Initialize_Load(const _tchar* VIBufferTag, LEVEL TerrainLevelIndex, void* pArg = nullptr);
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	TERRAINDESC Get_TerrainDesc() { return m_TerrainDesc; }
	virtual void Update(_uint eDayState) override { m_eDayState = (DAY_STATE)eDayState; };

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	TERRAINDESC				m_TerrainDesc;
	SHADER_STATE m_eShaderID = SHADER_IDLE;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_Components(const _tchar* VIBufferTag, LEVEL TerrainLevelIndex, void* pArg = nullptr);
	
public:
	virtual _bool Picking(_float3* PickingPoint);
	virtual void PickingTrue();
	void Check_ShaderColor();


private:
	_float3 m_vecOutPos;
	_bool m_bPicking;

	/* for test Shader Color*/
	_float m_fDinnerMaxRange = 15.f;
	_float m_fDinnerMinRange = 10.f;
	_float m_fDinnerDelta = 0.f;
	_float m_fNightDelta = 0.f;
	_float m_fNightAlpha = 0.f;
	DAY_STATE m_eDayState = DAY_MORNING;
public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, _uint LevelIndex, void* pArg = nullptr);
	virtual void Free() override;
};

END