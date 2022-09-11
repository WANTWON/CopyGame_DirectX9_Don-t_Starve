#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Client)
class CBearger final : public CMonster
{
	enum STATE
	{
		IDLE,
		IDLE_AGGRO,
		WALK,
		RUN,
		CHARGE,
		ATTACK,
		POUND_GROUND,
		EAT,
		HIT,
		DIE,
		MAX
	};

private:
	CBearger(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBearger(const CBearger& rhs);
	virtual ~CBearger() = default;

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
	virtual void Change_Frame(_float fTimeDelta = 0) override;
	virtual void Change_Motion() override;

private:
	DIR_STATE m_eDir = DIR_STATE::DIR_DOWN;
	DIR_STATE m_ePreDir = DIR_STATE::DIR_END;
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;
	void Attack(_bool bIsSpecial = false);
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public:
	static CBearger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END