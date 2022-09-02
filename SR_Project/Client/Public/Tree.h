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
class CCollider;
END

BEGIN(Client)
class CTree final : public CInteractive_Object
{
public:
	enum STATE 
	{ 
		IDLE, 
		CHOP, 
		SHAKE, 
		GROW, 
		FALL_RIGHT, 
		FALL_LEFT, 
		STUMP, 
		MAX 
	};

private:
	CTree(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTree(const CTree& rhs);
	virtual ~CTree() = default;

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
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

public:
	static CTree* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar * VIBufferTag, void * pArg = nullptr) override;
	virtual void Free() override;
};
END