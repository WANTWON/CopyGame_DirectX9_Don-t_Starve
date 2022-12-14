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
		PRE_EAT,
		EAT,
		POST_EAT,
		HIT,
		DIE,
		MAX
	};
public:
	typedef struct BeargerDesc
	{
		CGameObject* ppObj;
		_float3		fInitPos;
	}BEARDESC;
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
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;
	_float m_fEatRadius;
	_float m_fCameraShakeRadius;
	_bool m_bIsEating = false;
	_float3 vPatrolPosition;
	_float fEatTimer = 0.f;
	_bool m_bFirstFrame = true;
	_float3 m_vecOutPos; // For Picking

	// Stagger Variables
	_float m_fStaggerDamageLimit = 80.f;
	_float m_fStaggerDamage = 0.f;

	_float m_fChargeTime = 0.f;
	_bool m_bUseHitShader = false;
	_float m_fHitTime = 0.f;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual void Find_Target() override;
	void Follow_Target(_float fTimeDelta, _bool bIsFood = false);
	void Attack(_bool bIsSpecial = false);
	void Check_CameraShake(STATE eState);
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public: /*For Picking */
	virtual _bool Picking(_float3* PickingPoint) override;
	virtual void PickingTrue() override;

public:
	static CBearger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END