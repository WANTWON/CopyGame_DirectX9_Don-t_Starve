#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"
#include "Player.h"
#include "Picking.h"
#include "PickingMgr.h"
#include "Level_Manager.h"
#include "Mouse.h"
#include "Inventory.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_bPicking = false;

	return S_OK;
}

HRESULT CTerrain::Initialize_Load(const _tchar * VIBufferTag, LEVEL TerrainLevelIndex, void * pArg)
{
	if (FAILED(SetUp_Components(VIBufferTag, TerrainLevelIndex, pArg)))
		return E_FAIL;

	m_bPicking = false;

	return S_OK;
}

int CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
		
	
	return OBJ_NOEVENT;
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pVIBufferCom->GetTerrainDesc().m_iTextureNum)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	m_TerrainDesc.m_iNumVerticesX = 50;
	m_TerrainDesc.m_iNumVerticesZ = 50;
	m_TerrainDesc.m_fTextureSize = 5.f;
	m_TerrainDesc.m_fSizeX = 1;
	m_TerrainDesc.m_fSizeZ = 1;
	m_TerrainDesc.m_iTextureNum = 17;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), (CComponent**)&m_pVIBufferCom, &m_TerrainDesc)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::SetUp_Components(const _tchar * VIBufferTag, LEVEL TerrainLevelIndex, void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), TerrainLevelIndex, VIBufferTag, (CComponent**)&m_pVIBufferCom, pArg)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	if (pArg != nullptr)
		TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, true);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CTerrain::SetUp_SamplerState()
{

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

		return S_OK;
}

HRESULT CTerrain::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}


_bool CTerrain::Picking(_float3* PickingPoint)
{
	if (true == m_pVIBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		m_vecOutPos = *PickingPoint;
		return true;
	}
	else
		return false;
}

void CTerrain::PickingTrue()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	CPickingMgr* pPicking = CPickingMgr::Get_Instance();

	CMouse*			pMouse = CMouse::Get_Instance();
	int iNum = pMouse->Get_Item_count();
	
	//for Character Skill
	if (pPlayer->Get_SkillShow())
	{
		pPlayer->Set_PickingTarget(m_vecOutPos);
	}
	//pPlayer->Set_PickingPoint(_float3(m_vecOutPos.x, m_vecOutPos.y + 0.5, m_vecOutPos.z));
	//TestCode
	if (pGameInstance->Key_Up('P'))
	{	
		if (iNum == 0)
		{
			CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
			pMouse->Set_Item_type(ITEM_END);
			auto mouse = pinv->Get_Mouse_item_list()->begin();
			pMouse->Set_picked(false);
			(*mouse)->set_texnum(ITEMNAME_END);
			(*mouse)->set_check(false);//마우스이미지
			pPlayer->Add_ActStack(CPlayer::ACTION_STATE::ANGRY);
			pPlayer->Set_bMove(false);
		}
		else 
		{		
			//for BuildPickingPoint
			pPlayer->Set_PickingPoint(_float3(m_vecOutPos.x, m_vecOutPos.y, m_vecOutPos.z), true);
			pPlayer->Set_IsBuild(false);
			pPlayer->Add_ActStack(CPlayer::ACTION_STATE::MOVE);
		}

	}
	else if (pPlayer->Get_IsBuild())
	{
		pPlayer->Set_IsBuild(false);
		

		if (pPicking->Get_PickingItemType() == ITEM_STRUCT)
		{
			
			if (iNum > 0)
			{
				iNum--;
				pMouse->Set_Item_count(iNum);
				pMouse->Set_Item_prev_count(iNum);

				switch (pMouse->Get_Item_name())
				{
				case ITEMNAME_FENCE:
					pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WoodWall"), LEVEL_GAMEPLAY, TEXT("Layer_UnInteractObject"), pPlayer->Get_PickingPoint());
					break;
				case ITEMNAME_POT:
					pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Cook_Pot"), LEVEL_GAMEPLAY, TEXT("Layer_Object"), pPlayer->Get_PickingPoint());
					break;
				}


				if (iNum == 0)
				{
					CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
					pMouse->Set_Item_type(ITEM_END);
					auto mouse = pinv->Get_Mouse_item_list()->begin();
					pMouse->Set_picked(false);
					(*mouse)->set_texnum(ITEMNAME_END);
					(*mouse)->set_check(false);//마우스이미지
				}


			}
		}
	}
		

	//pPlayer->Set_PickingTarget(_float3(m_vecOutPos.x, m_vecOutPos.y + 0.5, m_vecOutPos.z));
	//cout << "Collision Terrain : " << m_vecOutPos.x << " " << m_vecOutPos.y << " " << m_vecOutPos.z << endl;

	Safe_Release(pGameInstance);
}


CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrain*	pInstance = new CTerrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void* pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone_Load(const _tchar * VIBufferTag, _uint LevelIndex, void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize_Load(VIBufferTag, (LEVEL)LevelIndex, pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
