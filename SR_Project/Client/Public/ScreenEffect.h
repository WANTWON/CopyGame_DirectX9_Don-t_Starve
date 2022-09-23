#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CScreenEffect final : public CGameObject
{
private:
	CScreenEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CScreenEffect(const CScreenEffect& rhs);
	virtual ~CScreenEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	UI_SHADER_STATE			m_eShaderID = UI_SHADER_SCREEN;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float alpha = 0.f;

	DWORD m_dwDeadtime = GetTickCount();
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	_uint texnum = 0;
	_uint m_ihp;
   

	_bool m_bforboss = false;

	_bool m_bfirst = true;

public:
	static CScreenEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END