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

class CEquipment_front final : public CGameObject
{
private:
	CEquipment_front(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEquipment_front(const CEquipment_front& rhs);
	virtual ~CEquipment_front() = default;

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

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CEquipment_front* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	bool get_check() { return m_bcheck; }
	void set_check(bool tof) { m_bcheck = tof; }
	void plus_itemcount() { item_number = 6; }
	void set_texnum(ITEMNAME texnumber) { texnum = texnumber; }
	_uint get_item_number() { return item_number; }
	void Use_item(ITEMNAME item);
	int get_iNum() { return iNum; }
	ITEMNAME get_texnum() { return texnum; }

private:
	int* iNumber = nullptr;
	int iNum = 0;
	ITEMNAME texnum = ITEMNAME_END;
	WEAPON_TYPE weapontype = WEAPON_TYPE::WEAPON_HAND;
	_uint item_number = 0;//æ∆¿Ã≈€∞πºˆ
	bool m_bcheck = true;
};

END
