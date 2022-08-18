#include "stdafx.h"
#include "..\Public\Weapon.h"
#include "GameInstance.h"
#include "Player.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1.5f, 1.5f, 0);

	return S_OK;
}

int CWeapon::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (dwTime + 500 < GetTickCount())
	{
		pGameInstance->Delete_Timer(m_TimerTag);
		dwTime = GetTickCount();
		return 1;
	}
		

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	Safe_AddRef(pTarget);

	m_TargetPos = pTarget->Get_Pos();

	Safe_Release(pTarget);

	Safe_Release(pGameInstance);

	//m_pTransformCom->Look_Target(_float3(0.f, 1.f, 0.f), fTimeDelta, m_TargetPos);
	//m_pTransformCom->Go_Straight(fTimeDelta);
	m_pTransformCom->Follow_Target(m_TargetPos);

	return 0;
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CWeapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	//m_pTextureCom->MoveFrame(m_TimerTag);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::SetUp_Components(void* pArg)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Spear"))))
		return E_FAIL;

	m_TimerTag = TEXT("Timer_Spear");

	Safe_Release(pGameInstance);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 46;
	TextureDesc.m_fSpeed = 60;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spear_Attack_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Spear"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Transform*/
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));


	TransformDesc.fRotateionPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = { 0,1,0 };
	TransformDesc.Target = &m_TargetPos;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CWeapon::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CWeapon::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

CWeapon * CWeapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWeapon*	pInstance = new CWeapon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWeapon::Clone(void* pArg)
{
	CWeapon*	pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
