#include "stdafx.h"
#include "..\Public\Pawn.h"
#include "Level_Manager.h"
#include "GameInstance.h"

CPawn::CPawn(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CPawn::CPawn(const CPawn & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPawn::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPawn::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

int CPawn::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

void CPawn::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPawn::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPawn::Set_ShaderID()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	if (pGameObject->Get_Dead())
		m_eShaderID = SHADER_DEAD;
	else if (m_bPickingTrue)
		m_eShaderID = SHADER_PICKING;
	else if (iLevel == LEVEL_MAZE)
		m_eShaderID = SHADER_DARK;
	else
		m_eShaderID = SHADER_DINNER;
}

void CPawn::Free()
{
	__super::Free();
}
