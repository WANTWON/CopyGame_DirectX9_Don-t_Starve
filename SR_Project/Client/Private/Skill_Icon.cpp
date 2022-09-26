#include "stdafx.h"
#include "..\Public\Skill_Icon.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

CSkill_Icon::CSkill_Icon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSkill_Icon::CSkill_Icon(const CSkill_Icon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkill_Icon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Icon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	iNumber = (int*)pArg;

	iNum = *iNumber;

	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 40.f;
	m_fSizeY = 40.f;
	m_fX = 1060.f + (iNum*45);
	m_fY = 700.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

int CSkill_Icon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	


	return OBJ_NOEVENT;
}

void CSkill_Icon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (iNum == 0 && texnum == 0)
		forbomb = true;

	if(forbomb == true)
		usebomb(fTimeDelta);


	if (iNum == 1 && texnum1 == 0)
		forice = true;

	if (forice == true)
		useice(fTimeDelta);

	if (iNum == 2 && texnum2 == 0)
		forsand = true;

	if (forsand == true)
		usesand(fTimeDelta);

	if (iNum == 3 && texnum3 == 0)
		forteleport = true;

	if (forteleport == true)
		useteleport(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSkill_Icon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	
	if (iNum == 0)
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}
	else if (iNum == 1)
	{
		if (FAILED(m_pTextureCom1->Bind_OnGraphicDev(texnum1)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}
	else if (iNum == 2)
	{
		if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(texnum2)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom3->Bind_OnGraphicDev(texnum3)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}

	



	return S_OK;
}

HRESULT CSkill_Icon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_bomb"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture1"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ice"), (CComponent**)&m_pTextureCom1)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_sand"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_teleport"), (CComponent**)&m_pTextureCom3)))
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

HRESULT CSkill_Icon::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CSkill_Icon::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CSkill_Icon * CSkill_Icon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkill_Icon*	pInstance = new CSkill_Icon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkill_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkill_Icon::Clone(void* pArg)
{
	CSkill_Icon*	pInstance = new CSkill_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSkill_Icon"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Skill_list()->push_back(pInstance);

	return pInstance;
}


void CSkill_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom1);
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
}

void CSkill_Icon::usebomb(_float time)
{
	
	timer += time;

	if (timer >= 0.25)
	{
		++texnum;
	     

	 if (texnum >= 22)
	forbomb = false;

		 timer = 0.f;
	}
		
}

void CSkill_Icon::useice(_float time)
{
	timer += time;

	if (timer >= 0.125)
	{
		++texnum1;


		if (texnum1 >= 22)
			forice = false;

		timer = 0.f;
	}
}

void CSkill_Icon::usesand(_float time)
{

	

	timer += time;

	if (timer >= 0.33)
	{
		++texnum2;


		if (texnum2 >= 22)
			forsand = false;

		timer = 0.f;
	}
}

void CSkill_Icon::useteleport(_float time)
{
	timer += time;

	if (timer >= 0.375)
	{
		++texnum3;


		if (texnum3 >= 22)
			forteleport = false;

		timer = 0.f;
	}
}
