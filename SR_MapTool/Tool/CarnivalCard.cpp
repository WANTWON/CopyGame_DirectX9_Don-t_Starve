#include "stdafx.h"
#include "CarnivalCard.h"
#include "GameInstance.h"

CCarnivalCard::CCarnivalCard(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarnivalCard::CCarnivalCard(const CCarnivalCard & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCarnivalCard::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnivalCard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eInteract_OBJ_ID = INTERACTOBJ_ID::COOKPOT;
	
	m_bInteract = false;

	return S_OK;
}

int CCarnivalCard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarnivalCard::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CCarnivalCard::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCarnivalCard::Interact(_uint Damage)
{
	
}

HRESULT CCarnivalCard::Drop_Items()
{
	return S_OK;
}

HRESULT CCarnivalCard::SetUp_Components(void* pArg)
{
	/* For.Com_Texture */
	Texture_Clone();

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
	TransformDesc.fRotationPerSec = D3DXToRadian(0.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnivalCard::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	TextureDesc.m_iEndTex = 18;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hint_Bad"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Good"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	
	return S_OK;
}

void CCarnivalCard::Change_Frame(_float fTimeDelta)
{
	
}

void CCarnivalCard::Change_Motion()
{
	
}

CCarnivalCard* CCarnivalCard::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarnivalCard* pInstance = new CCarnivalCard(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCarnivalCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarnivalCard::Clone(void* pArg)
{
	CCarnivalCard* pInstance = new CCarnivalCard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarnivalCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarnivalCard::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}