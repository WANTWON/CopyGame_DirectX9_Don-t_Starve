#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)
class CBoulder final : public CGameObject
{
public:
	enum STATE { HEALTHY, DAMAGED, BROKEN, MAX };

private:
	CBoulder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoulder(const CBoulder& rhs);
	virtual ~CBoulder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /* For TransformCom*/
	void SetUp_BillBoard();

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Terrain* m_pVIBufferCom = nullptr;
	vector<CTexture*> m_vecTexture;

private:
	STATE m_eState = HEALTHY;
	STATE m_ePreState = MAX;
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;

public:
	static CBoulder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar * VIBufferTag, void * pArg = nullptr) override;
	virtual void Free() override;
};
END