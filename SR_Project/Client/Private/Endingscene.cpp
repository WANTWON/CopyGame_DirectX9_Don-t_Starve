#include "stdafx.h"
#include "..\Public\Endingscene.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "KeyMgr.h"
#include "Level_GamePlay.h"
#include "CameraManager.h"

CEndingscene::CEndingscene(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CEndingscene::CEndingscene(const CEndingscene & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEndingscene::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEndingscene::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 1280;
	m_fSizeY = 720.f;
	m_fX = 640.f;
	m_fY = 360.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CEndingscene::Tick(_float fTimeDelta)
{
	
	if (m_bforending == true)
	{
		CCamera* pCamera = CCameraManager::Get_Instance()->Get_CurrentCamera();
		dynamic_cast<CCameraDynamic*>(pCamera)->Set_EndingMode();
		if (CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
		{
			if (!m_bOnce)
			{
				CGameInstance::Get_Instance()->StopAll();
				CGameInstance::Get_Instance()->PlaySounds(TEXT("endsong.mp3"), SOUND_UI, 1.f);
				m_bOnce = true;
			}
			
			CLevel* pLevel = CLevel_Manager::Get_Instance()->Get_CurrentLevel();
			m_bcheck = dynamic_cast<CLevel_GamePlay*>(pLevel)->Get_Ending();
		}
	}

	
		

	if (m_bcheck == true)
	{
		__super::Tick(fTimeDelta);
	}
	

	return OBJ_NOEVENT;
}

void CEndingscene::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{
		__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
	
}

HRESULT CEndingscene::Render()
{
	if (m_bcheck == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4		ViewMatrix;
		D3DXMatrixIdentity(&ViewMatrix);

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}

	



	return S_OK;
}

HRESULT CEndingscene::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	///* For.Com_Texture */
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture1"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_loadingback"), (CComponent**)&m_pTextureCom1)))
	//	return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Endingscene"), (CComponent**)&m_pTextureCom)))
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

HRESULT CEndingscene::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CEndingscene::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CEndingscene * CEndingscene::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEndingscene*	pInstance = new CEndingscene(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEndingscene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEndingscene::Clone(void* pArg)
{
	CEndingscene*	pInstance = new CEndingscene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEndingscene"));
		Safe_Release(pInstance);
	}
	CInventory_Manager::Get_Instance()->Get_Ending_list()->push_back(pInstance);
	return pInstance;
}


void CEndingscene::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
