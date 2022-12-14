#include "stdafx.h"
#include "..\Public\Daycount.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

CDaycount::CDaycount(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDaycount::CDaycount(const CDaycount & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDaycount::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDaycount::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 80.f;
	m_fSizeY = 80.f;
	m_fX = 1230.f;
	m_fY = 50.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	texnum = 0;

	return S_OK;
}

int CDaycount::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	//CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
	//Safe_AddRef(pGameInstance);
	//Safe_AddRef(pinv);

	//pinv->sethp((dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Player_Stat().fCurrentHealth));

	//texnum = pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->get_
	//texnum = (dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Get_Player_Stat().fCurrentHealth) / 2;
	

	//Safe_Release(pGameInstance);
	//Safe_Release(pinv);
	++texnum;
	if (texnum >= 30)
		texnum = 0;

	return OBJ_NOEVENT;
}

void CDaycount::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDaycount::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CDaycount::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Daycount"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDaycount::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CDaycount::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CDaycount * CDaycount::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDaycount*	pInstance = new CDaycount(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDaycount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDaycount::Clone(void* pArg)
{
	CDaycount*	pInstance = new CDaycount(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDaycount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CDaycount::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	
}
