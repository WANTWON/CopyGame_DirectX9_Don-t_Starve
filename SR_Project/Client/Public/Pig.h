#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Client)
class CPig final : public CMonster
{
	enum STATE
	{
		IDLE,
		WALK,
		RUN,
		ATTACK,
		HIT,
		DIE,
		HAPPY,
		INTERACT,
		MAX
	};

private:
	CPig(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPig(const CPig& rhs);
	virtual ~CPig() = default;

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

public: /*Interact*/
	void Interact(_float _fTimeDelta, _uint _iNum);

public://Get&&Set
	void	Set_CanMove(_bool _bMove) { m_bMove = _bMove; }
	_bool	Get_CanMove(void) { return m_bMove; }
private:
	DIR_STATE m_eDir = DIR_STATE::DIR_DOWN;
	DIR_STATE m_ePreDir = DIR_STATE::DIR_END;
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;
	_float fHappyTimer = 0.f;
	_float3 m_vecOutPos; // For Picking

	_bool	m_bMove = true;
public:
	void Give_Food() { m_eState = STATE::HAPPY; }

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public:
	static CPig* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END