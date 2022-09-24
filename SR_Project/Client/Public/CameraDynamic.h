#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCameraDynamic final : public CCamera
{
public:
	enum CAMERAMODE {CAM_PLAYER, CAM_TURNMODE, CAM_REVIVE, CAM_ZOOMIN, CAM_ZOOMOUT, CAM_SHAKING };
	//Default ÇÊ¿äx , 
	typedef struct tagCameraDesc_Derived
	{
		_float3						vDistance = _float3(0, 6, -10);
		CCamera::CAMERADESC			CameraDesc;
		_float3						vOffset = _float3(0, 0, 0);

	}CAMERADESC_DERIVED;
private:
	CCameraDynamic(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCameraDynamic(const CCameraDynamic& rhs);
	virtual ~CCameraDynamic() = default;
	

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public: //SetEnum
	_int Set_CamMode(CAMERAMODE _eCamMode, _int _TurnCount = 0) 
	{
		m_eCamMode = _eCamMode; return Switch_TurnCnt(_TurnCount);
	}
	void Set_CamMode(CAMERAMODE _eCamMode, _float fPower, _float fVelocity, _float fMinusVelocity)
	{
		m_eCamMode = _eCamMode; m_fPower = fPower; m_fVelocity = fVelocity; m_fMinusVelocity = fMinusVelocity;
	}
	CAMERAMODE Get_CamMode() { return m_eCamMode; }
	void Set_TalkingMode(_bool type) {if (type) m_eCamMode = CAM_ZOOMIN; else m_eCamMode = CAM_ZOOMOUT;}
	void Set_Target(class CGameObject* pGameObject) { m_pTarget = pGameObject; }


private:
	void Player_Camera(_float fTimeDelta);
	void Turn_Camera(_float fTimeDelta);
	_int Switch_TurnCnt(_int _TurnCount);
	void Revive_Camera(_float fTimeDelta);
	void ZoomIn_Camera(_float fTimeDelta, CGameObject* pGameObject);
	void ZoomOut_Camera(_float fTimeDelta);
	void Shaking_Camera(_float fTimeDelta, _float fPower);

private:

	_float3			m_vDistance;
	_double			m_lMouseWheel = 0;
	CAMERAMODE		m_eCamMode = CAM_PLAYER;
	_int			m_iTurnCount = 0;
	class CGameObject*	m_pTarget = nullptr;
	
	
private:
	_float			m_fPower = 0.5f;
	_float			m_fVelocity = 0.1f;
	_float			m_fMinusVelocity = 0.01f;
	_int			m_iShakingCount = 0;


public:
	static CCameraDynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual void Free() override;

};

END