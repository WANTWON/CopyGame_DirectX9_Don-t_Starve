#include "..\Public\Transform.h"


CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs.m_pGraphic_Device)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_TransformDesc(rhs.m_TransformDesc)
{
}

HRESULT CTransform::Initialize_Prototype()
{
	D3DXMatrixIdentity(&m_WorldMatrix);

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));
		Set_State(CTransform::STATE_POSITION, m_TransformDesc.InitPos);
	}
		

	return S_OK; 
}

_float3 CTransform::Get_Scale()
{
	_float3		vRight = Get_State(CTransform::STATE_RIGHT);
	_float3		vUp = Get_State(CTransform::STATE_UP);
	_float3		vLook = Get_State(CTransform::STATE_LOOK);

	return _float3(D3DXVec3Length(&vRight), D3DXVec3Length(&vUp), D3DXVec3Length(&vLook));
}

void CTransform::Set_Scale(_float x, _float y, _float z)
{
	/*_float3		vRight = Get_State(CTransform::STATE_RIGHT);
	_float3		vUp = Get_State(CTransform::STATE_UP);
	_float3		vLook = Get_State(CTransform::STATE_LOOK);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight)*vScale.x*x);
	Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp)*vScale.y*y);
	Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook)*vScale.z*z);*/

	_float3		vRight = Get_State(CTransform::STATE_RIGHT);
	_float3		vUp = Get_State(CTransform::STATE_UP);
	_float3		vLook = Get_State(CTransform::STATE_LOOK);

	Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * x);
	Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * y);
	Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook) * z);
}

void CTransform::Go_Straight(_float fTimeDelta, _float fHeight)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vLook = Get_State(CTransform::STATE_LOOK);

	vPos += *D3DXVec3Normalize(&vLook, &vLook)*fTimeDelta*m_TransformDesc.fSpeedPerSec;
	if (NONE_HEIGHT != fHeight)
		vPos.y = fHeight;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_Backward(_float fTimeDelta, _float fHeight)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vLook = Get_State(CTransform::STATE_LOOK);

	vPos -= *D3DXVec3Normalize(&vLook, &vLook)*fTimeDelta*m_TransformDesc.fSpeedPerSec;
	if (NONE_HEIGHT != fHeight)
		vPos.y = fHeight;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_Right(_float fTimeDelta, _float fHeight)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vRight = Get_State(CTransform::STATE_RIGHT);

	vPos += *D3DXVec3Normalize(&vRight, &vRight)*fTimeDelta*m_TransformDesc.fSpeedPerSec;
	if (NONE_HEIGHT != fHeight)
		vPos.y = fHeight;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_Left(_float fTimeDelta, _float fHeight)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vRight = Get_State(CTransform::STATE_RIGHT);

	vPos -= *D3DXVec3Normalize(&vRight, &vRight)*fTimeDelta*m_TransformDesc.fSpeedPerSec;
	if (NONE_HEIGHT != fHeight)
		vPos.y = fHeight;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_PosUp(_float fTimeDelta)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	vPos.z += fTimeDelta*m_TransformDesc.fSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_PosDown(_float fTimeDelta)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	vPos.z -= fTimeDelta*m_TransformDesc.fSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_PosLeft(_float fTimeDelta)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	vPos.x -= fTimeDelta*m_TransformDesc.fSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_PosRight(_float fTimeDelta)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	vPos.x += fTimeDelta*m_TransformDesc.fSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::Go_PosTarget(_float fTimeDelta, _float3 TargetPos, _float3 distance)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vNewPos = { TargetPos.x + distance.x, TargetPos.y + distance.y, TargetPos.z + distance.z };
	_float3 vDir = vNewPos - vPos;

	D3DXVec3Normalize(&vDir, &vDir);

	vPos += vDir*fTimeDelta*m_TransformDesc.fSpeedPerSec;

	Set_State(CTransform::STATE_POSITION, vPos);
}

void CTransform::LookAt(_float3 TargetPos)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vLook = TargetPos - vPos;
	_float3 vRight;

	D3DXVec3Cross(&vRight, &_float3(0,1,0), &vLook);

	_float3		vUp = *D3DXVec3Cross(&vUp, &vLook, &vRight);

	_float3		vScale = Get_Scale();
	
	Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight)*vScale.x);
	Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp)*vScale.y);
	Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize(&vLook, &vLook)*vScale.z);
}

void CTransform::Turn(_float3 vAxis,_float fTimeDelta)
{
	_float3 vRight = Get_State(CTransform::STATE_RIGHT);
	_float3 vUp = Get_State(CTransform::STATE_UP);
	_float3 vLook = Get_State(CTransform::STATE_LOOK);

	_float4x4 RotateMatrix;
	D3DXMatrixRotationAxis(&RotateMatrix, &vAxis, m_TransformDesc.fRotationPerSec*fTimeDelta);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotateMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotateMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotateMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}


void CTransform::Follow_Target(_float3 TargetPos, _float3 distance)
{
	_float3 vPos = Get_State(CTransform::STATE_POSITION);
	_float3 vNewPos = { TargetPos.x + distance.x, TargetPos.y+ distance.y, TargetPos.z + distance.z };

	Set_State(CTransform::STATE_POSITION, vNewPos);
}


HRESULT CTransform::Bind_OnGraphicDev()
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

	return S_OK;
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDevice)
{
	CTransform*	pInstance = new CTransform(pGraphicDevice);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

	//Safe_Release(m_pGraphic_Device);
}
