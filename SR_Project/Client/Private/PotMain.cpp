#include "stdafx.h"
#include "..\Public\PotMain.h"
#include "GameInstance.h"
#include "Inventory.h"

CPotMain::CPotMain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPotMain::CPotMain(const CPotMain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPotMain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPotMain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 100.f;
	m_fSizeY = 300.f;
	m_fX = 740.f;
	m_fY = 400.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CPotMain::Tick(_float fTimeDelta)
{
	if (m_bcheck_pot == true)
	{
		__super::Tick(fTimeDelta);
		/*RECT		rcRect;
		SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		*/

		if (m_eState != m_ePreState)
		{
			switch (m_eState)
			{
			case CPotMain::IDLE:
				Change_Texture(TEXT("Com_Texture"));
				break;
			case CPotMain::OPEN:
				Change_Texture(TEXT("Com_Texture_Pot_open"));
				break;
			case CPotMain::CLOSE:
				Change_Texture(TEXT("Com_Texture_Pot_close"));
				break;
			}

			m_ePreState = m_eState;
		}

	}
	

	
	return OBJ_NOEVENT;
}

void CPotMain::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck_pot == true)
	{
		__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);


		switch (m_eState)
		{
		case IDLE:
			//m_pTextureCom->MoveFrame(m_TimerTag, false);
			break;
			//if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			//break;
		case OPEN:
			if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			{
				m_eState = IDLE;
				m_pTextureCom->Set_ZeroFrame();
				Change_Texture(TEXT("Com_Texture"));
			}

			break;
		case CLOSE:
			if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			{
				m_bcheck_pot = false;
			}


			break;
		}
	}

}

HRESULT CPotMain::Render()
{
	if (m_bcheck_pot == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4		ViewMatrix;
		D3DXMatrixIdentity(&ViewMatrix);

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;

	}
	



	return S_OK;
}

HRESULT CPotMain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(Texture_Clone()))
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

HRESULT CPotMain::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPotMain::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPotMain * CPotMain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPotMain*	pInstance = new CPotMain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPotMain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPotMain::Clone(void* pArg)
{
	CPotMain*	pInstance = new CPotMain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPotMain"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Pot_list()->push_back(pInstance);

	return pInstance;
}

HRESULT CPotMain::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 10;

	TextureDesc.m_iStartTex = 1;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pot"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);


	// Tall
	TextureDesc.m_iEndTex = 11;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pot_open"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Potopen"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 9;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pot_close"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Potclose"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);



	return S_OK;
}

HRESULT CPotMain::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CPotMain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
