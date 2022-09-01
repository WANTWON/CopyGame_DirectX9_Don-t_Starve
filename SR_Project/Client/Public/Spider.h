#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Client)
class CSpider final : public CMonster
{
	enum STATE
	{
		IDLE,
		MOVE,
		ATTACK,
		TAUNT,
		HIT,
		DIE,
		MAX
	};

private:
	CSpider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSpider(const CSpider& rhs);
	virtual ~CSpider() = default;

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
	virtual void Change_Frame() override;
	virtual void Change_Motion() override;
	
private:
	DIR_STATE m_eDir = DIR_STATE::DIR_DOWN;
	DIR_STATE m_ePreDir = DIR_STATE::DIR_END;
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual HRESULT Drop_Items() override;
	virtual _bool IsDead() override;

public:
	static CSpider* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
	virtual void Free() override;
};
END