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

class CDeadcountpont final : public CGameObject
{
private:
	CDeadcountpont(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDeadcountpont(const CDeadcountpont& rhs);
	virtual ~CDeadcountpont() = default;

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
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY , m_firsty;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CDeadcountpont* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	bool get_check() { return m_bcheck; }
	int get_iNum() { return iNum; }
	void set_check(bool tof) { m_bcheck = tof; }
	void set_pont_num(int num) { texnum = num; }
	void timeron() { count = 30; }

	void Open_DeadUI(_float time) { m_fY -= 135.f * time; }



private:
	int* iNumber = nullptr;
	int iNum = 0;
	int texnum = 0;
	bool m_bcheck = false;

	_uint count = 30;

	DWORD m_dwdeadcount = GetTickCount();
};

END