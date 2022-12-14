#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CDaycountpont final : public CGameObject, public CObserver
{
private:
	CDaycountpont(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDaycountpont(const CDaycountpont& rhs);
	virtual ~CDaycountpont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom1 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	virtual void Update(_uint eDayState) override;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	_uint m_iTexNum = 0;
	_uint m_ihp;
	_uint m_iTexNum1 = 1;
	_bool m_bfirst = true;

	DWORD m_dwdaytime = GetTickCount();
	DWORD m_dwdaynight = GetTickCount();

	DAY_STATE m_eDayState = DAY_MORNING;

public:
	static CDaycountpont* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	_uint get_daycount() { return m_iTexNum; }

	DAY_STATE Get_nightandday() { return m_eDayState; }
};

END