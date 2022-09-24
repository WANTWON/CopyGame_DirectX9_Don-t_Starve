#include "stdafx.h"
#include "..\Public\CardEffect.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

CCardEffect::CCardEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCardEffect::CCardEffect(const CCardEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCardEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCardEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		memcpy(&effectdesc, pArg, sizeof(foreffect));

	}

	
	pos = effectdesc.pos;
	

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	

	if (effectdesc.whateffect == 0)
		thunder = true;
	else
		thunder = false;

	


	if (thunder == true)
	{
		m_eShaderID = UI_SHADER_MINIMAP;
		m_pTransformCom->Set_Scale(1.5f, 3.f, 1.f);
		m_bcheck = false;
	}
	else
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);


	return S_OK;
}

int CCardEffect::Tick(_float fTimeDelta)
{
	if(!thunder)
	SetUp_BillBoard();

	if (thunder == true)
	{

		time2 += fTimeDelta;

		if (time2 > 1.f)
			m_bcheck = true;


	}

	if (m_bdead == true)
	{
		CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
		pinv->plus_cardgamecount();
		return OBJ_DEAD;
	}
		

	if (m_bcheck == true)
	{
		__super::Tick(fTimeDelta);


		time += fTimeDelta;

		if (time > 0.075f)
		{
			++texnum;
			time = 0.f;
		}



		
	
		

											   








	}
	return OBJ_NOEVENT;
}

void CCardEffect::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{
		__super::Late_Tick(fTimeDelta);


		if (thunder == true)
		{


			if (texnum == 6)
			{

				m_bdead = true;
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				(dynamic_cast<CPlayer*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_HP(-50));
			}
				
			
		}
		else
		{
			if (texnum == 14)
			{
				m_bdead = true;
				
			}
				
		}

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

}

HRESULT CCardEffect::Render()
{
	if (m_bcheck == true)
	{
		if (m_bcheck == true)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;

			if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
				return E_FAIL;

			if (thunder == true)
			{
				if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureCom1->Bind_OnGraphicDev(texnum)))
					return E_FAIL;
			}

			if (FAILED(SetUp_RenderState()))
				return E_FAIL;


			m_pVIBufferCom->Render();



			if (FAILED(Release_RenderState()))
				return E_FAIL;






		}
	}

	return E_FAIL;
}

HRESULT CCardEffect::SetUp_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BlackThunder"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture1"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_cardeffect"), (CComponent**)&m_pTextureCom1)))
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

HRESULT CCardEffect::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);*/


	

	return S_OK;
}

HRESULT CCardEffect::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	return S_OK;
}

CCardEffect * CCardEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCardEffect*	pInstance = new CCardEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCardEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCardEffect::Clone(void* pArg)
{
	CCardEffect*	pInstance = new CCardEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCardEffect"));
		Safe_Release(pInstance);
	}

	//CInventory_Manager::Get_Instance()->Get_Minimapicon_list()->push_back(pInstance);





	return pInstance;
}


void CCardEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom1);
}


void CCardEffect::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	/*m_pTransformCom1->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom1->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom1->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);*/
}