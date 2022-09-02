#include "stdafx.h"
#include "..\Public\Equip_Animation.h"
#include "GameInstance.h"
#include "Transform.h"

CEquip_Animation::CEquip_Animation(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CEquip_Animation::CEquip_Animation(const CEquip_Animation & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEquip_Animation::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquip_Animation::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

int CEquip_Animation::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eState != ACTION_STATE::ATTACK
		&& m_eState != ACTION_STATE::BUILD)
	{
		m_ePreState = m_eState;
		m_bIsAttack = false;
	}

	Direction_Check();
	Select_Animation();

	return OBJ_NOEVENT;
}

void CEquip_Animation::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	Set_Offset();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_MovePos);
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CEquip_Animation::Render()
{
	if (m_bIsAttack)
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
	}

	return S_OK;
}

HRESULT CEquip_Animation::SetUp_Components(void)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Equip"))))
		return E_FAIL;

	m_TimerTag = TEXT("Timer_Equip");

	Safe_Release(pGameInstance);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	/*if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
	return E_FAIL;*/

	/* For.Com_Texture */
	if (FAILED(Texture_Clone()))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquip_Animation::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CEquip_Animation::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

void CEquip_Animation::Select_Animation()
{
	if (m_eState == ACTION_STATE::ATTACK)
	{
		if (m_eState != m_ePreState)
		{
			switch (m_eDirState)
			{
			case DIR_STATE::DIR_DOWN:
				switch (m_eWeaponType)
				{
				case WEAPON_TYPE::WEAPON_HAND:
					m_bIsAttack = false;
					break;
				case WEAPON_TYPE::WEAPON_SWORD:
					m_bIsAttack = true;
					Change_Texture(TEXT("Com_Texture_Hambat_Down"));
					break;
				case WEAPON_TYPE::WEAPON_STAFF:
					m_bIsAttack = true;
					Change_Texture(TEXT("Com_Texture_Staff_Down"));
					break;
				case WEAPON_TYPE::WEAPON_DART:
					m_bIsAttack = false;
					break;

				}
				break;
			case DIR_STATE::DIR_UP:
				switch (m_eWeaponType)
				{
				case WEAPON_TYPE::WEAPON_HAND:
					m_bIsAttack = false;
					break;
				case WEAPON_TYPE::WEAPON_SWORD:
					m_bIsAttack = true;
					Change_Texture(TEXT("Com_Texture_Hambat_Up"));
					break;
				case WEAPON_TYPE::WEAPON_STAFF:
					m_bIsAttack = true;
					Change_Texture(TEXT("Com_Texture_Staff_Up"));
					break;
				case WEAPON_TYPE::WEAPON_DART:
					m_bIsAttack = false;
					break;
				}
				break;
			case DIR_STATE::DIR_LEFT:
			case DIR_STATE::DIR_RIGHT:
				switch (m_eWeaponType)
				{
				case WEAPON_TYPE::WEAPON_HAND:
					m_bIsAttack = false;
					break;
				case WEAPON_TYPE::WEAPON_SWORD:
					m_bIsAttack = true;
					Change_Texture(TEXT("Com_Texture_Hambat_Side"));
					break;
				case WEAPON_TYPE::WEAPON_STAFF:
					m_bIsAttack = true;
					Change_Texture(TEXT("Com_Texture_Staff_Side"));
					break;
				case WEAPON_TYPE::WEAPON_DART:
					m_bIsAttack = false;
					break;
				}
				break;
			}
			m_ePreState = m_eState;
		}
	}

	if (m_eState == ACTION_STATE::BUILD)
	{
		if (m_eState != m_ePreState)
		{
			switch (m_eDirState)
			{
			case DIR_STATE::DIR_UP:
			case DIR_STATE::DIR_DOWN:
			case DIR_STATE::DIR_LEFT:
			case DIR_STATE::DIR_RIGHT:
				m_bIsAttack = true;
				Change_Texture(TEXT("Com_Texture_Build_Effect"));
				break;
			}
		}
		m_ePreState = m_eState;
	}


}

void CEquip_Animation::Direction_Check()
{

	if (m_ePreDirState != m_eDirState)
	{
		if (m_eDirState == DIR_STATE::DIR_LEFT)
		{
			m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
		}
		else if (m_eDirState == DIR_STATE::DIR_RIGHT)
		{
			m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
		}
		m_ePreDirState = m_eDirState;
	}

}

void CEquip_Animation::Set_Offset()
{
	switch (m_eState)
	{
	case ACTION_STATE::ATTACK:
		if (m_eDirState == DIR_STATE::DIR_RIGHT)
		{
			m_MovePos.x += 0.18f;
		}
		else if (m_eDirState == DIR_STATE::DIR_LEFT)
		{
			m_MovePos.x -= 0.18f;
		}
		else if (m_eDirState == DIR_STATE::DIR_DOWN)
		{
			m_MovePos.x += 0.24f;
		}
		else if (m_eDirState == DIR_STATE::DIR_UP)
		{
			m_MovePos.x -= 0.22f;
		}
		break;
	case ACTION_STATE::BUILD:
		if (m_eDirState == DIR_STATE::DIR_RIGHT)
		{
			m_MovePos.x += 0.18f;
			m_MovePos.y -= 0.1f;
			m_MovePos.z -= 0.15f;
		}
		else if (m_eDirState == DIR_STATE::DIR_LEFT)
		{
			m_MovePos.x -= 0.18f;
			m_MovePos.y -= 0.1f;
			m_MovePos.z -= 0.15f;
		}
		else if (m_eDirState == DIR_STATE::DIR_DOWN)
		{
			m_MovePos.x += 0.24f;
			m_MovePos.y -= 0.1f;
			m_MovePos.z -= 0.1f;
		}
		else if (m_eDirState == DIR_STATE::DIR_UP)
		{
			m_MovePos.x -= 0.22f;
		}
		break;
	default:
		break;
	}
	
}

HRESULT CEquip_Animation::Texture_Clone(void)
{
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	/*Hambat*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 36;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hambat_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_Hambat_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hambat_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_Hambat_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hambat_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_Hambat_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	/*Staff*/
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Staff_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_FireStaff_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Staff_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_FireStaff_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Staff_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Anim_FireStaff_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Build_Effect*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 25;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Effect"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuildEffect"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);


	return S_OK;
}

HRESULT CEquip_Animation::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CEquip_Animation::SetUp_BillBoard()
{
	_float4x4		ViewMatrix;

	if (m_eDirState == DIR_STATE::DIR_LEFT)
		return;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CEquip_Animation * CEquip_Animation::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEquip_Animation*	pInstance = new CEquip_Animation(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEquipment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquip_Animation::Clone(void * pArg)
{
	CEquip_Animation*	pInstance = new CEquip_Animation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEquipment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CEquip_Animation::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	//Safe_Release(m_pColliderCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
