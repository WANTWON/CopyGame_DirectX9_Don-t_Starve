#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCameraFPS final : public CCamera
{
	//Default ÇÊ¿äx , 
	typedef struct tagCameraDesc_Derived
	{
		_uint						iTest;
		CCamera::CAMERADESC			CameraDesc;
	}CAMERADESC_DERIVED;
private:
	CCameraFPS(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCameraFPS(const CCameraFPS& rhs);
	virtual ~CCameraFPS() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

private:
	void FPS_Camera(_float fTimeDelta);
	


private:
	_long			m_lMouseWheel = 0;
	_float3			m_vDistance = _float3(0, 8, -5);

public:
	static CCameraFPS* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual void Free() override;

};

END