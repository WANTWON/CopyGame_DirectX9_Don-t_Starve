#include "stdafx.h"
#include "..\Public\Interactive_Object.h"

CInteractive_Object::CInteractive_Object(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CInteractive_Object::CInteractive_Object(const CInteractive_Object & rhs)
	:CGameObject(rhs)
{
}

HRESULT CInteractive_Object::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractive_Object::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

int CInteractive_Object::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

void CInteractive_Object::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CInteractive_Object::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CInteractive_Object::Interact(_uint Damage)
{
}

HRESULT CInteractive_Object::Drop_Items(void)
{
	return S_OK;
}

CGameObject * CInteractive_Object::Clone(void * pArg)
{
	return nullptr;
}

void CInteractive_Object::Free()
{
	__super::Free();
}
