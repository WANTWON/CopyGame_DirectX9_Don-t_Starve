#include "stdafx.h"
#include "..\Public\Spider.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"

CSpider::CSpider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSpider::CSpider(const CSpider & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(5.f, 1.f, 1.f);

	return S_OK;
}

int CSpider::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	__super::Tick(fTimeDelta);

	m_pTarget = Find_Target();

	if (m_pTarget)
		Follow_Target(fTimeDelta);

	// Change Texture based on State
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case STATE::MOVE:
			// TODO: Check for Direction
			Change_Texture(TEXT("Com_Texture_MOVE"));
			break;
		case STATE::ATTACK:
			// TODO: Check for Direction
			Change_Texture(TEXT("Com_Texture_ATTACK"));
			break;
		case STATE::HIT:
			// TODO: Check for Direction
			Change_Texture(TEXT("Com_Texture_HIT"));
			break;
		case STATE::DIE:
			Change_Texture(TEXT("Com_Texture_DIE"));
			break;
		}

		m_ePreState = m_eState;
	}

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CSpider::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_MONSTER, this);
}

HRESULT CSpider::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	m_pTextureCom->MoveFrame(m_TimerTag);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpider::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Spider"))))
	//	return E_FAIL;

	//m_TimerTag = TEXT("Timer_Spider");

	Safe_Release(pGameInstance);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	/* For Com_Texture */
	CTexture::TEXTUREDESC TextureDesc;
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 32;
	TextureDesc.m_fSpeed = 60;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_RUN_DOWN"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Transform*/
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = { 3,1,3 };

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpider::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CSpider::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CSpider::Texture_Clone()
{
	return E_NOTIMPL;
}

HRESULT CSpider::Change_Texture(const _tchar * LayerTag)
{
	return E_NOTIMPL;
}

void CSpider::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CGameObject* CSpider::Find_Target()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	Safe_Release(pGameInstance);

	if (pTarget)
		return pTarget;
	else
		return nullptr;
}

void CSpider::Follow_Target(_float fTimeDelta)
{
	m_pTransformCom->LookAt(m_pTarget->Get_Position());
	m_pTransformCom->Go_Straight(fTimeDelta * .1f);
}

CSpider* CSpider::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSpider* pInstance = new CSpider(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpider::Clone(void* pArg)
{
	CSpider* pInstance = new CSpider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpider::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CSpider::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}