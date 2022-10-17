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

class CBagInventory final : public CGameObject
{
private:
	CBagInventory(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBagInventory(const CBagInventory& rhs);
	virtual ~CBagInventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

	bool get_check_bag() { return m_bcheck_bag; }
	void set_check_bag(bool tof) { m_bcheck_bag = tof; }
	void set_closebag() { m_eState = CLOSE; }
	void set_openbag() { m_eState = OPEN; }

public: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

	vector<CTexture*> m_vecTexture;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CBagInventory* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	bool m_bcheck_bag = false;

public:
	enum STATE
	{IDLE , OPEN , CLOSE , MAX};
private:
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;
	const _tchar*	m_TimerTag = TEXT("");
};

END