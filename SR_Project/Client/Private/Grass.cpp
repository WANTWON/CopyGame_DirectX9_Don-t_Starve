#include "stdafx.h"
#include "../Public/Grass.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"

CGrass::CGrass(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CGrass::CGrass(const CGrass & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGrass::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.6f, 1.f, 1.f);
	
	return S_OK;
}

int CGrass::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// Change Texture based on State
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CGrass::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case CGrass::PICK:
			Change_Texture(TEXT("Com_Texture_PICK"));
			break;
		case CGrass::RUSTLE:
			Change_Texture(TEXT("Com_Texture_RUSTLE"));
			break;
		case CGrass::PICKED:
			Change_Texture(TEXT("Com_Texture_PICKED"));
			break;
		}

		m_ePreState = m_eState;
	}

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CGrass::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_OBJECT, this);

	if (m_pColliderCom->Collision_with_Group(CCollider::COLLISION_PLAYER, this) && (CKeyMgr::Get_Instance()->Key_Down('F')))
		Interact();

	// Move Texture Frame
	switch (m_eState)
	{
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case PICK:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		break;
	case RUSTLE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case PICKED:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

HRESULT CGrass::Render()
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

void CGrass::Interact()
{
	if (m_eState < PICK)
	{
		m_eState = PICK;
		Drop_Items();
	}
}

HRESULT CGrass::Drop_Items()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

	// Random Position Drop based on Object Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_bool bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_bool bSignZ = rand() % 2;
	_float fPosX = bSignX ? (Get_Pos().x + fOffsetX) : (Get_Pos().x - fOffsetX);
	_float fPosZ = bSignZ ? (Get_Pos().z + fOffsetZ) : (Get_Pos().z - fOffsetZ);

	ItemDesc.fPosition = _float3(fPosX, Get_Pos().y, fPosZ);
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Cut_Grass");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_GRASS;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CGrass::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_TimerTag = TEXT("Timer_Grass");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
		//return E_FAIL;

	Safe_Release(pGameInstance);

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
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CGrass::Release_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CGrass::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 20;

	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PICK"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_PICK"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_RUSTLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_RUSTLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PICKED"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_PICKED"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

HRESULT CGrass::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CGrass::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CGrass* CGrass::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGrass* pInstance = new CGrass(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CGrass"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrass::Clone(void* pArg)
{
	CGrass* pInstance = new CGrass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CGrass"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrass::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CGrass::Free()
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