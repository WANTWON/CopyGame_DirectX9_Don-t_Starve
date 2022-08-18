#include "stdafx.h"
#include "..\Public\CameraDynamic.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
CCameraDynamic::CCameraDynamic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCameraDynamic::CCameraDynamic(const CCameraDynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCameraDynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraDynamic::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;

	return S_OK;
}

int CCameraDynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (CKeyMgr::Get_Instance()->Key_Down(VK_F1))
	{
		if (m_eCamMode == CAM_DEFAULT)
			m_eCamMode = CAM_PLAYER;
		else
			m_eCamMode = CAM_DEFAULT;
	}


	if (m_eCamMode == CAM_DEFAULT)
	{
		Default_Camera(fTimeDelta);
	}
	else
	{
		Player_Camera(fTimeDelta);
	}
		

	if (FAILED(Bind_OnGraphicDev()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CCameraDynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCameraDynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

void CCameraDynamic::Default_Camera(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_lMouseWheel > 0)
		m_lMouseWheel -= 0.001;
	if (m_lMouseWheel < 0)
		m_lMouseWheel += 0.001;


	if (m_lMouseWheel += pGameInstance->Get_DIMMoveState(DIMM_WHEEL)*0.05)
		m_pTransform->Go_Straight(fTimeDelta*m_lMouseWheel*0.01f);

	if (GetKeyState('Q') < 0)
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);

	if (GetKeyState('E') < 0)
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), -fTimeDelta);

	if (GetKeyState('W') < 0)
		m_pTransform->Go_Straight(fTimeDelta);

	if (GetKeyState('S') < 0)
		m_pTransform->Go_Backward(fTimeDelta);

	if (GetKeyState('A') < 0)
		m_pTransform->Go_Left(fTimeDelta);

	if (GetKeyState('D') < 0)
		m_pTransform->Go_Right(fTimeDelta);

	Safe_Release(pGameInstance);
}

void CCameraDynamic::Player_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_lMouseWheel > 0)
		m_lMouseWheel -= 0.001;
	if (m_lMouseWheel < 0)
		m_lMouseWheel += 0.001;


	if (m_lMouseWheel += pGameInstance->Get_DIMMoveState(DIMM_WHEEL)*0.05)
	{
		m_vDistance.y -= fTimeDelta*m_lMouseWheel*0.01f;
		m_vDistance.z += fTimeDelta*m_lMouseWheel*0.01f;

	}


	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	Safe_AddRef(pTarget);

	_float3 m_TargetPos = pTarget->Get_Pos();

	Safe_Release(pTarget);

	m_pTransform->LookAt(m_TargetPos);
	m_pTransform->Follow_Target(m_TargetPos,  m_vDistance);

	Safe_Release(pGameInstance);
}

CCameraDynamic * CCameraDynamic::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCameraDynamic*	pInstance = new CCameraDynamic(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCameraDynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCamera * CCameraDynamic::Clone(void * pArg)
{
	CCameraDynamic*	pInstance = new CCameraDynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCameraDynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraDynamic::Free()
{
	__super::Free();
}
