#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CInteractive_Object : public CGameObject
{
protected:
	CInteractive_Object(LPDIRECT3DDEVICE9 pGraphic_Device);
	CInteractive_Object(const CInteractive_Object& rhs);
	virtual ~CInteractive_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	_bool	Get_Dead(void) { return m_bDead; }

public:
	virtual void Interact(_uint Damage = 0);
	virtual HRESULT Drop_Items(void);
	INTERACTOBJ_ID	Get_InteractName(void) { return m_eInteract_OBJ_ID; }
	_uint		Set_Damage(_uint _iDamage) { m_iDamage = 0; }
	_bool		Get_CanInteract(void) { return m_bInteract; }

protected:
	INTERACTOBJ_ID		m_eInteract_OBJ_ID;
	_uint			m_iDamage =0;
	_bool			m_bInteract = true;
public:
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END