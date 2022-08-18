#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect_Spear;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CWeapon final : public CGameObject
{
private:
	CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect_Spear*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

private:
	HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	_float3			m_TargetPos = { 1, 0, 1 };
	const _tchar*	m_TimerTag = TEXT("");
	_float  dwTime = GetTickCount();

public:
	static CWeapon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

