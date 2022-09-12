#pragma once
#include "CameraDynamic.h"
#include "CameraFPS.h"
#include "CameraTarget.h"

BEGIN(Client)

class CCameraManager final : public CBase
{
	DECLARE_SINGLETON(CCameraManager)
public:

	enum CAM_STATE { CAM_PLAYER, CAM_FPS, CAM_TARGET, CAM_END };
	enum CAM_DIR { DIR_DEFAULT, DIR_1, DIR_2, DIR_3, DIR_MAX };

public:
	CCameraManager();
	virtual ~CCameraManager() = default;

public: /* Get*/ 
	CAM_STATE Get_CamState() { return m_eCamState; }
	CAM_DIR Get_CamDir() { return m_eCamDir; }
	CCamera* Get_CurrentCamera() { return m_pCurrentCamera; }

public:/*Set*/
	void Set_CamState(CAM_STATE _eState);
	void Set_CurrentCamera(CCamera* _pCamera) { m_pCurrentCamera = _pCamera; }

public: /*For Player Cam*/
	void PlayerCamera_TurnLeft(LEVEL LevelIndex);
	void PlayerCamera_TurnRight(LEVEL LevelIndex);
	
public:
	HRESULT Ready_Camera(LEVEL eLevelIndex);
private:
	CCamera*					m_pCurrentCamera = nullptr;
	CAM_STATE					m_eCamState = CAM_END;
	CAM_DIR						m_eCamDir = DIR_MAX;
	LEVEL						m_eCurrentLevel;
public:
	virtual void Free() override;
};

END