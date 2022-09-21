#include "stdafx.h"
#include "..\Public\CameraTarget.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Transform.h"
#include "CameraManager.h"

CCameraTarget::CCameraTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCameraTarget::CCameraTarget(const CCameraTarget & rhs)
	: CCamera(rhs)
{
}

HRESULT CCameraTarget::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraTarget::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;

	m_vDistance = ((CAMERADESC_DERIVED*)pArg)->vDistance;
	m_vInitDistance = m_vDistance;
	return S_OK;
}

int CCameraTarget::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);

	switch (m_eCamMode)
	{
	case Client::CCameraTarget::CAM_ZOOM:
		Target_Camera(fTimeDelta, m_pTarget);
		break;
	case Client::CCameraTarget::CAM_FOLLOW:
		Target_Follow(fTimeDelta, m_pTarget);
		break;
	case Client::CCameraTarget::CAM_ZOOMOUT:
		OutTarget_Camera(fTimeDelta);
		break;
	case Client::CCameraTarget::CAM_GO:
		Going_Target(fTimeDelta, m_pTarget);
		break;
	case Client::CCameraTarget::CAM_RETURN:
		Return_Camera(fTimeDelta);
		break;
	case Client::CCameraTarget::CAM_IDLE:
		Idle_Camera(fTimeDelta);
		break;
	case Client::CCameraTarget::CAM_POSITION:
		Position_Target(fTimeDelta, m_vPosition);
		break;
	default:
		break;
	}

	Update_Position(m_pTransform->Get_State(CTransform::STATE_POSITION));

	if (CCameraManager::Get_Instance()->Get_CamState() != CCameraManager::CAM_TARGET)
		return OBJ_NOEVENT;

	if (FAILED(Bind_OnGraphicDev()))
		return OBJ_NOEVENT;

	if (m_bChange)
	{
		m_eCamMode = CAM_IDLE;
		m_vDistance = m_vInitDistance;
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
		m_bChange = false;
	}


	return OBJ_NOEVENT;
}

void CCameraTarget::Late_Tick(_float fTimeDelta)
{
	if (CCameraManager::Get_Instance()->Get_CamState() != CCameraManager::CAM_PLAYER)
		return;

	__super::Late_Tick(fTimeDelta);
}


void CCameraTarget::Idle_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	if (m_lMouseWheel > 0)
		m_lMouseWheel -= 0.001;
	if (m_lMouseWheel < 0)
		m_lMouseWheel += 0.001;

	if (m_lMouseWheel += (pGameInstance->Get_DIMMoveState(DIMM_WHEEL)*0.05))
	{
		if (m_vDistance.y > 15 || m_vDistance.y < 3)
			return;

		m_vDistance.y -= (fTimeDelta*m_lMouseWheel*0.01f);
		m_vDistance.z += (fTimeDelta*m_lMouseWheel*0.01f);
	}

	if (pGameInstance->Key_Pressing(VK_DOWN))
	{
		m_vDistance.y -= 0.03f;
		m_vDistance.z -= 0.06f;
	}
	if (pGameInstance->Key_Pressing(VK_UP))
	{
		m_vDistance.y += 0.03f;
		m_vDistance.z += 0.06f;
	}

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	_float3 m_TargetPos = pTarget->Get_Pos();
	m_pTransform->LookAt(m_TargetPos);
	m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, _float3(m_vDistance.x, m_vDistance.y, m_vDistance.z));
}

void CCameraTarget::Target_Camera(_float fTimeDelta, CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return;

	_float3 m_TargetPos = pGameObject->Get_Position();
	m_TargetPos.y -= m_CameraDesc.vAt.y;
	_float3 vCameraPos = Get_Position();

	_float3 vDistance = m_CameraDesc.vEye;
	_float3 vDir = (m_TargetPos+vDistance) - vCameraPos;

	if (fabsf(vDir.y) < 0.5f && fabsf(vDir.z) < 0.5f)
	{
		m_pTransform->LookAt(m_TargetPos);
		m_vDistance = vDistance;
		m_eCamMode = CAM_FOLLOW;
	}
	else
	{
		D3DXVec3Normalize(&vDir, &vDir);
		vCameraPos += vDir*0.2f;
		m_pTransform->LookAt(m_TargetPos);
	}
	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);
}

void CCameraTarget::OutTarget_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	Safe_AddRef(pTarget);

	_float3 m_TargetPos = pTarget->Get_Pos();
	Safe_Release(pTarget);

	_float3 vCameraPos = Get_Position();
	_float3 vEndPosition = (m_TargetPos + m_vInitDistance);
	_float3 vDir = vEndPosition - vCameraPos;

	if (fabsf(vEndPosition.y - vCameraPos.y) < 0.5f && fabsf(vEndPosition.z - vCameraPos.z) < 0.5f)
	{
		m_bChange = true;
		Safe_Release(pGameInstance);
		return;
	}


	D3DXVec3Normalize(&vDir, &vDir);
	vCameraPos += vDir*0.2f;

	m_pTransform->LookAt(m_TargetPos);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Safe_Release(pGameInstance);
}

void CCameraTarget::Target_Follow(_float fTimeDelta, CGameObject * pGameObject)
{
	if (pGameObject == nullptr)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

		_float3 TargetPos = m_TargetPos;
		TargetPos.y -= m_CameraDesc.vAt.y;
		m_pTransform->LookAt(TargetPos);
		m_pTransform->Follow_Target(fTimeDelta, TargetPos, _float3(m_vDistance.x, m_vDistance.y, m_vDistance.z));
		return;
	}
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	_float3 TargetPos = pGameObject->Get_Position();
	TargetPos.y -= m_CameraDesc.vAt.y;
	m_pTransform->LookAt(TargetPos);
	m_pTransform->Follow_Target(fTimeDelta, TargetPos, _float3(m_vDistance.x, m_vDistance.y, m_vDistance.z));
}

void CCameraTarget::Going_Target(_float fTimeDelta, CGameObject * pGameObject)
{
	if (pGameObject == nullptr)
	{
		_float3 TargetPos = m_TargetPos;
		TargetPos += m_vDistance;
		_float3 vCameraPos = Get_Position();
		_float3 vDir = m_TargetPos - vCameraPos;

		if (fabsf(vDir.x) < 0.3f && fabsf(vDir.z) < 0.3f && fabsf(vDir.x) < 0.3f)
		{
			m_eCamMode = CAM_FOLLOW;
		}
		else
		{
			D3DXVec3Normalize(&vDir, &vDir);
			vCameraPos += vDir*0.2f;
		}
		m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);
		return;
	}
		

	_float3 TargetPos = pGameObject->Get_Position();
	TargetPos += m_vDistance;
	m_TargetPos = TargetPos;
	_float3 vCameraPos = Get_Position();
	_float3 vDir = TargetPos  - vCameraPos;

	if (fabsf(vDir.x) < 0.3f && fabsf(vDir.z) < 0.3f && fabsf(vDir.x) < 0.3f)
	{
		m_eCamMode = CAM_FOLLOW;
	}
	else
	{
		D3DXVec3Normalize(&vDir, &vDir);
		vCameraPos += vDir*0.2f;
	}
	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);
}

void CCameraTarget::Position_Target(_float fTimeDelta, _float3 pPosition)
{

	_float3 m_TargetPos = pPosition;
	m_TargetPos.y -= m_CameraDesc.vAt.y;
	_float3 vCameraPos = Get_Position();

	_float3 vDir = (m_TargetPos + m_vOffSetDistance) - vCameraPos;

	if (fabsf(vDir.y) < 0.1f && fabsf(vDir.z) < 0.1f)
	{
		m_pTransform->LookAt(m_TargetPos);
		m_vDistance = m_vOffSetDistance;
		m_eCamMode = CAM_FOLLOW;
	}
	else
	{
		D3DXVec3Normalize(&vDir, &vDir);
		vCameraPos += vDir*0.2f;
		m_pTransform->LookAt(m_TargetPos);
	}
	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);
}


void CCameraTarget::Return_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	Safe_AddRef(pTarget);

	_float3 m_TargetPos = pTarget->Get_Pos();
	Safe_Release(pTarget);

	_float3 vCameraPos = Get_Position();
	_float3 vEndPosition = (m_TargetPos + m_vInitDistance);
	_float3 vDir = vEndPosition - vCameraPos;

	if (fabsf(vEndPosition.y - vCameraPos.y) < 0.5f && fabsf(vEndPosition.z - vCameraPos.z) < 0.5f && fabsf(vEndPosition.x - vCameraPos.x) < 0.5f)
	{
		m_bChange = true;
		Safe_Release(pGameInstance);
		return;
	}

	D3DXVec3Normalize(&vDir, &vDir);
	vCameraPos += vDir*0.2f;
	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);

	Safe_Release(pGameInstance);
}

CCameraTarget * CCameraTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCameraTarget*	pInstance = new CCameraTarget(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCameraTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCamera * CCameraTarget::Clone(void * pArg)
{
	CCameraTarget*	pInstance = new CCameraTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCameraTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraTarget::Free()
{
	__super::Free();
}
