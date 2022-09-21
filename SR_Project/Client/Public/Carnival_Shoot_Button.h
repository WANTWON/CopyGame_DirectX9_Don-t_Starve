#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Interactive_Object.h"


BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CCarnival_Shoot_Button final : public CInteractive_Object
{
public:
	enum STATECARD
	{
		IDLE,
		PRESS,
		MAX
	};

private:
	CCarnival_Shoot_Button(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarnival_Shoot_Button(const CCarnival_Shoot_Button& rhs);
	virtual ~CCarnival_Shoot_Button() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) override;
	void Change_Camera();
	_bool Check_EveryTarget_Complete();

private: /*For TextureCom */
	virtual HRESULT Texture_Clone() override;
	virtual void Change_Frame(_float fTimeDelta) override;
	virtual void Change_Motion() override;

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items()override { return S_OK; };
private:
	STATECARD m_eState = IDLE;
	STATECARD m_ePreState = MAX;

public:
	static CCarnival_Shoot_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END