#include "stdafx.h"
#include "..\Public\CameraDynamic.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Transform.h"
#include "CameraManager.h"

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

	if (CCameraManager::Get_Instance()->Get_CamState() != CCameraManager::CAM_PLAYER)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);

	if (m_bTalkingMode && !m_bOutZoom)
	{
		Target_Camera(fTimeDelta, m_pTarget);
	}
	else if (!m_bTalkingMode && m_bOutZoom)
	{
		OutTarget_Camera(fTimeDelta);
	}
	else if (m_eCamMode == CAM_PLAYER)
	{
		Player_Camera(fTimeDelta);
	}
	else if (m_eCamMode == CAM_TURNMODE)
	{
		Turn_Camera(fTimeDelta);
	}

	Update_Position(m_pTransform->Get_State(CTransform::STATE_POSITION));

	if (FAILED(Bind_OnGraphicDev()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CCameraDynamic::Late_Tick(_float fTimeDelta)
{
	if (CCameraManager::Get_Instance()->Get_CamState() != CCameraManager::CAM_PLAYER)
		return;

	__super::Late_Tick(fTimeDelta);
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
		m_vDistance.y -= (fTimeDelta*m_lMouseWheel*0.01f);
		m_vDistance.z += (fTimeDelta*m_lMouseWheel*0.01f);

		//m_CameraDesc.fFovy += (fTimeDelta*m_lMouseWheel*0.01f);
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

	Safe_AddRef(pTarget);

	_float3 m_TargetPos = pTarget->Get_Pos();

	Safe_Release(pTarget);

	m_pTransform->LookAt(m_TargetPos);

	switch (m_iTurnCount % 4)
	{
	case 0:
		m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, _float3(m_vDistance.x, m_vDistance.y, m_vDistance.z));
		break;
	case 1:
		m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, _float3(m_vDistance.z, m_vDistance.y, m_vDistance.x));
		break;
	case 2:
		m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, _float3(m_vDistance.x, m_vDistance.y, -m_vDistance.z));
		break;
	case 3:
		m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, _float3(-m_vDistance.z, m_vDistance.y, m_vDistance.x));
		break;
	}


	Safe_Release(pGameInstance);
}

void CCameraDynamic::Turn_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
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


void CCameraDynamic::Switch_TurnCnt(_int _TurnCount)
{
	switch (_TurnCount)
	{
	case 0:
		break;
	case 1:
		++m_iTurnCount;
		break;
	case 2:
		--m_iTurnCount;
		break;
	}

	if (m_iTurnCount >= 4)
	{
		m_iTurnCount = 0;
	}
	else if (m_iTurnCount < 0)
	{
		m_iTurnCount = 3;
	}
}

void CCameraDynamic::Target_Camera(_float fTimeDelta, CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return;

	_float3 m_TargetPos = pGameObject->Get_Position();
	m_TargetPos.y -= 0.5f;
	_float3 vCameraPos = Get_Position();
	
	_float3 vDir = m_TargetPos - vCameraPos;
	vDir -= _float3(0.f, 3.5f, 5.f);

	_float3 vDistance = _float3(0, 5, -5);
	if (fabsf(vDir.y) < vDistance.y)
		return;

	D3DXVec3Normalize(&vDir, &vDir);
	vCameraPos += vDir*0.1f;

	m_pTransform->LookAt(m_TargetPos);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);
}

void CCameraDynamic::OutTarget_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	Safe_AddRef(pTarget);

	_float3 m_TargetPos = pTarget->Get_Pos();
	Safe_Release(pTarget);

	_float3 vCameraPos = Get_Position();
	_float3 vDir = (m_TargetPos+m_vDistance)- vCameraPos;

	if (fabsf(m_TargetPos.y + m_vDistance.y - vCameraPos.y) < 0.5f)
	{
		m_bOutZoom = false;
		return;
	}
		

	D3DXVec3Normalize(&vDir, &vDir);
	vCameraPos += vDir*0.1f;

	m_pTransform->LookAt(m_TargetPos);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vCameraPos);

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
