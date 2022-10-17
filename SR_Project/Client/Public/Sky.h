#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer.h"

BEGIN(Engine)
class CObserver;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
class CShader;
END

BEGIN(Client)

class CSky final : public CGameObject, public CObserver
{
private:
	CSky(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;
	void Check_ShaderColor();
	virtual void Update(_uint eDayState) override { m_eDayState = (DAY_STATE)eDayState; };

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Cube*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	SHADER_STATE			m_eShaderID;

	/* for test Shader Color*/
	_float g_fDinnerDelta = 0.f;
	_float g_fNightDelta = 0.f;
	DAY_STATE m_eDayState = DAY_MORNING;

private:
	HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private:
	int iTextnum = 2;

public:
	static CSky* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END