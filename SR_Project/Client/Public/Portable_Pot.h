#pragma once
#include "Interactive_Object.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
//class CCollider;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPortable_Pot : public CInteractive_Object
{
public:
	enum STATE
	{
		PLACE,
		COOK,
		DESTROY,
		IDLE,
		MAX
	};
	typedef struct tagPotDesc
	{
		CGameObject* pOwner;
		_float3		 vInitPos;
	}POTDESC;

private:
	CPortable_Pot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPortable_Pot(const CPortable_Pot& rhs);
	virtual ~CPortable_Pot() = default;

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
	virtual void Change_Frame(_float fTimeDelta) override;
	virtual void Change_Motion() override;

private:
	STATE m_eState = PLACE;
	STATE m_ePreState = MAX;
	POTDESC	m_tDesc;
public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;
	virtual void Destroy() override;

public:
	static CPortable_Pot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END