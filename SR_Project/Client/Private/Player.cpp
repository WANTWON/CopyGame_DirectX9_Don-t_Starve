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

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
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


	m_iPreLevelIndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	

	return S_OK;
}

int CPlayer::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider::COLLISION_PLAYER, this);

	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	if (m_iPreLevelIndex != m_iCurrentLevelndex)
	{
		Clear_ActStack();
		m_iPreLevelIndex = m_iCurrentLevelndex;
		Change_Texture(TEXT("Com_Texture_Idle_Side"));
		m_bInPortal = false;
		m_bMove = true;
	}
	m_iCameraMode = CCameraManager::Get_Instance()->Get_CamState();
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return OBJ_NOEVENT;

	__super::Tick(fTimeDelta);

	//Origin Look Test
	if (iCnt == 0)
	{
		m_pOriginMatrix = m_pTransformCom->Get_WorldMatrix();
	}
	//SkillCoolTime
	Cooltime_Update(fTimeDelta);

	//Collider Add
	//if (nullptr != m_pColliderCom)
	//	m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_PLAYER, this);

	Sleep_Restore();

	//Act Auto
	Tick_ActStack(fTimeDelta);

	//KeyInput
	GetKeyDown(fTimeDelta);

	//Move
	Move_to_PickingPoint(fTimeDelta);
	WalkingTerrain();

	
	m_Equipment->Set_TargetPos(Get_Pos());
	//TEst
	RangeCheck(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//cout << "Player HP : " << m_tStat.fCurrentHealth << endl;


	return OBJ_NOEVENT;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{

	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

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
	
	if (m_eDirState == DIR_STATE::DIR_LEFT)
		m_pColliderCom->Set_IsInverse(true);
	else
		m_pColliderCom->Set_IsInverse(false);
	m_pColliderCom->Update_ColliderBox(m_pTransformCom->Get_WorldMatrix());

	Create_Bullet();

	

	_float3 vDistance = _float3(0,0,0);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Collision_with_Group(CCollider::COLLISION_BLOCK, this, &vDistance))
	{
		_float3 vPosition = Get_Position();

		if(fabsf(vDistance.x) < fabsf(vDistance.z))
			vPosition.x -= vDistance.x;
		else
			vPosition.z -= vDistance.z;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}


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

	if (vPlayerPos.x < m_vPickingPoint.x)
	{
		Move_Right(fTimedelta);
	}
	else if (vPlayerPos.x > m_vPickingPoint.x)
	{
		Move_Left(fTimedelta);
	}

	if (abs(vPlayerPos.x - m_vPickingPoint.x) < 0.1 &&
		abs(vPlayerPos.z - m_vPickingPoint.z) < 0.1)
	{

		m_bPicked = false;
		m_bArrive = true;
		m_bInputKey = false;
	}
}

HRESULT CPlayer::Render()
{
	WEAPON_TYPE type = m_eWeaponType;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_pTextureCom->Get_Frame().m_iCurrentTex)))
		return E_FAIL;


	m_pTextureCom->MoveFrame(m_TimerTag);

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;



	if (!m_bSleeping)
	{
		m_pVIBufferCom->Render();
	}

#ifdef _DEBUG
	m_pTextureCom->Bind_OnGraphicDev_Debug();
	m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG


	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

_float CPlayer::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_eState != ACTION_STATE::DAMAGED || !m_bGhost)
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
	else if (!m_bGhost && !Check_Dead())
	{
		m_ActStack.push(ACTION_STATE::DAMAGED);

		m_bMove = false;
		m_bAutoMode = true;
	}


	return fDamage;
}

void CPlayer::Set_Position(_float3 Position)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Position);
}

HRESULT CPlayer::SetUp_Components()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Player"))))
		return E_FAIL;

	m_TimerTag = TEXT("Timer_Player");



	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Rect::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Rect::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.4f;
	CollRectDesc.fRadiusX = 0.3f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = -0.25f;

	/* For.Com_Collider_Rect*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Rect"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
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

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = _float3(40.f, 2.f, 25.f);

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

	m_KeySets[INTERACTKEY::KEY_CAMFPSMODE] = VK_F1;

	m_KeySets[INTERACTKEY::KEY_CAMTPSMODE] = VK_F2;
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
	SkillDesc.fMaxCoolTime = 3.f;
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


	//Test Power

#pragma region Debug&CamKey
	if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_DEBUG]))
	{
		m_bDebugKey = !m_bDebugKey;
	}
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_CAMFPSMODE]))
	{
		m_bIsFPS = true;
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_FPS);
	}
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_CAMTPSMODE]))
	{
		m_bIsFPS = false;
		CCameraManager::Get_Instance()->Set_CamState(CCameraManager::CAM_PLAYER);
	}
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_CAMLEFT]))
	{
		if (m_bIsFPS)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
		else
		{
			
			CCameraManager::Get_Instance()->PlayerCamera_TurnLeft(m_iCurrentLevelndex);
			Turn_OriginMat(false);
			SetUp_BillBoard();
		}

	}
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_CAMRIGHT]))
	{
		if (m_bIsFPS)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), -_fTimeDelta);
		else
		{
			CCameraManager::Get_Instance()->PlayerCamera_TurnRight(m_iCurrentLevelndex);
			Turn_OriginMat(true);
			SetUp_BillBoard();
		}
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
				{
					m_pRange->Set_IsShow(true);
					m_pPicker->Set_IsShow(true);
				}
			}

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
				if (m_pPicker->Get_IsShow())
				{
					{
						m_pRange->Set_IsShow(false);
						m_pPicker->Set_IsShow(false);
					}
				}
			}



		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN2]))
		{
			if (!m_pPicker->Get_IsShow())
			{
				{
					m_pRange->Set_IsShow(true);
					m_pPicker->Set_IsShow(true);
				}
			}

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
			else if (m_vecSkillDesc[1].iCnt == 1
				&& m_vecSkillDesc[1].bSkillUsed == false
				&& m_pPicker->Get_IsCorrect())
			{
				Ice_Spike(_fTimeDelta);
				m_vecSkillDesc[1].bSkillUsed = true;
				if (m_pPicker->Get_IsShow())
				{
					{
						m_pRange->Set_IsShow(false);
						m_pPicker->Set_IsShow(false);
					}
				}
			}

		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN3]))
		{//나중에 얘도 좀 수정필요할듯  낭만없음
			if (!m_pPicker->Get_IsShow())
			{
				{
					m_pRange->Set_IsShow(true);
					m_pPicker->Set_IsShow(true);
				}
			}

			if (m_vecSkillDesc[2].iCnt == 0)
			{
				for (auto& iter : m_vecSkillDesc)
				{
					iter.iCnt = 0;
				}
				m_vecSkillDesc[2].iCnt += 1;
				m_fAtkRange = m_vecSkillDesc[2].fAtkRange;
				m_fAtkScale = m_vecSkillDesc[2].fAtkScale;
				m_pRange->Set_Scale(_float3(m_fAtkScale, m_fAtkScale, 1.f));

			}
			else if (m_vecSkillDesc[2].iCnt == 1
				&& m_vecSkillDesc[2].bSkillUsed == false
				&& m_pPicker->Get_IsCorrect())
			{
				Sand_Mines(_fTimeDelta);
				m_vecSkillDesc[2].bSkillUsed = true;
				if (m_pPicker->Get_IsShow())
				{
					{
						m_pRange->Set_IsShow(false);
						m_pPicker->Set_IsShow(false);
					}
				}
			}

			//Take_Damage(105.f, nullptr, this);
		}
		else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_INVEN4]))
		{
			if (!m_pPicker->Get_IsShow())
			{
				{
					m_pRange->Set_IsShow(true);
					m_pPicker->Set_IsShow(true);
				}
			}

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
			else if (m_vecSkillDesc[3].iCnt == 1
				&& m_vecSkillDesc[3].bSkillUsed == false
				&& m_pPicker->Get_IsCorrect())
			{
				Teleport(_fTimeDelta);
				m_vecSkillDesc[3].bSkillUsed = true;
				if (m_pPicker->Get_IsShow())
				{
					{
						m_pRange->Set_IsShow(false);
						m_pPicker->Set_IsShow(false);
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
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_ACTION]))
	{
		Clear_ActStack();
		Multi_Action(_fTimeDelta);
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
		break;
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
			m_pTransformCom->Go_Straight(_fTimeDelta * 2, m_fTerrain_Height);
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
			m_pTransformCom->Go_Backward(_fTimeDelta * 2, m_fTerrain_Height);
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
			m_pTransformCom->Go_Right(_fTimeDelta, m_fTerrain_Height);
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
		ParticleDesc.dDuration = 0.2; //  ƼŬ  ð 
		ParticleDesc.dParticleLifeTime = 0.2; //    
		ParticleDesc.dSpawnTime = 1; //     Ÿ  
		ParticleDesc.fParticlePerSecond = 75;
		ParticleDesc.fVelocityDeviation = 1.f;
		ParticleDesc.iMaxParticleCount = 5;
		ParticleDesc.vParticleScale = _float2(0.5, 0.5);
		ParticleDesc.vParticleDeviation = _float3(1 * 0.6f, 0.f, 1 * 0.6f);
		ParticleDesc.iTextureNum = 1;
		ParticleDesc.vVelocity = _float3((rand() % 10)*0.1f, (rand() % 10) * 0.1f, rand() % 10 * 0.1f);
		ParticleDesc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		ParticleDesc.vPosition.z += (_float)0.001;

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
		ParticleDesc.iTextureNum = 4;
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

	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iCurrentTex - 1)
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

	if (m_fReviveTime > 3.f && m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		m_bInPortal = true;

		m_bMove = true;
	}
	else if (m_fReviveTime < 3.f && m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		if (!Check_Interact_End())
		{
			dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact();
		}

		m_pTextureCom->Get_Frame().m_iCurrentTex = 37;
	}

}

void CPlayer::Dead(_float _fTimeDelta)
{

	m_eState = ACTION_STATE::DEAD;

	if (m_ePreState != m_eState)
	{
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

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Skeleton"), LEVEL_GAMEPLAY, TEXT("Layer_Object"), vPos)))
			return;

		m_bMove = true;
	}
}

void CPlayer::Revive(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::REVIVE;

	m_fReviveTime += _fTimeDelta;
	if (m_ePreState != m_eState)
	{
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
	}

	if (m_fReviveTime > 3.f &&m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		Change_Texture(TEXT("Com_Texture_Idle_Down"));
		m_bDead = false;
		m_bGhost = false;
		m_bMove = true;
		//m_bAutoMode = false;
		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact(10);
		m_fReviveTime = 0.f;
	}
	else if (m_fReviveTime < 3.f && m_pTextureCom->Get_Frame().m_iCurrentTex == 15)
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
	else if(m_fBuildTime < 1.f &&m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 2)
	{

		m_pTextureCom->Get_Frame().m_iCurrentTex = 0;
	}

}

void CPlayer::Angry(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::ANGRY;

	if (m_ePreState != m_eState)
	{
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

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iCurrentTex - 1)
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
		Clear_ActStack();
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

		CParticle::STATEDESC ParticleDesc;
		ZeroMemory(&ParticleDesc, sizeof(CParticle::STATEDESC));
		ParticleDesc.eTextureScene = m_iCurrentLevelndex;
		ParticleDesc.pTextureKey = TEXT("Prototype_Component_Texture_Snow");
		ParticleDesc.iTextureNum = 1;
		ParticleDesc.vVelocity = _float3((rand() % 10)*0.1f, -0.1f, -rand() % 10 * 0.1f);

		for (int i = 0; i < 200; ++i)
		{
			if (FAILED(CGameInstance::Get_Instance()->Add_GameObject(TEXT("GameObject_Particle"), ParticleDesc.eTextureScene, TEXT("Layer_Particle"), &ParticleDesc)))
				return;
		}

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
	}
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
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Object"));

	_uint iIndex = 0;

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
				++iIndex;
				iter_Obj++;
				continue;
			}
		}
		else
		{
			if ((*iter_Obj) == nullptr || !dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_CanInteract()
				|| dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::SKELETON)
			{
				++iIndex;
				iter_Obj++;
				continue;
			}

			if (m_pTarget == nullptr)
			{
				m_pTarget = *iter_Obj;
			}


			_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
				+ (Get_Pos().y - (*iter_Obj)->Get_Position().y)*(Get_Pos().y - (*iter_Obj)->Get_Position().y)
				+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

			_float fTargetDir = (Get_Pos().x - (m_pTarget)->Get_Position().x)*(Get_Pos().x - (m_pTarget)->Get_Position().x)
				+ (Get_Pos().y - (m_pTarget)->Get_Position().y)*(Get_Pos().y - (m_pTarget)->Get_Position().y)
				+ (Get_Pos().z - (m_pTarget)->Get_Position().z)*(Get_Pos().z - (m_pTarget)->Get_Position().z);

			if (dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::PORTAL
				&& fCmpDir >= 5.f)
			{
				++iIndex;
				iter_Obj++;
				continue;
			}

			if (fCmpDir < fTargetDir)
			{
				m_pTarget = *iter_Obj;
			}

			++iIndex;
			iter_Obj++;
		}

	}


	Safe_Release(pGameInstance);
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

void CPlayer::Sleep_Restore()
{
	if (m_bSleeping)
	{
		if (GetTickCount() > m_dwSleepTime + 1000)
		{
			if (m_tStat.fCurrentHealth < m_tStat.fMaxHealth)
				m_tStat.fCurrentHealth += 1;
			
			if (m_tStat.fCurrentMental < m_tStat.fMaxMental)
				m_tStat.fCurrentMental += 1;
			
			if (m_tStat.fCurrentHungry > 0)
				m_tStat.fCurrentHungry -= 1;

			m_dwSleepTime = GetTickCount();
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
	m_pRange->Set_Pos(Get_Pos());

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
			if (m_bMove)
			{
				m_ActStack.pop();
			}
			else
			{
				Angry(fTimeDelta);
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
	m_bAutoMode = false;
	m_bIsBuild = false;
	m_bBuildTrigger = false;
	m_pTarget = nullptr;
}

CPlayer::ACTION_STATE CPlayer::Select_Interact_State(INTERACTOBJ_ID _eObjID)
{
	switch (_eObjID)
	{
	case INTERACTOBJ_ID::BERRYBUSH:
	case INTERACTOBJ_ID::CARROT:
	case INTERACTOBJ_ID::GRASS:
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
	case INTERACTOBJ_ID::TENT:
	case INTERACTOBJ_ID::NPC:
	case INTERACTOBJ_ID::ITEMS:
		return ACTION_STATE::PICKUP;
		break;
	case INTERACTOBJ_ID::SKELETON:
		return ACTION_STATE::REVIVE;
		break;
	default:
		return ACTION_STATE::ACTION_END;
	}
}

_bool CPlayer::Check_Interact_End(void)
{
	//CInteractive_Object* pObj = (CInteractive_Object*)m_pTarget;
	if (m_pTarget == nullptr)
		return true;

	return	(dynamic_cast<CInteractive_Object*>(m_pTarget)->Get_CanInteract() ? false : true);

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
		m_bIsFPS && m_eDirState != DIR_STATE::DIR_LEFT)
		return;

	if (!m_bIsFPS && m_eDirState == DIR_STATE::DIR_LEFT)
		return;
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
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

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_Equipment);

	for (auto& iter : m_mapTexture)
		Safe_Release((iter).second);

	m_mapTexture.clear();
}
