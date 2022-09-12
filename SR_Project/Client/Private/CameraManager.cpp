#include "stdafx.h"
#include "..\Public\CameraManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCameraManager);

CCameraManager::CCameraManager()
{
}


void CCameraManager::Set_CamState(CAM_STATE _eState)
{
	m_eCamState = _eState;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CCamera* pCamera = dynamic_cast<CCamera*>(pGameInstance->Get_Object(m_eCurrentLevel, TEXT("Layer_Camera"), _eState));
	if (nullptr == pCamera)
		return;

	m_pCurrentCamera = pCamera;
}

void CCameraManager::PlayerCamera_TurnLeft(LEVEL LevelIndex)
{
	if (m_eCamState == CAM_PLAYER )
	{
		CCameraDynamic* pDynamicCamera = dynamic_cast<CCameraDynamic*>(m_pCurrentCamera);
		if (pDynamicCamera)
		{
			m_eCamDir = (CAM_DIR)pDynamicCamera->Set_CamMode(CCameraDynamic::CAM_TURNMODE, 1);
		}	
	}
}

void CCameraManager::PlayerCamera_TurnRight(LEVEL LevelIndex)
{
	if (m_eCamState == CAM_PLAYER)
	{
		CCameraDynamic* pDynamicCamera = dynamic_cast<CCameraDynamic*>(m_pCurrentCamera);
		if (pDynamicCamera)
		{
			m_eCamDir = (CAM_DIR)pDynamicCamera->Set_CamMode(CCameraDynamic::CAM_TURNMODE, 2);
		}
	}
}

HRESULT CCameraManager::Ready_Camera(LEVEL eLevelIndex)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CCamera* pCamera = dynamic_cast<CCamera*>(pGameInstance->Get_Object(eLevelIndex, TEXT("Layer_Camera")));

	if (nullptr == pCamera)
		return E_FAIL;

	m_pCurrentCamera = pCamera;
	m_eCamState = CAM_PLAYER;
	m_eCamDir = DIR_DEFAULT;
	m_eCurrentLevel = eLevelIndex;
	return S_OK;
}

void CCameraManager::Free()
{
}
