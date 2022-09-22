#pragma once
#include "Interactive_Object.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
//class CCollider;
END

BEGIN(Client)
class CBattery : public CInteractive_Object
{
public:
	typedef struct tagBatteryDesc
	{
		CGameObject* pOwner;
		_float3		 vInitPos;
	}BatteryDesc;
public:
	CBattery(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBattery(const CBattery& rhs);
	virtual ~CBattery() = default;
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
	void Position_Offest();

private:
	_uint		m_iBattery = 6;
	BatteryDesc m_tDesc;
public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;
	virtual void Destroy() override;

public:
	static CBattery* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
