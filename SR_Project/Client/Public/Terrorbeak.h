#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Client)
class CTerrorbeak final : public CMonster
{
	enum STATE
	{
		APPEAR,
		ATTACK,
		DISAPPEAR,
		IDLE,
		WALK,
		MAX
	};

private:
	CTerrorbeak(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTerrorbeak(const CTerrorbeak& rhs);
	virtual ~CTerrorbeak() = default;

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
	STATE m_eState = STATE::APPEAR;
	STATE m_ePreState = STATE::MAX;
	_float3 m_vecOutPos; // For Picking

	_float m_bFirstFrame = true;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;
	void Attack(_float fTimeDelta);
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public:
	static CTerrorbeak* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END