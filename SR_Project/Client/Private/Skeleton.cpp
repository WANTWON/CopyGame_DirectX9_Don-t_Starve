#include "stdafx.h"
#include "..\Public\Skeleton.h"
#include "GameInstance.h"
#include "Player.h"

CSkeleton::CSkeleton(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CInteractive_Object(pGraphic_Device)
{
}

CSkeleton::CSkeleton(const CSkeleton & rhs)
	:CInteractive_Object(rhs)
{
}

HRESULT CSkeleton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkeleton::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	//Test
	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::SKELETON;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);



	return S_OK;
}

int CSkeleton::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CSkeleton::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CSkeleton::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkeleton::SetUp_Components(void * pArg)
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
	/*if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;*/

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

HRESULT CSkeleton::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 1;
	TextureDesc.m_fSpeed = 20;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Skeleton"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CSkeleton::Change_Frame()
{
}

void CSkeleton::Change_Motion()
{
}

void CSkeleton::Interact(_uint Damage)
{
	m_bInteract = false;
}

HRESULT CSkeleton::Drop_Items()
{
	return S_OK;
}

CSkeleton * CSkeleton::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkeleton* pInstance = new CSkeleton(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkeleton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkeleton::Clone(void * pArg)
{
	CSkeleton* pInstance = new CSkeleton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSkeleton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkeleton::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
