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
class CItem final : public CInteractive_Object
{
public:
	typedef struct tagItemDesc
	{
		_float3 fPosition = { 0,0,0 };
		const _tchar* pTextureComponent;
		const _tchar* pTexturePrototype;
		ITEMNAME eItemName;
	} ITEMDESC;

private:
	CItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }
	//New
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /*For TextureCom */
	HRESULT Texture_Clone();

private: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

	vector<CTexture*> m_vecTexture;

private:
	_float m_fTerrain_Height = 0.f;
	ITEMDESC m_ItemDesc;

public:
	static CItem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END