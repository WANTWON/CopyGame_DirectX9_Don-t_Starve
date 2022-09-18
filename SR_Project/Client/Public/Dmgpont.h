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

class CDmgpont final : public CGameObject
{
private:
	CDmgpont(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDmgpont(const CDmgpont& rhs);
	virtual ~CDmgpont() = default;

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
	void set_pos(_float3 p) { pos = p; }

	void set_first(bool tof) { m_bfirst = tof; }
	void SetUp_BillBoard();

	foreffect effectdesc;

	DWORD m_dwDeadtime = GetTickCount();


	int get_iNum() { return iNum; }

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pTransformCom1 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX = 0, m_fY = 0, m_fSizeX, m_fSizeY;

	_float3 mypos = _float3(0.f, 0.f, 0.f);
	_float3 pos = _float3(0.f, 0.f, 0.f);

	_float3 vdir = _float3(0.f, 0.f, 0.f);

	_float i = 1;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CDmgpont* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
private:

	int* iNumber = nullptr;
	int iNum = 0;
	bool m_bcheck_bag = true;
	bool m_bcheck = true;
	bool m_Lclick = true;
	bool one = false;
	
	_uint texnum = 0;
	_uint texnum1 = 0;
	_uint random = 0;
	
	bool m_bfirst = false;
	//bool m_bcheck = true;
};

END