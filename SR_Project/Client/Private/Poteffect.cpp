#include "stdafx.h"
#include "..\Public\Poteffect.h"
#include "GameInstance.h"
#include "Inventory.h"


CPoteffect::CPoteffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPoteffect::CPoteffect(const CPoteffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPoteffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPoteffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//mypos = (*(_float3*)pArg);
	
	
	if (pArg != nullptr)
	{
		memcpy(&effectdesc, pArg, sizeof(foreffect));
		
	}


	//return S_OK;

	//D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.6f, 0.6f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, effectdesc.pos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(mypos.x - g_iWinSizeX * 0.5f, -pos.y + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CPoteffect::Tick(_float fTimeDelta)
{

	if (GetTickCount() > m_dwDeadtime + 1500)
	{
		m_dwDeadtime = GetTickCount();
		return OBJ_DEAD;
	}
	
	


	if (m_bcheck == true)
	{

		

		i += 0.01f;

		if (i >= 1.0f)
			i = 0.6f;

		__super::Tick(fTimeDelta);
		m_pTransformCom->Set_Scale(i, i, 1.f);
		Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		//	RECT		rcRect;
		//	SetRect(&rcRect, (int)(mypos.x - m_fSizeX * 0.5f), (int)(mypos.y - m_fSizeY * 0.5f), (int)(mypos.x + m_fSizeX * 0.5f), (int)(mypos.y + m_fSizeY * 0.5f));

		//POINT		ptMouse;
		//GetCursorPos(&ptMouse);
		//ScreenToClient(g_hWnd, &ptMouse);
		/*if (m_bfirst == true)
		{
			_float4x4		ViewMatrix, ProjMatrix;

			m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
			D3DXVec3TransformCoord(&pos, &pos, &ViewMatrix);

			m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);
			D3DXVec3TransformCoord(&pos, &pos, &ProjMatrix);

			mypos.x = (float)pos.x * 640.f + 639.f;
			mypos.y = (float)pos.y * 359.f + 359.f;



			m_bfirst = false;
		}*/


	

		
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(mypos.x - g_iWinSizeX * 0.5f, -mypos.y + g_iWinSizeY * 0.5f, 0.f));




		//if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON) && m_bcheck == true)
		//{
		//	m_bcheck = false;
		//}
	}

	return OBJ_NOEVENT;
}

void CPoteffect::Late_Tick(_float fTimeDelta)
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

HRESULT CPoteffect::Render()
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

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(effectdesc.itemname)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}




	return S_OK;
}

HRESULT CPoteffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainInventory_front"), (CComponent**)&m_pTextureCom)))
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

HRESULT CPoteffect::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

void CPoteffect::SetUp_BillBoard()
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

HRESULT CPoteffect::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPoteffect * CPoteffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPoteffect*	pInstance = new CPoteffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMouse_item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPoteffect::Clone(void* pArg)
{
	CPoteffect*	pInstance = new CPoteffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMouse_item"));
		Safe_Release(pInstance);
	}

	//CInventory_Manager::Get_Instance()->Get_Eatitem_list()->push_back(pInstance);

	return pInstance;
}


void CPoteffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

