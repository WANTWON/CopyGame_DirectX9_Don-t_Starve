#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"

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

	return S_OK;
}

HRESULT CTerrain::Initialize_Load(const _tchar * VIBufferTag, void * pArg)
{
	if (FAILED(SetUp_Components(VIBufferTag, pArg)))
		return E_FAIL;

	return S_OK;
}

int CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Picking();

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


HRESULT CTerrain::Save_Data(HANDLE hFile, _ulong& dwByte)
{
	return m_pVIBufferCom->Save_VertexPos(hFile, dwByte);
}



HRESULT CTerrain::SetUp_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), (CComponent**)&m_pVIBufferCom, pArg)))
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

HRESULT CTerrain::SetUp_Components(const _tchar * VIBufferTag, void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, VIBufferTag, (CComponent**)&m_pVIBufferCom, pArg)))
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

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

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

void CTerrain::Picking()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_float3 OutPos;

	if (pGameInstance->Key_Pressing('1'))
	{
		m_pVIBufferCom->UpTerrain(m_pTransformCom, &OutPos);
	}

	if (pGameInstance->Key_Pressing('2'))
	{
		m_pVIBufferCom->DownTerrain(m_pTransformCom, &OutPos);
	}

	Safe_Release(pGameInstance);
}

bool CTerrain::Picking(_float3 * OutPos)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_pVIBufferCom->Picking(m_pTransformCom, OutPos);

	Safe_Release(pGameInstance);

	return true;
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

CGameObject * CTerrain::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize_Load(VIBufferTag, pArg)))
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
