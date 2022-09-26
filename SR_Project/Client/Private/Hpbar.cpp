#include "stdafx.h"
#include "..\Public\Hpbar.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Statue.h"


CHpbar::CHpbar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CHpbar::CHpbar(const CHpbar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHpbar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpbar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	mypos = (*(_float3*)pArg);

	mypos.y += 2.5f;


	/*if (pArg != nullptr)
	{
		memcpy(&effectdesc, pArg, sizeof(foreffect));

	}*/




	//return S_OK;

	//D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);



	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(3.f, 0.5f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, mypos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(mypos.x - g_iWinSizeX * 0.5f, -pos.y + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CHpbar::Tick(_float fTimeDelta)
{

	/*if (GetTickCount() > m_dwDeadtime + 1500)
	{
		m_dwDeadtime = GetTickCount();
		return OBJ_DEAD;
	}*/

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	texnum = (_uint)dynamic_cast<CStatue*>(pGameInstance->Get_Object(LEVEL_MAZE, TEXT("Layer_Statue")))->Get_hp() / 25;


	if (dynamic_cast<CStatue*>(pGameInstance->Get_Object(LEVEL_MAZE, TEXT("Layer_Statue")))->Get_hp() <= 0)
		return OBJ_DEAD;
	

	

		
	

	return OBJ_NOEVENT;
}

void CHpbar::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{

		__super::Late_Tick(fTimeDelta);
		SetUp_BillBoard();
		//RECT		rcRect;
		//SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		//POINT		ptMouse;
		//GetCursorPos(&ptMouse);
		//ScreenToClient(g_hWnd, &ptMouse);

		//if (!PtInRect(&rcRect, ptMouse))
		//{
		//	m_fSizeX = 40;
		//	m_fSizeY = 40;
		//	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);


		//	//ERR_MSG(L"Ãæµ¹");
		//}



		/*if (m_bItem)
		{

		}*/

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}


}

HRESULT CHpbar::Render()
{
	if (m_bcheck == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		//	_float4x4		ViewMatrix;
		//	D3DXMatrixIdentity(&ViewMatrix);

		//		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		//	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum - 1)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}




	return S_OK;
}

HRESULT CHpbar::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hpbar"), (CComponent**)&m_pTextureCom)))
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

HRESULT CHpbar::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

void CHpbar::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

HRESULT CHpbar::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CHpbar * CHpbar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHpbar*	pInstance = new CHpbar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMouse_item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHpbar::Clone(void* pArg)
{
	CHpbar*	pInstance = new CHpbar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMouse_item"));
		Safe_Release(pInstance);
	}

	//CInventory_Manager::Get_Instance()->Get_Eatitem_list()->push_back(pInstance);

	return pInstance;
}


void CHpbar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

