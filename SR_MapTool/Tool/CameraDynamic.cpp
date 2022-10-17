#include "stdafx.h"
#include "CameraDynamic.h"
#include "GameInstance.h"

CCameraDynamic::CCameraDynamic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCameraDynamic::CCameraDynamic(const CCameraDynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCameraDynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraDynamic::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;

	return S_OK;
}

int CCameraDynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (CKeyMgr::Get_Instance()->Key_Down(VK_F1))
	{
		m_eCamMode = CAM_DEFAULT;
	}


	if (m_eCamMode == CAM_DEFAULT)
	{
		Default_Camera(fTimeDelta);
	}
		

	if (FAILED(Bind_OnGraphicDev()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CCameraDynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCameraDynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

void CCameraDynamic::Default_Camera(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (GetKeyState(VK_RIGHT) < 0)
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
	else if (GetKeyState(VK_LEFT) < 0)
		m_pTransform->Turn(_float3(0.f, 1.f, 0.f), -fTimeDelta);
	else if (GetKeyState(VK_UP) < 0)
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), -fTimeDelta);
	else if (GetKeyState(VK_DOWN) < 0)
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta);

	if (GetKeyState('W') < 0)
		m_pTransform->Go_Straight(fTimeDelta*0.01f);

	if (GetKeyState('S') < 0)
		m_pTransform->Go_Backward(fTimeDelta*0.01f);

	if (GetKeyState('A') < 0)
		m_pTransform->Go_Left(fTimeDelta*0.01f);

	if (GetKeyState('D') < 0)
		m_pTransform->Go_Right(fTimeDelta*0.01f);

	Safe_Release(pGameInstance);
}


CCameraDynamic * CCameraDynamic::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCameraDynamic*	pInstance = new CCameraDynamic(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCameraDynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCamera * CCameraDynamic::Clone(void * pArg)
{
	CCameraDynamic*	pInstance = new CCameraDynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCameraDynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCameraDynamic::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CCameraDynamic::Free()
{
	__super::Free();
}
