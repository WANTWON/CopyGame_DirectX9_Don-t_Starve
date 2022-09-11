#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCameraDynamic final : public CCamera
{
public:
	enum CAMERAMODE {CAM_PLAYER, CAM_TURNMODE};
	//Default ÇÊ¿äx , 
	typedef struct tagCameraDesc_Derived
	{
		_uint						iTest;
		CCamera::CAMERADESC			CameraDesc;
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
	void Set_CamMode(CAMERAMODE _eCamMode, _int _TurnCount = 0) 
	{
		m_eCamMode = _eCamMode; Switch_TurnCnt(_TurnCount);
	}
	CAMERAMODE Get_CamMode() { return m_eCamMode; }
	void Set_TalkingMode(_bool type) { m_bTalkingMode = type; m_bOutZoom = !type; }
	void Set_Target(class CGameObject* pGameObject) { m_pTarget = pGameObject; }

private:
	void Player_Camera(_float fTimeDelta);
	void Turn_Camera(_float fTimeDelta);
	void Switch_TurnCnt(_int _TurnCount);
	void Target_Camera(_float fTimeDelta, CGameObject* pGameObject);
	void OutTarget_Camera(_float fTimeDelta);
private:
	_long			m_lMouseWheel = 0;
	_float3			m_vDistance = _float3(0, 8, -6);
	CAMERAMODE		m_eCamMode = CAM_PLAYER;
	_int			m_iTurnCount = 0;
	class CGameObject*	m_pTarget = nullptr;
	_bool			m_bTalkingMode = false;
	_bool			m_bOutZoom = false;


public:
	static CCameraDynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual void Free() override;

};

END