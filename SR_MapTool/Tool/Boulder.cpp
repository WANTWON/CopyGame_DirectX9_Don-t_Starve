#include "stdafx.h"
#include "Boulder.h"
#include "GameInstance.h"

CBoulder::CBoulder(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CBoulder::CBoulder(const CBoulder & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoulder::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoulder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 60;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	return S_OK;
}

int CBoulder::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (CKeyMgr::Get_Instance()->Key_Down('F'))
		Apply_Damage(10);

	if (m_tInfo.iCurrentHp > 40)
		m_eState = HEALTHY;
	else if (m_tInfo.iCurrentHp <= 40 && m_tInfo.iCurrentHp > 0)
		m_eState = DAMAGED;
	else if (m_tInfo.iCurrentHp <= 0)
		m_eState = BROKEN;

	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CBoulder::HEALTHY:
			Change_Texture(TEXT("Com_Texture_HEALTHY"));
			break;
		case CBoulder::DAMAGED:
			Change_Texture(TEXT("Com_Texture_DAMAGED"));
			break;
		case CBoulder::BROKEN:
			Change_Texture(TEXT("Com_Texture_BROKEN"));
			break;
		}

		m_ePreState = m_eState;
	}

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return OBJ_NOEVENT;
}

void CBoulder::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBoulder::Render()
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

void CBoulder::Apply_Damage(_uint Damage)
{
	if (m_tInfo.iCurrentHp == 0)
		return;

	if (Damage > m_tInfo.iCurrentHp) 
		m_tInfo.iCurrentHp = 0;
	else 
		m_tInfo.iCurrentHp -= Damage;
}

HRESULT CBoulder::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_TimerTag = TEXT("Timer_Boulder");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);

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

HRESULT CBoulder::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CBoulder::Release_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBoulder::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HEALTHY"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boulder_HEALTHY"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DAMAGED"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boulder_DAMAGED"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_BROKEN"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boulder_BROKEN"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

HRESULT CBoulder::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CBoulder::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);	// Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);		// Get Inverse of View Matrix (World Matrix of Camera)

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CBoulder* CBoulder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoulder* pInstance = new CBoulder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBoulder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoulder::Clone(void* pArg)
{
	CBoulder* pInstance = new CBoulder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBoulder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoulder::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
}

void CBoulder::Free()
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