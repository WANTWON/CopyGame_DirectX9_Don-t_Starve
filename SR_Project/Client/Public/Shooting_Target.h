
#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Client)
class CShooting_Target final : public CMonster
{
public:
	enum TARGETTYPE {TARGET_BAD, TARGET_GOOD};
	enum STATE
	{
		IDLE,
		HIT,
		STOP,
		MAX
	};

	typedef struct ShootingtargetDesc
	{
		TARGETTYPE eType = TARGET_BAD;
		_float3 vPosition;
	}TARGETDESC;

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

public:
	_bool Get_Complete() { if (m_ShootingTargetDesc.eType == TARGET_GOOD && m_eState == HIT) return true; else return false; }

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
	TARGETDESC m_ShootingTargetDesc;

private:
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	virtual _bool IsDead() override;

public:
	static CShooting_Target* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END