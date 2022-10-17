#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Tool)
class CDirt final : public CGameObject
{
public:
	enum STATE
	{
		IDLE,
		EMPTY,
		MAX
	};

private:
	CDirt(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDirt(const CDirt& rhs);
	virtual ~CDirt() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	 HRESULT SetUp_Components(void* pArg = nullptr);
	 void SetUp_BillBoard();
	 HRESULT SetUp_RenderState();
	 HRESULT Release_RenderState();
private: /*For TextureCom */
	 HRESULT Texture_Clone();

private:
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;


private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	vector<CTexture*> m_vecTexture;


public:
	static CDirt* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END