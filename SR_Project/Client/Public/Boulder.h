#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Interactive_Object.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBoulder final : public CInteractive_Object
{
public:
	enum STATE 
	{ 
		HEALTHY, 
		DAMAGED, 
		BROKEN, 
		MAX 
	};

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
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

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
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CCollider* m_pColliderCom = nullptr;

	vector<CTexture*> m_vecTexture;

private:
	STATE m_eState = HEALTHY;
	STATE m_ePreState = MAX;
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;
	OBJINFO m_tInfo;


public: /*For Picking */
	virtual _bool Picking(_float3* PickingPoint);
	virtual void PickingTrue();
private:
	_float3 m_vecOutPos;
	_bool m_bPicking;


public:
	static CBoulder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END