#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCameraTarget final : public CCamera
{
public:
	enum CAMERAMODE { CAM_ZOOM, CAM_FOLLOW, CAM_ZOOMOUT, CAM_GO, CAM_POSITION, CAM_RETURN, CAM_IDLE };

	typedef struct tagCameraDesc_Derived
	{
		_float3						vDistance = _float3(0, 6, -10);
		CCamera::CAMERADESC			CameraDesc;
	}CAMERADESC_DERIVED;
private:
	CCameraTarget(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCameraTarget(const CCameraTarget& rhs);
	virtual ~CCameraTarget() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public: //SetEnum
	void Set_TalkingMode(_bool type) { if(type)m_eCamMode = CAM_ZOOM; else m_eCamMode = CAM_ZOOMOUT;}
	void Set_GoingMode(_bool type) { if (type)m_eCamMode = CAM_GO; else m_eCamMode = CAM_RETURN; }
	void Set_PositionMode(_bool type) { if (type)m_eCamMode = CAM_POSITION; else m_eCamMode = CAM_RETURN; }
	void Set_Target(class CGameObject* pGameObject) { m_pTarget = pGameObject; }
	void Set_Position(_float3 pPosition) { m_vPosition = pPosition; }
	void Set_OffSetDistance(_float3 pPosition) { m_vOffSetDistance = pPosition; }

private:
	void Idle_Camera(_float fTimeDelta);
	void Target_Camera(_float fTimeDelta, CGameObject* pGameObject);
	void OutTarget_Camera(_float fTimeDelta);
	void Target_Follow(_float fTimeDelta, CGameObject* pGameObject);
	void Going_Target(_float fTimeDelta, CGameObject* pGameObject);
	void Position_Target(_float fTimeDelta, _float3 pPosition);
	void Return_Camera(_float fTimeDelta);

private:
	CAMERAMODE		m_eCamMode = CAM_IDLE;
	_long			m_lMouseWheel = 0;
	_float3			m_vDistance;
	_float3			m_vInitDistance;
	

private:
	class CGameObject*	m_pTarget = nullptr;
	_float3			m_TargetPos;
	_bool			m_bTalkingMode = false;
	_bool			m_bOutZoom = false;
	_bool			m_bChange = false;
	_float3			m_vPosition;

	_float3			m_vOffSetDistance;


public:
	static CCameraTarget* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual void Free() override;

};

END