#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Client)
class CBoarrior final : public CMonster
{
	enum STATE
	{
		IDLE,
		WALK,
		DASH,
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
		STUN,
		HIT,
		DIE,
		MAX
	};

private:
	CBoarrior(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoarrior(const CBoarrior& rhs);
	virtual ~CBoarrior() = default;

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
	// Boarrior Variables
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;
	int m_iPattern = 0;
	_float m_fFollowTime = 0.f;
	_float m_fSpecialAttackRadius;
	_float3 m_vAttackPos = _float3(0.f, 0.f, 0.f);
	
	// Bullet Variables
	_bool m_bShouldSpawnBullet = false;
	_float m_fBulletAliveTime = 999.f;
	_uint m_iBulletCount = 0;

	_float m_fStaggerDamageLimit = 80.f;
	_float m_fStaggerDamage = 0.f;
	
private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;
	void Attack(_float fTimeDelta, STATE eAttack = STATE::ATTACK_1);
	void Spawn_Bullet(_float fTimeDelta);
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public:
	static CBoarrior* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END