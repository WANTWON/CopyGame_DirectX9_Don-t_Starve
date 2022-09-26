#include "stdafx.h"
#include "Boarrior.h"
#include "GameInstance.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "CameraManager.h"
#include "House.h"
#include "Totem.h"
#include "TotemEffect.h"
#include "PickingMgr.h"
#include "Portal.h"
#include "Level_Boss.h"
#include "ShockEffect.h"

CBoarrior::CBoarrior(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CBoarrior::CBoarrior(const CBoarrior & rhs)
	: CMonster(rhs)
{
}

HRESULT CBoarrior::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoarrior::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(6.f, 6.f, 1.f);
	m_fRadius = m_pTransformCom->Get_Scale().y * 0.5f;

	m_tInfo.iMaxHp = 1500.f;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
	m_tInfo.fDamage = 20.f;

	m_fRadius = 6.f;
	m_fAggroRadius = 7.f;
	m_fAttackRadius = 3.f;
	m_fSpecialAttackRadius = 6.f;

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
	
	return S_OK;
}

int CBoarrior::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta) && m_bDeadAnimExpired)
	{
		CPickingMgr::Get_Instance()->Out_PickingGroup(this);
		CLevel* pLevel =  CLevel_Manager::Get_Instance()->Get_CurrentLevel();
		dynamic_cast<CLevel_Boss*>(pLevel)->Set_PortalMake(true);
		/*CPortal::PORTALDESC PortalDesc;
		PortalDesc.m_eType = CPortal::PORTAL_GAMEPLAY;
		PortalDesc.vPosition = _float3(14.f, 1.f, 15.f);

		if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_BOSS, TEXT("Layer_Object"), &PortalDesc)))
			return E_FAIL;*/

		auto line = CInventory_Manager::Get_Instance()->Get_Line_list();

		for (auto k : *line)
			k->set_quest5(true);

		return OBJ_DEAD;
	}
		
	if (m_bShouldSpawnBullet)
		Spawn_Bullet(fTimeDelta);

	Check_Totem_Effect(fTimeDelta);
	Check_Health_Percent();

	// A.I.
	AI_Behaviour(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CBoarrior::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Change_Motion();
	Change_Frame(fTimeDelta);

	// Use/Reset Shader Hit
	if (m_bUseHitShader)
	{
		if (m_fHitTime > .25f)
		{
			m_eShaderID = SHADER_IDLE;
			m_fHitTime = 0.f;
			m_bUseHitShader = false;
		}
		else
		{
			m_eShaderID = SHADER_HIT;
			m_fHitTime += fTimeDelta;
		}
	}

	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);
	if (!m_bPicking)
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}
}

HRESULT CBoarrior::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	if (g_ColliderRender &&  m_pColliderCom != nullptr)
		m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

HRESULT CBoarrior::SetUp_Components(void* pArg)
{
	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.InitPos = *(_float3*)pArg;

	MINIMAP		minidesc;
	ZeroMemory(&minidesc, sizeof(MINIMAP));
	minidesc.name = MIN_BOARRIOR;
	minidesc.pointer = this;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), LEVEL_BOSS, TEXT("MiniMap_Icon"), &minidesc);
	Safe_Release(pGameInstance);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	
	CollRectDesc.fRadiusY = .2f;
	CollRectDesc.fRadiusX = .2f;
	CollRectDesc.fRadiusZ = .8f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -2.f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	SetUp_DebugComponents(pArg);

	return S_OK;
}

HRESULT CBoarrior::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;
	
	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_1_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_1_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 25;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_1_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_1_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 44;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_2"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_2"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_3_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_3_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 57;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_ATTACK_3_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Attack_3_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 15;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DASH_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Dash_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 15;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DASH_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Dash_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 15;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DASH_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Dash_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 92;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_DEATH"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Death"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Hit_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Hit_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 16;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_HIT_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Hit_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 31;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 14;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_SPAWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Spawn"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 6;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_STUN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Stun"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 36;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_DOWN"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Walk_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 36;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_SIDE"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Walk_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 36;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_WALK_UP"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Boarrior_Walk_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CBoarrior::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case STATE::IDLE:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-6.f, 6.f, 1.f);
		else
			m_pTransformCom->Set_Scale(6.f, 6.f, 1.f);

		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::WALK:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-6.f, 6.f, 1.f);
		else
			m_pTransformCom->Set_Scale(6.f, 6.f, 1.f);

		Check_CameraShake();
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case STATE::DASH:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-6.f, 6.f, 1.f);
		else
			m_pTransformCom->Set_Scale(6.f, 6.f, 1.f);

		if ((m_pTextureCom->MoveFrame(m_TimerTag)) == true)
			m_eState = IDLE;
		break;
	case STATE::ATTACK_1:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-6.f, 6.f, 1.f);
		else
			m_pTransformCom->Set_Scale(6.f, 6.f, 1.f);

		Attack(fTimeDelta);
		break;
	case STATE::ATTACK_2:
		Attack(fTimeDelta, m_eState);
		break;
	case STATE::ATTACK_3:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-6.f, 6.f, 1.f);
		else
			m_pTransformCom->Set_Scale(6.f, 6.f, 1.f);

		Attack(fTimeDelta, m_eState);
		break;
	case STATE::SPAWN:
	{
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 1 && m_bFirstFrame)
		{
			// Screen Effect
			_bool forboss = true;
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Screen_Effect"), LEVEL_BOSS, TEXT("Layer_Screeneffect"), (bool*)&forboss);
			m_bfirst = false;

			// Play Sound
			_tchar szFileName[MAX_PATH] = TEXT("");
			wsprintf(szFileName, TEXT("bannercall_boarrior_%02d.wav"), 1);
			pGameInstance->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

			m_bFirstFrame = false;
		}
		else
			m_bFirstFrame = true;

		m_fSpawnTime += fTimeDelta;
		if (m_fSpawnTime > 2.f)
		{
			m_fSpawnTime = 0.f;
			m_eState = STATE::IDLE;
			m_bfirst = true;
			Spawn_Adds(fTimeDelta);
		}

		m_pTextureCom->MoveFrame(m_TimerTag);
	}
		break;
	case STATE::STUN:
		m_pTextureCom->MoveFrame(m_TimerTag);

		m_fStunTime += fTimeDelta;
		if (m_fStunTime > m_fStunTimeLimit)
		{
			m_fStunTime = 0.f;
			m_bStun = false;

			// Reset Variables
			m_bIsAttacking = false;
			m_dwAttackTime = GetTickCount();
			m_iPattern = 0;
			m_bDidDamage = false;
			m_vAttackPos = _float3(0.f, 0.f, 0.f);
			m_bShouldSpawn = false;
			m_fFollowTime = 0.f;
			iAnimFrameSyncCounter = 0;
		}
			
		break;
	case STATE::HIT:
		if (m_eDir == DIR_STATE::DIR_LEFT)
			m_pTransformCom->Set_Scale(-6.f, 6.f, 1.f);
		else
			m_pTransformCom->Set_Scale(6.f, 6.f, 1.f);

		if ((m_pTextureCom->MoveFrame(m_TimerTag, false) == true))
		{
			m_eShaderID = SHADER_IDLE;
			m_bHit = false;
		}
		break;
	case STATE::DIE:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 29)
		{
			if (m_bFirstFrame)
			{
				// Play Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("impact_mech_dull_%d.wav"), 2);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

				m_bFirstFrame = false;
			}
		}
		else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 51)
		{
			if (m_bFirstFrame)
			{
				// Play Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("impact_mech_dull_%d.wav"), 1);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

				m_bFirstFrame = false;
			}
		}
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 70)
		{
			if (m_bFirstFrame)
			{
				// Play Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("impact_flesh_dull_%d.wav"), 2);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

				m_bFirstFrame = false;
			}
		}
		else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 55)
		{
			if (m_bFirstFrame)
			{
				// Camera Shake
				CCameraDynamic* pCamera = dynamic_cast<CCameraDynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
				if (pCamera)
					pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.5f, 0.1f, 0.01f);

				// Play Step Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("bearger_step_%d.wav"), rand() % 8 + 1);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .7f);

				m_bFirstFrame = false;
			}
		}
		else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 74)
		{
			if (m_bFirstFrame)
			{
				Drop_Items();

				m_bFirstFrame = false;
			}
		}
		else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 92)
			m_bDeadAnimExpired = true;
		else
			m_bFirstFrame = true;

		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	}
}

void CBoarrior::Change_Motion()
{
	if (m_eState != m_ePreState || m_eDir != m_ePreDir)
	{
		switch (m_eState)
		{
		case STATE::IDLE:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_IDLE_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_IDLE_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_IDLE_SIDE"));
				break;
			}
			break;
		case STATE::WALK:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_WALK_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_WALK_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_WALK_SIDE"));
				break;
			}
			break;
		case STATE::DASH:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_DASH_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_DASH_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_DASH_SIDE"));
				break;
			}
			break;
		case STATE::ATTACK_1:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_ATTACK_1_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_ATTACK_1_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_ATTACK_1_SIDE"));
				break;
			}
			break;
		case STATE::ATTACK_2:
			Change_Texture(TEXT("Com_Texture_ATTACK_2"));
			break;
		case STATE::ATTACK_3:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_ATTACK_3_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_ATTACK_3_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_ATTACK_3_SIDE"));
				break;
			}
			break;
		case STATE::SPAWN:
			Change_Texture(TEXT("Com_Texture_SPAWN"));
			break;
		case STATE::STUN:
			Change_Texture(TEXT("Com_Texture_STUN"));
			break;
		case STATE::HIT:
			switch (m_eDir)
			{
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_HIT_UP"));
				break;
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_HIT_DOWN"));
				break;
			case DIR_STATE::DIR_RIGHT:
			case DIR_STATE::DIR_LEFT:
				Change_Texture(TEXT("Com_Texture_HIT_SIDE"));
				break;
			}
			break;
		case STATE::DIE:
			Change_Texture(TEXT("Com_Texture_DEATH"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;
		if (m_eDir != m_ePreDir)
			m_ePreDir = m_eDir;
	}
}

_bool CBoarrior::Picking(_float3 * PickingPoint)
{

	if (CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
		return false;

	if (true == m_pVIBufferCom->Picking(m_pTransformCom, PickingPoint))
	{
		m_vecOutPos = *PickingPoint;



		return true;
	}
	else
	{
		m_bPickingTrue = false;
		CInventory_Manager* pInvenManager = CInventory_Manager::Get_Instance(); Safe_AddRef(pInvenManager);
		m_eShaderID = SHADER_IDLE;
		auto i = pInvenManager->Get_Monsterinfo_list()->front();
		auto k = pInvenManager->Get_Monsterhp_list();

		i->set_monstername(MONSTER_END);
		i->set_check(false);


		for (auto j : *k)
			j->set_check(false);
		Safe_Release(pInvenManager);
			return false;
	}

	return true;
}

void CBoarrior::PickingTrue()
{
	m_bPickingTrue = true;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance(); Safe_AddRef(pGameInstance);
	CInventory_Manager* pInvenManager = CInventory_Manager::Get_Instance(); Safe_AddRef(pInvenManager);

	auto i = pInvenManager->Get_Monsterinfo_list()->front();
	auto k = pInvenManager->Get_Monsterhp_list();
	m_eShaderID = SHADER_PICKING;

	i->set_monstername(MONSTER_BOARRIOR);
	i->set_check(true);

	for (auto j : *k)
	{
		j->set_check(true);
		j->set_hp((_uint)m_tInfo.iCurrentHp);
	}

	Safe_Release(pGameInstance);
	Safe_Release(pInvenManager);
}

void CBoarrior::Check_CameraShake()
{
	CCameraDynamic* pCamera = dynamic_cast<CCameraDynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());

	switch (m_eDir)
	{
		case DIR_STATE::DIR_DOWN:
		{
			if ((m_pTextureCom->Get_Frame().m_iCurrentTex == 0 || m_pTextureCom->Get_Frame().m_iCurrentTex == 18) && m_bFirstFrame)
			{
				// Play Walk Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("boarrior_step_%03d.wav"), rand() % 15 + 1);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .6f);

				// Camera Shake
				if (pCamera)
					pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);

				m_bFirstFrame = false;
			}
			else
				m_bFirstFrame = true;
		}
		break;
		case DIR_STATE::DIR_UP:
		{
			if ((m_pTextureCom->Get_Frame().m_iCurrentTex == 8 || m_pTextureCom->Get_Frame().m_iCurrentTex == 23) && m_bFirstFrame)
			{
				// Play Walk Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("boarrior_step_%03d.wav"), rand() % 15 + 1);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .6f);

				// Camera Shake
				if (pCamera)
					pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);

				m_bFirstFrame = false;
			}
			else
				m_bFirstFrame = true;
		}
		break;
		case DIR_STATE::DIR_RIGHT:
		case DIR_STATE::DIR_LEFT:
		{
			if ((m_pTextureCom->Get_Frame().m_iCurrentTex == 0 || m_pTextureCom->Get_Frame().m_iCurrentTex == 18) && m_bFirstFrame)
			{
				// Play Walk Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("boarrior_step_%03d.wav"), rand() % 15 + 1);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .6f);

				// Camera Shake
				if (pCamera)
					pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);

				m_bFirstFrame = false;
			}
			else
				m_bFirstFrame = true;
		}
		break;
	}
}

void CBoarrior::Check_Totem_Effect(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
	if (!pGameInstance || !pLevelManager)
		return;

	list<CGameObject*>* lGameObject = pGameInstance->Get_ObjectList(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Totem"));
	if (!lGameObject)
		return;

	// Check for Active Totems
	CTotem* pTotem = nullptr;
	for (auto& iter = lGameObject->begin(); iter != lGameObject->end(); ++iter)
	{
		pTotem = dynamic_cast<CTotem*>(*iter);
		if (!pTotem)
			continue;

		if (pTotem->Get_TotemDesc().eType == CTotem::TOTEM_TYPE::DEFENSE || pTotem->Get_TotemDesc().eType == CTotem::TOTEM_TYPE::HEAL)
			goto Effect;
	}

Effect:
	if (pTotem)
	{
		switch (pTotem->Get_TotemDesc().eType)
		{
		case CTotem::TOTEM_TYPE::DEFENSE:
			m_bHasDefenseBoost = true;
			break;
		case CTotem::TOTEM_TYPE::HEAL:
			Totem_Heal(fTimeDelta); // Heal over time
			break;
		}
	}
	else
	{
		m_bHasDefenseBoost = false;
		return;
	}
}

void CBoarrior::Totem_Heal(_float fTimeDelta)
{
	if (m_fHealTimer > 5.f && !m_bDead)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);

		// Spawn Heal Effect
		CTotemEffect::TOTEMEFFECTDESC TotemEffectDesc;
		TotemEffectDesc.pEffectTarget = this;
		TotemEffectDesc.eType = CTotemEffect::TOTEM_EFFECT_TYPE::HEAL;
		TotemEffectDesc.vInitPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
		TotemEffectDesc.vInitPosition.y = pVIBuffer_Terrain->Compute_Height(TotemEffectDesc.vInitPosition, pTransform_Terrain->Get_WorldMatrix(), .01f);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem_Effect"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Totem"), &TotemEffectDesc);

		// Play Heal Sound
		pGameInstance->PlaySounds(TEXT("heal_effect.wav"), SOUND_MONSTER_EFFECT, 1.f);

		// Spawn Heal Effect Particles
		TotemEffectDesc.eType = CTotemEffect::TOTEM_EFFECT_TYPE::PARTICLES;
		_float3 vLook;
		D3DXVec3Normalize(&vLook, &m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK));
		TotemEffectDesc.vInitPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3] - vLook;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem_Effect"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Totem"), &TotemEffectDesc);

		list<CGameObject*>* lGameObject = pGameInstance->Get_ObjectList(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Totem"));
		if (!lGameObject)
			return;

		// Check for Active Totems
		CTotem* pTotem = nullptr;
		for (auto& iter = lGameObject->begin(); iter != lGameObject->end(); ++iter)
		{
			pTotem = dynamic_cast<CTotem*>(*iter);
			if (!pTotem)
				continue;

			if (pTotem->Get_TotemDesc().eType == CTotem::TOTEM_TYPE::HEAL)
			{
				// Spawn Heal Effect on Totems
				TotemEffectDesc.pEffectTarget = nullptr;
				TotemEffectDesc.eType = CTotemEffect::TOTEM_EFFECT_TYPE::HEAL;
				TotemEffectDesc.vInitPosition = pTotem->Get_Position();
				TotemEffectDesc.vInitPosition.y = pVIBuffer_Terrain->Compute_Height(TotemEffectDesc.vInitPosition, pTransform_Terrain->Get_WorldMatrix(), .01f);
				pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem_Effect"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Totem"), &TotemEffectDesc);

				pTotem->Set_IsActive(true);
			}
		}

		// Increase Health
		m_tInfo.iCurrentHp += 150;
		if (m_tInfo.iCurrentHp > m_tInfo.iMaxHp)
			m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

		m_fHealTimer = 0.f;
	}
	else
		m_fHealTimer += fTimeDelta;
}

void CBoarrior::Check_Health_Percent()
{
	if (m_tInfo.iCurrentHp <= 0 || m_bDead)
		return;
	
	if (m_tInfo.iCurrentHp <= (m_tInfo.iMaxHp / 100 * 20) && !m_bIsBelow20Percent)
	{
		m_bIsBelow20Percent = true;
		m_bShouldSpawn = true;
	}	
	else if (m_tInfo.iCurrentHp <= (m_tInfo.iMaxHp / 100 * 40) && !m_bIsBelow40Percent)
	{
		m_bIsBelow40Percent = true;
		m_bShouldSpawn = true;
	}
	else if (m_tInfo.iCurrentHp <= (m_tInfo.iMaxHp / 100 * 60) && !m_bIsBelow60Percent)
	{
		m_bIsBelow60Percent = true;
		m_bShouldSpawn = true;
	}
	else if (m_tInfo.iCurrentHp <= (m_tInfo.iMaxHp / 100 * 80) && !m_bIsBelow80Percent)
	{
		m_bIsBelow80Percent = true;
		m_bShouldSpawn = true;
	}
}

void CBoarrior::AI_Behaviour(_float fTimeDelta)
{
	if (m_bDead)
		return;

	if (m_bStun)
	{
		if (m_eState == STATE::SPAWN)
		{
			m_bStun = false;
			return;
		}

		if (m_eState != STATE::STUN)
		{
			m_eState = STATE::STUN;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();

			// Play Stun Sound
			_tchar szFileName[MAX_PATH] = TEXT("");
			wsprintf(szFileName, TEXT("boarrior_shock_%d.wav"), rand() % 3 + 1);
			CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

			// Spawn Shock Effect
			CShockEffect::SHOCKDESC ShockDesc;
			ShockDesc.eShockType = CShockEffect::SHOCKTYPE::SHOCK;
			ShockDesc.pEffectTarget = this;
			ShockDesc.fShockTimeLimit = m_fStunTimeLimit;

			_float3 vLook;
			D3DXVec3Normalize(&vLook, &m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK));
			ShockDesc.vInitPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3] - vLook;
		
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shock_Effect"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Effect"), &ShockDesc);
		}

		return;
	}
		
	if (m_bHit)
	{
		m_eState = STATE::HIT;
		return;
	}

	if (m_bIsAttacking)
		return;

	Find_Target();  // Check: pTarget, bAggro, fDistanceToTarget

	if (m_pTarget && m_bAggro)
	{
		// Randomly choose a Pattern (if not already chosen)
		if (!m_iPattern)
		{
			m_iPattern = m_bShouldSpawn ? 4 : rand() % 3 + 1;

			if (m_iPattern == 3 || m_iPattern == 4)
			{			
				// Play Taunt Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("taunt_boarrior_%02d.wav"), rand() % 5 + 1);
				CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_VOICE, .7f);
			}
		}

		// Set Attack Radius based on Pattern chosen before
		_float fRadius;
		switch (m_iPattern)
		{
		case 1:
			fRadius = m_fAttackRadius;
			break;
		case 2:
		case 3:
			fRadius = m_fSpecialAttackRadius;
			break;
		case 4:
			fRadius = 999.f; // Spawn Adds
			break;
		default:
			fRadius = m_fAttackRadius;
		}

		Calculate_Direction(m_pTarget->Get_Position());

		// If in AttackRadius: Attack
		if (m_fDistanceToTarget < fRadius)
		{
			if (GetTickCount() > m_dwAttackTime + 1500)
			{
				if (m_iPattern == 1 || m_iPattern == 2 || m_iPattern == 3 || m_iPattern == 4)
				{
					m_eState = m_iPattern == 1 ? STATE::ATTACK_1 : m_iPattern == 2 ? STATE::ATTACK_2 : m_iPattern == 3 ? STATE::ATTACK_3 : STATE::SPAWN;
					m_bIsAttacking = true;
					m_fFollowTime = 0.f;
				}
			}
			else
				m_eState = STATE::IDLE;
		}
		// If NOT in AttackRadius: Follow Target
		else
			Follow_Target(fTimeDelta);
	}
	else
		Patrol(fTimeDelta);
}

void CBoarrior::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	if (m_eState == STATE::IDLE)
	{
		if (GetTickCount() > m_dwIdleTime + 3000)
		{
			m_eState = STATE::WALK;
			m_dwWalkTime = GetTickCount();

			// Find Random Patroling Position
			_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
			_int bSignX = rand() % 2;
			_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
			_int bSignZ = rand() % 2;
			m_fPatrolPosX = bSignX ? (m_pTransformCom->Get_TransformDesc().InitPos.x + fOffsetX) : (m_pTransformCom->Get_TransformDesc().InitPos.x - fOffsetX);
			m_fPatrolPosZ = bSignZ ? (m_pTransformCom->Get_TransformDesc().InitPos.z + fOffsetZ) : (m_pTransformCom->Get_TransformDesc().InitPos.z - fOffsetZ);
		}
	}
	else if (m_eState == STATE::WALK)
	{
		if (GetTickCount() > m_dwWalkTime + 1500)
		{
			m_eState = STATE::IDLE;
			m_dwIdleTime = GetTickCount();
		}
	}

	// Movement
	if (m_eState == STATE::WALK)
	{
		// Adjust PatrolPosition Y
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (!pGameInstance)
			return;
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_BOSS, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		if (!pVIBuffer_Terrain)
			return;
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_BOSS, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
		if (!pTransform_Terrain)
			return;

		_float3 vPatrolPosition = { m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ };
		_float3 vScale = m_pTransformCom->Get_Scale();

		vPatrolPosition.y = pVIBuffer_Terrain->Compute_Height(vPatrolPosition, pTransform_Terrain->Get_WorldMatrix(), (1 * vScale.y / 2));

		Calculate_Direction(vPatrolPosition);

		m_pTransformCom->Go_PosTarget(fTimeDelta * .1f, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
	}
}

void CBoarrior::Find_Target()
{
	if (m_bDead)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);

	if (pPlayer)
	{
		if (pPlayer->Get_Dead())
		{
			if (m_bAggro)
			{
				m_pTarget = nullptr;
				m_eState = STATE::IDLE;
				m_bAggro = false;
			}
			return;
		}
			
		if (pTarget)
		{
			_float3 vTargetPos = pTarget->Get_Position();
			m_fDistanceToTarget = D3DXVec3Length(&(vTargetPos - Get_Position()));

			if (m_fDistanceToTarget < m_fAggroRadius || m_bAggro)
			{
				// Set Target and Aggro
				m_pTarget = pTarget;
				m_bAggro = true;
			}
		}
	}
}

void CBoarrior::Follow_Target(_float fTimeDelta)
{
	if (m_fFollowTime < 3.f)
	{
		m_fFollowTime += fTimeDelta;
	
		// Set State 
		m_eState = STATE::WALK;

		_float3 fTargetPos = m_pTarget->Get_Position();
		m_pTransformCom->Go_PosTarget(fTimeDelta * .2f, fTargetPos, _float3(0, 0, 0));

		m_bIsAttacking = false;
	}
	else
	{
		m_iPattern = rand() % 2 + 2;
		m_fFollowTime = 0.f;
	}
}

void CBoarrior::Attack(_float fTimeDelta, STATE eAttack)
{
	if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
	{
		// Resetting Variables
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();
		m_bDidDamage = false;
		m_iPattern = 0; 
		m_vAttackPos = _float3(0.f, 0.f, 0.f);
		iAnimFrameSyncCounter = 0;
	}
	else 
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		switch (eAttack)
		{
		case STATE::ATTACK_1:
		{
			// Normal Attack
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 5 && !m_bDidDamage)
			{
				// Play Sound
				_tchar szFileName[MAX_PATH] = TEXT("");
				wsprintf(szFileName, TEXT("boarrior_attack_%d.wav"), rand() % 4 + 1);
				pGameInstance->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

				// Create Standard Bullet
				BULLETDATA BulletData;
				ZeroMemory(&BulletData, sizeof(BulletData));

				BulletData.bIsPlayerBullet = false;
				BulletData.vPosition = m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
				BulletData.eDirState = Get_Unprocessed_Dir(m_eDir);
				BulletData.fOffsetSide = 1.f;
				BulletData.fOffsetUp = 1.f;
				BulletData.fOffsetDown = 1.f;

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
					return;

				m_bDidDamage = true;
			}
		}
		break;
		case STATE::ATTACK_2:
		{
			// Whirling Attack

			// Move towards Player
			if (m_vAttackPos == _float3(0.f, 0.f, 0.f))
				m_vAttackPos = m_pTarget->Get_Position();
			
			m_pTransformCom->Go_PosTarget(fTimeDelta, m_vAttackPos, _float3(0, 0, 0));

			// Spawn Fire Effect
			BULLETDATA BulletData;
			ZeroMemory(&BulletData, sizeof(BulletData));

			BulletData.bIsPlayerBullet = false;
			BulletData.eWeaponType = WEAPON_TYPE::BOARRIOR_FIRE;
			
			_float3 vCenterPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
			_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK);
			D3DXVec3Normalize(&vLook, &vLook);
			_float fDistance = 1.f;

			BulletData.vPosition = vCenterPosition - _float3(vLook.x * fDistance, 0.f, vLook.z * fDistance);
			
			switch (m_pTextureCom->Get_Frame().m_iCurrentTex)
			{
			case 12:
				if (iAnimFrameSyncCounter == 0)
				{
					// Play Sound
					_tchar szFileName[MAX_PATH] = TEXT("");
					wsprintf(szFileName, TEXT("bearger_swhoosh_%d.wav"), rand() % 4 + 1);
					pGameInstance->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

					// Play Fire Sound
					pGameInstance->PlaySounds(TEXT("boarrior_fire.wav"), SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			case 13:
				if (iAnimFrameSyncCounter == 1)
				{
					BulletData.vPosition.x = vCenterPosition.x + cosf(D3DXToRadian(315.f)) * fDistance - sin(D3DXToRadian(315.f)) * fDistance;
					BulletData.vPosition.z = vCenterPosition.z + sin(D3DXToRadian(315.f)) * fDistance + cos(D3DXToRadian(315.f)) * fDistance;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			case 15:
				if (iAnimFrameSyncCounter == 2)
				{
					BulletData.vPosition.x = vCenterPosition.x + cosf(D3DXToRadian(360.f)) * fDistance - sin(D3DXToRadian(360.f)) * fDistance;
					BulletData.vPosition.z = vCenterPosition.z + sin(D3DXToRadian(360.f)) * fDistance + cos(D3DXToRadian(360.f)) * fDistance;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			case 17:
				if (iAnimFrameSyncCounter == 3)
				{
					BulletData.vPosition.x = vCenterPosition.x + cosf(D3DXToRadian(45.f)) * fDistance - sin(D3DXToRadian(45.f)) * fDistance;
					BulletData.vPosition.z = vCenterPosition.z + sin(D3DXToRadian(45.f)) * fDistance + cos(D3DXToRadian(45.f)) * fDistance;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			case 19:
				if (iAnimFrameSyncCounter == 4)
				{
					// Play Sound
					_tchar szFileName[MAX_PATH] = TEXT("");
					wsprintf(szFileName, TEXT("bearger_swhoosh_%d.wav"), rand() % 4 + 1);
					pGameInstance->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

					// Play Fire Sound
					pGameInstance->PlaySounds(TEXT("boarrior_fire.wav"), SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

					BulletData.vPosition.x = vCenterPosition.x + cosf(D3DXToRadian(90.f)) * fDistance - sin(D3DXToRadian(90.f)) * fDistance;
					BulletData.vPosition.z = vCenterPosition.z + sin(D3DXToRadian(90.f)) * fDistance + cos(D3DXToRadian(90.f)) * fDistance;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			case 20:
				if (iAnimFrameSyncCounter == 5)
				{
					BulletData.vPosition.x = vCenterPosition.x + cosf(D3DXToRadian(135.f)) * fDistance - sin(D3DXToRadian(135.f)) * fDistance;
					BulletData.vPosition.z = vCenterPosition.z + sin(D3DXToRadian(135.f)) * fDistance + cos(D3DXToRadian(135.f)) * fDistance;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			case 22:
				if (iAnimFrameSyncCounter == 6)
				{
					BulletData.vPosition.x = vCenterPosition.x + cosf(D3DXToRadian(180.f)) * fDistance - sin(D3DXToRadian(180.f)) * fDistance;
					BulletData.vPosition.z = vCenterPosition.z + sin(D3DXToRadian(180.f)) * fDistance + cos(D3DXToRadian(180.f)) * fDistance;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			case 25:
				if (iAnimFrameSyncCounter == 7)
				{
					BulletData.vPosition.x = vCenterPosition.x + cosf(D3DXToRadian(225.f)) * fDistance - sin(D3DXToRadian(225.f)) * fDistance;
					BulletData.vPosition.z = vCenterPosition.z + sin(D3DXToRadian(225.f)) * fDistance + cos(D3DXToRadian(225.f)) * fDistance;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData)))
						return;

					iAnimFrameSyncCounter++;
				}
				break;
			}

			// If did NOT Apply Damage yet
			if (!m_bDidDamage)
			{
				// If Colliding with Player 
				vector<CGameObject*> vecDamagedActor;
				if (pGameInstance->Collision_Check_Group_Multi(CCollider_Manager::COLLISION_PLAYER, vecDamagedActor, this, CCollider_Manager::COLLSIION_BOX))
				{
					CGameInstance::Apply_Damage_Multi(m_tInfo.fDamage, vecDamagedActor, this, nullptr);
					m_bDidDamage = true;
				}
			}
		}
		break;
		case STATE::ATTACK_3:
		{
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 14)
			{
				if (m_bFirstFrame)
				{
					CGameInstance::Get_Instance()->PlaySounds(TEXT("hit_ground.wav"), SOUND_ID::SOUND_MONSTER_EFFECT, 1.f);
					m_bFirstFrame = false;
				}
			}
			else if (m_pTextureCom->Get_Frame().m_iCurrentTex == 38 && !m_bShouldSpawnBullet)
			{
				m_bShouldSpawnBullet = true;

				// Play Sound
				CGameInstance::Get_Instance()->PlaySounds(TEXT("firehound_explo.wav"), SOUND_ID::SOUND_MONSTER_EFFECT, 1.f);

				CCameraDynamic* pCamera = dynamic_cast<CCameraDynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
				if (pCamera)
					pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.7f, 0.2f, 0.01f);
			}
			else
				m_bFirstFrame = true;

			break;
		}
		}
	}
}

void CBoarrior::Spawn_Bullet(_float fTimeDelta)
{
	if (m_fBulletAliveTime < .05f)
		m_fBulletAliveTime += fTimeDelta;
	else
	{
		m_fBulletAliveTime = 0.f;
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		// Special Attack
		BULLETDATA BulletData1;
		BULLETDATA BulletData2;
		ZeroMemory(&BulletData1, sizeof(BulletData1));
		ZeroMemory(&BulletData2, sizeof(BulletData2));

		BulletData1.bIsPlayerBullet = false;
		BulletData2.bIsPlayerBullet = false;
		BulletData1.eWeaponType = WEAPON_TYPE::BOARRIOR_SPECIAL;
		BulletData2.eWeaponType = WEAPON_TYPE::BOARRIOR_SPECIAL;
		BulletData1.vPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
		BulletData2.vPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];

		_float fForwardOffset = 1.25f;
		_float fSideOffset = 0.4f;

		switch (Get_Unprocessed_Dir(m_eDir))
		{
		case DIR_STATE::DIR_DOWN:
			BulletData1.vPosition -= _float3(fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			BulletData2.vPosition -= _float3(-fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			break;
		case DIR_STATE::DIR_RIGHT:
			BulletData1.vPosition += _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, -fSideOffset);
			BulletData2.vPosition += _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, fSideOffset);
			break;
		case DIR_STATE::DIR_UP:
			BulletData1.vPosition += _float3(-fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			BulletData2.vPosition += _float3(fSideOffset, 0.f, 1.f + fForwardOffset * m_iBulletCount);
			break;
		case DIR_STATE::DIR_LEFT:
			BulletData1.vPosition -= _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, fSideOffset);
			BulletData2.vPosition -= _float3(1.f + fForwardOffset * m_iBulletCount, 0.f, -fSideOffset);
			break;
		}

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData1)))
			return;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), iLevelIndex, TEXT("Bullet"), &BulletData2)))
			return;

		m_iBulletCount += 1;

		if (m_iBulletCount == 5)
		{
			m_bShouldSpawnBullet = false;
			m_iBulletCount = 0;
		}	
	}
}

void CBoarrior::Spawn_Adds(_float fTimeDelta)
{
	// Reset Variables
	m_bIsAttacking = false;
	m_dwAttackTime = GetTickCount();
	m_iPattern = 0;
	m_bDidDamage = false;
	m_vAttackPos = _float3(0.f, 0.f, 0.f);
	m_bShouldSpawn = false;
	m_fFollowTime = 0.f;
	iAnimFrameSyncCounter = 0;

	// Shake
	CCameraDynamic* pCamera = dynamic_cast<CCameraDynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
	if (pCamera)
		pCamera->Set_CamMode(CCameraDynamic::CAM_SHAKING, 0.5f, 0.2f, 0.01f);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CTotem::TOTEMDESC TotemDesc;

	if (m_bIsBelow20Percent)
	{
		TotemDesc.eType = CTotem::TOTEM_TYPE::HEAL;
		_tchar szFileName[MAX_PATH] = TEXT("");
		wsprintf(szFileName, TEXT("banner_craft_heal_0%d.wav"), 1);
		pGameInstance->PlaySounds(szFileName, SOUND_ID::SOUND_OBJECT, 1.f);

		goto SpawnTotem;
	}
	else if (m_bIsBelow40Percent)
		goto SpawnAdds;
	else if (m_bIsBelow60Percent)
	{
		TotemDesc.eType = CTotem::TOTEM_TYPE::DEFENSE;
		_tchar szFileName[MAX_PATH] = TEXT("");
		wsprintf(szFileName, TEXT("banner_craft_shield_0%d.wav"), 1);
		pGameInstance->PlaySounds(szFileName, SOUND_ID::SOUND_OBJECT, 1.f);

		goto SpawnTotem;
	}
	else if (m_bIsBelow80Percent)
		goto SpawnAdds;

SpawnAdds:
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
	list<CGameObject*>* lObjects = pGameInstance->Get_ObjectList(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_House"));

	// Play Spawn Sound
	pGameInstance->PlaySounds(TEXT("spawn_smoke.wav"), SOUND_MONSTER_EFFECT, 1.f);

	if (!lObjects) return;
	for (auto& iter = lObjects->begin(); iter != lObjects->end(); ++iter)
	{
		CHouse* pSpawner = dynamic_cast<CHouse*>(*iter);
		if (pSpawner)
			pSpawner->Spawn_Boaron(fTimeDelta);
	}
	return;

SpawnTotem:
	TotemDesc.vInitPosition = _float3(10.f, 0.f, 10.f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem"), LEVEL_BOSS, TEXT("Layer_Totem"), &TotemDesc);
	TotemDesc.vInitPosition = _float3(10.f, 0.f, 19.f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem"), LEVEL_BOSS, TEXT("Layer_Totem"), &TotemDesc);
	TotemDesc.vInitPosition = _float3(19.f, 0.f, 10.f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem"), LEVEL_BOSS, TEXT("Layer_Totem"), &TotemDesc);
	TotemDesc.vInitPosition = _float3(19.f, 0.f, 19.f);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem"), LEVEL_BOSS, TEXT("Layer_Totem"), &TotemDesc);
	return;
}

_float CBoarrior::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	DAMAGETYPEDESC tDamageTypeDesc;
	ZeroMemory(&tDamageTypeDesc, sizeof(DAMAGETYPEDESC));

	if (DamageType)
		memcpy(&tDamageTypeDesc, (DAMAGETYPEDESC*)DamageType, sizeof(DAMAGETYPEDESC));

	if (m_bHasDefenseBoost)
	{
		fDamage = fDamage / 100 * 20;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
		CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
		CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);

		// Play Shield Sound
		_tchar szFileName[MAX_PATH] = TEXT("");
		wsprintf(szFileName, TEXT("boarrior_shield_hit_%02d.wav"), rand() % 4 + 1);
		CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_EFFECT, .8f);

		// Spawn Shield Effect
		CTotemEffect::TOTEMEFFECTDESC TotemEffectDesc;
		TotemEffectDesc.pEffectTarget = this;
		TotemEffectDesc.eType = CTotemEffect::TOTEM_EFFECT_TYPE::SHIELD;
		_float3 vLook;
		D3DXVec3Normalize(&vLook, &m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK));
		TotemEffectDesc.vInitPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3] - vLook;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem_Effect"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Totem"), &TotemEffectDesc);

		// Spawn Defense Effect
		TotemEffectDesc.eType = CTotemEffect::TOTEM_EFFECT_TYPE::DEFENSE;
		TotemEffectDesc.vInitPosition = (_float3)m_pColliderCom->Get_CollRectDesc().StateMatrix.m[3];
		TotemEffectDesc.vInitPosition.y = pVIBuffer_Terrain->Compute_Height(TotemEffectDesc.vInitPosition, pTransform_Terrain->Get_WorldMatrix(), .01f);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem_Effect"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Totem"), &TotemEffectDesc);

		// Set Totem Active Animation
		list<CGameObject*>* lGameObject = pGameInstance->Get_ObjectList(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Totem"));
		if (!lGameObject)
			goto ApplyDamage;

		for (auto& iter = lGameObject->begin(); iter != lGameObject->end(); ++iter)
		{
			CTotem* pTotem = dynamic_cast<CTotem*>(*iter);
			if (!pTotem)
				continue;

			if (pTotem->Get_TotemDesc().eType == CTotem::TOTEM_TYPE::DEFENSE)
			{
				TotemEffectDesc.pEffectTarget = nullptr;
				TotemEffectDesc.vInitPosition = pTotem->Get_Position();
				TotemEffectDesc.vInitPosition.y = pVIBuffer_Terrain->Compute_Height(TotemEffectDesc.vInitPosition, pTransform_Terrain->Get_WorldMatrix(), .01f);
				pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Totem_Effect"), pLevelManager->Get_DestinationLevelIndex(), TEXT("Layer_Totem"), &TotemEffectDesc);

				pTotem->Set_IsActive(true);
			}
		}
	}
		
ApplyDamage:
	_float fDmg = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (fDmg > 0)
	{
		m_bUseHitShader = true;

		foreffect		effectdesc;
		ZeroMemory(&effectdesc, sizeof(foreffect));
		effectdesc.dmg = fDmg;
		effectdesc.pos = Get_Position();
		effectdesc.pos.z -= 0.01f;
		//effectdesc.pos.y += 1.25f;
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dmg_pont"), LEVEL_GAMEPLAY, TEXT("Layer_dmgp"), &effectdesc)))
			return OBJ_NOEVENT;

		if (!m_bDead && m_eState != STATE::SPAWN && m_eState != STATE::STUN)
		{
			if (tDamageTypeDesc.eDamageType == DAMAGE_ATTRIBUTE::ATTR_STUN)
			{
				if (m_fStunDamage > m_fStunDamageLimit)
				{
					m_bStun = true;

					m_bIsAttacking = false;
					m_bAggro = true;
					m_vAttackPos = _float3(0.f, 0.f, 0.f);
					m_dwAttackTime = GetTickCount();
					m_iPattern = 0;
					m_fStunDamage = 0.f;
					m_fFollowTime = 0.f;
					iAnimFrameSyncCounter = 0;
				}
				else
					m_fStunDamage += fDamage;
			}
			else
			{
				if (m_fStaggerDamage > m_fStaggerDamageLimit)
				{
					m_bHit = true;

					m_bIsAttacking = false;
					m_bAggro = true;
					m_vAttackPos = _float3(0.f, 0.f, 0.f);
					m_dwAttackTime = GetTickCount();
					m_iPattern = 0;
					m_fStaggerDamage = 0.f;
					m_fSpawnTime = 0.f;
					m_fFollowTime = 0.f;
					iAnimFrameSyncCounter = 0;

					// Play Hit Sound
					_tchar szFileName[MAX_PATH] = TEXT("");
					wsprintf(szFileName, TEXT("hit_boarrior_%02d.wav"), rand() % 14 + 1);
					pGameInstance->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_VOICE, .7f);
				}
				else
					m_fStaggerDamage += fDamage;
			}
		}
		else if (m_bDead)
		{
			// Play Taunt Sound
			_tchar szFileName[MAX_PATH] = TEXT("");
			wsprintf(szFileName, TEXT("taunt_boarrior_%02d.wav"), rand() % 5 + 1);
			CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_VOICE, .7f);
		}
	}

	return fDmg;
}

HRESULT CBoarrior::Drop_Items()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CItem::ITEMDESC ItemDesc;
	ZeroMemory(&ItemDesc, sizeof(CItem::ITEMDESC));

	// Random Position Drop based on Object Position
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * .5f;
	_int bSignZ = rand() % 2;
	_float fPosX = bSignX ? (Get_Position().x + fOffsetX) : (Get_Position().x - fOffsetX);
	_float fPosZ = bSignZ ? (Get_Position().z + fOffsetZ) : (Get_Position().z - fOffsetZ);

	ItemDesc.fPosition = _float3(fPosX, Get_Position().y, fPosZ);
	ItemDesc.pTextureComponent = TEXT("Com_Texture_Spider_Meat");
	ItemDesc.pTexturePrototype = TEXT("Prototype_Component_Texture_Equipment_front");
	ItemDesc.eItemName = ITEMNAME::ITEMNAME_SPIDERMEAT;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Item"), LEVEL_BOSS, TEXT("Layer_Object"), &ItemDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

_bool CBoarrior::IsDead()
{
	if (m_bDead && m_eState == STATE::DIE && GetTickCount() > m_dwDeathTime + 1500)
		return true;
	else if (m_bDead && m_eState != STATE::DIE)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DIE;
	}

	return false;
}

CBoarrior* CBoarrior::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoarrior* pInstance = new CBoarrior(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBoarrior"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoarrior::Clone(void* pArg)
{
	CBoarrior* pInstance = new CBoarrior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBoarrior"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoarrior::Free()
{
	__super::Free();
}