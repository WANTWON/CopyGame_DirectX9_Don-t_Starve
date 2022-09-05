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

class CToolboxConstruct_back final : public CGameObject
{
private:
	CToolboxConstruct_back(LPDIRECT3DDEVICE9 pGraphic_Device);
	CToolboxConstruct_back(const CToolboxConstruct_back& rhs);
	virtual ~CToolboxConstruct_back() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

	bool get_check_bag() { return m_bcheck_bag; }
	void set_check_bag(bool tof) { m_bcheck_bag = tof; }
	int get_iNum() { return iNum; }

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY, m_firstx;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CToolboxConstruct_back* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
public:
	void Open_Weapontool(_float time) { m_fX += 130.f * time; }
	bool get_onof() { return m_bonof; }
	void set_onof(bool tof) { m_bonof = tof; }
	void gobackfirstX() { m_fX = m_firstx; }
private:

	int* iNumber = nullptr;
	int iNum = 0;
	bool m_bcheck_bag = false;
	_uint backtexnum = 0;

	bool m_bonof = false;
	//bool m_bcheck = true;
};

END