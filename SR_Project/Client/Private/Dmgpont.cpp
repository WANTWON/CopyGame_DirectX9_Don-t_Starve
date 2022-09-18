#include "stdafx.h"
#include "..\Public\Dmgpont.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "KeyMgr.h"



CDmgpont::CDmgpont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDmgpont::CDmgpont(const CDmgpont & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDmgpont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDmgpont::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//mypos = (*(_float3*)pArg);


	if (pArg != nullptr)
	{
		memcpy(&effectdesc, pArg, sizeof(foreffect));

	}


	//return S_OK;
	
	//_float3 vRight = *(_float3*)&effectdesc.pos.m[0][0];

	//D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);



	if (FAILED(SetUp_Components()))
		return E_FAIL;

	texnum = ((_uint)effectdesc.dmg % 100) / 10;
	texnum1 =((_uint)effectdesc.dmg % 10);

	if (effectdesc.dmg < 10)
	{
		one = true;
	}

	pos = effectdesc.pos;
	
	//pos.x += 2.f;

	
	m_pTransformCom->Set_Scale(0.25f, 0.25f, 1.f);
	m_pTransformCom1->Set_Scale(0.25f, 0.25f, 1.f);
	
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, effectdesc.pos);d
	

	_float3 vRight;

	vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	*D3DXVec3Normalize(&vRight, &vRight);

	random = rand() % 2;

	//pos += (vRight * 1.f);

//	m_pTransformCom1->Set_State(CTransform::STATE_POSITION, pos);d
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(mypos.x - g_iWinSizeX * 0.5f, -pos.y + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CDmgpont::Tick(_float fTimeDelta)
{

	if (GetTickCount() > m_dwDeadtime + 400)
	{
		m_dwDeadtime = GetTickCount();
		return OBJ_DEAD;
	}

	_float3 vRight;

	vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	*D3DXVec3Normalize(&vRight, &vRight);



	

	if (random == 0)
	{
		//effectdesc.pos.x += 0.02f;
		effectdesc.pos += (vRight*0.02f);
		if (GetTickCount() > m_dwDeadtime + 200)
		{
			effectdesc.pos.y -= 0.02f;

		}
		else
			effectdesc.pos.y += 0.02f;
	}
	else

	{
		//effectdesc.pos.x -= 0.02f;
		effectdesc.pos -= (vRight*0.02f);
		if (GetTickCount() > m_dwDeadtime + 200)
		{
			effectdesc.pos.y -= 0.02f;

		}
		else
			effectdesc.pos.y += 0.02f;
	}
	
	
		
		
	pos = effectdesc.pos + (vRight * 0.25f);
	
	


	//pos.y = effectdesc.pos.y; 
	 


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, effectdesc.pos);
	m_pTransformCom1->Set_State(CTransform::STATE_POSITION, pos);



	
	
	


	return OBJ_NOEVENT;
}

void CDmgpont::Late_Tick(_float fTimeDelta)
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

HRESULT CDmgpont::Render()
{
	if (m_bcheck == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		
		if (one == false)
		{
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
				return E_FAIL;

			if (FAILED(SetUp_RenderState()))
				return E_FAIL;

			m_pVIBufferCom->Render();
		}
		

		//effectdesc.pos.x += 2.f;
		//Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, effectdesc.pos);

		if (FAILED(m_pTransformCom1->Bind_OnGraphicDev()))
			return E_FAIL;



		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum1)))
			return E_FAIL;
		
		m_pVIBufferCom->Render();



		if (FAILED(Release_RenderState()))
			return E_FAIL;






	}




	return S_OK;
}

HRESULT CDmgpont::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpPont"), (CComponent**)&m_pTextureCom)))
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



	if (FAILED(__super::Add_Components(TEXT("Com_Transform1"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom1, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CDmgpont::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

void CDmgpont::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	m_pTransformCom1->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom1->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
    m_pTransformCom1->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

HRESULT CDmgpont::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CDmgpont * CDmgpont::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDmgpont*	pInstance = new CDmgpont(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDmgpont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDmgpont::Clone(void* pArg)
{
	CDmgpont*	pInstance = new CDmgpont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDmgpont"));
		Safe_Release(pInstance);
	}

	//CInventory_Manager::Get_Instance()->Get_Eatitem_list()->push_back(pInstance);

	return pInstance;
}


void CDmgpont::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

