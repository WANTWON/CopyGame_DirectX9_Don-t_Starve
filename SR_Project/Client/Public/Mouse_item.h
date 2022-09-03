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

class CMouse_item final : public CGameObject
{
private:
	CMouse_item(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMouse_item(const CMouse_item& rhs);
	virtual ~CMouse_item() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

	bool get_check_bag() { return m_bcheck_bag; }
	bool get_check() { return m_bcheck; }
	void set_check(bool tof) { m_bcheck = tof; }
	void set_check_bag(bool tof) { m_bcheck_bag = tof; }
	void set_texnum(ITEMNAME itemname) { texnum = itemname; }
	
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
	static CMouse_item* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
private:

	int* iNumber = nullptr;
	int iNum = 0;
	bool m_bcheck_bag = false;
	bool m_bcheck = false;
	bool m_Lclick = false;
	ITEMNAME texnum = ITEMNAME_END;
	//bool m_bcheck = true;
};

END