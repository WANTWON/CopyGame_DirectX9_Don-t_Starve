#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CVertex final : public CGameObject
{
private:
	CVertex(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVertex(const CVertex& rhs);
	virtual ~CVertex() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;
	bool Picking(_float3* OutPos);


private: /* For.Components */
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT SetUp_SamplerState();
	HRESULT Release_RenderState();
	void Picking();
	

public:
	static CVertex* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool		m_EditMode = true;
};

END