#include "stdafx.h"
#include "..\Public\Sky.h"
#include "GameInstance.h"
#include "CameraDynamic.h"
#include "Player.h"
#include "DayCycle.h"

CSky::CSky(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	CDayCycle::Get_Instance()->RegisterObserver(this);
	return S_OK;
}

int CSky::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_float3*)&ViewMatrix.m[3][0]);
	
	return OBJ_NOEVENT;
}

void CSky::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

	Check_ShaderColor();
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	if (pGameObject->Get_Dead())
		m_eShaderID = SHADER_DEAD;
	else if (iLevel == LEVEL_MAZE)
		m_eShaderID = SHADER_DARK;
	else if (iLevel == LEVEL_BOSS)
		m_eShaderID = SHADER_FIRE;
	else
		m_eShaderID = SHADER_DAYCYClE;

	//m_eShaderID = SHADER_IDLE;
}

HRESULT CSky::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix, PlayerMatrix;

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return E_FAIL;

	PlayerMatrix = *D3DXMatrixTranspose(&WorldMatrix, &pTransform_Terrain->Get_WorldMatrix());

	_float3  fPlayerPosition = pTarget->Get_Position();
	m_pShaderCom->Set_RawValue("g_PlayerPosition", &fPlayerPosition, sizeof(_float3));
	m_pShaderCom->Set_RawValue("g_PlayerWorldMatrix", &PlayerMatrix, sizeof(_float4x4));

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(iTextnum));

	m_pShaderCom->Begin(m_eShaderID);

	m_pVIBufferCom->Render();
	m_pShaderCom->End();

	return S_OK;
}

void CSky::Check_ShaderColor()
{

	if (m_eDayState == DAY_MORNING)
	{
			if (g_fNightDelta <= 0.0f)
				g_fNightDelta = 0.0f;
			else
				g_fNightDelta -= 0.005f;
	}
	else if (m_eDayState == DAY_DINNER)
	{

		if (g_fDinnerDelta >= 1.0f)
			g_fDinnerDelta = 1.0f;
		else
			g_fDinnerDelta += 0.005f;
	}
	else if (m_eDayState == DAY_NIGHT)
	{
		
		g_fDinnerDelta -= 0.002f;
		if (g_fDinnerDelta <= 0.0f)
			g_fDinnerDelta = 0.0f;

		if (g_fNightDelta >= 1.0f)
				g_fNightDelta = 1.0f;
		else
			g_fNightDelta += 0.005f;
	}

	m_pShaderCom->Set_RawValue("g_fDinnerDelta", &g_fDinnerDelta, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fNightDelta", &g_fNightDelta, sizeof(_float));

}

HRESULT CSky::SetUp_Components(void* pArg)
{
	if (pArg == nullptr)
		iTextnum = 2;
	else
		iTextnum = *(int*)pArg;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSky::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;	

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	/*깊이 비교를 하지 않는다. : 가장 먼저 그리니깐. */
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);

	/*깊이 비교를 통과한 픽셀들의 깊이를 깊이버퍼에 저장하지 않는다. : 하늘 이후에 그려지는 객체들이 하늘과 깊이비교를 못하게 만들어주기 위해서. */
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CSky::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	return S_OK;
}

CSky * CSky::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSky*	pInstance = new CSky(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSky::Clone(void* pArg)
{
	CSky*	pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
