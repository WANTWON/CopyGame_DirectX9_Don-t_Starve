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

	void Update_Position(_float3 position) { m_vPosition = position; }
	//Get
	_float3 Get_Position() { return m_vPosition; }
	_float  Get_Radius() { return m_fRadius; }
	//Set
	void	Set_Radius(_float _fRadius) { m_fRadius = _fRadius;}
protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;

protected:
	map<const _tchar*, class CComponent*>				m_Components;
	_float3												m_vPosition;
	_float												m_fRadius = 0.5;
	_bool												m_bDead = false;

protected:
	HRESULT Add_Components(const _tchar* pComponentTag, _uint iLevelIndex, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Change_Component(const _tchar* pComponentTag, CComponent** ppOut);

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END

