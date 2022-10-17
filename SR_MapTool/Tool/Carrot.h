#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Tool)
class CCarrot final : public CGameObject
{
public:
	enum STATE
	{
		IDLE,
		MAX
	};

private:
	CCarrot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarrot(const CCarrot& rhs);
	virtual ~CCarrot() = default;

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
private:
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;

public:
	 void Interact(_uint Damage = 0);

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Terrain* m_pVIBufferCom = nullptr;
	vector<CTexture*> m_vecTexture;

private:
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;
	OBJINFO m_tInfo;

public:
	static CCarrot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END