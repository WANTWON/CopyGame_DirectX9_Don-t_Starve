#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE {STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END};

	typedef struct tagTransformDesc
	{
		_float3 InitPos = {0,0,0};
		_float fSpeedPerSec;
		_float fRotationPerSec;
	}TRANSFORMDESC;


private:
	CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	_float3 Get_State(STATE eState) { return *(_float3*)&m_WorldMatrix.m[eState][0]; }
	void Set_State(STATE eState, _float3 vState) { memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof(_float3)); }
	_float3 Get_Scale();
	void Set_Scale(_float x, _float y, _float z);
	_float4x4 Get_WorldMatrix() const { return m_WorldMatrix;}
	void Set_TransformDesc(TRANSFORMDESC TransformDesc) { m_TransformDesc = TransformDesc; }
	TRANSFORMDESC Get_TransformDesc(void) { return m_TransformDesc; }
public:
	void Go_Straight(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Go_Backward(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Go_Right(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Go_Left(_float fTimeDelta, _float fHeight = NONE_HEIGHT);
	void Turn(_float3 vAxis, _float fTimeDelta);

	/* Just Position Trans*/
	void Go_PosUp(_float fTimeDelta);
	void Go_PosDown(_float fTimeDelta);
	void Go_PosLeft(_float fTimeDelta);
	void Go_PosRight(_float fTimeDelta);
	void Go_PosTarget(_float fTimeDelta, _float3 TargetPos, _float3 distance);

public:
	void LookAt(_float3 TargetPos);
	void Follow_Target(_float fTimeDelta, _float3 TargetPos, _float3 distance);
	

public:
	HRESULT Bind_OnGraphicDev();
	void Set_Target(_float3 target) { m_Target = target; }

private:
	_float4x4		m_WorldMatrix;
	TRANSFORMDESC	m_TransformDesc;
	_float3			m_Target;
public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END