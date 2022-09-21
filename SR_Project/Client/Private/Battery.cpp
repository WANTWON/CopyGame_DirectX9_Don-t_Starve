#include "stdafx.h"
#include "..\Public\Battery.h"
#include "GameInstance.h"
#include "Catapult.h"
#include "Battery_Tower.h"

CBattery::CBattery(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CBattery::CBattery(const CBattery & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CBattery::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattery::Initialize(void* pArg)
{
	ZeroMemory(&m_tDesc, sizeof(m_tDesc));

	m_tDesc = *(BatteryDesc*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::BATTERY;
	
	static_cast <CBattery_Tower*>(m_tDesc.pOwner)->Set_Target(this);

	m_bInteract = false;

	m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
	return S_OK;
}

int CBattery::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;

	Position_Offest();

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CBattery::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	_float3 vPosition = Get_Position();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	Change_Frame(fTimeDelta);
	
}

HRESULT CBattery::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBattery::Interact(_uint Damage)
{
	m_iBattery = Damage;
}

HRESULT CBattery::Drop_Items()
{
	return S_OK;
}

void CBattery::Destroy()
{

}

HRESULT CBattery::SetUp_Components(void* pArg)
{
	//	m_TimerTag = TEXT("Timer_Tree");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

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
	TransformDesc.InitPos = m_tDesc.vInitPos;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.25f;
	CollRectDesc.fRadiusX = 0.25f;
	CollRectDesc.fRadiusZ = 0.5f;
	CollRectDesc.fOffSetX = 0.0f;
	CollRectDesc.fOffSetY = 0.5f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattery::Texture_Clone()
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;
	TextureDesc.m_iEndTex = 6;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Battery"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battery"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CBattery::Change_Frame(_float fTimeDelta)
{
	if (m_iBattery == 7)
	{
		Safe_Release(m_tDesc.pOwner);
		m_pTextureCom->Get_Frame().m_iCurrentTex = 0;
		m_bDead = true;
	}
	else
	{
		m_pTextureCom->Get_Frame().m_iCurrentTex = m_iBattery;
	}

}

void CBattery::Change_Motion()
{

}

void CBattery::Position_Offest()
{
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vLook, &vLook);
	_float3 vPos = m_tDesc.pOwner->Get_Position() -= vLook * .1f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

CBattery* CBattery::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBattery* pInstance = new CBattery(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBattery"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBattery::Clone(void* pArg)
{
	CBattery* pInstance = new CBattery(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBattery"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattery::Free()
{
	__super::Free();

}

