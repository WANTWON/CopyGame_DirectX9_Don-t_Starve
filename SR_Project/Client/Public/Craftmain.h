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

class CCraftmain final : public CGameObject
{
private:
	CCraftmain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCraftmain(const CCraftmain& rhs);
	virtual ~CCraftmain() = default;

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

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY, m_firstx, m_firsty;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CCraftmain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	void Open_Weapontool(_float time) { m_fX += 20 * time; }

	void Open_Craft(_float time) { m_fY += 50 * time; }

	bool get_onof() { return m_bonof; }
	void set_onof(bool tof) { m_bonof = tof; }
	void gobackfirstX() { m_fX = m_firstx; }
	void gobackfirstY() { m_fX = m_firsty; }

	void set_makewhat(MAKEWHAT item) { m_makewhat = item; }

private:
	MAKEWHAT m_makewhat = MAKE_END;
	bool m_bcheck = false;
	bool m_bonof = false;
};

END