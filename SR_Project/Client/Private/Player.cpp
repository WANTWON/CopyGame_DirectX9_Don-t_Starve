#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "KeyMgr.h"
#include "Inventory.h"
#include "Equip_Animation.h"
#include "Bullet.h"
#include "CameraManager.h"
#include "Interactive_Object.h"
#include "PickingMgr.h"
#include "AttackRange.h"
#include "ParticleSystem.h"
#include "Level_Manager.h"
#include "Skeleton.h"
#include "NPC.h"
#include "DayCycle.h"
#include "Portal.h"

_bool	   g_ColliderRender = false;


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPawn(pGraphic_Device)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CPawn(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(SetUp_KeySettings()))
		return E_FAIL;


	//Test
	Change_Texture(TEXT("Com_Texture_Idle_Down"));
	m_ActStack.push(ACTION_STATE::IDLE);


	m_eObjID = OBJID::OBJ_PLAYER;

	m_iPreLevelIndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();


	return S_OK;
}

int CPlayer::Tick(_float fTimeDelta)
{
	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return OBJ_NOEVENT;

	Check_SanitySpawn(fTimeDelta);

	Setup_LevelChange();

	if (!Decrease_Stat(fTimeDelta))
		return OBJ_NOEVENT;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_PLAYER, this);

	__super::Tick(fTimeDelta);

	//Origin Look Test

	m_pOriginMatrix = m_pTransformCom->Get_WorldMatrix();

	//SkillCoolTime
	Cooltime_Update(fTimeDelta);
	Invincible_Update(fTimeDelta);

	//Act Auto
	Tick_ActStack(fTimeDelta);

	//KeyInput
	GetKeyDown(fTimeDelta);

	//Move
	Move_to_PickingPoint(fTimeDelta);
	WalkingTerrain();

	Play_Sound(fTimeDelta);

	m_Equipment->Set_TargetPos(Get_Pos());
	//TEst
	RangeCheck(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{

	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom/* && !m_bSleeping*/)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	if (m_tStat.fCurrentHealth > m_tStat.fMaxHealth)
	{
		m_tStat.fCurrentHealth = m_tStat.fMaxHealth;
	}

	if (m_tStat.fCurrentHungry > m_tStat.fMaxHungry)
	{
		m_tStat.fCurrentHungry = m_tStat.fMaxHungry;
	}

	if (m_tStat.fCurrentMental > m_tStat.fMaxMental)
	{
		m_tStat.fCurrentMental = m_tStat.fMaxMental;
	}


	Setup_Collider();

	Set_ShaderID();


	Create_Bullet();
}


_float3 CPlayer::Get_Pos()
{
	return (m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

_float3 CPlayer::Get_Look()
{
	return (m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

_float3 CPlayer::Get_Right()
{
	return (m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
}

_bool CPlayer::Get_SkillShow(void)
{
	return m_pPicker->Get_IsShow();
}

CGameObject * CPlayer::Get_VecParty(const _tchar * _NpcName)
{
	auto iter = find_if(m_vecParty.begin(), m_vecParty.end(), [&](auto& MyPair)->bool {

		if (MyPair.first == _NpcName)
			return true;

		return false;
	});
	if (iter == m_vecParty.end())
	{
		return nullptr;
	}

	return iter->second;
}


void CPlayer::Move_to_PickingPoint(_float fTimedelta)
{
	if (m_eState == ACTION_STATE::DEAD)
		return;

	if (m_bPicked == false || m_bArrive == true || m_bInputKey == true)
		return;

	WalkingTerrain();
	m_vPickingPoint.y = m_fTerrain_Height;
	m_pTransformCom->Go_PosTarget(fTimedelta, m_vPickingPoint, _float3(0.f, 0.f, 0.f));

	_float3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_eState = ACTION_STATE::MOVE;

	if (m_ePreState != m_eState
		|| m_ePreDirState != m_eDirState)
	{
		m_eDirState = Check_Direction();

		switch (m_eDirState)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Run_Up"));
			break;
		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Run_Down"));
			break;
		case DIR_LEFT:
			Change_Texture(TEXT("Com_Texture_Run_Side"));
			break;
		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Run_Side"));
			break;
		}
		m_ePreState = m_eState;
		m_ePreDirState = m_eDirState;
	}
	

	/*if (vPlayerPos.x < m_vPickingPoint.x)
	{
		Move_Right(fTimedelta);
	}
	else if (vPlayerPos.x > m_vPickingPoint.x)
	{
		Move_Left(fTimedelta);
	}*/

	if (abs(vPlayerPos.x - m_vPickingPoint.x) < 0.3f &&
		abs(vPlayerPos.z - m_vPickingPoint.z) < 0.3f)
	{
		m_bPicked = false;
		m_bArrive = true;
		m_bInputKey = false;
	}
}

HRESULT CPlayer::Render()
{
	WEAPON_TYPE type = m_eWeaponType;
	m_pTextureCom->MoveFrame(m_TimerTag);

	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex));

	m_pShaderCom->Begin(m_eShaderID);

	if (!m_bSleeping)
	{
		m_pVIBufferCom->Render();
	}
	m_pShaderCom->End();



#ifdef _DEBUG
	if (g_ColliderRender && m_pColliderCom != nullptr)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}
#endif // _DEBUG


	return S_OK;
}

_float CPlayer::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_bInincibleMode || m_eState == CPlayer::ACTION_STATE::TALK)
		return 0.f;

	_float fResultDmg = fDamage - m_tStat.fArmor;
	if (fResultDmg < 0.f)
		fResultDmg = 0.f;

	if ((m_eState != ACTION_STATE::DAMAGED || !m_bGhost) && !m_bHited)
	{
		m_tStat.fCurrentHealth -= fDamage;

	}

	if (Check_Dead() && !m_bGhost)
	{
		m_bDead = true;
		m_bGhost = true;
		Clear_ActStack();

		m_ActStack.push(ACTION_STATE::DEAD);
		m_bAutoMode = true;
		m_bMove = false;
	}
	else if (!m_bGhost && !Check_Dead() && !m_bHited)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		_bool forboss = false;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Screen_Effect"), LEVEL_GAMEPLAY, TEXT("Layer_Screeneffect"), &forboss)))
			return OBJ_NOEVENT;

		m_ActStack.push(ACTION_STATE::DAMAGED);

		m_bMove = false;
		m_bAutoMode = true;
		m_bHited = true;
		Notify_NPC(1);

		Safe_Release(pGameInstance);
	}
	return fDamage;
}

void CPlayer::Set_Position(_float3 Position)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Position);
}

void CPlayer::Set_FPSMode(_bool type)
{

	if (m_bIsFPS == type)
		return;

	if (type)
	{
		m_bIsFPS = true;


		switch (m_eDirState)
		{
		case Client::DIR_DOWN:
			m_pTransformCom->Turn(_float3(0, 1, 0), 2.f);
			break;
		case Client::DIR_RIGHT:
			m_pTransformCom->Turn(_float3(0, 1, 0), 1.f);
			break;
		case Client::DIR_UP:
			break;
		case Client::DIR_LEFT:
			m_pTransformCom->Turn(_float3(0, 1, 0), 1.f);
			m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
			break;
		}
	}
	else
		m_bIsFPS = false;
}

void CPlayer::Release_Party(const _tchar * _Name)
{
	auto iter = find_if(m_vecParty.begin(), m_vecParty.end(), [&](auto& MyPair)->bool {

		if (MyPair.first == _Name)
			return true;

		return false;
	});

	if (iter != m_vecParty.end())
	{
		iter = m_vecParty.erase(iter);
	}

}

_float3 CPlayer::Set_PartyPostion(CNPC * _NPC)
{
	_float3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	D3DXVec3Normalize(&vRight, &vRight);
	_float3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vLook, &vLook);
	_float3 vMyPos = Get_Position();


	for (_uint i = 0; i < m_vecParty.size(); ++i)
	{
		if (m_vecParty[i].second == _NPC)
		{
			switch (i)
			{
			case 0:
				return Get_Position();
				break;
			case 1:
				if (m_eDirState == DIR_STATE::DIR_LEFT || m_eDirState == DIR_STATE::DIR_RIGHT)
				{
					return vMyPos + vLook* -2.f;
				}
				else
				{
					return vMyPos + vRight* -2.f;
				}
				break;
			case 2:
				if (m_eDirState == DIR_STATE::DIR_LEFT || m_eDirState == DIR_STATE::DIR_RIGHT)
				{
					return vMyPos + vLook* 2.f;
				}
				else
				{
					return vMyPos + vRight* 2.f;
				}

				break;
			}
		}
	}
	return Get_Position();
	//iter->second->Set_TargetPos
	//return _float3();
}


HRESULT CPlayer::SetUp_Components()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Player"))))
		return E_FAIL;

	m_TimerTag = TEXT("Timer_Player");

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	/*CCollider_Rect::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Rect::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.4f;
	CollRectDesc.fRadiusX = 0.3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -0.25f;*/

	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.4f;
	CollRectDesc.fRadiusX = 0.3f;
	CollRectDesc.fRadiusZ = 0.3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -0.25f;
	CollRectDesc.fOffsetZ = 0.f;

	/* For.Com_Collider_Rect*/
	/*if (FAILED(__super::Add_Components(TEXT("Com_Collider_Rect"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (Texture_Clone())
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = _float3(40.f, 0.5f, 25.f);

	/*MINIMAP		minidesc;
	ZeroMemory(&minidesc, sizeof(MINIMAP));
	minidesc.name = MIN_PLAYER;
	minidesc.pointer = this;

	LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), CurrentLevelndex, TEXT("MiniMap_Icon"), &minidesc);*/


	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/*For. Others*/
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Equipment"), LEVEL_STATIC, TEXT("Layer_Equip"), nullptr)))
		return E_FAIL;

	_bool bPicker = true;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Picker"), LEVEL_STATIC, TEXT("Layer_Picker"), &bPicker)))
		return E_FAIL;
	bPicker = false;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Picker"), LEVEL_STATIC, TEXT("Layer_Range"), &(bPicker))))
		return E_FAIL;


	Init_Data();

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CPlayer::SetUp_KeySettings()
{
	m_KeySets[INTERACTKEY::KEY_LBUTTON] = VK_LBUTTON;

	m_KeySets[INTERACTKEY::KEY_RBUTTON] = VK_RBUTTON;

	m_KeySets[INTERACTKEY::KEY_UP] = 'W';

	m_KeySets[INTERACTKEY::KEY_RIGHT] = 'D';

	m_KeySets[INTERACTKEY::KEY_DOWN] = 'S';

	m_KeySets[INTERACTKEY::KEY_LEFT] = 'A';

	m_KeySets[INTERACTKEY::KEY_MAP] = VK_TAB;

	m_KeySets[INTERACTKEY::KEY_ATTACK] = 'F';

	m_KeySets[INTERACTKEY::KEY_ACTION] = VK_SPACE;

	m_KeySets[INTERACTKEY::KEY_INVEN1] = '1';

	m_KeySets[INTERACTKEY::KEY_INVEN2] = '2';

	m_KeySets[INTERACTKEY::KEY_INVEN3] = '3';

	m_KeySets[INTERACTKEY::KEY_INVEN4] = '4';

	m_KeySets[INTERACTKEY::KEY_INVEN5] = '5';

	m_KeySets[INTERACTKEY::KEY_INVEN6] = '6';

	m_KeySets[INTERACTKEY::KEY_INVEN7] = '7';

	m_KeySets[INTERACTKEY::KEY_INVEN8] = '8';

	m_KeySets[INTERACTKEY::KEY_INVEN9] = '9';

	m_KeySets[INTERACTKEY::KEY_INVEN0] = '0';

	m_KeySets[INTERACTKEY::KEY_MENU] = VK_ESCAPE;

	m_KeySets[INTERACTKEY::KEY_DEBUG] = VK_OEM_6;

	m_KeySets[INTERACTKEY::KEY_CAMLEFT] = 'Q';

	m_KeySets[INTERACTKEY::KEY_CAMRIGHT] = 'E';

	m_KeySets[INTERACTKEY::KEY_CAMFPSMODE] = 'I';

	m_KeySets[INTERACTKEY::KEY_CAMTPSMODE] = 'O';
	return S_OK;
}

HRESULT CPlayer::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPlayer::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	return S_OK;
}

void CPlayer::Init_Data()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_Equipment = (CEquip_Animation*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Equip"));

	m_pPicker = (CAttackRange*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Picker"));
	m_pPicker->Set_Scale(_float3(0.4f, 0.4f, 1.f));

	m_pRange = (CAttackRange*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Range"));
	//m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));

	m_pRange->Set_IsShow(false);
	m_pPicker->Set_IsShow(false);

	Safe_Release(pGameInstance);


	//InitSkillDesc
	SKILLDESC SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(SkillDesc));
	//Bomb
	SkillDesc.bSkillUsed = false;
	SkillDesc.fAtkRange = 9.f;
	SkillDesc.fAtkScale = 6.3f;
	SkillDesc.fMaxCoolTime = 5.f;
	SkillDesc.fCurrent_CoolTime = 0.f;
	SkillDesc.iCnt = 0;
	m_vecSkillDesc.push_back(SkillDesc);
	//IceSpike
	SkillDesc.bSkillUsed = false;
	SkillDesc.fAtkRange = 6.f;
	SkillDesc.fAtkScale = 5.f;
	SkillDesc.fMaxCoolTime = 3.f;
	SkillDesc.fCurrent_CoolTime = 0.f;
	SkillDesc.iCnt = 0;
	m_vecSkillDesc.push_back(SkillDesc);
	//Mines
	SkillDesc.bSkillUsed = false;
	SkillDesc.fAtkRange = 3.f;
	SkillDesc.fAtkScale = 3.f;
	SkillDesc.fMaxCoolTime = 7.f;
	SkillDesc.fCurrent_CoolTime = 0.f;
	SkillDesc.iCnt = 0;
	m_vecSkillDesc.push_back(SkillDesc);

	//Teleport
	SkillDesc.bSkillUsed = false;
	SkillDesc.fAtkRange = 10.f;
	SkillDesc.fAtkScale = 6.3f;
	SkillDesc.fMaxCoolTime = 1.f;
	SkillDesc.fCurrent_CoolTime = 0.f;
	SkillDesc.iCnt = 0;
	m_vecSkillDesc.push_back(SkillDesc);

	//Test Init
	m_fAtkRange = m_vecSkillDesc[0].fAtkRange;
	m_fAtkScale = m_vecSkillDesc[0].fAtkScale;
}

void CPlayer::GetKeyDown(_float _fTimeDelta)
{
	if (!m_bMove)
		return;

	if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_ACTION]))
	{
		if (!m_bTalkMode)
		{
			Clear_ActStack();
			Multi_Action(_fTimeDelta);
		}
		m_bActivated = true;
	}
	if (m_bTalkMode && m_bSelect)
	{
		if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN1]))
		{
			if (m_bTalkMode && m_bSelect)
			{
				m_bSelect = false;
				m_iTalkNum = 1;
				m_bActivated = true;
			}
		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN2]))
		{
			if (m_bTalkMode && m_bSelect)
			{
				m_bSelect = false;
				m_iTalkNum = 2;
				m_bActivated = true;
			}
		}
	}

	if (m_bOnlyActionKey)
		return;

#pragma region Debug&CamKey
	if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_DEBUG]))
	{
		if (g_ColliderRender)
			g_ColliderRender = false;
		else
			g_ColliderRender = true;
	}
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_CAMFPSMODE]))
	{
		CGameInstance::Get_Instance()->PlaySounds(TEXT("getitem.wav"), SOUND_UI, 0.9f);
		Set_FPSMode(true);
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_FPS);
	}
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_CAMTPSMODE]))
	{
		CGameInstance::Get_Instance()->PlaySounds(TEXT("getitem.wav"), SOUND_UI, 0.9f);
		Set_FPSMode(false);
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
	}
	else if (CKeyMgr::Get_Instance()->Key_Up(m_KeySets[INTERACTKEY::KEY_CAMLEFT]))
	{
		if (m_bIsFPS)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
		else
		{
			CGameInstance::Get_Instance()->PlaySounds(TEXT("getitem.wav"), SOUND_UI, 0.9f);
			CCameraManager::Get_Instance()->PlayerCamera_TurnLeft(m_iCurrentLevelndex);
			Turn_OriginMat(false);
			SetUp_BillBoard();
		}

	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_CAMLEFT]))
	{
		if (m_bIsFPS)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), -_fTimeDelta);

	}
	else if (CKeyMgr::Get_Instance()->Key_Up(m_KeySets[INTERACTKEY::KEY_CAMRIGHT]))
	{
		if (m_bIsFPS)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
		else
		{
			CGameInstance::Get_Instance()->PlaySounds(TEXT("getitem.wav"), SOUND_UI, 0.9f);
			CCameraManager::Get_Instance()->PlayerCamera_TurnRight(m_iCurrentLevelndex);
			Turn_OriginMat(true);
			SetUp_BillBoard();
		}
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_CAMRIGHT]))
	{
		if (m_bIsFPS)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);

	}


#pragma endregion Debug&CamKey	

#pragma region Action
	//Action
	if (!m_bGhost)
	{
		if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN1]))
		{
			if (!m_pPicker->Get_IsShow())
			{

				m_pRange->Set_IsShow(true);
				m_pPicker->Set_IsShow(true);

				if (m_vecSkillDesc[0].iCnt == 0)
				{
					for (auto& iter : m_vecSkillDesc)
					{
						iter.iCnt = 0;
					}
					m_vecSkillDesc[0].iCnt += 1;
					m_fAtkRange = m_vecSkillDesc[0].fAtkRange;
					m_fAtkScale = m_vecSkillDesc[0].fAtkScale;
					m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));
				}
			}
			else
			{
				if (m_vecSkillDesc[0].iCnt == 0)
				{
					for (auto& iter : m_vecSkillDesc)
					{
						iter.iCnt = 0;
					}
					m_vecSkillDesc[0].iCnt += 1;
					m_fAtkRange = m_vecSkillDesc[0].fAtkRange;
					m_fAtkScale = m_vecSkillDesc[0].fAtkScale;
					m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));

				}
				else if (m_vecSkillDesc[0].iCnt == 1
					&& m_vecSkillDesc[0].bSkillUsed == false
					&& m_pPicker->Get_IsCorrect())
				{
					Throw_Bomb(_fTimeDelta);
					m_vecSkillDesc[0].bSkillUsed = true;


					CInventory_Manager* inv = CInventory_Manager::Get_Instance();
					auto i = inv->Get_Skill_list();

					for (auto f : *i)
					{
						if (f->GetiNum() == 0)
							f->setbombon();
					}
					if (m_pPicker->Get_IsShow())
					{
						{
							m_pRange->Set_IsShow(false);
							m_pPicker->Set_IsShow(false);
						}
					}
				}

			}

		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN2]))
		{
			if (!m_pPicker->Get_IsShow())
			{

				m_pRange->Set_IsShow(true);
				m_pPicker->Set_IsShow(true);
				if (m_vecSkillDesc[1].iCnt == 0)
				{
					for (auto& iter : m_vecSkillDesc)
					{
						iter.iCnt = 0;
					}
					m_vecSkillDesc[1].iCnt += 1;
					m_fAtkRange = m_vecSkillDesc[1].fAtkRange;
					m_fAtkScale = m_vecSkillDesc[1].fAtkScale;
					m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));
				}
			}
			else
			{
				if (m_vecSkillDesc[1].iCnt == 0)
				{
					for (auto& iter : m_vecSkillDesc)
					{
						iter.iCnt = 0;
					}
					m_vecSkillDesc[1].iCnt += 1;
					m_fAtkRange = m_vecSkillDesc[1].fAtkRange;
					m_fAtkScale = m_vecSkillDesc[1].fAtkScale;
					m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));
				}

				if (m_vecSkillDesc[1].iCnt == 1
					&& m_vecSkillDesc[1].bSkillUsed == false
					&& m_pPicker->Get_IsCorrect())
				{
					Ice_Spike(_fTimeDelta);
					m_vecSkillDesc[1].bSkillUsed = true;

					CInventory_Manager* inv = CInventory_Manager::Get_Instance();
					auto i = inv->Get_Skill_list();

					for (auto f : *i)
					{
						if (f->GetiNum() == 1)
							f->seticeon();
					}

					if (m_pPicker->Get_IsShow())
					{
						{
							m_pRange->Set_IsShow(false);
							m_pPicker->Set_IsShow(false);
						}
					}
				}
			}


		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN3]))
		{//나중에 얘도 좀 수정필요할듯  낭만없음
			if (/*m_vecSkillDesc[2].iCnt == 1
				&&*/ m_vecSkillDesc[2].bSkillUsed == false
				&& m_pPicker->Get_IsCorrect())
			{
				Sand_Mines(_fTimeDelta);
				m_vecSkillDesc[2].bSkillUsed = true;

				CInventory_Manager* inv = CInventory_Manager::Get_Instance();
				auto i = inv->Get_Skill_list();

				for (auto f : *i)
				{
					if (f->GetiNum() == 2)
						f->setsandon();
				}
				if (m_pPicker->Get_IsShow())
				{
					{
						m_pRange->Set_IsShow(false);
						m_pPicker->Set_IsShow(false);
					}
				}
			}



		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN4]))
		{
			if (!m_pPicker->Get_IsShow())
			{
				m_pRange->Set_IsShow(true);
				m_pPicker->Set_IsShow(true);
				if (m_vecSkillDesc[3].iCnt == 0)
				{
					for (auto& iter : m_vecSkillDesc)
					{
						iter.iCnt = 0;
					}
					m_vecSkillDesc[3].iCnt += 1;
					m_fAtkRange = m_vecSkillDesc[3].fAtkRange;
					m_fAtkScale = m_vecSkillDesc[3].fAtkScale;
					m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));
				}
			}
			else
			{
				if (m_vecSkillDesc[3].iCnt == 0)
				{
					for (auto& iter : m_vecSkillDesc)
					{
						iter.iCnt = 0;
					}
					m_vecSkillDesc[3].iCnt += 1;
					m_fAtkRange = m_vecSkillDesc[3].fAtkRange;
					m_fAtkScale = m_vecSkillDesc[3].fAtkScale;
					m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));
				}

				if (m_vecSkillDesc[3].iCnt == 1
					&& m_vecSkillDesc[3].bSkillUsed == false
					&& m_pPicker->Get_IsCorrect())
				{
					Teleport(_fTimeDelta);
					m_vecSkillDesc[3].bSkillUsed = true;

					CInventory_Manager* inv = CInventory_Manager::Get_Instance();
					auto i = inv->Get_Skill_list();

					for (auto f : *i)
					{
						if (f->GetiNum() == 3)
							f->setteleporton();
					}
					if (m_pPicker->Get_IsShow())
					{
						{
							m_pRange->Set_IsShow(false);
							m_pPicker->Set_IsShow(false);
						}
					}
				}
			}
		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN5]))
		{
			if (m_pPicker->Get_IsShow())
			{
				m_pRange->Set_IsShow(false);
				m_pPicker->Set_IsShow(false);
			}
		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN9]))
		{
			for (auto& iter : m_vecParty)
			{
				iter.second->Set_InvincibleMode(!m_bInincibleMode);
			}
			m_bInincibleMode = !m_bInincibleMode;
		}
	}

#pragma endregion Action

#pragma region Move
	//Move
	if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_UP]) && !m_bSleeping)
	{
		m_bInputKey = true;
		m_bPicked = false;

		Clear_ActStack();
		Move_Up(_fTimeDelta);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_RIGHT]) && !m_bSleeping)
	{
		m_bInputKey = true;
		m_bPicked = false;
		Clear_ActStack();
		Move_Right(_fTimeDelta);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_DOWN]) && !m_bSleeping)
	{
		m_bInputKey = true;
		m_bPicked = false;

		Clear_ActStack();
		Move_Down(_fTimeDelta);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_LEFT]) && !m_bSleeping)
	{
		m_bInputKey = true;
		m_bPicked = false;

		Clear_ActStack();
		Move_Left(_fTimeDelta);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_ATTACK]) && !m_bSleeping)
	{
		if (!m_bGhost)
		{
			m_bInputKey = true;
			m_bPicked = false;

			Clear_ActStack();
			Attack(_fTimeDelta);
		}


	}
	else
	{
		if (!m_bPicked && m_ActStack.top() == ACTION_STATE::IDLE)
		{
			Move_Idle(_fTimeDelta);
		}
	}
#pragma endregion Move
}

bool CPlayer::ResetAction(_float _fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CPlayer::ACTION_STATE::IDLE:
	case Client::CPlayer::ACTION_STATE::SLEEP:
	case Client::CPlayer::ACTION_STATE::COOK:
	case Client::CPlayer::ACTION_STATE::TALK:
	case Client::CPlayer::ACTION_STATE::MOVE:
		break;
	case Client::CPlayer::ACTION_STATE::ATTACK:
	case Client::CPlayer::ACTION_STATE::MINING:
	case Client::CPlayer::ACTION_STATE::CHOP:
	case Client::CPlayer::ACTION_STATE::WEEDING:
	case Client::CPlayer::ACTION_STATE::EAT:
	case Client::CPlayer::ACTION_STATE::PICKUP:
	case Client::CPlayer::ACTION_STATE::DAMAGED:
	case Client::CPlayer::ACTION_STATE::PORTAL:
	case Client::CPlayer::ACTION_STATE::DEAD:
	case Client::CPlayer::ACTION_STATE::REVIVE:
	case Client::CPlayer::ACTION_STATE::BUILD:
	case Client::CPlayer::ACTION_STATE::ANGRY:

		if (m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	case Client::CPlayer::ACTION_STATE::ACTION_END:
		break;
	default:
		break;
	}

	return true;
}

void CPlayer::Move_Idle(_float _fTimeDelta)
{

	if (!ResetAction(_fTimeDelta))
		return;

	//if(m_eState == ACTION_STATE::ATTACK || m_eState == ACTION_STATE::AXE || m_eState == ACTION_STATE:: MINING)
	m_eState = ACTION_STATE::IDLE;
	m_Equipment->Set_ActionState((_uint)ACTION_STATE::IDLE);
	if (m_ePreState != m_eState)
	{
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::IDLE);
		if (!m_bGhost)
		{
			switch (m_eDirState)
			{
			case DIR_STATE::DIR_DOWN:
				Change_Texture(TEXT("Com_Texture_Idle_Down"));
				break;
			case DIR_STATE::DIR_UP:
				Change_Texture(TEXT("Com_Texture_Idle_Up"));
				break;
			case DIR_STATE::DIR_LEFT:
			case DIR_STATE::DIR_RIGHT:
				Change_Texture(TEXT("Com_Texture_Idle_Side"));
				break;
			}

		}
		m_ePreState = m_eState;
	}
}

void CPlayer::Move_Up(_float _fTimeDelta)
{
	if (m_bInputKey)
	{
		if (m_iCameraMode == CCameraManager::CAM_PLAYER)
			m_pTransformCom->Go_Straight(_fTimeDelta * 1.5, m_fTerrain_Height);
		else
			m_pTransformCom->Go_Straight(_fTimeDelta, m_fTerrain_Height);
	}


	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_UP;
	if (m_ePreState != m_eState
		|| m_ePreDirState != m_eDirState)
	{
		if (!m_bGhost)
		{
			Change_Texture(TEXT("Com_Texture_Run_Up"));
		}

		m_ePreState = m_eState;
		m_ePreDirState = m_eDirState;

		m_Equipment->Set_Directon(m_eDirState);
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::MOVE);
	}
}

void CPlayer::Move_Right(_float _fTimeDelta)
{
	if (m_bInputKey)
	{
		if (m_bIsFPS)
			m_pTransformCom->Go_Right(_fTimeDelta, m_fTerrain_Height);
		else
			m_pTransformCom->Go_Right(_fTimeDelta, m_fTerrain_Height);
	}

	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_RIGHT;
	if (m_ePreState != m_eState || m_ePreDirState != m_eDirState)
	{
		if (m_ePreDirState != m_eDirState)
		{
			if (!m_bIsFPS)
				m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
			//m_bInverseScale = false;
		}
		if (!m_bGhost)
		{
			Change_Texture(TEXT("Com_Texture_Run_Side"));
		}


		m_ePreState = m_eState;
		m_ePreDirState = m_eDirState;

		m_Equipment->Set_Directon(m_eDirState);
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::MOVE);
	}
}

void CPlayer::Move_Down(_float _fTimeDelta)
{
	if (m_bInputKey)
	{
		if (m_iCameraMode == CCameraManager::CAM_PLAYER)
			m_pTransformCom->Go_Backward(_fTimeDelta * 1.5, m_fTerrain_Height);
		else
			m_pTransformCom->Go_Backward(_fTimeDelta, m_fTerrain_Height);
	}

	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_DOWN;

	if (m_ePreState != m_eState || m_ePreDirState != m_eDirState)
	{
		if (!m_bGhost)
		{
			Change_Texture(TEXT("Com_Texture_Run_Down"));
		}


		m_ePreState = m_eState;
		m_ePreDirState = m_eDirState;

		m_Equipment->Set_Directon(m_eDirState);
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::MOVE);
	}
}

void CPlayer::Move_Left(_float _fTimeDelta)
{
	if (m_bInputKey)
	{
		if (m_bIsFPS)
			m_pTransformCom->Go_Left(_fTimeDelta, m_fTerrain_Height);
		else
		{
			//m_pTransformCom->Go_PosTarget(_fTimeDelta * .1f, _float3(Get_Position().x-3.f, Get_Position().y, Get_Position().z), _float3{ 0.f, 0.f, 0.f });
			m_pTransformCom->Go_Right(_fTimeDelta, m_fTerrain_Height);
		}

	}
	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_LEFT;

	if (m_ePreState != m_eState || m_ePreDirState != m_eDirState)
	{

		if (m_ePreDirState != m_eDirState)
		{
			if (!m_bIsFPS)
				m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
			//m_bInverseScale = false;
		}
		/*if (!m_bInverseScale)
		{
		m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
		m_bInverseScale = true;
		}*/
		if (!m_bGhost)
		{
			Change_Texture(TEXT("Com_Texture_Run_Side"));
		}
		m_ePreState = m_eState;
		m_ePreDirState = m_eDirState;

		m_Equipment->Set_Directon(m_eDirState);
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::MOVE);
	}
}

void CPlayer::Attack(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::ATTACK;

	if (m_eState != m_ePreState)
	{
		m_bSoundEnd = false;
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::ATTACK);
		m_Equipment->Set_WeaponType(m_eWeaponType);
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			switch (m_eWeaponType)
			{
			case WEAPON_TYPE::WEAPON_HAND:
				Change_Texture(TEXT("Com_Texture_Punch_Down"));
				break;
			case WEAPON_TYPE::WEAPON_SWORD:
				Change_Texture(TEXT("Com_Texture_Weapon_Down"));
				break;
			case WEAPON_TYPE::WEAPON_STAFF:
				Change_Texture(TEXT("Com_Texture_Weapon_Down"));
				break;
			case WEAPON_TYPE::WEAPON_DART:
				Change_Texture(TEXT("Com_Texture_Dart_Down"));
				break;
			}
			break;
		case DIR_STATE::DIR_UP:
			switch (m_eWeaponType)
			{
			case WEAPON_TYPE::WEAPON_HAND:
				Change_Texture(TEXT("Com_Texture_Punch_Up"));
				break;
			case WEAPON_TYPE::WEAPON_SWORD:
				Change_Texture(TEXT("Com_Texture_Weapon_Up"));
				break;
			case WEAPON_TYPE::WEAPON_STAFF:
				Change_Texture(TEXT("Com_Texture_Weapon_Up"));
				break;
			case WEAPON_TYPE::WEAPON_DART:
				Change_Texture(TEXT("Com_Texture_Dart_Up"));
				break;
			}
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			switch (m_eWeaponType)
			{
			case WEAPON_TYPE::WEAPON_HAND:
				Change_Texture(TEXT("Com_Texture_Punch_Side"));
				break;
			case WEAPON_TYPE::WEAPON_SWORD:
				Change_Texture(TEXT("Com_Texture_Weapon_Side"));
				break;
			case WEAPON_TYPE::WEAPON_STAFF:
				Change_Texture(TEXT("Com_Texture_Weapon_Side"));
				break;
			case WEAPON_TYPE::WEAPON_DART:
				Change_Texture(TEXT("Com_Texture_Dart_Side"));
				break;
			}
			break;
		}
		m_ePreState = m_eState;

		Notify_NPC(1);
	}



}

void CPlayer::Mining(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::MINING;

	if (m_ePreState != m_eState)
	{
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Mining_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Mining_Up"));
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Mining_Side"));
			break;
		}
		m_ePreState = m_eState;
	}
	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 24)
	{
		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact(10);


		CParticleSystem::STATEDESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATEDESC));
		ParticleDesc.eType = CParticleSystem::PARTICLE_ROCK;
		ParticleDesc.eTextureScene = m_iCurrentLevelndex;
		ParticleDesc.pTextureKey = TEXT("Prototype_Component_Texture_Rock");
		ParticleDesc.dDuration = 0.4; //  ƼŬ  ð 
		ParticleDesc.dParticleLifeTime = 0.4; //    
		ParticleDesc.dSpawnTime = 1; //     Ÿ  
		ParticleDesc.fParticlePerSecond = 75;
		ParticleDesc.fVelocityDeviation = 1.f;
		ParticleDesc.iMaxParticleCount = 7;
		ParticleDesc.vParticleScale = _float2(0.5, 0.5);
		ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
		ParticleDesc.iTextureNum = 1;
		ParticleDesc.vVelocity = _float3((rand() % 10)*0.1f, (rand() % 10) * 0.1f, rand() % 10 * 0.1f);
		ParticleDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		ParticleDesc.vPosition.z += (_float)0.1;

		if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("GameObject_ParticleSystem"), ParticleDesc.eTextureScene, TEXT("Layer_Particle"), &ParticleDesc)))
			return;
	}
}

void CPlayer::Chop(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::CHOP;

	if (m_ePreState != m_eState)
	{
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Chop_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Chop_Up"));
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Chop_Side"));
			break;
		}
		m_ePreState = m_eState;
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 28)
	{
		// Play Sound
		_tchar szFileName[MAX_PATH] = TEXT("");
		wsprintf(szFileName, TEXT("chop_tree_%d.wav"), rand() % 3 + 1);
		CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_OBJECT, .7f);

		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact(20);

		CParticleSystem::STATEDESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticleSystem::STATEDESC));
		ParticleDesc.eType = CParticleSystem::PARTICLE_LEAF;
		ParticleDesc.eTextureScene = m_iCurrentLevelndex;
		ParticleDesc.pTextureKey = TEXT("Prototype_Component_Texture_Leaf");
		ParticleDesc.dDuration = 1; //  ƼŬ  ð 
		ParticleDesc.dParticleLifeTime = 1; //    
		ParticleDesc.dSpawnTime = 1; //     Ÿ  
		ParticleDesc.fParticlePerSecond = 75;
		ParticleDesc.fVelocityDeviation = 1.f;
		ParticleDesc.iMaxParticleCount = 5;
		ParticleDesc.vParticleScale = _float2(0.5, 0.5);
		ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
		ParticleDesc.iTextureNum = rand() % 8;
		ParticleDesc.vVelocity = _float3(0.01f, -0.5, 0.f);
		ParticleDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		ParticleDesc.vPosition.z -= 0.001f;
		ParticleDesc.vPosition.y += 1;

		if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("GameObject_ParticleSystem"), ParticleDesc.eTextureScene, TEXT("Layer_Particle"), &ParticleDesc)))
			return;
	}
}

void CPlayer::Cutting_Grass(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::WEEDING;

	m_Equipment->Set_ActionState((_uint)3);
	m_Equipment->Set_WeaponType(m_eWeaponType);

	if (m_ePreState != m_eState)
	{
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Build_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Build_Up"));
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Build_Side"));
			break;
		}
		m_ePreState = m_eState;
	}
	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 28)
	{
		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact(20);
	}
}

void CPlayer::Eatting(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::EAT;

	if (m_ePreState != m_eState)
	{
		m_bSoundEnd = false;
		m_bMove = false;
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
		case DIR_STATE::DIR_UP:
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Eat"));
			break;
		}
		m_ePreState = m_eState;
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bMove = true;
	}
}

void CPlayer::Pickup(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::PICKUP;

	if (m_ePreState != m_eState)
	{
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Pickup_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Pickup_Up"));
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Pickup_Side"));
			break;
		}
		m_ePreState = m_eState;
	}
	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 2)
	{
		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact();
	}
}

void CPlayer::Damaged(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::DAMAGED;

	if (m_ePreState != m_eState)
	{
		m_bMove = false;
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Damaged_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Damaged_Up"));
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Damaged_Side"));
			break;
		}
		m_ePreState = m_eState;
		m_bSoundEnd = false;
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 2)
	{
		m_bMove = true;
	}
}

void CPlayer::Jump(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::PORTAL;
	m_fReviveTime += _fTimeDelta;
	//m_bMove = false;
	if (m_ePreState != m_eState)
	{
		m_bSoundEnd = false;
		m_bAutoMode = true;
		m_bMove = false;
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Jump_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Jump_Up"));
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Jump_Side"));
			break;
		}
		m_ePreState = m_eState;

		_float3 vPos = Get_Pos();
		vPos.z += 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex == 38)
	{
		m_pTextureCom->Get_Frame().m_iCurrentTex = 37;
		if (!Check_Interact_End())
		{
			dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact();
		}

		//m_bInPortal = true;
		m_bMove = false;
	}

	CPortal* pBossPortal = dynamic_cast<CPortal*>(m_pTarget);
	if (pBossPortal && pBossPortal->Get_PortalDesc().m_eType == CPortal::PORTALTYPE::PORTAL_BOSS)
	{
		if (m_pTextureCom->Get_Frame().m_iCurrentTex < 20)
		{
			_float3 vPos = Get_Pos();
			vPos.y += 2.f * _fTimeDelta;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		}
	}
}

void CPlayer::Dead(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::DEAD;

	if (m_ePreState != m_eState)
	{
		m_bSoundEnd = false;
		m_bMove = false;
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
		case DIR_STATE::DIR_UP:
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Dead"));
			break;
		}
		m_ePreState = m_eState;
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		Change_Texture(TEXT("Com_Texture_Ghost_Idle"));

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skeleton"), m_iCurrentLevelndex, TEXT("Layer_Object"), vPos)))
			return;

		CInventory_Manager* inv = CInventory_Manager::Get_Instance();

		m_tStat.fCurrentHealth = 1.f;
		m_tStat.fCurrentHungry = 1.f;
		m_tStat.fCurrentMental = 1.f;

		inv->Dead_on();
		m_bMove = true;
	}
}

void CPlayer::Revive(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::REVIVE;

	m_fReviveTime += _fTimeDelta;
	if (m_ePreState != m_eState)
	{
		CInventory_Manager* inv = CInventory_Manager::Get_Instance();
		inv->Dead_off();

		m_bSoundEnd = false;
		m_bAutoMode = true;
		m_bMove = false;
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
		case DIR_STATE::DIR_UP:
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Revive"));
			break;
		}
		m_ePreState = m_eState;

		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
		CCamera* pCamera = CCameraManager::Get_Instance()->Get_CurrentCamera();
		dynamic_cast<CCameraDynamic*>(pCamera)->Set_CamMode(CCameraDynamic::CAM_REVIVE);
	}

	if (m_fReviveTime > 1.5f &&m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{

		Change_Texture(TEXT("Com_Texture_Idle_Down"));
		m_bDead = false;
		m_bGhost = false;
		m_bMove = true;
		//m_bAutoMode = false;
		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact(10);
		m_fReviveTime = 0.f;

		m_tStat.fCurrentHealth = m_tStat.fMaxHealth;
		m_tStat.fCurrentHungry = m_tStat.fMaxHungry;
		m_tStat.fCurrentMental = m_tStat.fMaxMental;
	}
	else if (m_fReviveTime < 1.5f && m_pTextureCom->Get_Frame().m_iCurrentTex == 15)
	{
		m_pTextureCom->Get_Frame().m_iCurrentTex = 9;
	}
}

void CPlayer::Building(_float _fTImeDelta)
{
	m_eState = ACTION_STATE::BUILD;

	m_bIsBuild = false;
	m_bBuildTrigger = true;

	m_fBuildTime += _fTImeDelta;

	m_Equipment->Set_ActionState((_uint)3);
	m_Equipment->Set_WeaponType(m_eWeaponType);

	if (m_ePreState != m_eState)
	{
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Build_Down"));
			break;
		case DIR_STATE::DIR_UP:
			Change_Texture(TEXT("Com_Texture_Build_Up"));
			break;
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Build_Side"));
			break;
		}
		m_ePreState = m_eState;
	}
	if (m_fBuildTime > 1.f/* &&m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex -2*/)
	{
		m_bIsBuild = true;
		m_bBuildTrigger = false;
		m_fBuildTime = 0.f;
	}
	else if (m_fBuildTime < 1.f &&m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 2)
	{

		m_pTextureCom->Get_Frame().m_iCurrentTex = 0;
	}

}

void CPlayer::Angry(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::ANGRY;

	if (m_ePreState != m_eState)
	{
		m_bSoundEnd = false;
		m_bMove = false;
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_DOWN:
		case DIR_STATE::DIR_UP:
		case DIR_STATE::DIR_LEFT:
		case DIR_STATE::DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Angry"));
			break;
		}
		m_ePreState = m_eState;

	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bMove = true;
	}
}

void CPlayer::Multi_Action(_float _fTimeDelta)
{
	//Only Objects
	Find_Priority();

	if (m_pTarget != nullptr)
	{
		
		_float3 vPickPos = m_pTarget->Get_Position();
		_float3 vOffSet = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		D3DXVec3Normalize(&vOffSet, &vOffSet);
		vPickPos -= vOffSet*0.5f;
		Set_PickingPoint(vPickPos);
		m_bAutoMode = true;
		m_ActStack.push(ACTION_STATE::MOVE);
	}
	else {
		m_bAutoMode = true;
		m_ActStack.push(ACTION_STATE::ANGRY);
		//Clear_ActStack();
	}


}

void CPlayer::Throw_Bomb(_float _fTimeDelta)
{
	if (m_pPicker->Get_IsCorrect())
	{
		/*Test Bomb*/
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		BulletData.bIsPlayerBullet = true;
		BulletData.eDirState = DIR_STATE::DIR_DOWN;
		BulletData.eWeaponType = WEAPON_TYPE::WEAPON_BOMB;
		BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_UP);
		BulletData.vPosition = Get_Pos();

		_float3 temp = { m_vTargetPicking.x - Get_Pos().x, 0.f, m_vTargetPicking.z - Get_Pos().z };
		//D3DXVec3Normalize(&temp, &temp);
		//BulletData.fAdd_X = m_fMaxTime;
		BulletData.vTargetPos = temp;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
			return;
	}
}

void CPlayer::Ice_Spike(_float _fTimeDelta)
{
	if (m_pPicker->Get_IsCorrect())
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		BulletData.bIsPlayerBullet = true;

		BulletData.eWeaponType = WEAPON_TYPE::WEAPON_ICESPIKE4;

		BulletData.eDirState = DIR_STATE::DIR_END;

		_float3 vLookTemp = m_vTargetPicking - Get_Pos();

		BulletData.vLook = vLookTemp; // m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		BulletData.vPosition = m_vTargetPicking;

	/*	CParticle::STATEDESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticle::STATEDESC));
		ParticleDesc.eTextureScene = m_iCurrentLevelndex;
		ParticleDesc.pTextureKey = TEXT("Prototype_Component_Texture_Snow");
		ParticleDesc.iTextureNum = rand() % 4;
		ParticleDesc.vVelocity = _float3((rand() % 10)*0.1f, -0.1f, -rand() % 10 * 0.1f);

		for (int i = 0; i < 200; ++i)
		{
			if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("GameObject_Particle"), ParticleDesc.eTextureScene, TEXT("Layer_Particle"), &ParticleDesc)))
				return;
		}*/

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
			return;
	}
}

void CPlayer::Sand_Mines(_float _fTimeDelta)
{
	/*Teste Mines*/
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	BULLETDATA BulletData;
	ZeroMemory(&BulletData, sizeof(BulletData));
	BulletData.bIsPlayerBullet = true;

	BulletData.eWeaponType = WEAPON_TYPE::WEAPON_MINES;
	BulletData.vLook = *(_float3*)&m_pOriginMatrix.m[2][0];//m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//_float3 vTempPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//BulletData.vPosition.y -= 0.5f;

	_float3 vRight = *(_float3*)&m_pOriginMatrix.m[0][0];
	BulletData.vRight = *(_float3*)&m_pOriginMatrix.m[0][0];// m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	D3DXVec3Normalize(&vRight, &vRight);
	if (m_bIsFPS)
	{
		BulletData.eDirState = DIR_STATE::DIR_END;
		vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
			return;

		BulletData.vPosition += vRight *1.f;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
			return;

		BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		BulletData.vPosition -= vRight *1.f;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
			return;
	}
	else
	{
		BulletData.eDirState = m_eDirState;
		switch (m_eDirState)
		{
		case DIR_STATE::DIR_UP:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition += vRight *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			BulletData.vPosition -= vRight *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;
			break;
		case DIR_STATE::DIR_DOWN:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition += vRight *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			BulletData.vPosition -= vRight *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;
			break;
		case DIR_STATE::DIR_LEFT:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition += BulletData.vLook *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			BulletData.vPosition -= BulletData.vLook *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;
			break;
		case DIR_STATE::DIR_RIGHT:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition += BulletData.vLook *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;

			BulletData.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			BulletData.vPosition -= BulletData.vLook *1.f;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
				return;
			break;

		}
	}
}

void CPlayer::Teleport(_float _fTimeDelta)
{
	if (m_pPicker->Get_IsCorrect())
	{
		/*Test Bomb*/
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		BulletData.bIsPlayerBullet = true;
		BulletData.eDirState = DIR_STATE::DIR_DOWN;
		BulletData.eWeaponType = WEAPON_TYPE::WEAPON_PUFF;
		BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_UP);
		BulletData.vPosition = Get_Pos();

		_float3 temp = { m_vTargetPicking.x - Get_Pos().x, 0.f, m_vTargetPicking.z - Get_Pos().z };

		BulletData.vTargetPos = temp;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
			return;
		temp = { m_vTargetPicking.x, m_vTargetPicking.y + 0.5f, m_vTargetPicking.z };

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);

		SetUp_BillBoard();

		_tchar szFileName[MAX_PATH] = TEXT("");
		wsprintf(szFileName, TEXT("flash_%d.wav"), rand() % 10 + 1);
		CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_PLAYER, 1.f);
	}
}

_bool CPlayer::Decrease_Stat(_float _fTimeDelta)
{
	if (m_bDead)
		return true;


	m_fMentalitytime += _fTimeDelta;
	m_fHungertime += _fTimeDelta;

	if ((LEVEL)(m_iCurrentLevelndex) != LEVEL_BOSS || (LEVEL)(m_iCurrentLevelndex) != LEVEL_MAZE
		|| m_eState != ACTION_STATE::SLEEP)
	{
		if (CInventory_Manager::Get_Instance()->Get_Daycountpont_list()->front()->Get_nightandday() == DAY_DINNER && m_fMentalitytime > 1.f)
		{
			m_tStat.fCurrentMental -= 0.5f;
			if (m_tStat.fCurrentMental <= 0.f)
				goto GoDead;
			m_fMentalitytime = 0.f;
		}
		else if (CInventory_Manager::Get_Instance()->Get_Daycountpont_list()->front()->Get_nightandday() == DAY_NIGHT)
		{
			if (m_eWeaponType != WEAPON_LIGHT)
				m_fMentalitytime2 += _fTimeDelta;
			else
				m_fMentalitytime2 = 0.f;

			if (m_fMentalitytime2 > 5.f)
			{
				if (m_tStat.fCurrentMental <= 5.f)
				{
					m_tStat.fCurrentMental = 0.f;
					goto GoDead;
				}
				m_tStat.fCurrentMental -= 5.f;
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				_bool forboss = false;

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Screen_Effect"), LEVEL_GAMEPLAY, TEXT("Layer_Screeneffect"), &forboss)))
					return false;
				m_fMentalitytime2 = 0.f;
			}
		}
	}

	if (m_fHungertime > 5.f)
	{
		--m_tStat.fCurrentHungry;
		if (m_tStat.fCurrentHungry <= 0.f)
			goto GoDead;
		m_fHungertime = 0.f;
	}

	return true;

	GoDead:
	if (!m_bGhost)
	{
		m_bDead = true;
		m_bGhost = true;
		Clear_ActStack();

		m_ActStack.push(ACTION_STATE::DEAD);
		m_bAutoMode = true;
		m_bMove = false;
	}
	return true;
}

void CPlayer::Create_Bullet()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();



	if (m_eState == ACTION_STATE::ATTACK)
	{
		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		if (m_bIsFPS)
		{
			BulletData.eDirState = DIR_STATE::DIR_END;
			BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		}
		else
		{
			BulletData.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			BulletData.eDirState = m_eDirState;
		}
		BulletData.bIsPlayerBullet = true;
		BulletData.eWeaponType = m_eWeaponType;
		BulletData.vPosition = Get_Pos();

		switch (m_eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_HAND:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 10)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		case WEAPON_TYPE::WEAPON_SWORD:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 5)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		case WEAPON_TYPE::WEAPON_STAFF:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 20)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		case WEAPON_TYPE::WEAPON_DART:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 17)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), m_iCurrentLevelndex, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		}
	}
}

void CPlayer::Detect_Enemy(void)
{

}

void CPlayer::Find_Priority()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Object"));

	if (list_Obj == nullptr)
		return;

	_uint iIndex = 0;

	if (Find_NPC())
	{
		//m_ActStack.push(ACTION_STATE::TALK);
		return;
	}
	

	m_pTarget = nullptr;
	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if (m_bGhost)
		{
			if (*iter_Obj != nullptr &&dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::SKELETON
				&& dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_CanInteract())
			{
				m_pTarget = *iter_Obj;
				return;
			}
			else
			{
				goto GoNext;
			}
		}
		else
		{
			if ((*iter_Obj) == nullptr || !dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_CanInteract()
				|| dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::SKELETON)
			{
				goto GoNext;
			}

			if (m_eWeaponType == WEAPON_AXE)
			{
				if (dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::BOULDER)
				{
					goto GoNext;
				}
			}
			else if (m_eWeaponType == WEAPON_PICKAXE)
			{
				if (dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::TREE)
				{
					goto GoNext;
				}
			}
			else {
				if (dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::BOULDER
					|| dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::TREE)
				{
					goto GoNext;
				}
			}

			_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
				+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

			if (fCmpDir > m_fActRadius)
			{
				goto GoNext;
			}

			if (m_pTarget == nullptr)
			{
				m_pTarget = *iter_Obj;
			}

			_float fTargetDir = (Get_Pos().x - (m_pTarget)->Get_Position().x)*(Get_Pos().x - (m_pTarget)->Get_Position().x)
				+ (Get_Pos().y - (m_pTarget)->Get_Position().y)*(Get_Pos().y - (m_pTarget)->Get_Position().y)
				+ (Get_Pos().z - (m_pTarget)->Get_Position().z)*(Get_Pos().z - (m_pTarget)->Get_Position().z);

			if (dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::PORTAL
				&& fCmpDir >= 3.f)
			{
				goto GoNext;
			}

			if (fCmpDir < fTargetDir)
			{
				m_pTarget = *iter_Obj;
			}
		}
	GoNext:
		++iIndex;
		iter_Obj++;
		continue;
	}

}

_bool CPlayer::Find_NPC()
{//Test
	if (m_bGhost || (LEVEL)m_iCurrentLevelndex != LEVEL_GAMEPLAY || m_eState == ACTION_STATE::SLEEP)
		return false;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_uint iIndex = 0;
	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(LEVEL_STATIC, TEXT("Layer_NPC"));
	if (list_Obj == nullptr)
		return false;

	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{


		if ((*iter_Obj) == nullptr || !static_cast<CNPC*>(*iter_Obj)->Get_CanTalk())
		{
			++iIndex;
			iter_Obj++;
			continue;
		}


		_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
			
			+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

		if (fCmpDir >= 2.f)
		{
			++iIndex;
			iter_Obj++;
			continue;
		}

		if (m_pTarget == nullptr)
		{
			m_pTarget = *iter_Obj;
		}

		_float fTargetDir = (Get_Pos().x - (m_pTarget)->Get_Position().x)*(Get_Pos().x - (m_pTarget)->Get_Position().x)
			+ (Get_Pos().y - (m_pTarget)->Get_Position().y)*(Get_Pos().y - (m_pTarget)->Get_Position().y)
			+ (Get_Pos().z - (m_pTarget)->Get_Position().z)*(Get_Pos().z - (m_pTarget)->Get_Position().z);

		if (fCmpDir < fTargetDir)
		{
			m_pTarget = *iter_Obj;
		}

		++iIndex;
		iter_Obj++;
	}

	if (m_pTarget == nullptr)
		return false;

	return true;
}

_bool CPlayer::Check_Dead()
{
	if (m_tStat.fCurrentHealth <= 0.f)
	{
		m_tStat.fCurrentHealth = m_tStat.fMaxHealth;
		return true;
	}
	else
	{
		return false;
	}


}

void CPlayer::Setup_Collider(void)
{
	memcpy(*(_float3*)&m_CollisionMatrix.m[3][0], (m_pTransformCom->Get_State(CTransform::STATE_POSITION)), sizeof(_float3));
	m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	//Block Collision
	_float3 vDistance = _float3(0, 0, 0);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Collision_with_Group(CCollider_Manager::COLLISION_BLOCK, this, CCollider_Manager::COLLSIION_BOX, &vDistance))
	{
		_float3 vPosition = Get_Position();

		if (fabsf(vDistance.x) < fabsf(vDistance.z))
			vPosition.x -= vDistance.x;
		else
			vPosition.z -= vDistance.z;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

}

void CPlayer::Notify_NPC(_uint _iNum)
{
	for (auto& iter : m_vecParty)
	{
		iter.second->Make_Interrupt(this, _iNum);
	}

}

void CPlayer::Setup_LevelChange(void)
{
	if (m_iPreLevelIndex != m_iCurrentLevelndex)
	{
		Clear_ActStack();
		m_iPreLevelIndex = m_iCurrentLevelndex;
		Change_Texture(TEXT("Com_Texture_Idle_Side"));
		m_bInPortal = false;
		m_bMove = true;


		MINIMAP		minidesc;
		ZeroMemory(&minidesc, sizeof(MINIMAP));
		minidesc.name = MIN_PLAYER;
		minidesc.pointer = this;

		LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), CurrentLevelndex, TEXT("MiniMap_Icon"), &minidesc);
	}
	m_iCameraMode = CCameraManager::Get_Instance()->Get_CamState();
}

void CPlayer::Check_SanitySpawn(_float fTimeDelta)
{
	DAY_STATE eDayState = CDayCycle::Get_Instance()->Get_DayState();
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();

	if (eDayState != DAY_STATE::DAY_NIGHT || pLevelManager->Get_CurrentLevelIndex() != LEVEL_GAMEPLAY)
		return;

	_uint fPercent = 70; // Modify this to change the BadSanity percent limit.
	_float fBadSanity = m_tStat.fMaxMental / 100 * fPercent;

	if (m_tStat.fCurrentMental < fBadSanity)
	{
		// Spawn Sanity Monster every 20 seconds.
		if (m_fSanitySpawnTimer > 20.f)
		{
			// Play Sound
			_tchar szFileName[MAX_PATH] = TEXT("");
			wsprintf(szFileName, TEXT("terrorbeak_hit_%03d.wav"), rand() % 6);
			CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_MONSTER_VOICE, .8f);

			_uint iSpawns = 4; // Number of Sanity Monster to spawn.
			for (int i = 0; i < iSpawns; ++i)
			{
				_float fSpawnRadius = 4.f;

				// Calculate Random Position
				_float fOffsetX = ((_float)rand() / (_float)(RAND_MAX)) * fSpawnRadius;
				_int bSignX = rand() % 2;
				_float fOffsetZ = ((_float)rand() / (_float)(RAND_MAX)) * fSpawnRadius;
				_int bSignZ = rand() % 2;
				_float fSpawnPositionX = Get_Position().x + (bSignX ? fOffsetX : -fOffsetX);
				_float fSpawnPositionZ = Get_Position().z + (bSignZ ? fOffsetZ : -fOffsetZ);

				_float3 fSpawnPosition = _float3(fSpawnPositionX, 1.f, fSpawnPositionZ);

				CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_Terrorbeak"), pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), fSpawnPosition);
			}

			m_fSanitySpawnTimer = 0.f;
		}
		else
			m_fSanitySpawnTimer += fTimeDelta;
	}
}

void CPlayer::Sleep_Restore(_float _fTimeDelta)
{
	m_eState = CPlayer::ACTION_STATE::SLEEP;

	if (m_bSleeping)
	{
		m_fSleepTime += _fTimeDelta;
		if (1.f < m_fSleepTime)
		{
			if (m_tStat.fCurrentHealth < m_tStat.fMaxHealth)
				m_tStat.fCurrentHealth += 1;

			if (m_tStat.fCurrentMental < m_tStat.fMaxMental)
				m_tStat.fCurrentMental += 1;

			if (m_tStat.fCurrentHungry > 0)
				m_tStat.fCurrentHungry -= 1;

			m_fSleepTime = 0.f;
			++iCnt;
			if (iCnt > 3)
			{
				iCnt = 0;
				m_bSoundEnd = false;
			}
		}
	}

	if (m_bActivated)
	{
		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact();
		m_bActivated = false;
	}

}

void CPlayer::Talk_NPC(_float _fTimeDelta)
{
	m_eState = CPlayer::ACTION_STATE::TALK;
	//움직임 봉쇄 코드 넣기.
	if (m_bActivated)
	{
		m_bSoundEnd = false;
		//m_bTalkMode = true;
		if (dynamic_cast<CNPC*>(m_pTarget)->Get_TalkCnt() == 2)
		{
			m_bSelect = true;
		}
		else
		{
			m_bSelect = false;
			m_iTalkNum = 0;
		}
		m_bTalkMode = true;
		m_bActivated = false;
		m_bOnlyActionKey = true;

		dynamic_cast<CNPC*>(m_pTarget)->Make_Interrupt(this, 0);

		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact(m_iTalkNum);
	}
	switch (m_eDirState)
	{
	case DIR_STATE::DIR_DOWN:
		Change_Texture(TEXT("Com_Texture_Idle_Down"));
		break;
	case DIR_STATE::DIR_UP:
		Change_Texture(TEXT("Com_Texture_Idle_Up"));
		break;
	case DIR_STATE::DIR_LEFT:
	case DIR_STATE::DIR_RIGHT:
		Change_Texture(TEXT("Com_Texture_Idle_Side"));
		break;
	}

}

void CPlayer::Invincible_Update(_float _fTimeDelta)
{
	if (m_bHited)
	{
		m_fInvincible_Time += _fTimeDelta;
		if (m_fInvincible_Time >= 2.f)
		{
			m_bHited = false;
			m_fInvincible_Time = 0.f;
		}
	}

}

void CPlayer::Cooltime_Update(_float _fTimeDelta)
{
	for (auto& iter : m_vecSkillDesc)
	{
		if (iter.bSkillUsed == true)
		{
			iter.fCurrent_CoolTime += _fTimeDelta;

			if (iter.fCurrent_CoolTime >= iter.fMaxCoolTime)
			{
				iter.bSkillUsed = false;
				iter.fCurrent_CoolTime = 0.f;
			}
		}

	}
}

void CPlayer::Play_Sound(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_tchar	szFullPath[MAX_PATH];// = TEXT("footstep_grass_%d.wav");
	_uint iNum = 0; 
	
	_float fVolume = 0.5f;
	switch (m_eState)
	{
	case Client::CPlayer::ACTION_STATE::IDLE://x
	case Client::CPlayer::ACTION_STATE::MINING://x
	case Client::CPlayer::ACTION_STATE::CHOP://x
	case Client::CPlayer::ACTION_STATE::COOK://x
	case Client::CPlayer::ACTION_STATE::PICKUP:
		break;
	case Client::CPlayer::ACTION_STATE::SLEEP: //do
		if (!m_bSoundEnd)
		{
			wcscpy_s(szFullPath, TEXT("wilson_yawn.wav"));
			m_bSoundEnd = true;
		}
		break;
	case Client::CPlayer::ACTION_STATE::ANGRY:
		if (!m_bSoundEnd)
		{
			iNum = rand() % 4;
			wcscpy_s(szFullPath, TEXT("WilsonVoice_generic_%d.wav"));
			wsprintf(szFullPath, szFullPath, iNum);
			m_bSoundEnd = true;
		}
		break;
	case Client::CPlayer::ACTION_STATE::TALK://do
		if (!m_bSoundEnd && (static_cast<CNPC*>(m_pTarget)->Get_NPCID() !=NPC_PIGKING) )
		{
			iNum = rand() % 4;
			wcscpy_s(szFullPath, TEXT("WilsonVoice_generic_%d.wav"));
			wsprintf(szFullPath, szFullPath, iNum);
			m_bSoundEnd = true;
		}
		break;
	case Client::CPlayer::ACTION_STATE::MOVE:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 20
			|| m_pTextureCom->Get_Frame().m_iCurrentTex == 4)
		{
			fVolume = 0.3f;
			iNum = rand() % 4;
			if ((LEVEL)m_iCurrentLevelndex == LEVEL_GAMEPLAY)
			{
				wcscpy_s(szFullPath, TEXT("footstep_grass_%d.wav"));
			}
			else
			{
				wcscpy_s(szFullPath, TEXT("DSS_woodlegs_footstep_wood_%d.wav"));
			}
			wsprintf(szFullPath, szFullPath, iNum);
		}	
		break;
	case Client::CPlayer::ACTION_STATE::ATTACK:
		if (!m_bSoundEnd &&m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
		{
			iNum = rand() % 4;
			fVolume = 0.7f;
			switch (m_eWeaponType)
			{
			case WEAPON_SWORD:
				wcscpy_s(szFullPath, TEXT("Weapon_Swing.wav"));
				fVolume = 1.f;
				break;
			case WEAPON_STAFF:
				wcscpy_s(szFullPath, TEXT("firestaff_swing_%d.wav"));
				wsprintf(szFullPath, szFullPath, iNum);
				break;
			case WEAPON_HAND:
				wcscpy_s(szFullPath, TEXT("wet_punches_mutated_hound_DST-%d.wav"));
				wsprintf(szFullPath, szFullPath, iNum);
				break;
			case WEAPON_DART:
				iNum = rand() % 3;
				wcscpy_s(szFullPath, TEXT("blowdart_blow_%d.wav"));
				wsprintf(szFullPath, szFullPath, iNum);
				break;
			}
			m_bSoundEnd = true;
		}
		break;
	case Client::CPlayer::ACTION_STATE::EAT: // do
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
		{
			
			wcscpy_s(szFullPath, TEXT("eat_%d.wav"));
			wsprintf(szFullPath, szFullPath, iNum);
		}
		break;
	case Client::CPlayer::ACTION_STATE::DAMAGED:
		if (!m_bSoundEnd &&m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
		{		
			iNum = rand() % 3;
			wcscpy_s(szFullPath, TEXT("wet_punches_mutated_hound_DST-%d.wav"));
			wsprintf(szFullPath, szFullPath, iNum);
			pGameInstance->PlaySounds(szFullPath, SOUND_OBJECT, fVolume);
			//Player Hit
			wcscpy_s(szFullPath, TEXT("WilsonVoice_hurt_%d.wav"));
			wsprintf(szFullPath, szFullPath, iNum);	
			m_bSoundEnd = true;
		}
		break;
	case Client::CPlayer::ACTION_STATE::PORTAL:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
		{
			iNum = 0;
			wcscpy_s(szFullPath, TEXT("jump_%d.wav"));
			wsprintf(szFullPath, szFullPath, iNum);
		}	
		break;
	case Client::CPlayer::ACTION_STATE::DEAD:
		if (!m_bSoundEnd &&m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
		{
			fVolume = 0.7f;
			wcscpy_s(szFullPath, TEXT("WilsonVoice_dead.mp3"));
			m_bSoundEnd = true;
		}
		break;
	case Client::CPlayer::ACTION_STATE::REVIVE://do
		if (!m_bSoundEnd &&m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
		{
			wcscpy_s(szFullPath, TEXT("Revive_DST.wav"));
			m_bSoundEnd = true;
		}
		break;
	case Client::CPlayer::ACTION_STATE::WEEDING:
	case Client::CPlayer::ACTION_STATE::BUILD:
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 1)
		{
			wcscpy_s(szFullPath, TEXT("Shake_hand.wav"));
			fVolume = 0.3f;	
		}
		break;
	}

	pGameInstance->PlaySounds(szFullPath, SOUND_PLAYER, fVolume);

	Safe_Release(pGameInstance);
}

DIR_STATE CPlayer::Check_Direction(void)
{
	//내 자신의 Look에서 3번 변환한 값
	_float3 vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float4x4 OriginMat = m_pTransformCom->Get_WorldMatrix();
	_float4x4 RotateMat = OriginMat;
	_float3 vRot45 = *(_float3*)&OriginMat.m[2][0];
	_float3 vRot135 = *(_float3*)&OriginMat.m[2][0];

	//Turn
	D3DXMatrixRotationAxis(&RotateMat, &_float3(0.f, 1.f, 0.f), D3DXToRadian(90.f));
	D3DXVec3TransformNormal(&vRot45, &vRot45, &RotateMat);
	//vRot45 = *(_float3*)&RotateMat.m[2][0];
	D3DXMatrixRotationAxis(&RotateMat, &_float3(0.f, 1.f, 0.f), D3DXToRadian(135.f));
	D3DXVec3TransformNormal(&vRot135, &vRot135, &RotateMat);
	//vRot135 = *(_float3*)&RotateMat.m[2][0];;
	///////
	_float3 vTargetLook = m_vPickingPoint - Get_Pos();

	D3DXVec3Normalize(&vRot45, &vRot45);
	D3DXVec3Normalize(&vRot135, &vRot135);
	D3DXVec3Normalize(&vMyLook, &vMyLook);
	D3DXVec3Normalize(&vTargetLook, &vTargetLook);

	//각 내적값들 구하기
	_float fTargetDot = D3DXVec3Dot(&vMyLook, &vRot45);
	fTargetDot = acos(fTargetDot);
	_float fDegree45 = D3DXToDegree(fTargetDot);

	fTargetDot = D3DXVec3Dot(&vMyLook, &vRot135);
	fTargetDot = acos(fTargetDot);
	_float fDegree135 = D3DXToDegree(fTargetDot);

	fTargetDot = D3DXVec3Dot(&vMyLook, &vTargetLook);
	fTargetDot = acos(fTargetDot);
	_float fDegreeTarget = D3DXToDegree(fTargetDot);

	//외적하기
	_float3 vCrossOut;
	//D3DXVec3Cross(&vCrossOut, &vMyLook, &vTargetLook);
	D3DXVec3Cross(&vCrossOut, &vMyLook, &vTargetLook);

	DIR_STATE eDir = DIR_END;
	//Check Left Or Right
	if (vCrossOut.y > 0.f)//Right
	{
		if (fDegreeTarget > 0.f && fDegreeTarget <= 60.f)
		{
			//cout << "y+, UP, Degree: " << fDegreeTarget << endl;
			return DIR_UP;
		}
		else if (fDegreeTarget > 60.f && fDegreeTarget <= 135.f)
		{
			//cout << "y+, Right, Degree: " << fDegreeTarget << endl;
			return DIR_RIGHT;
		}
		else if (fDegreeTarget > 135.f && fDegreeTarget <= 180.f)
		{
			//cout << "y+, Down, Degree: " << fDegreeTarget << endl;
			return DIR_DOWN;
		}
	}
	else//Left
	{
		if (fDegreeTarget > 0.f && fDegreeTarget <= 60.f)
		{
			//cout << "y-, UP, Degree: " << fDegreeTarget << endl;
			return DIR_UP;
		}
		else if (fDegreeTarget > 60.f && fDegreeTarget <= 135.f)
		{
			//cout << "y-, Left, Degree: " << fDegreeTarget << endl;
			return DIR_LEFT;
		}
		else if (fDegreeTarget > 135.f && fDegreeTarget <= 180.f)
		{
			//cout << "y-, Down, Degree: " << fDegreeTarget << endl;
			return DIR_DOWN;
		}
	}

	//cout << "Error " << fDegreeTarget << endl;
	return DIR_DOWN;

}

void CPlayer::Test_Func(_int _iNum)
{
}

void CPlayer::Test_Detect(_float fTImeDelta)
{

}

void CPlayer::RangeCheck(_float _fTimeDelta)
{
	if (!m_pPicker->Get_IsShow())
		return;

	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	CPickingMgr::Get_Instance()->Picking();

	_float Compare_Range = (m_vTargetPicking.x - Get_Pos().x)*(m_vTargetPicking.x - Get_Pos().x)
		+ (m_vTargetPicking.y - Get_Pos().y)*(m_vTargetPicking.y - Get_Pos().y)
		+ (m_vTargetPicking.z - Get_Pos().z)*(m_vTargetPicking.z - Get_Pos().z);

	m_pPicker->Set_Pos(m_vTargetPicking);

	_float3 vPosition = Get_Pos();
	vPosition.y -= 0.4f;
	m_pRange->Set_Pos(vPosition);

	if (m_fAtkRange > Compare_Range)
	{
		m_pPicker->Set_IsCorrect(true);
		m_pRange->Set_IsCorrect(true);
	}
	else
	{
		m_pPicker->Set_IsCorrect(false);
		m_pRange->Set_IsCorrect(false);
	}
}

void CPlayer::Turn_OriginMat(_bool _bIsRight)
{
	_float3 vRight = *(_float3*)&m_pOriginMatrix.m[0][0];
	_float3 vUp = *(_float3*)&m_pOriginMatrix.m[1][0];
	_float3 vLook = *(_float3*)&m_pOriginMatrix.m[2][0];

	_float3 vAxis = { 0.f, 1.f, 0.f };
	_float4x4 RotateMatrix;

	if (_bIsRight)
	{
		D3DXMatrixRotationAxis(&RotateMatrix, &vAxis, 1.f);
	}
	else
	{
		D3DXMatrixRotationAxis(&RotateMatrix, &vAxis, -1.f);
	}

	D3DXVec3TransformNormal(&vRight, &vRight, &RotateMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotateMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotateMatrix);

	memcpy(&m_pOriginMatrix.m[0][0], &vRight, sizeof(_float3));
	memcpy(&m_pOriginMatrix.m[1][0], &vUp, sizeof(_float3));
	memcpy(&m_pOriginMatrix.m[2][0], &vLook, sizeof(_float3));

}

void CPlayer::Tick_ActStack(_float fTimeDelta)
{
	if (m_bAutoMode)
	{
		CInteractive_Object* pObj = (CInteractive_Object*)m_pTarget;
		INTERACTOBJ_ID eObjID = INTERACTOBJ_ID::ID_END;
		if (m_pTarget != nullptr)
		{
			eObjID = pObj->Get_InteractName();
		}

		switch (m_ActStack.top())
		{
		case ACTION_STATE::IDLE:
			m_bAutoMode = false;
			break;
		case ACTION_STATE::MOVE:	//만약)m_pTarget ==nullptr,automode == true, m_bBuild == false; 빌드로 이동하게 만들기?
			if (m_bArrive && !Check_Interact_End())
			{
				m_ActStack.push(Select_Interact_State(eObjID));
			}
			else if ((m_pTarget == nullptr || Check_Interact_End()) && m_bBuildTrigger == false)
			{
				m_ActStack.pop();
			}
			else if (m_pTarget == nullptr && m_bBuildTrigger == true && m_bArrive == true)
			{
				m_ActStack.push(ACTION_STATE::BUILD);
			}
			break;
		case ACTION_STATE::MINING:
			if (Check_Interact_End())
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else {
				Mining(fTimeDelta);
			}
			break;
		case ACTION_STATE::CHOP:
			if (Check_Interact_End())
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else {
				Chop(fTimeDelta);
			}
			break;
		case ACTION_STATE::WEEDING:
			if (Check_Interact_End())
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else {
				Cutting_Grass(fTimeDelta);
			}
			break;
		case ACTION_STATE::EAT:
			if (m_bMove)
			{
				m_ActStack.pop();
			}
			else {
				Eatting(fTimeDelta);
			}
			break;
		case ACTION_STATE::PICKUP:
			if (Check_Interact_End())
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else {
				Pickup(fTimeDelta);
			}
			break;
		case ACTION_STATE::ATTACK:
			break;
		case ACTION_STATE::DAMAGED:
			if (m_bMove)
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else
			{
				Damaged(fTimeDelta);
			}
			break;
		case ACTION_STATE::DEAD:
			if (m_bMove)
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else
			{
				Dead(fTimeDelta);
			}
			break;
		case ACTION_STATE::REVIVE:
			if (!m_bGhost)
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else
			{
				Revive(fTimeDelta);
			}
			break;
		case ACTION_STATE::PORTAL:
			if (m_bInPortal)
			{
				m_ActStack.pop();
				//m_pTarget = nullptr;
			}
			else
			{
				Jump(fTimeDelta);
			}
			break;
		case ACTION_STATE::BUILD:
			if (!m_bBuildTrigger)
			{
				m_ActStack.pop();
			}
			else
			{
				Building(fTimeDelta);
			}
			break;
		case ACTION_STATE::ANGRY:
			Angry(fTimeDelta);
			if (m_bMove)
			{
				m_ActStack.pop();
			}
			break;
		case ACTION_STATE::SLEEP:
			Sleep_Restore(fTimeDelta);
			if (!m_bSleeping)
			{
				m_ActStack.pop();
			}
			break;
		case ACTION_STATE::TALK:
			Talk_NPC(fTimeDelta);
			if (!m_bTalkMode)
			{
				m_ActStack.pop();
				Clear_ActStack();
			}
			break;
		default:
			break;
		}

	}

}

void CPlayer::Clear_ActStack()
{
	while (m_ActStack.size() != 1)
	{
		m_ActStack.pop();
	}
	m_bInPortal = false;
	m_bActivated = false;
	m_bAutoMode = false;
	m_bIsBuild = false;
	m_bBuildTrigger = false;
	m_pTarget = nullptr;
	m_bTalkMode = false;
}

CPlayer::ACTION_STATE CPlayer::Select_Interact_State(INTERACTOBJ_ID _eObjID)
{
	switch (_eObjID)
	{
	case INTERACTOBJ_ID::BERRYBUSH:
	case INTERACTOBJ_ID::CARROT:
	case INTERACTOBJ_ID::GRASS:
	case INTERACTOBJ_ID::DIRT: // Move to New ACTION_STATE
		return ACTION_STATE::WEEDING;
		break;
	case INTERACTOBJ_ID::TREE:
		return ACTION_STATE::CHOP;
		break;
	case INTERACTOBJ_ID::BOULDER:
		return ACTION_STATE::MINING;
		break;
	case INTERACTOBJ_ID::PORTAL:
		return ACTION_STATE::PORTAL;
		break;
	case INTERACTOBJ_ID::COOKPOT:
	case INTERACTOBJ_ID::ITEMS:
		return ACTION_STATE::PICKUP;
		break;
	case INTERACTOBJ_ID::SKELETON:
		return ACTION_STATE::REVIVE;
		break;
	case INTERACTOBJ_ID::TENT:
		return ACTION_STATE::SLEEP;
		break;
	case INTERACTOBJ_ID::NPC:
		return ACTION_STATE::TALK;
		break;
	default:
		return ACTION_STATE::ACTION_END;
	}
}

_bool CPlayer::Check_Interact_End(void)
{
	if (m_pTarget == nullptr)
		return true;

	return dynamic_cast<CInteractive_Object*>(m_pTarget)->Get_CanInteract() ? false : true;
}



HRESULT CPlayer::Texture_Clone()
{

	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 32;
	TextureDesc.m_fSpeed = 60;
	/*Run*/
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Run_Side"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Run_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Run_Down"), m_pTextureCom));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 66;
	TextureDesc.m_fSpeed = 60;
	/*Idle*/
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Idle_Side"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Idle_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Idle_Down"), m_pTextureCom));

	/*Punch*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 24;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Punch_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Punch_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Punch_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Punch_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Punch_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Punch_Side"), m_pTextureCom));

	/*Attack_Weapon*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 36;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Weapon_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Weapon_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Weapon_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Weapon_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Weapon_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Weapon_Side"), m_pTextureCom));

	/*Attack_Dart*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dart_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Dart_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dart_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Dart_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dart_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Dart_Side"), m_pTextureCom));

	/*Mining*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Mining_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Mining_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Mining_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Mining_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Mining_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Mining_Side"), m_pTextureCom));

	/*Chop*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 39;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Chop_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Chop_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Chop_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Chop_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Chop_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Chop_Side"), m_pTextureCom));

	/*Build*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Build_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Build_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Build_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Build_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Build_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Build_Side"), m_pTextureCom));

	/*Eat*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 21;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Eat"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Eat"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Eat"), m_pTextureCom));

	/*Pickup*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 10;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Pickup_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Pickup_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Pickup_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Pickup_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Pickup_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Pickup_Side"), m_pTextureCom));

	/*Damaged*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Damaged_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Damaged_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Damaged_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Damaged_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Damaged_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Damaged_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Damaged_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Damaged_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Damaged_Side"), m_pTextureCom));

	/*Jump*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 39;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Jump_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Jump_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Jump_Up"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Jump_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Jump_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Jump_Down"), m_pTextureCom));

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Jump_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Jump_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Jump_Side"), m_pTextureCom));

	/*Dead*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 55;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dead"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Dead"), m_pTextureCom));
	/*Ghost_Idle*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 64;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Ghost_Idle"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Ghost_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Ghost_Idle"), m_pTextureCom));
	/*Revive*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 16;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Revive"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Revive"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Revive"), m_pTextureCom));

	/*Angry*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Angry"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Angry"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_mapTexture.insert(make_pair(TEXT("Com_Texture_Angry"), m_pTextureCom));

	return S_OK;
}

HRESULT CPlayer::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

void CPlayer::SetUp_BillBoard()
{
	if (m_bIsFPS && m_eDirState != DIR_STATE::DIR_RIGHT &&
		m_bIsFPS && m_eDirState != DIR_STATE::DIR_LEFT &&
		m_bIsFPS && m_eDirState != DIR_STATE::DIR_DOWN)
		return;

	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	if (!m_bIsFPS && m_eDirState == DIR_STATE::DIR_LEFT)
	{
		m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
	}
}

void CPlayer::WalkingTerrain()
{
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(m_iCurrentLevelndex, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(m_iCurrentLevelndex, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.5f);

	Set_TerrainY(vPosition.y);
}


CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer*	pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void* pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pPicker);
	Safe_Release(m_pRange);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_Equipment);

	/*for (auto& iter : m_vecParty)
		Safe_Release((iter).second);*/

	m_vecParty.clear();

	for (auto& iter : m_mapTexture)
		Safe_Release((iter).second);

	m_mapTexture.clear();
}
