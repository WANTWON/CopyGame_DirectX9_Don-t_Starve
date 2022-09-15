#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLoadingscene final : public CGameObject
{
private:
	CLoadingscene(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLoadingscene(const CLoadingscene& rhs);
	virtual ~CLoadingscene() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

	void bosson() { m_boss = true; }

	void bossoff() { m_boss = false; }

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom1 = nullptr;
	CTexture*				m_pTextureCom2 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	//m_pTextureCom

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	_uint texnum = 0;
	_uint texnum1 = 0;
	DWORD m_dwdaytime = GetTickCount();

	_bool m_boss = false;

public:
	static CLoadingscene* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	
};

END