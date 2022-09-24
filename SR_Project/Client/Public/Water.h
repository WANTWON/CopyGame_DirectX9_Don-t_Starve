#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
class CShader;
END

BEGIN(Client)

class CWater final : public CGameObject
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
	CWater(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWater(const CWater& rhs);
	virtual ~CWater() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	HRESULT Initialize_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
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
	CShader*				m_pShaderCom = nullptr;
private:
	TERRAINDESC				m_TerrainDesc;
	SHADER_STATE m_eShaderID = SHADER_IDLE;
private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_Components(const _tchar* VIBufferTag, void* pArg = nullptr);

public:
	static CWater* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
	virtual void Free() override;
};

END