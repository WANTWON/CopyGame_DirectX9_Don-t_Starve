#include "..\Public\Camera.h"

_tchar* CCamera::m_pTransformTag = TEXT("Com_Transform");


CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDevice)
	: CGameObject(pGraphicDevice)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	m_pTransform = CTransform::Create(m_pGraphic_Device);
	if (m_pTransform == nullptr)
		return E_FAIL;

	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransform->Set_State(CTransform::STATE_POSITION, m_CameraDesc.vEye);
	m_pTransform->LookAt(m_CameraDesc.vAt);
	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);
	
	m_Components.emplace(m_pTransformTag, m_pTransform);

	Safe_AddRef(m_pTransform);

	return S_OK;
}

int CCamera::Tick(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Bind_OnGraphicDev()
{
	_float4x4 matWorld = m_pTransform->Get_WorldMatrix();

	_float4x4 matView = *D3DXMatrixInverse(&matView, nullptr, &matWorld);
	_float4x4 matProj = *D3DXMatrixPerspectiveFovLH(&matProj, m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

	return S_OK;
}


void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
}
