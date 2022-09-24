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
		SPAWN,
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

public: /*For Picking */
	virtual _bool Picking(_float3* PickingPoint) override;
	virtual void PickingTrue() override;

private:
	// Boarrior Variables
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;
	int m_iPattern = 0;
	_float m_fFollowTime = 0.f;
	_float m_fSpecialAttackRadius;
	_float3 m_vAttackPos = _float3(0.f, 0.f, 0.f);
	_bool m_bStun = false;
	_float m_fStunTime = 0.f;
	_float m_fStunTimeLimit = 3.f;
	
	// Bullet Variables
	_bool m_bShouldSpawnBullet = false;
	_float m_fBulletAliveTime = 999.f;
	_uint m_iBulletCount = 0;
	_uint iAnimFrameSyncCounter = 0;

	// Stagger Variables
	_float m_fStaggerDamageLimit = 80.f;
	_float m_fStaggerDamage = 0.f;

	// Spawn Variables
	_bool m_bIsBelow80Percent = false;
	_bool m_bIsBelow60Percent = false;
	_bool m_bIsBelow40Percent = false;
	_bool m_bIsBelow20Percent = false;
	_bool m_bShouldSpawn = false;
	_float m_fSpawnTime = 0.f;

	// Totem Variables
	_bool m_bHasDefenseBoost = false;
	_float m_fHealTimer = 0.f;

	_bool m_bfirst = true;

	_float3 m_vecOutPos; // For Picking

	
private:
	void Check_Totem_Effect(_float fTimeDelta);
	void Totem_Heal(_float fTimeDelta);
	void Check_Health_Percent();
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;
	void Attack(_float fTimeDelta, STATE eAttack = STATE::ATTACK_1);
	void Spawn_Bullet(_float fTimeDelta);
	void Spawn_Adds(_float fTimeDelta);
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	void Set_Stun() { m_bStun = true; }
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public:
	static CBoarrior* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END