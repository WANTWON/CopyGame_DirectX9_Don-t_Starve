#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Particle.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CTexture;
class CCollider_Manager;
class CShader;
END


BEGIN(Client)

class CParticle : public CGameObject
{
public:
	typedef struct tagStateDesc
	{
		LEVEL			eTextureScene;
		const _tchar*		pTextureKey;
		_uint				iTextureNum;
		_float3				vVelocity;

	}STATEDESC;

private:
	CParticle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void SetUp_BillBoard();
	void Set_ShaderID();
private:
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	SHADER_STATE		m_eShaderID = SHADER_IDLE;

private:
	STATEDESC			m_StateDesc;
	_uint				m_iCurrentFrameIndex = 0;
	_float				m_fAlpha = 0;

public:
	static CParticle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END