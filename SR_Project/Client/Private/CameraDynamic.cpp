#include "stdafx.h"
#include "..\Public\CameraDynamic.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Transform.h"

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
		else if (m_eCamMode == CAM_PLAYER)
			m_eCamMode = CAM_FPS;
		else if (m_eCamMode == CAM_FPS)
			m_eCamMode = CAM_DEFAULT;
	}

	if (m_eCamMode == CAM_DEFAULT)
	{
		Default_Camera(fTimeDelta);
	}
	else if (m_eCamMode == CAM_PLAYER)
	{
		Player_Camera(fTimeDelta);
	}
	else if (m_eCamMode == CAM_TURNMODE)
		Turn_Camera(fTimeDelta);
	else if (m_eCamMode == CAM_FPS)
		FPS_Camera(fTimeDelta);


	Update_Position(m_pTransform->Get_State(CTransform::STATE_POSITION));


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


	if (m_lMouseWheel += (_long)(pGameInstance->Get_DIMMoveState(DIMM_WHEEL)*0.05))
		Add_Fov(fTimeDelta*m_lMouseWheel*0.01f);

	if (GetKeyState('Q') < 0)
		m_pTransform->Turn(_float3(0, 1 ,0), fTimeDelta);

	if (GetKeyState('E') < 0)
		m_pTransform->Turn(_float3(0, 1, 0), -fTimeDelta);

	if (GetKeyState(VK_UP) < 0)
		m_pTransform->Go_Straight(fTimeDelta);

	if (GetKeyState(VK_DOWN) < 0)
		m_pTransform->Go_Backward(fTimeDelta);

	if (GetKeyState(VK_LEFT) < 0)
		m_pTransform->Go_Left(fTimeDelta);

	if (GetKeyState(VK_RIGHT) < 0)
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


	if (m_lMouseWheel += (_long)(pGameInstance->Get_DIMMoveState(DIMM_WHEEL)*0.05))
	{
		Add_Fov(fTimeDelta*m_lMouseWheel*0.01f);
	}

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	Safe_AddRef(pTarget);

	_float3 m_TargetPos = pTarget->Get_Pos();

	Safe_Release(pTarget);

	m_pTransform->LookAt(m_TargetPos);
	

	if (m_eCamMode == CAM_PLAYER && pGameInstance->Key_Up('Q'))
	{
		m_iTurnCount++;
		m_eCamMode = CAM_TURNMODE;
		Safe_Release(pGameInstance);
		return;
	}

	

	switch (m_iTurnCount % 4)
	{
	case 0:
		m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, _float3(m_vDistance.x, m_vDistance.y, m_vDistance.z));
		break;
	case 1:
		m_pTransform->Follow_Target(fTimeDelta,m_TargetPos, _float3(m_vDistance.z, m_vDistance.y, m_vDistance.x));
		break;
	case 2:
		m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, _float3(m_vDistance.x, m_vDistance.y, -m_vDistance.z));
		break;
	case 3:
		m_pTransform->Follow_Target(fTimeDelta,m_TargetPos, _float3(-m_vDistance.z, m_vDistance.y, m_vDistance.x));
		break;
	}


	Safe_Release(pGameInstance);
}

void CCameraDynamic::Turn_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	Safe_AddRef(pTarget);

	_float3 m_TargetPos = pTarget->Get_Pos();
	Safe_Release(pTarget);

	_float3 NewTransPos(0, 0, 0);
	switch (m_iTurnCount % 4)
	{
	case 0:
		NewTransPos = _float3(m_vDistance.x, m_vDistance.y, m_vDistance.z);
		break;
	case 1:
		NewTransPos = _float3(m_vDistance.z, m_vDistance.y, m_vDistance.x);
		break;
	case 2:
		NewTransPos = _float3(m_vDistance.x, m_vDistance.y, -m_vDistance.z);
		break;
	case 3:
		NewTransPos = _float3(-m_vDistance.z, m_vDistance.y, m_vDistance.x);
		break;
	}

	m_pTransform->LookAt(m_TargetPos);
	m_pTransform->Go_PosTarget(fTimeDelta, m_TargetPos, NewTransPos);
	NewTransPos += m_TargetPos;

	_float3 vDistance = NewTransPos - m_pTransform->Get_State(CTransform::STATE_POSITION);

	if (fabsf(vDistance.x) < 0.5f && fabsf(vDistance.y) < 0.5f && fabsf(vDistance.z) < 0.5f)
		m_eCamMode = CAM_PLAYER;

	Safe_Release(pGameInstance);
}

void CCameraDynamic::FPS_Camera(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	Safe_AddRef(pTarget);

	_float3 vecTargetLook = pTarget->Get_Look();
	D3DXVec3Normalize(&vecTargetLook, &vecTargetLook);
	_float3 vecargetPos = pTarget->Get_Pos() + vecTargetLook;
	_float3 PlayerLook = pTarget->Get_Look();

	m_pTransform->Follow_Target(fTimeDelta, vecargetPos, _float3(0, 0, 0));

	m_pTransform->LookAt(PlayerLook);

	Safe_Release(pTarget);
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

CGameObject * CCameraDynamic::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CCameraDynamic::Free()
{
	__super::Free();
}
