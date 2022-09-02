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

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) override;

private: /*For TextureCom */
	virtual HRESULT Texture_Clone() override;
	virtual void Change_Frame() override;
	virtual void Change_Motion() override;

public: /*For Picking */
	virtual _bool Picking(_float3* PickingPoint) override;
	virtual void PickingTrue() override;

private:
	STATE m_eState = HEALTHY;
	STATE m_ePreState = MAX;

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

public:
	static CBoulder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END