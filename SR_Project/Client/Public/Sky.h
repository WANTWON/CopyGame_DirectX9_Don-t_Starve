#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CSky final : public CGameObject
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

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Cube*			m_pVIBufferCom = nullptr;

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