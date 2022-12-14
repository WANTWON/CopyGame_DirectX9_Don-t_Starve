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

class CMiniMap_Icon final : public CGameObject
{
private:
	CMiniMap_Icon(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMiniMap_Icon(const CMiniMap_Icon& rhs);
	virtual ~CMiniMap_Icon() = default;

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
	UI_SHADER_STATE			m_eShaderID = UI_SHADER_IDLE;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float alpha = 0.f;
	_float3 pos = { 0.f,0.f,0.f };
	_float3 portalpos = { 0.f,0.f,0.f };
	_float3 portalpos2 = { 0.f,0.f,0.f };
	//_float3 pos = { 0.f,0.f,0.f };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();


	_uint m_ihp;

	_bool m_bcheck = false;


	LEVEL					m_iCurrentLevelndex;
	LEVEL                   m_iPreLevelIndex;
	


	MINIMAP minidesc;


	MININAME texnum;

public:
	void set_check(bool tof) { m_bcheck = tof; }
	bool get_check(void) { return m_bcheck; }
	/*void Set_portalpos(_float3 pos) { portalpos = pos; }
	void Set_portalpos2(_float3 pos) { portalpos2 = pos; }*/
	static CMiniMap_Icon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END