#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"
#include "CarnivalMemory.h"

BEGIN(Client)
class CCarnival_Egg final : public CMonster
{
	enum STATE
	{
		IDLE,
		WALK,
		COMPLETE, 
		PLACE,
		MAX
	};



private:
	CCarnival_Egg(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarnival_Egg(const CCarnival_Egg& rhs);
	virtual ~CCarnival_Egg() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;
	void Set_Memory(CCarnivalMemory* pMemory) { m_pMemory = pMemory; }
private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) override;
	
private: /*For TextureCom */
	virtual HRESULT Texture_Clone() override;
	virtual void Change_Frame(_float fTimeDelta) override;
	virtual void Change_Motion() override;

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
	_bool	m_bMove = false;

	CCarnivalMemory* m_pMemory = nullptr;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Patrol(_float fTimeDelta);
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public:
	static CCarnival_Egg* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END