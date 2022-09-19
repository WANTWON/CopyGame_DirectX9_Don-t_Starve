
#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Client)
class CShooting_Target final : public CMonster
{
	enum STATE
	{
		IDLE,
		MOVE,
		HIT,
		DIE,
		MAX
	};

private:
	CShooting_Target(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShooting_Target(const CShooting_Target& rhs);
	virtual ~CShooting_Target() = default;

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
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;
	_float3 m_vecOutPos; // For Picking

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual _bool IsDead() override;

public:
	static CShooting_Target* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END