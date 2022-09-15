#include "stdafx.h"
#include "..\Public\CameraFPS.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Transform.h"
#include "CameraManager.h"

CCameraFPS::CCameraFPS(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCameraFPS::CCameraFPS(const CCameraFPS & rhs)
	: CCamera(rhs)
{
}

HRESULT CCameraFPS::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraFPS::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;


	m_vDistance = ((CAMERADESC_DERIVED*)pArg)->vDistance;

	return S_OK;
}

int CCameraFPS::Tick(_float fTimeDelta)
{
	if (CCameraManager::Get_Instance()->Get_CamState() != CCameraManager::CAM_FPS)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);

	FPS_Camera(fTimeDelta);

	if (FAILED(Bind_OnGraphicDev()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CCameraFPS::Late_Tick(_float fTimeDelta)
{

	if (CCameraManager::Get_Instance()->Get_CamState() != CCameraManager::CAM_FPS)
		return;

	__super::Late_Tick(fTimeDelta);
}


void CCameraFPS::FPS_Camera(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	Safe_AddRef(pTarget);

	_float3 vecTargetLook = pTarget->Get_Look();
	D3DXVec3Normalize(&vecTargetLook, &vecTargetLook);
	_float3 vectargetPos = pTarget->Get_Pos() + _float3(0.f, 0.5f, vecTargetLook.z);
	_float3 PlayerLook = (pTarget->Get_Look());
	_float3 PlayerRight = (pTarget->Get_Right());



	m_pTransform->Follow_Target(fTimeDelta, vectargetPos, _float3(0, 0, 0));

	m_pTransform->Set_State(CTransform::STATE_RIGHT, PlayerRight);
	m_pTransform->Set_State(CTransform::STATE_LOOK, _float3(PlayerLook.x, 0.f, PlayerLook.z));

	Safe_Release(pTarget);
	Safe_Release(pGameInstance);
}




CCameraFPS * CCameraFPS::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCameraFPS*	pInstance = new CCameraFPS(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCameraFPS"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCamera * CCameraFPS::Clone(void * pArg)
{
	CCameraFPS*	pInstance = new CCameraFPS(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCameraFPS"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCameraFPS::Free()
{
	__super::Free();
}
