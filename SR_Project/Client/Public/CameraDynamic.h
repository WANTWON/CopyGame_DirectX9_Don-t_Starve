#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCameraDynamic final : public CCamera
{
public:
	enum CAMERAMODE {CAM_DEFAULT, CAM_PLAYER, CAM_TURNMODE, CAM_FPS, CAM_END};

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
	virtual HRESULT Render();

private:
	void Default_Camera(_float fTimeDelta);
	void Player_Camera(_float fTimeDelta);
	void Turn_Camera(_float fTimeDelta);
	void FPS_Camera(_float fTimeDelta);

private:
	_long			m_lMouseWheel = 0;
	_float3			m_vDistance = _float3(0, 3, -3);
	CAMERAMODE		m_eCamMode = CAM_DEFAULT;
	_int			m_iTurnCount = 0;

public:
	static CCameraDynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
	virtual void Free() override;

};

END