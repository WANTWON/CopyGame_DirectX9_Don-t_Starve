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

class CMouse_iteminfo final : public CGameObject
{
private:
	CMouse_iteminfo(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMouse_iteminfo(const CMouse_iteminfo& rhs);
	virtual ~CMouse_iteminfo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

	
	bool get_check() { return m_bcheck; }
	void set_check(bool tof) { m_bcheck = tof; }

	bool get_test() { return test; }
	void set_test(bool tof) { test = tof; }
	
	void set_texnum(_uint itemname) { texnum = itemname; }
	void set_itemname(ITEMNAME name) { itemname = name; }

	int get_iNum() { return iNum; }

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CMouse_iteminfo* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
private:

	int* iNumber = nullptr;
	int iNum = 0;
	
	bool m_bcheck = false;
	bool m_Lclick = false;
	bool test = true;
	_uint texnum = 30;
	ITEMNAME itemname = ITEMNAME_END;
	//bool m_bcheck = true;
};

END