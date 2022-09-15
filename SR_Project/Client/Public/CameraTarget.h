#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCameraTarget final : public CCamera
{
public:
	enum CAMERAMODE { CAM_ZOOM, CAM_FOLLOW, CAM_ZOOMOUT, CAM_IDLE };

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
	void Set_Target(class CGameObject* pGameObject) { m_pTarget = pGameObject; }

private:
	void Idle_Camera(_float fTimeDelta);
	void Target_Camera(_float fTimeDelta, CGameObject* pGameObject);
	void OutTarget_Camera(_float fTimeDelta);
	void Target_Follow(_float fTimeDelta, CGameObject* pGameObject);

private:
	CAMERAMODE		m_eCamMode = CAM_IDLE;
	_long			m_lMouseWheel = 0;
	_float3			m_vDistance;
	_float3			m_vInitDistance;
	class CGameObject*	m_pTarget = nullptr;
	_bool			m_bTalkingMode = false;
	_bool			m_bOutZoom = false;


public:
	static CCameraTarget* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual void Free() override;

};

END