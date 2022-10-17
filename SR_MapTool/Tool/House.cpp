#include "stdafx.h"
#include "House.h"
#include "GameInstance.h"

CHouse::CHouse(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CHouse::CHouse(const CHouse & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHouse::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse::Initialize(void* pArg)
{
	ZeroMemory(&m_HouseDesc, sizeof(HOUSEDECS));
	memcpy(&m_HouseDesc, (HOUSEDECS*)pArg, sizeof(HOUSEDECS));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(&m_HouseDesc.vInitPosition)))
		return E_FAIL;

	switch (m_HouseDesc.m_eState)
	{
	case HOUSETYPE::BOARONSPAWNER:
	case HOUSETYPE::MAZESPAWNER:
	{
		m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);
		m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
		break;
	}
	case HOUSETYPE::SPIDERHOUSE:
	{
		_float fSize = 3;
		m_pTransformCom->Set_Scale(fSize, fSize, 1.f);
		m_fRadius *= fSize;
		m_fRadius -= 0.4f;
		break;
	}
	case HOUSETYPE::PIGHOUSE:
	{
		_float fSize = 2;
		m_pTransformCom->Set_Scale(fSize*0.9f, fSize, 1.f);
		m_fRadius *= fSize;
		m_fRadius -= 0.4f;
		break;
	}
	}

	return S_OK;
}

int CHouse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return OBJ_NOEVENT;
}

void CHouse::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CHouse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (m_HouseDesc.m_eState == MAZESPAWNER)
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;
	}
	

	m_pTextureCom->MoveFrame(m_TimerTag);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_TimerTag = TEXT("Timer_BerryBush");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	Safe_Release(pGameInstance);

	/* For.Com_Texture */
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	switch (m_HouseDesc.m_eState)
	{
	case HOUSETYPE::PIGHOUSE:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pig_House"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case HOUSETYPE::SPIDERHOUSE:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spider_House"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case HOUSETYPE::BOARONSPAWNER:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spawner"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case HOUSETYPE::MAZESPAWNER:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spawner"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	}
	

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CHouse::Release_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CHouse::SetUp_BillBoard()
{

	if (m_HouseDesc.m_eState == HOUSETYPE::BOARONSPAWNER || m_HouseDesc.m_eState == HOUSETYPE::MAZESPAWNER)
		return;

	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);	// Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);		// Get Inverse of View Matrix (World Matrix of Camera)

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
//	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CHouse* CHouse::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHouse* pInstance = new CHouse(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBerryBush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHouse::Clone(void* pArg)
{
	CHouse* pInstance = new CHouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBerryBush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHouse::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CHouse::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

}