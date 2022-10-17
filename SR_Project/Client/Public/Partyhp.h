#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CPartyhp final : public CGameObject
{
private:
	CPartyhp(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPartyhp(const CPartyhp& rhs);
	virtual ~CPartyhp() = default;

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
	CShader*				m_pShaderCom = nullptr;
	UI_SHADER_STATE			m_eShaderID = UI_SHADER_HP;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	_uint texnum = 50;
	_uint m_ihp;
	_uint iNum = 0;
	_uint whatnpc = 0;

	_bool m_bcheck = false;

	_bool m_bfirst = true;

	_float alpha1 = 0.f;

public:
	_bool Get_check() { return m_bcheck;}
	void Set_check(_bool tof) { m_bcheck = tof; }
	void Set_whatnpc(_uint num) { whatnpc = num; }


public:
	static CPartyhp* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END