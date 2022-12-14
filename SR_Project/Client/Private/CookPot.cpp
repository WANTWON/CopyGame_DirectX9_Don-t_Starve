#include "stdafx.h"
#include "CookPot.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "PickingMgr.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Inventory.h"

CCookPot::CCookPot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCookPot::CCookPot(const CCookPot & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCookPot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookPot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::COOKPOT;

	return S_OK;
}

int CCookPot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bConstruct)
	{
		_float3 vPickingPos = CPickingMgr::Get_Instance()->Get_PickingPos();
		vPickingPos.y += m_fRadius;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPickingPos);
	}

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCookPot::Late_Tick(_float fTimeDelta)
{
	//__super::Late_Tick(fTimeDelta);



	SetUp_BillBoard();

	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	if (m_pColliderCom)
	{
		memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	}

	if (!m_bPicking && !CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}
	

	Set_ShaderID();
	Change_Motion();
	Change_Frame(fTimeDelta);
	Compute_CamDistance(Get_Position());
	if (m_bConstruct)
		m_eShaderID = SHADER_CONSTRUCT;
}

HRESULT CCookPot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCookPot::Interact(_uint Damage)
{
	m_bInteract = false;
}

HRESULT CCookPot::Drop_Items()
{
	return S_OK;
}

HRESULT CCookPot::SetUp_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_TimerTag = TEXT("Timer_CookPot");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static_Blend"), (CComponent**)&m_pShaderCom)))
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
	TransformDesc.InitPos = *(_float3*)pArg;

	MINIMAP		minidesc;
	ZeroMemory(&minidesc, sizeof(MINIMAP));
	minidesc.name = MIN_POT;
	minidesc.pointer = this;

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), LEVEL_GAMEPLAY, TEXT("MiniMap_Icon"), &minidesc);
	Safe_Release(pGameInstance);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookPot::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 60;

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 22;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_PLACE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 17;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_COOKING"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CookPot_Cooking"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCookPot::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case PLACE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = IDLE;
			break;
	case COOKING:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	}
}

void CCookPot::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case CCookPot::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case CCookPot::PLACE:
			Change_Texture(TEXT("Com_Texture_PLACE"));
			break;
		case CCookPot::COOKING:
			Change_Texture(TEXT("Com_Texture_COOKING"));
			break;
		}

		m_ePreState = m_eState;
	}
}

_bool CCookPot::Picking(_float3 * PickingPoint)
{
	if (CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
		return false;

	if (true == m_pVIBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		m_vecOutPos = *PickingPoint;
		return true;
	}
	else
		return false;
	return true;
}

void CCookPot::PickingTrue()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CInventory_Manager* pInvenManager = CInventory_Manager::Get_Instance();
	
	if (pGameInstance->Key_Up(VK_LBUTTON))
	{
		if (!pInvenManager->Get_PickingPot())
		{
			pInvenManager->Use_pot(this);
			CGameInstance* pInstance = CGameInstance::Get_Instance();
			pInstance->PlaySounds(TEXT("cookpot_open.wav"), SOUND_UIOPEN, 0.9f);
			/*CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			_float3 pos = Get_Position();
			pos.y += 1.f;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Poteffect"), LEVEL_HUNT, TEXT("Layer_Poteffect"), (_float3*)&pos)))
				return;*/
			pInvenManager->Set_PickingPot(true);
		}
		else
		{
			pInvenManager->Off_pot(this);
			CGameInstance* pInstance = CGameInstance::Get_Instance();
			pInstance->PlaySounds(TEXT("cookpot_close.wav"), SOUND_UICLOSE, 0.9f);
			pInvenManager->Set_PickingPot(false);
		}
			
	}	
	//cout << "Collision CookPot : " << m_vecOutPos.x << " " << m_vecOutPos.y << " " << m_vecOutPos.z << endl;
	Safe_Release(pGameInstance);
}

CCookPot* CCookPot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCookPot* pInstance = new CCookPot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCookPot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCookPot::Clone(void* pArg)
{
	CCookPot* pInstance = new CCookPot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCookPot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCookPot::Free()
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