#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)
class CPortal final : public CGameObject
{
private:
	CPortal(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPortal(const CPortal& rhs);
	virtual ~CPortal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();
private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CCollider*	m_pColliderCom = nullptr;

private:
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;

public:
	static CPortal* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END