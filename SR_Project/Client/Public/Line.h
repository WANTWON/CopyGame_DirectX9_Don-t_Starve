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

class CLine final : public CGameObject
{
private:
	CLine(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLine(const CLine& rhs);
	virtual ~CLine() = default;

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
	static CLine* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	bool get_check() { return m_bcheck; }
	void set_check(bool tof) { m_bcheck = tof; }
	int get_pontnum() { return iNum; }
	void set_pont_num(int num) { texnum = num; }
	int get_pontex() { return texnum; }
	void set_makewhat(MAKEWHAT item) { m_makewhat = item; }
	void plus_quest2count() { ++quest2count; }

	void gobackfirstX() { m_fX = m_firstx; }
	void gobackfirstY() { m_fX = m_firsty; }
	void set_line();

private:
	int* iNumber = nullptr;
	int iNum = 0;
	int texnum = 0;
	bool m_bcheck = true;
	_uint questnum = 0;
	_uint quest2count = 0;

	MAKEWHAT m_makewhat = MAKE_END;
};

END