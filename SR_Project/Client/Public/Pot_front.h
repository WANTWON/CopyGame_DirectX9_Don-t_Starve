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

class CPot_front final : public CGameObject
{
private:
	CPot_front(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPot_front(const CPot_front& rhs);
	virtual ~CPot_front() = default;

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
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float                  m_pfX, m_pfY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CPot_front* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


	bool get_check() { return m_bcheck; }

	bool get_check_pot() { return m_bcheck_pot; }
	void set_check(bool tof) { m_bcheck = tof; }
	void set_check_pot(bool tof) { m_bcheck_pot = tof; }
	void plus_itemcount() { ++item_number; }
	void minus_itemcount() { --item_number; }
	void minus_material(_uint minus) { item_number -= minus; }
	void set_itemcount(_uint count) { item_number = count; }
	void set_texnum(ITEMNAME texnumber) { texnum = texnumber; }
	ITEMNAME get_texnum() { return texnum; }
	_uint get_item_number() { return item_number; }
	
	int get_iNum() { return iNum; }

private:
	int* iNumber = nullptr;
	int iNum = 100;
	ITEMNAME texnum = ITEMNAME_END;
	ITEMID m_itemtype = ITEM_END;
	_uint item_number = 1;//æ∆¿Ã≈€∞πºˆ
	bool m_bcheck = true;
	bool collided = false;
	bool m_bcheck_pot = false;

};

END