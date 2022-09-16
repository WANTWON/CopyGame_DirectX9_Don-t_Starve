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

class CSkill_Icon final : public CGameObject
{
private:
	CSkill_Icon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkill_Icon(const CSkill_Icon& rhs);
	virtual ~CSkill_Icon() = default;

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

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	_uint texnum = 22;
	_uint texnum1 = 22;
	_uint texnum2 = 22;
	_uint texnum3 = 22;

	_float timer = 0.f;

	bool forbomb = false;
	bool forice = false;
	bool forsand = false;
	bool forteleport = false;

	
	int* iNumber = nullptr;
	int iNum = 0;

public:
	static CSkill_Icon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


	void usebomb(_float time);
	void useice(_float time);
	void usesand(_float time);
	void useteleport(_float time);

	void setbombon() { texnum = 0; }
	void seticeon() { texnum1 = 0; }
	void setsandon() { texnum2 = 0; }
	void setteleporton() { texnum3 = 0; }

	int GetiNum() { return iNum; }
	
};

END