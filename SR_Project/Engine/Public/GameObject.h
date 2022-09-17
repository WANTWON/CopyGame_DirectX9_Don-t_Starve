#pragma once

#include "Base.h"

/* 게임객체들의 부모가되는 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Update_Position(_float3 position) { m_vPosition = position; }
	_float3 Get_Position() { return m_vPosition; }
	_float  Get_Radius() { return m_fRadius; }
	_bool	Get_Dead() { return m_bDead; }
	_float  Get_CamDistance() { return m_fCamDistance; }
	void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	void	Set_Radius(_float _fRadius) { m_fRadius = _fRadius;}
	void	Set_bConstruct(_bool type) { m_bConstruct = type; }


public:
	virtual _bool Picking(_float3* PickingPoint) { return true; }
	virtual void PickingTrue() { return; }
	void Compute_CamDistance(_float3 WorldPos);

	//Damage Func
	//Apply_Damage
	//virtual void	Apply_Damage(_float fDamage, CGameObject* DamagedObj, void* AttackType = nullptr);
	//virtual void	Apply_Damage_Multi(_float fDamage, vector<CGameObject*>& vecDamagedObj, void* AttackType = nullptr);
	
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser);
protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;

protected:
	map<const _tchar*, class CComponent*>				m_Components;
	_float3												m_vPosition;
	_float												m_fRadius = 0.5;
	_bool												m_bDead = false;
	_bool												m_bConstruct = false;
	_float4x4											m_CollisionMatrix;
	_float												m_fCamDistance;

protected:
	HRESULT Add_Components(const _tchar* pComponentTag, _uint iLevelIndex, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Change_Component(const _tchar* pComponentTag, CComponent** ppOut);

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, _uint LevelIndex, void* pArg = nullptr) { return nullptr; };
	virtual void Free() override;
};

END

