#pragma once
#include "Camera.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CCameraDynamic final : public CCamera
{
public:
	enum CAMERAMODE {CAM_DEFAULT, CAM_END};

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

private:
	_long			m_lMouseWheel = 0;
	_float3			m_vDistance = _float3(0, 3, -3);
	CAMERAMODE		m_eCamMode = CAM_DEFAULT;

public:
	static CCameraDynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCamera* Clone(void* pArg);
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr) override;
	virtual void Free() override;

};

END