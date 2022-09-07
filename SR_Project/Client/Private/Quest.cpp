#include "stdafx.h"
#include "..\Public\Quest.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"


CQuest::CQuest(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CQuest::CQuest(const CQuest & rhs)
	: CGameObject(rhs)
{
}

HRESULT CQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuest::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 350.f;
	m_fSizeY = 350.f;
	m_fX = 1000.f;
	m_fY = 400.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	texnum = 0;

	return S_OK;
}

int CQuest::Tick(_float fTimeDelta)
{
	/*if (b_onoff == true && CKeyMgr::Get_Instance()->Key_Up(VK_ESCAPE))
	{
		b_onoff = false;
		
	}
	else if (b_onoff == false && CKeyMgr::Get_Instance()->Key_Up(VK_ESCAPE))
	{
		b_onoff = true;
	}*/

	/*if (b_onoff == true);
	{
		if (texnum == 2)
		{
			m_fSizeX = 200.f;
			m_fSizeY = 150.f;
			m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		}*/

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


		return OBJ_NOEVENT;
   }
	


void CQuest::Late_Tick(_float fTimeDelta)
{
	if (b_onoff == true)
	{
		__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CQuest::Render()
{
	if (b_onoff == true)
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
	}


	return S_OK;
}

HRESULT CQuest::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_quest"), (CComponent**)&m_pTextureCom)))
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

HRESULT CQuest::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 20);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CQuest::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CQuest * CQuest::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CQuest*	pInstance = new CQuest(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQuest::Clone(void* pArg)
{
	CQuest*	pInstance = new CQuest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CQuest"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Quest_list()->push_back(pInstance);
	return pInstance;
}


void CQuest::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
