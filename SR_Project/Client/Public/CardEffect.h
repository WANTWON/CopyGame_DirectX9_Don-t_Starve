#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Pig.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CCardEffect final : public CGameObject
{
private:
	CCardEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCardEffect(const CCardEffect& rhs);
	virtual ~CCardEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

 void	SetUp_BillBoard();

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom1 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	UI_SHADER_STATE			m_eShaderID = UI_SHADER_CARD;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float alpha = 0.f;
	_float3 pos = { 0.f,0.f,0.f };
	_float3 portalpos = { 0.f,0.f,0.f };
	_float3 portalpos2 = { 0.f,0.f,0.f };


	_float time = 0.f;
	_float time2 = 0.f;
	//_float3 pos = { 0.f,0.f,0.f };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();


	_uint m_ihp;

	_bool m_bcheck = true;

	_bool thunder ;

	_bool m_bdead = false;


	LEVEL					m_iCurrentLevelndex;
	LEVEL                   m_iPreLevelIndex;


	foreffect effectdesc;



	_uint texnum = 0;

public:
	void set_check(bool tof) { m_bcheck = tof; }
	bool get_check(void) { return m_bcheck; }
	/*void Set_portalpos(_float3 pos) { portalpos = pos; }
	void Set_portalpos2(_float3 pos) { portalpos2 = pos; }*/
	static CCardEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END