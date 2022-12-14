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

class CCraftmain_back final : public CGameObject
{
private:
	CCraftmain_back(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCraftmain_back(const CCraftmain_back& rhs);
	virtual ~CCraftmain_back() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

	bool get_check_bag() { return m_bcheck_bag; }
	void set_check_bag(bool tof) { m_bcheck_bag = tof; }
	int get_iNum() { return iNum; }


	void setcolor();

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
	static CCraftmain_back* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
public:
	void Open_Weapontool(_float time) { m_fX += 20 * time; }

	void Open_Craft(_float time) { m_fY += 50 * time; }

	bool get_onof() { return m_bonof; }
	void set_onof(bool tof) { m_bonof = tof; }
	void gobackfirstX() { m_fX = m_firstx; }
	void gobackfirstY() { m_fX = m_firsty; }

	_uint get_texnum() { return backtexnum; }

	void set_makewhat(MAKEWHAT item) { m_makewhat = item; }
private:

	int* iNumber = nullptr;
	int iNum = 0;
	bool m_bcheck_bag = false;
	_uint backtexnum = 0;
	MAKEWHAT m_makewhat = MAKE_END;

	bool m_bonof = false;
	//bool m_bcheck = true;
};

END