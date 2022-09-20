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
class CCarnivalMemory;
class CCarnivalCard final : public CInteractive_Object
{
public:
	enum STATECARD
	{
		HINT_BAD,
		HINT_GOOD,
		OFF,
		ON,
		REVEAL_BAD,
		REVEAL_BAD_POST,
		REVEAL_BAD_PRE,
		REVEAL_GOOD,
		REVEAL_GOOD_POST,
		REVEAL_GOOD_PRE,
		TURN_OFF,
		TURN_ON,
		MAX
	};

private:
	CCarnivalCard(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarnivalCard(const CCarnivalCard& rhs);
	virtual ~CCarnivalCard() = default;

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

public: /*For Picking */
	virtual _bool Picking(_float3* PickingPoint) override;
	virtual void PickingTrue() override;

public:
	void Set_Memory(CCarnivalMemory* pMemory) { m_pMemory = pMemory; }
	void Turn_On() { m_eState = STATECARD::TURN_ON; }
	void Give_Hint(_bool isGood) 
	{
		m_bIsGood = isGood;
		isGood ? m_eState = STATECARD::HINT_GOOD : m_eState = STATECARD::HINT_BAD;
	}
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

private:
	STATECARD m_eState = OFF;
	STATECARD m_ePreState = MAX;
	CCarnivalMemory* m_pMemory = nullptr;
	_bool m_bIsGood = false;

public:
	static CCarnivalCard* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END