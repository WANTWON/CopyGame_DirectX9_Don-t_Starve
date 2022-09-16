#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CTotemEffect final : public CGameObject
{
private:
	CTotemEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTotemEffect(const CTotemEffect& rhs);
	virtual ~CTotemEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void SetUp_BillBoard();
	void WalkingTerrain();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /*For TextureCom */
	virtual HRESULT Texture_Clone();
	virtual void Change_Frame();

private:
	const _tchar* m_TimerTag = TEXT("");

	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;

	vector<CTexture*> m_vecTexture;

public:
	static CTotemEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END