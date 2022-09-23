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

class CStatus_pont final : public CGameObject
{
private:
	CStatus_pont(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStatus_pont(const CStatus_pont& rhs);
	virtual ~CStatus_pont() = default;

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
	static CStatus_pont* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	bool get_check() { return m_bcheck; }
	int get_iNum() { return iNum; }
	void set_check(bool tof) { m_bcheck = tof; }
	void set_pont_num(_uint num) { texnum = num; }

	void set_hp(_uint hp) { m_ihp = hp; }
	//void set_boss(bool tof) { noboss = tof; }

private:
	int* iNumber = nullptr;
	int iNum = 0;
	_uint texnum = 0;
	bool m_bcheck = true;
	_uint m_ihp = 0;
	//bool noboss = true;

	_uint m_iattack = 0;
	_uint m_iarmor = 0;
};

END