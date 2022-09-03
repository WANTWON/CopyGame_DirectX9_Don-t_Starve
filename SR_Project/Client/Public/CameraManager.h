#pragma once
#include "CameraDynamic.h"

BEGIN(Client)

class CCameraManager final : public CBase
{
	DECLARE_SINGLETON(CCameraManager)
public:
	enum CAM_STATE { CAM_PLAYER, CAM_FPS, CAM_END };
public:
	CCameraManager();
	virtual ~CCameraManager() = default;

public: /* Get*/ 
	CAM_STATE Get_CamState() { return m_eCamState; }
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
	LEVEL						m_eCurrentLevel;
public:
	virtual void Free() override;
};

END