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
	enum TYPE { CARD, BIRD, TYPE_MAX };

	typedef struct TagDescription
	{
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
		TYPE eType = TYPE_MAX;
	}DESC;

	enum STATE
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
		FED,
		HUNGRY,
		HUNGRY_POST,
		HUNGRY_PRE,
		IDLE,
		STATE_MAX
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
	DESC Get_Desc() { return m_tDesc; }
	_bool Get_IsHungry() { return m_bIsHungry; }
	void Set_Memory(CCarnivalMemory* pMemory) { m_pMemory = pMemory; }
	void Set_GameWon() { m_bIsGameWon = true; }
	void Turn_On() { m_eState = STATE::TURN_ON; }
	void Turn_Off() { m_eState = STATE::TURN_OFF; }
	void Hungry_On() { m_eState = STATE::HUNGRY_PRE; m_bIsHungry = true; }
	void Give_Hint(_bool isGood) 
	{
		m_bIsGood = isGood;
		isGood ? m_eState = STATE::HINT_GOOD : m_eState = STATE::HINT_BAD;
	}
	_bool Check_Hungry(_float fTimeDelta);
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

private:
	// Game Variables
	DESC m_tDesc;
	STATE m_eState = OFF;
	STATE m_ePreState = STATE_MAX;
	
	// Memory Variables
	CCarnivalMemory* m_pMemory = nullptr;
	_bool m_bIsGood = false;

	// Bird Variables
	_float m_fIdleTime = 0.f;
	_float m_fRandomIdlePause = rand() % 10 + 1;
	_bool m_bIsHungry = false;
	_uint m_iFeedMultiplier = 8;
	_float m_fFeedRandomTimeLimit = rand() % m_iFeedMultiplier + 1;
	_float m_fFeedTime = 0.f;
	_bool m_bIsGameWon = false;

public:
	static CCarnivalCard* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END