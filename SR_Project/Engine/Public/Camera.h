#pragma once
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float3		vEye; //À§Ä¡
		_float3		vAt;

		_float		fAspect;
		_float		fFovy;
		_float		fNear;
		_float		fFar;

		CTransform::TRANSFORMDESC		TransformDesc;

	}CAMERADESC;

protected:
	CCamera(LPDIRECT3DDEVICE9 pGraphicDevice);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Bind_OnGraphicDev();
	void Add_Fov(_float fAspect) { m_CameraDesc.fFovy += fAspect; }

protected:
	static _tchar*		m_pTransformTag;
	class CTransform*	m_pTransform = nullptr;
	CAMERADESC			m_CameraDesc;


public:
	virtual CCamera* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END