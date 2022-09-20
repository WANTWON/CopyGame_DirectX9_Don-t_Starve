#include "stdafx.h"
#include "..\Public\UnInteractive_Object.h"
#include "GameInstance.h"
#include "Pawn.h"

CUnInteractive_Object::CUnInteractive_Object(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CUnInteractive_Object::CUnInteractive_Object(const CUnInteractive_Object & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUnInteractive_Object::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUnInteractive_Object::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;


	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	return S_OK;
}

int CUnInteractive_Object::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	WalkingTerrain();
	return OBJ_NOEVENT;
}

void CUnInteractive_Object::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	if (m_pColliderCom)
	{
		memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	}
		
}

HRESULT CUnInteractive_Object::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	Set_Texture();

	m_pShaderCom->Begin(m_eShaderID);

	m_pVIBufferCom->Render();

	m_pShaderCom->End();

	return S_OK;
}

void CUnInteractive_Object::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}


void CUnInteractive_Object::Set_ShaderID()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	if (pGameObject->Get_Dead())
		m_eShaderID = SHADER_DEAD;
	else if (iLevel == LEVEL_MAZE)
		m_eShaderID = SHADER_DARK;
	else
		m_eShaderID = SHADER_IDLE_ALPHATEST;
}

HRESULT CUnInteractive_Object::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CUnInteractive_Object::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CUnInteractive_Object::WalkingTerrain()
{

	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (!pGameInstance)
		return;
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (!pVIBuffer_Terrain)
		return;
	CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (!pTransform_Terrain)
		return;

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScale = m_pTransformCom->Get_Scale();
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y *0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}