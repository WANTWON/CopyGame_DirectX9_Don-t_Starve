#include "stdafx.h"
#include "..\Public\Particle.h"
#include "GameInstance.h"

CParticle::CParticle(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}


CParticle::CParticle(const CParticle & rhs)
	: CGameObject(rhs)
	,m_StateDesc(rhs.m_StateDesc)
	,m_iCurrentFrameIndex(m_iCurrentFrameIndex)
	,m_fAlpha(m_fAlpha)
{
}

HRESULT CParticle::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CParticle::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

int CParticle::Tick(_float fTimeDelta)
{
	

	return OBJ_NOEVENT;
}

void CParticle::Late_Tick(_float fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;

	m_pTextureCom->MoveFrame(TEXT(""));

	//if (FAILED(SetUp_RenderState()))
		//return E_FAIL;

	m_pVIBufferCom->Render();

	//if (FAILED(Release_RenderState()))
		//return E_FAIL;

	return S_OK;
}

HRESULT CParticle::SetUp_Components(void * pArg)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Monster"))))
	//	return E_FAIL;

	//m_TimerTag = TEXT("Timer_Monster");

	Safe_Release(pGameInstance);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/


	/* For Com_Texture */
	CTexture::TEXTUREDESC		TextureDesc;
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 32;
	TextureDesc.m_fSpeed = 60;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pig_RUN_DOWN"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Transform*/
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));


	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = { 0,0,0 };

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Particle"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	//Test
	Set_Radius(0.25f);

	return S_OK;
}

CParticle * CParticle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CParticle*	pInstance = new CParticle(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle::Clone(void * pArg)
{
	CParticle*	pInstance = new CParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}