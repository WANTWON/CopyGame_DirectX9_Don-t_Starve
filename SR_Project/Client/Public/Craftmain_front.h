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

class CCraftmain_front final : public CGameObject
{
private:
	CCraftmain_front(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCraftmain_front(const CCraftmain_front& rhs);
	virtual ~CCraftmain_front() = default;

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
	_float                  m_pfX, m_pfY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CCraftmain_front* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	bool get_check() { return m_bcheck; }
	bool get_pontcheck() { return m_bpontcheck; }
	bool get_check_bag() { return m_bcheck_bag; }
	void set_check(bool tof) { m_bcheck = tof; }
	void set_check_bag(bool tof) { m_bcheck_bag = tof; }
	/*void plus_itemcount() { ++item_number; }
	void minus_itemcount() { --item_number; }
	void set_itemcount(_uint count) { item_number = count; }*/
	void set_texnum(ITEMNAME texnumber) { texnum = texnumber; }
	ITEMNAME get_texnum() { return texnum; }
	/*_uint get_item_number() { return item_number; }*/
	//void Use_item(ITEMNAME item);
	int get_iNum() { return iNum; }

public:
	void Open_Weapontool(_float time) { m_fX += 20 * time; }

	void Open_Craft(_float time) { m_fY += 50 * time; }

	bool get_onof() { return m_bonof; }
	void set_onof(bool tof) { m_bonof = tof; }

	void gobackfirstX() { m_fX = m_firstx; }
	void gobackfirstY() { m_fX = m_firsty; }

	void set_makewhat(MAKEWHAT item) { m_makewhat = item; }

private:
	int* iNumber = nullptr;
	int iNum = 100;
	ITEMNAME texnum = ITEMNAME_END;
	ITEMID m_itemtype = ITEM_END;
	
	bool m_bcheck = true;
	bool m_bpontcheck = true;
	bool m_bcheck_bag = false;
	MAKEWHAT m_makewhat = MAKE_END;
	/*_uint itemnumber = 0;*/
	TOOLTYPE  m_tooltype = TOOL_END;

	bool m_bonof = false;
};

END