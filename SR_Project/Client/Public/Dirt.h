#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Interactive_Object.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CDirt final : public CInteractive_Object
{
public:
	enum STATE
	{
		IDLE,
		EMPTY,
		MAX
	};

private:
	CDirt(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDirt(const CDirt& rhs);
	virtual ~CDirt() = default;

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

private:
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;

	// Monster List
	vector<WCHAR*> m_lMonstersTable =
	{
		TEXT("Prototype_GameObject_Pig"),
		TEXT("Prototype_GameObject_Spider"),
		TEXT("Prototype_GameObject_Spider_Warrior")
	};

	// Item List
	vector<_uint> m_lItemIdTable = 
	{ 
		ITEMNAME_BERRY, 
		ITEMNAME_WOOD, 
		ITEMNAME_CARROT, 
		ITEMNAME_GRASS, 
		ITEMNAME_GOLD, 
		ITEMNAME_ROCK2 
	};

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

public:
	static CDirt* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END