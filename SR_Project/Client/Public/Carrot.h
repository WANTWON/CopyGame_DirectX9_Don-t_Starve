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
class CCarrot final : public CInteractive_Object
{
public:
	enum STATE
	{
		IDLE,
		MAX
	};

private:
	CCarrot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarrot(const CCarrot& rhs);
	virtual ~CCarrot() = default;

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
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

public:
	static CCarrot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END