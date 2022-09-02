#include "stdafx.h"
#include "../Public/Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

#include "ParticleSystem.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
}

CItem::CItem(const CItem & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CItem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
		memcpy(&m_ItemDesc, pArg, sizeof(ITEMDESC));

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	//ITEMNAME::
	m_pTransformCom->Set_Scale(.3f, .3f, 1.f);
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::ITEMS;

	return S_OK;
}

int CItem::Tick(_float fTimeDelta)
{

	if (m_bDead)
		return OBJ_DEAD;

	__super::Tick(fTimeDelta);

	WalkingTerrain();
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_OBJECT, this);

	return OBJ_NOEVENT;
}

void CItem::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CItem::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CItem::Interact(_uint Damage)
{
	// Test
	int iTestNum = 0;

	CInventory_Manager* pInventory_Manager = CInventory_Manager::Get_Instance();
	auto Maininvenlist = pInventory_Manager->Get_Inven_list();
	auto equipmentlist = pInventory_Manager->Get_Equipment_list()->begin();

	m_bInteract = false;
	m_bDead = true;
	if (m_bDead)
	{
		CGameInstance* pInstance = CGameInstance::Get_Instance();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"), 0));
		pPlayer->Check_Target(m_bDead);
	}
	++equipmentlist;

	for (auto iter = Maininvenlist->begin(); iter != Maininvenlist->end();)
	{
		if ((*equipmentlist)->get_texnum() == ITEMNAME_BAG) // 가방을 장착하고있을때
		{
			for (auto iter = Maininvenlist->begin(); iter != Maininvenlist->end();)
			{
				if ((*iter)->get_texnum() == (m_ItemDesc.eItemName) && (*iter)->get_check() == true && (*iter)->get_item_number() <= 8)
				{
					(*iter)->plus_itemcount();   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가
					return;
				}
				else if ((*iter)->get_check() == false)
				{
					(*iter)->set_texnum(m_ItemDesc.eItemName); //추후에 아이템enum 만들고부터는 숫자대신 원하는 아이템 넣어주세요
					(*iter)->plus_itemcount();
					(*iter)->set_check(true);

					return;
				}
				else
					++iter;
			}
		}
		else // 가방을 장착하고있지 않을때
		{
			for (auto iter = Maininvenlist->begin(); iter != Maininvenlist->end();)
			{
				if ((*iter)->get_iNum() >= 10)
					return;

				if ((*iter)->get_texnum() == (m_ItemDesc.eItemName) && (*iter)->get_check() == true)
				{
					(*iter)->plus_itemcount();   //먹은 아이템이 인벤토리에 이미 존재할때 카운트 증가
					return;
				}
				else if ((*iter)->get_check() == false)
				{
					(*iter)->set_texnum(m_ItemDesc.eItemName); //추후에 아이템enum 만들고부터는 숫자대신 원하는 아이템 넣어주세요
					(*iter)->plus_itemcount();
					(*iter)->set_check(true);

					return;
				}
				else 
				{
					++iter;
					++iTestNum;
				}
			}
		}	
	}
}

HRESULT CItem::Drop_Items()
{
	return S_OK;
}

HRESULT CItem::SetUp_Components()
{
	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(0.f);
	TransformDesc.InitPos = m_ItemDesc.fPosition;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CItem::Release_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CItem::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = m_ItemDesc.eItemName;
	TextureDesc.m_fSpeed = 60;
	TextureDesc.m_iEndTex = m_ItemDesc.eItemName;

	if (FAILED(__super::Add_Components(m_ItemDesc.pTextureComponent, LEVEL_STATIC, m_ItemDesc.pTexturePrototype, (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CItem::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);	// Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);		// Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CItem::WalkingTerrain()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.15f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItem* pInstance = new CItem(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}