#include "stdafx.h"
#include "../Public/Tree.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"

CTree::CTree(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CTree::CTree(const CTree & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTree::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 60;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	return S_OK;
}

int CTree::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_OBJECT, this);

	// If Hp <= 0 : Fall and Drop Items
	if (m_tInfo.iCurrentHp <= 0 && m_eState < FALL_RIGHT)
	{
		_bool bLeftRight = rand() % 2;
		STATE eFall = bLeftRight ? FALL_RIGHT : FALL_LEFT;
		m_eState = eFall;

		Drop_Items();
	}

	// Change Texture based on State
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CTree::IDLE:
			Change_Texture(TEXT("Com_Texture_Tall_IDLE"));
			break;
		case CTree::CHOP:
			Change_Texture(TEXT("Com_Texture_Tall_CHOP"));
			break;
		case CTree::SHAKE:
			Change_Texture(TEXT("Com_Texture_Tall_SHAKE"));
			break;
		case CTree::GROW:
			Change_Texture(TEXT("Com_Texture_Tall_GROW"));
			break;
		case CTree::FALL_RIGHT:
			Change_Texture(TEXT("Com_Texture_Tall_FALL_RIGHT"));
			break;
		case CTree::FALL_LEFT:
			Change_Texture(TEXT("Com_Texture_Tall_FALL_LEFT"));
			break;
		case CTree::STUMP:
			Change_Texture(TEXT("Com_Texture_Tall_STUMP"));
			break;
		}

		m_ePreState = m_eState;
	}

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CTree::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	if (m_pColliderCom->Collision_with_Group(CCollider::COLLISION_PLAYER, this) && (CKeyMgr::Get_Instance()->Key_Down('F')))
		Interact(10);

	// Move Texture Frame
	switch (m_eState)
	{
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case CHOP:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case SHAKE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case GROW:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
		break;
	case FALL_LEFT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		break;
	case FALL_RIGHT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
		break;
	case STUMP:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

HRESULT CTree::Render()
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

void CTree::Interact(_uint Damage)
{
	if (m_tInfo.iCurrentHp == 0)
		return;

	if (Damage > m_tInfo.iCurrentHp)
		m_tInfo.iCurrentHp = 0;
	else
		m_tInfo.iCurrentHp -= Damage;

	m_eState = CHOP;
}

HRESULT CTree::Drop_Items()
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
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Log");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_WOOD;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_GAMEPLAY, TEXT("Layer_Item"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTree::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

//	m_TimerTag = TEXT("Timer_Tree");
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

HRESULT CTree::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CTree::Release_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CTree::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	// Tall
	TextureDesc.m_iEndTex = 79;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_CHOP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_CHOP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_SHAKE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_SHAKE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_FALL_RIGHT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_RIGHT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 37;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_FALL_LEFT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_LEFT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Tall_STUMP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Tall_STUMP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	// Normal
	//TextureDesc.m_iEndTex = 79;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Normal_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 14;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Normal_CHOP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_CHOP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 31;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Normal_SHAKE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_SHAKE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 19;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Normal_GROW"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_GROW"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 37;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Normal_FALL_RIGHT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_FALL_RIGHT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 37;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Normal_FALL_LEFT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_FALL_LEFT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 0;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Normal_STUMP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal_STUMP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//// Short
	//TextureDesc.m_iEndTex = 79;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Short_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_IDLE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 14;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Short_CHOP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_CHOP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 30;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Short_SHAKE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_SHAKE"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 19;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Short_GROW"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_GROW"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 37;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Short_FALL_RIGHT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_FALL_RIGHT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 37;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Short_FALL_LEFT"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_FALL_LEFT"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	//TextureDesc.m_iEndTex = 0;
	//if (FAILED(__super::Add_Components(TEXT("Com_Texture_Short_STUMP"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Short_STUMP"), (CComponent**)&m_pTextureCom, &TextureDesc)))
	//	return E_FAIL;
	//m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

HRESULT CTree::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CTree::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);	// Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);		// Get Inverse of View Matrix (World Matrix of Camera)

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CTree* CTree::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTree* pInstance = new CTree(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTree::Clone(void* pArg)
{
	CTree* pInstance = new CTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTree::Free()
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

CGameObject * CTree::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}
