#include "stdafx.h"
#include "..\Public\Mouse_Monster.h"
#include "GameInstance.h"
#include "Inventory.h"


CMouse_Monster::CMouse_Monster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMouse_Monster::CMouse_Monster(const CMouse_Monster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMouse_Monster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMouse_Monster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/*iNumber = (int*)pArg;

	iNum = *iNumber;*/

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 180.f;
	m_fSizeY = 170.f;


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CMouse_Monster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_bcheck == true )
	{
		RECT		rcRect;
		SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);



		m_fX = (float)ptMouse.x + 100.f;
		m_fY = (float)ptMouse.y ;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));


		switch (monstername)
		{
		case MONSTER_PIG:
			texnum = 0;
			break;
		case MONSTER_SPIDER:
			texnum = 1;
			break;
		case MONSTER_SPIDERWARRIOR:
			texnum = 2;
			break;
		case MONSTER_BEARGER:
			texnum = 3;
			break;
		case MONSTER_BOARON:
			texnum = 4;
			break;
		case MONSTER_BOARRIOR:
			texnum = 5;
			break;
		}
		//if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON) && m_bcheck == true)
		//{
		//	m_bcheck = false;
		//}
	}



	return OBJ_NOEVENT;
}

void CMouse_Monster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	//RECT		rcRect;
	//SetRect(&rcRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

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

	if (nullptr != m_pRendererCom && m_bcheck == true )
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CMouse_Monster::Render()
{
	if (m_bcheck == true && test == true)
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

HRESULT CMouse_Monster::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MonsterInfo"), (CComponent**)&m_pTextureCom)))
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

HRESULT CMouse_Monster::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMouse_Monster::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CMouse_Monster * CMouse_Monster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMouse_Monster*	pInstance = new CMouse_Monster(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMouse_Monster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMouse_Monster::Clone(void* pArg)
{
	CMouse_Monster*	pInstance = new CMouse_Monster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMouse_Monster"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Monsterinfo_list()->push_back(pInstance);

	return pInstance;
}


void CMouse_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

