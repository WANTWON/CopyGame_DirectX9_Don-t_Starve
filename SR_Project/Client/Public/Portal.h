#pragma once

#include "Client_Defines.h"
#include "Interactive_Object.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
//class CCollider;
END

BEGIN(Client)
class CPortal final : public CInteractive_Object
{
public: 

	enum  PORTALTYPE { PORTAL_NORMAL, PORTAL_BOSS, PORTAL_END };

	typedef struct PortalDesc
	{
		PORTALTYPE m_eType = PORTAL_NORMAL;
		_float3 vPosition;
	}PORTALDESC;
	


	enum STATE
	{
		IDLE_CLOSE,
		OPENING,
		IDLE_OPEN,
		CLOSING,
		MAX
	};

private:
	CPortal(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPortal(const CPortal& rhs);
	virtual ~CPortal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) override;
	void SetUp_BillBoard();

private: /*For TextureCom */
	virtual HRESULT Texture_Clone() override;
	virtual void Change_Frame() override;
	virtual void Change_Motion() override;

private:
	PORTALDESC m_ePortalDesc;
	STATE m_eState;
	STATE m_ePreState = MAX;
	_float m_fOpenRadius = 0.f;
	_bool m_bShouldClosePortal = false;
	_bool m_bShouldTeleport = false;

public:
	void AI_Behaviour();
	_bool Check_Target();
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

public:
	static CPortal* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END