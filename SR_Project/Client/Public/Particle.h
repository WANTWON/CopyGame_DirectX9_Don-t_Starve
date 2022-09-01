#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CTexture;
class CCollider;
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

private:
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;


private:
	STATEDESC			m_StateDesc;
	_uint				m_iCurrentFrameIndex;
	_float				m_fAlpha;

public:
	static CParticle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END