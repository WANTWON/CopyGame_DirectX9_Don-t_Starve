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

class CTalk final : public CGameObject
{
private:
	CTalk(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTalk(const CTalk& rhs);
	virtual ~CTalk() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom1 = nullptr;
	CTexture*				m_pTextureCom2 = nullptr;
	CTexture*				m_pTextureCom3 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	
public:
	void	Check_Quest(void);
	HRESULT	Excute(void);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	_uint texnum = 0;
	_uint texnum1 = 0; //forwendy
	_uint m_ihp;
	bool m_bcheck = false;
	bool m_SetTargetBearger = false;
	_bool m_bActivated = false;
	_bool	m_bTalkEnd = false;
	_bool	m_bIsStartText = true;
	_bool	m_bforwendy = false;
	_bool	m_bforWinona = false;
	_bool   m_bforWoody = false;


	_bool m_bquest1 = false;
	_bool m_bquest2 = false;
	_bool m_bquest3 = false;
	_bool m_bquest4 = false;
	_bool m_bquest5 = false;

	_bool m_bOnce = false;

public:
	bool getcheck() { return m_bcheck;  }
	void setcheck(bool tof) { m_bcheck = tof; }
	
	_uint gettexnum() { return texnum; }
	void settexnum(_uint num) { texnum = num; }

	void Set_Activated(_bool _Activated) { m_bActivated = _Activated; }
	_bool Get_Activated(void) { return m_bActivated; }

	void Set_TalkEnd(_bool _Activated) { m_bTalkEnd = _Activated; }
	_bool Get_TalkEnd(void) { return m_bTalkEnd; }

	void Set_StartText(_bool _Activated) { m_bIsStartText = _Activated; }
	_bool Get_StartText(void) { return m_bIsStartText; }

	void Set_WendyTalk(_bool _Activated) { m_bforwendy = _Activated; }

	void Set_Texnum1(_uint _iNum) { texnum1 = _iNum; }

	void Set_WinonaTalk(_bool _Activated) { m_bforWinona = _Activated; }

	void Set_WoodyTalk(_bool _Activated) { m_bforWoody = _Activated; }

private:
	CGameObject* Find_Boss(void);
public:
	static CTalk* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END