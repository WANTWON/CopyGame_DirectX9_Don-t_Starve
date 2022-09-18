#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCameraFPS final : public CCamera
{
	//Default ÇÊ¿äx , 
	typedef struct tagCameraDesc_Derived
	{
		_float3						vDistance = _float3(0, 6, -10);
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
	void Set_Dir(DIR_STATE Dir) { m_StartDir = Dir; }
	


private:
	_long			m_lMouseWheel = 0;
	_float3			m_vDistance;
	DIR_STATE		m_StartDir = DIR_END;

public:
	static CCameraFPS* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual void Free() override;

};

END