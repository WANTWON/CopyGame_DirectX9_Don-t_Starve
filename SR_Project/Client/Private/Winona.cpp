#include "stdafx.h"
#include "..\Public\Winona.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Player.h"
#include "BerryBush.h"
#include "Pig.h"
#include "Bullet.h"
#include "Skill.h"
#include "Inventory.h"
#include "Catapult.h"
#include "Battery_Tower.h"
CWinona::CWinona(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CNPC(pGraphic_Device)
{
}

CWinona::CWinona(const CWinona & rhs)
	: CNPC(rhs)
{
}

HRESULT CWinona::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWinona::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	//Test
	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_eObjID = OBJID::OBJ_NPC;
	m_eNPCID = NPCID::NPC_WINONA;

	m_fAtk_Max_CoolTime = 20.f;
	m_fAtk_Cur_CoolTime = m_fAtk_Max_CoolTime;

	m_fSkill_Max_CoolTime = 9.f;
	m_fSkill_Cur_CoolTime = m_fSkill_Max_CoolTime;

	m_fSkillRange = 7.f;
	m_fAtkRange = 10.f;
	m_fOwnerRadius = 10.f;
	m_fMinRange = 5.f;
	m_fDetectRange = 15.f;

	m_tInfo.iMaxHp = 100;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;
	m_tInfo.fDamage = 20.f;

	//Init BehavirvTree
	BehaviorTree = new CBT_NPC(this);

	BehaviorTree->Initialize();

	Change_Texture(TEXT("Com_Texture_Idle_Down"));
	return S_OK;
}

int CWinona::Tick(_float fTimeDelta)
{
	if (!Setup_LevelChange(fTimeDelta))
		return OBJ_NOEVENT;


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_PLAYER, this);

	__super::Tick(fTimeDelta);

	//cout << "x:" << Get_Position().x << "y: " << Get_Position().y << "z : " << Get_Position().z << endl;

	CatapultCheck();

	BehaviorTree->Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CWinona::Late_Tick(_float fTimeDelta)
{
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	if (m_iCurrentLevelndex != LEVEL_GAMEPLAY && !m_bOwner)
		return;

	__super::Late_Tick(fTimeDelta);

	Change_Frame(fTimeDelta);

	if (m_bDead)
	{
		return;
	}

	if (m_eCur_Dir == DIR_STATE::DIR_LEFT)
	{
		m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
	}
}

HRESULT CWinona::Render()
{
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;


#ifdef _DEBUG

	//m_pColliderCom->Render_ColliderBox();
#endif // _DEBUG

	return S_OK;
}

void CWinona::Decreas_Catapult(CCatapult * _pCatapult)
{
	auto iter = find_if(m_vecCatapults.begin(), m_vecCatapults.end(), [&](auto& iter)->bool {

		if (iter == _pCatapult)
			return true;

		return false;
	});

	if (iter != m_vecCatapults.end())
	{
		iter = m_vecCatapults.erase(iter);
	}


}

HRESULT CWinona::SetUp_Components(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//m_TimerTag = TEXT("Timer_Grass");
	//if (FAILED(pGameInstance->Add_Timer(m_TimerTag)))
	//return E_FAIL;

	Safe_Release(pGameInstance);

	/* For.Com_Texture */
	Texture_Clone();
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

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

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f / 1.13f;
	TransformDesc.fRotationPerSec = D3DXToRadian(0.f);
	TransformDesc.InitPos = _float3(35.f, 2.f, 25.f);;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CWinona::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	/*Idle*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 68;
	TextureDesc.m_fSpeed = 40;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Run*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 20;
	TextureDesc.m_fSpeed = 40;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Run_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Run_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Run_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Build*/
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Build_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Build_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Build_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Build_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Pickup*/
	TextureDesc.m_iEndTex = 7;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Pickup_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Pickup_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pickup_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Pickup_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Dance*/
	TextureDesc.m_iEndTex = 41;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dance"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Dance"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Talk*/
	TextureDesc.m_iEndTex = 51;
	TextureDesc.m_fSpeed = 40;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Talk"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Talk"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Dead*/
	TextureDesc.m_iEndTex = 55;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Dead"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Hit*/
	TextureDesc.m_iEndTex = 22;
	TextureDesc.m_fSpeed = 30;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hit"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Winona_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CWinona::Change_Frame(_float fTimeDelta)
{
	if (m_eState == NPC_STATE::DEAD)
	{
		m_pTextureCom->MoveFrame(m_TimerTag, false);
	}
	else
	{
		m_pTextureCom->MoveFrame(m_TimerTag);
	}
}

void CWinona::Change_Motion()
{
}

_float CWinona::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (m_bInvincibleMode || m_bFirstCall)
		return 0.f;

	if (!m_bDead && m_tInfo.iCurrentHp <= (_int)fDamage)
	{
		m_fReviveTime = 0.f;
		m_bDead = true;
		m_bCanTalk = false;
	}
	else if (!m_bDead && !m_bHited)
	{
		m_tInfo.iCurrentHp -= (_int)fDamage;
		cout << "WinonaHP: " << m_tInfo.iCurrentHp << endl;
		m_bHited = true;
	}
	return fDamage;
}

void CWinona::Interact(_uint Damage)
{
	if (m_iTalkCnt == 2 && Damage == 1)
	{
		m_bAccept = true;
		m_iTalkCnt++;
	}
	else if (m_iTalkCnt == 2 && Damage == 2)
	{
		m_bAccept = false;
		m_iTalkCnt++;
	}
	else if (m_iTalkCnt != 2)
	{
		m_iTalkCnt++;
	}
}

HRESULT CWinona::Drop_Items()
{
	return E_NOTIMPL;
}

void CWinona::Make_Interrupt(CPawn * pCauser, _uint _InterruptNum)
{
	switch (_InterruptNum)
	{
	case 0: // Talk
		Reset_Target();
		m_pTarget = pCauser;
		Safe_AddRef(m_pTarget);
		m_iInterruptNum = _InterruptNum;
		m_bInterrupted = true;
		break;
	case 1://FightMode
		if (!m_bFightMode && m_bOwner)
		{
			m_iInterruptNum = _InterruptNum;
			m_bInterrupted = true;
		}
		break;
	}
}

void CWinona::Move(_float _fTimeDelta)
{
	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_eCur_Dir = Check_Direction();

	m_eState = CNPC::MOVE;

	if (m_ePreState != m_eState
		|| m_ePre_Dir != m_eCur_Dir)
	{
		m_eCur_Dir = Check_Direction();
		switch (m_eCur_Dir)
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
		m_ePre_Dir = m_eCur_Dir;
	}

	if ((LEVEL)m_iCurrentLevelndex == LEVEL_GAMEPLAY)
	{
		_tchar	szFullPath[MAX_PATH] = TEXT("footstep_grass_%d.wav");
	}
	else
	{
		_tchar	szFullPath[MAX_PATH] = TEXT("DSS_woodlegs_footstep_wood_%d.wav");
	}

	Play_Sound(_fTimeDelta);

	m_pTransformCom->Go_PosTarget(_fTimeDelta, m_vTargetPos, _float3{ 0.f, 0.f, 0.f });

	SetUp_BillBoard();
}

void CWinona::Idle(_float _fTimeDelta)
{
	m_eState = CNPC::IDLE;

	if (m_ePreState != m_eState)
	{
		switch (m_eCur_Dir)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Idle_Up"));
			break;

		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Idle_Down"));
			break;

		case DIR_LEFT:
		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Idle_Side"));
			break;

		}
		m_ePreState = m_eState;
	}
}

void CWinona::Interaction(_float _fTimedelta)
{//Test Only Berry
	if (m_pTarget == nullptr)
		return;
	//나중에 분간.

	Revive_Berry(_fTimedelta);
}

void CWinona::Talk(_float _fTimeDelta)
{
	if (static_cast<CPawn*>(m_pTarget)->Get_ObjID() == OBJID::OBJ_PLAYER)
	{
		Talk_Player(_fTimeDelta);
	}
	else
	{//Monster
		Talk_Friend(_fTimeDelta);
	}
}

void CWinona::Dance(_float _fTimeDelta)
{
	m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::DANCE;

	if (m_ePreState != m_eState)
	{
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Dance"));
		m_ePreState = m_eState;
	}
	if (2.f < m_fInteractTIme)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = false;
	}
}

void CWinona::Attack(_float _fTimeDelta)
{//m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::ATTACK;

	if (m_ePreState != m_eState)
	{
		m_bInteract = true;
		Play_Sound(_fTimeDelta);
		switch (m_eCur_Dir)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Build_Up"));
			break;

		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Build_Down"));
			break;

		case DIR_LEFT:
		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Build_Side"));
			break;
		}
		m_ePreState = m_eState;
	}
	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 2)
	{
		Create_Bullet(_fTimeDelta);
		m_bInteract = false;
		m_bCanAttack = false;
	}
}

void CWinona::Interrupted(_float _fTimeDelta)
{
	if (m_bInterrupted)
	{

		switch (m_iInterruptNum)
		{
		case 0://TalkMode
			Clear_Activated();
			m_bFirstCall = true;
			m_bArrive = true;
			m_bInteract = true;
			m_bInterrupted = false;
			m_bSelectAct = false;
			m_bFinishInteract = false;
			m_bSoundEnd = false;
			break;
		case 1: // attackMode
			Clear_Activated();
			m_bFightMode = true;
			m_bInteract = true;
			m_bInterrupted = false;
			m_bSelectAct = false;
			m_bFinishInteract = false;
			m_bSoundEnd = false;
			break;
		}

	}
}

void CWinona::Skill(_float _fTimeDelta)
{
	m_eState = CNPC::SKILL;

	if (m_ePreState != m_eState)
	{
		Play_Sound(_fTimeDelta);
		m_bInteract = true;
		switch (m_eCur_Dir)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Build_Up"));
			break;

		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Build_Down"));
			break;

		case DIR_LEFT:
		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Build_Side"));
			break;
		}
		m_ePreState = m_eState;

	}
	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 2)
	{
		Create_Catapult(_fTimeDelta);
		m_bInteract = false;
		m_bCanSkill = false;
	}
}

_bool CWinona::Hit(_float _fTimeDelta)
{
	m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::HIT;

	if (m_ePreState != m_eState)
	{
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Hit"));
		m_ePreState = m_eState;
		Play_Sound(_fTimeDelta);
	}

	if (m_pTextureCom->Get_Frame().m_iCurrentTex >= m_pTextureCom->Get_Frame().m_iEndTex - 1)
	{
		Clear_Activated();
		m_bHited = false;
	}
	return true;
}

_bool CWinona::Dead(_float _fTimeDelta)
{
	m_eState = CNPC::DEAD;

	if (m_ePreState != m_eState)
	{
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Dead"));
		Play_Sound(_fTimeDelta);
		m_ePreState = m_eState;
	}
	return true;
}

void CWinona::Select_Target(_float _fTimeDelta)
{
	if (m_iCurrentLevelndex == LEVEL_LOADING)
		return;

	Find_Priority();

	m_bArrive = false;
	m_bInteract = true;
	//m_eCur_Dir = Check_Direction();
}

void CWinona::Set_RandPos(_float _fTimeDelta)
{
	_float fOffsetX = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
	_int bSignX = rand() % 2;
	_float fOffsetZ = ((_float)rand() / (float)(RAND_MAX)) * m_fPatrolRadius;
	_int bSignZ = rand() % 2;
	m_fPatrolPosX = bSignX ? (m_pTransformCom->Get_TransformDesc().InitPos.x + fOffsetX) : (m_pTransformCom->Get_TransformDesc().InitPos.x - fOffsetX);
	m_fPatrolPosZ = bSignZ ? (m_pTransformCom->Get_TransformDesc().InitPos.z + fOffsetZ) : (m_pTransformCom->Get_TransformDesc().InitPos.z - fOffsetZ);

	m_vTargetPos = _float3(m_fPatrolPosX, 0.5f, m_fPatrolPosZ);
	m_bArrive = false;
	m_bSelectAct = false;
}

_bool CWinona::Get_Target_Moved(_float _fTimeDelta, _uint _iTarget)
{
	_float fMinRange = 0.f;
	_float fMiddleRange = 0.f;
	_float Compare_Range = 0.f;
	switch (_iTarget)
	{
	case 0: //Target == Owner
		fMinRange = m_fMinRange;
		fMiddleRange = m_fOwnerRadius;
		m_vTargetPos = static_cast<CPlayer*>(m_pOwner)->Set_PartyPostion(this);
		break;
	case 1:// Basic AttackRange
		fMiddleRange = fMinRange = m_fAtkRange;
		m_vTargetPos = m_pTarget->Get_Position();
		break;
	case 2://SkillRange
		fMiddleRange = fMinRange = m_fSkillRange;

		m_vTargetPos = m_pTarget->Get_Position();
		break;
	default:
		if (m_pTarget)
		{
			m_vTargetPos = m_pTarget->Get_Position();
		}
		fMiddleRange = fMinRange = 0.4f;
		break;
	}


	Compare_Range = (m_vTargetPos.x - Get_Pos().x)*(m_vTargetPos.x - Get_Pos().x)
		+ (m_vTargetPos.z - Get_Pos().z)*(m_vTargetPos.z - Get_Pos().z);

	if (fMinRange > Compare_Range)
	{
		m_bArrive = true;
		Clear_Activated();
		return false;
	}

	if (m_bFightMode)
	{
		if (fMiddleRange < Compare_Range)
		{
			m_bArrive = false;
			Clear_Activated();
		}

	}
	else
	{
		if (fMinRange < Compare_Range)
		{
			m_bArrive = false;
			Clear_Activated();
		}
	}
	return true;
}

_bool CWinona::Detect_Enemy()
{
	Find_Enemy();

	if (m_pTarget == nullptr)
		return false;

	m_bInteract = true;
	return true;
}

void CWinona::Play_Sound(_float _fTimeDelta)
{
	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == false)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_int iNum = rand() % 4;
	_tchar	szFullPath[MAX_PATH];
	_float fVolume = 0.8f;
	switch (m_eState)
	{
	case CNPC::IDLE:
		break;
	case CNPC::ATTACK:
	case CNPC::SKILL:
		wcscpy_s(szFullPath, TEXT("Shake_hand.wav"));
		fVolume = 0.4f;
		break;
	case CNPC::MOVE:
		fVolume = 0.3f;
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == 14
			|| m_pTextureCom->Get_Frame().m_iCurrentTex == 2)
		{
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
	case CNPC::HIT:
		//Player Hit
		wcscpy_s(szFullPath, TEXT("winona_hit6_%d.wav"));
		wsprintf(szFullPath, szFullPath, iNum);
		break;
	case CNPC::DEAD:
		wcscpy_s(szFullPath, TEXT("winona_death6.wav"));
		break;
	case CNPC::TALK:
		/*fVolume = 0.4f;
		wcscpy_s(szFullPath, TEXT("winona_vo6_%d.wav"));
		wsprintf(szFullPath, szFullPath, iNum);*/
		break;
	}

	pGameInstance->PlaySounds(szFullPath, SOUND_WINONA, fVolume);

	Safe_Release(pGameInstance);
}

void CWinona::Revive_Berry(_float _fTimeDelta)
{
	m_eState = CNPC::INTERACT;

	if (m_ePreState != m_eState)
	{
		switch (m_eCur_Dir)
		{
		case DIR_UP:
			Change_Texture(TEXT("Com_Texture_Build_Up"));
			break;

		case DIR_DOWN:
			Change_Texture(TEXT("Com_Texture_Build_Down"));
			break;

		case DIR_LEFT:
		case DIR_RIGHT:
			Change_Texture(TEXT("Com_Texture_Build_Side"));
			break;

		}
		m_ePreState = m_eState;
	}

	m_fInteractTIme += _fTimeDelta;

	if (1.5f < m_fInteractTIme)
	{
		dynamic_cast<CInteractive_Object*>(m_pTarget)->Interact(60);
		m_fInteractTIme = 0.f;

		m_bInteract = false;
		m_bSelectAct = true;
		Reset_Target();
	}
}

void CWinona::Talk_Player(_float _fTimeDelta)
{
	CInventory_Manager::Get_Instance()->Get_Talk_list()->front()->Set_WinonaTalk(true);

	CInventory_Manager* pinven = CInventory_Manager::Get_Instance();
	pinven->Get_Talk_list()->front()->setcheck(true);

	if (!m_bFirstCall)
	{
		m_bNextAct = false;
		m_bInteract = false;
		m_bArrive = false;
		return;
	}

	m_eState = CNPC::TALK;
	static_cast<CPlayer*>(m_pTarget)->Set_TalkMode(true);
	static_cast<CPlayer*>(m_pTarget)->Set_bOnlyActionKey(true);

	if (m_ePreState != m_eState)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Talk"));
		m_ePreState = m_eState;

	}
	if (m_iPreTalkCnt != m_iTalkCnt)
	{
		Play_Sound(_fTimeDelta);
		if (!m_bOwner)
		{//IsPartyed
			switch (m_iTalkCnt)
			{
			case 1:
				pinven->Get_Talk_list()->front()->Set_Texnum1(0);
				break;
			case 2:
				pinven->Get_Talk_list()->front()->Set_Texnum1(1);
				break;
			case 3:
				if (m_bAccept)
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(2);
					m_bNextAct = true;

				}
				else
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(3);
					m_bNextAct = false;
				}
				break;
			case 4:
				m_bSelectAct = true;
				Clear_Activated();
				static_cast<CPlayer*>(m_pTarget)->Set_TalkMode(false);
				static_cast<CPlayer*>(m_pTarget)->Set_bOnlyActionKey(false);
				m_iTalkCnt = 0;
				m_bInteract = false;
				m_bFirstCall = false;
				if (m_bNextAct)
				{
					m_bOwner = true;
					m_pOwner = static_cast<CPawn*>(m_pTarget);
					static_cast<CPlayer*>(m_pTarget)->Add_Party(TEXT("Winona"), this);

					CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
					auto k = pinv->Get_Party_list();
					for (auto iter : *k)
					{
						if (iter->Get_check() == false)
						{
							iter->Set_check(true);
							iter->Set_texnum(1);
							break;
						}
					}
				}
				else
				{
					Reset_Target();
				}
				pinven->Get_Talk_list()->front()->setcheck(false);
				CInventory_Manager::Get_Instance()->Get_Talk_list()->front()->Set_WinonaTalk(false);
				break;
			}
		}
		else
		{//Solo
			switch (m_iTalkCnt)
			{
			case 1:
				pinven->Get_Talk_list()->front()->Set_Texnum1(4);
				break;
			case 2:
				pinven->Get_Talk_list()->front()->Set_Texnum1(5);
				break;
			case 3:
				if (m_bAccept)
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(6);
					m_bNextAct = false;
				}
				else
				{
					pinven->Get_Talk_list()->front()->Set_Texnum1(8);
					m_bNextAct = true;
				}
				break;
			case 4:
				m_bSelectAct = true;
				Clear_Activated();
				static_cast<CPlayer*>(m_pTarget)->Set_TalkMode(false);
				static_cast<CPlayer*>(m_pTarget)->Set_bOnlyActionKey(false);
				if (!m_bNextAct)
				{
					static_cast<CPlayer*>(m_pTarget)->Release_Party(TEXT("Winona"));
					m_bOwner = false;
					Reset_Target();
					m_pOwner = nullptr;

					CInventory_Manager* pinv = CInventory_Manager::Get_Instance();
					auto k = pinv->Get_Party_list();
					for (auto iter : *k)
					{
						if (iter->Get_check() == true && iter->Get_texnum() == 1)
						{
							iter->Set_check(false);
							iter->Set_texnum(3);
							break;
						}
					}
				}
				m_iTalkCnt = 0;
				m_bInteract = false; //Test
				m_bFirstCall = false;
				pinven->Get_Talk_list()->front()->setcheck(false);
				CInventory_Manager::Get_Instance()->Get_Talk_list()->front()->Set_WinonaTalk(false);
				break;
			}
		}
		m_iPreTalkCnt = m_iTalkCnt;
	}
}

void CWinona::Talk_Friend(_float _fTimeDelta)
{
	m_fInteractTIme += _fTimeDelta;

	m_eState = CNPC::TALK;

	if (m_ePreState != m_eState)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = true;
		Change_Texture(TEXT("Com_Texture_Talk"));
		m_ePreState = m_eState;
		static_cast<CPig*>(m_pTarget)->Interact(_fTimeDelta, 0);
		Play_Sound(_fTimeDelta);
	}

	if (2.f < m_fInteractTIme)
	{
		m_fInteractTIme = 0.f;
		m_bInteract = false;

		_int iRandNum = rand() % 2;
		if (iRandNum == 1)
		{
			static_cast<CPig*>(m_pTarget)->Interact(_fTimeDelta, 2);
			m_bNextAct = false;
		}
		else
		{
			static_cast<CPig*>(m_pTarget)->Interact(_fTimeDelta, 1);
			m_bNextAct = true;
		}
	}
}

void CWinona::Create_Bullet(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float3 vInitPos = Get_Position();
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Battery_Tower"), m_iCurrentLevelndex, TEXT("Skill"), &vInitPos)))
		return;

}

void CWinona::Create_Catapult(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CCatapult::CATAPULTDESC CatapultDesc;
	if (m_vecCatapults.size() >= 1)
	{
		SetPosForSkill(_fTimeDelta);
	}
	ZeroMemory(&CatapultDesc, sizeof(CatapultDesc));
	CatapultDesc.pOwner = this;
	CatapultDesc.vInitPos = m_vTargetPos;

	//Test
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Catapult"), m_iCurrentLevelndex, TEXT("Layer_Catapults"), &CatapultDesc)))
		return;
}

void CWinona::MoveWithOwner(_float _fTimeDelta)
{
	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (static_cast<CPawn*>(m_pTarget)->Get_ObjID() == OBJ_MONSTER && m_bFightMode)
	{
		m_pTransformCom->Go_PosTarget(_fTimeDelta, m_vTargetPos, _float3{ 0.f, 0.f, 0.f });
		if ((abs(vMyPos.x - m_vTargetPos.x) < 0.3 &&
			abs(vMyPos.z - m_vTargetPos.z) < 0.3))
		{
			m_bArrive = true;
			m_bDirChanged = false;
		}
	}
	else if (m_pTarget == m_pOwner)
	{
		m_pTransformCom->Go_PosTarget(_fTimeDelta, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
		if ((abs(vMyPos.x - m_fPatrolPosX) < 0.3 &&
			abs(vMyPos.z - m_fPatrolPosZ) < 0.3))
		{
			m_bArrive = true;
			m_bDirChanged = false;
		}
	}
}

void CWinona::MoveWithoutOwner(_float _fTimeDelta)
{
	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_pTarget)
	{
		if (m_bFightMode&&m_bCanSkill && m_vecCatapults.size() >= 1)
		{
			m_pTransformCom->Go_PosTarget(_fTimeDelta, m_vTargetPos, _float3{ 0.f, 0.f, 0.f });
			if ((abs(vMyPos.x - m_vTargetPos.x) < 0.01 &&
				abs(vMyPos.z - m_vTargetPos.z) < 0.01))
			{
				m_bArrive = true;
				m_bDirChanged = false;
			}
		}
		else
		{
			m_pTransformCom->Go_PosTarget(_fTimeDelta, m_vTargetPos, _float3{ 0.f, 0.f, 0.f });
			if ((abs(vMyPos.x - m_vTargetPos.x) < 0.3 &&
				abs(vMyPos.z - m_vTargetPos.z) < 0.3))
			{
				m_bArrive = true;
				m_bDirChanged = false;
			}
		}

	}
	else
	{
		m_pTransformCom->Go_PosTarget(_fTimeDelta, _float3(m_fPatrolPosX, Get_Position().y, m_fPatrolPosZ), _float3{ 0.f, 0.f, 0.f });
		if ((abs(vMyPos.x - m_fPatrolPosX) < 0.3 &&
			abs(vMyPos.z - m_fPatrolPosZ) < 0.3))
		{
			m_bArrive = true;
			m_bDirChanged = false;
		}
	}
}

void CWinona::SetPosForSkill(_float _fTimeDelta)
{
	_float3 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_bool bLoof = true;
	_bool bIsTarget = false;
	while (bLoof)
	{
		_float fOffsetX = _float(rand() % 2) *1.f + _float(rand() % 10) *0.1f;
		_uint bSignX = rand() % 2;
		_float fOffsetZ = _float(rand() % 2) *1.f + _float(rand() % 10) *0.1f;
		_uint bSignZ = rand() % 2;

		fOffsetX = bSignX ? fOffsetX * -1.f : fOffsetX;
		fOffsetZ = bSignZ ? fOffsetZ * -1.f : fOffsetZ;
		m_vTargetPos = _float3(vMyPos.x + fOffsetX, vMyPos.y, vMyPos.z + fOffsetZ);

		for (_uint i = 0; i < m_vecCatapults.size(); i++)
		{
			_float3 fPos = m_vecCatapults[i]->Get_Position();

			_float Compare_Range = (fPos.x - m_vTargetPos.x)*(fPos.x - m_vTargetPos.x)
				+ (fPos.y - m_vTargetPos.y)*(fPos.y - m_vTargetPos.y)
				+ (fPos.z - m_vTargetPos.z)*(fPos.z - m_vTargetPos.z);

			if (Compare_Range < 1.f)
			{
				bIsTarget = false;
				break;
			}
			else
			{
				bIsTarget = true;
			}
		}

		if (!bIsTarget)
		{
			continue;
		}
		else
		{
			bLoof = false;
		}
	}





}

_bool CWinona::Setup_LevelChange(_float _fTimeDelta)
{
	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();


	

	if ((LEVEL)m_iCurrentLevelndex == LEVEL_GAMEPLAY && m_bDead && m_fReviveTime > 10.f)
	{
		m_bCanTalk = true;
		m_bDead = false;
		Clear_Activated();
		Reset_Target();
	}

	if (m_iCurrentLevelndex != m_iPreLevelIndex)
	{
		if (m_bOwner)
		{
			_float3 Owner_Pos = static_cast<CPlayer*>(m_pOwner)->Get_Pos();
			Owner_Pos.x -= 2.f;
			Owner_Pos.z += 2.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Owner_Pos);

			if (m_bDead && (LEVEL)m_iCurrentLevelndex == LEVEL_GAMEPLAY)
			{
				m_bDead = false;
				Clear_Activated();
				Reset_Target();
			}

			for (auto& iter = m_vecCatapults.begin(); iter != m_vecCatapults.end();)
			{
				Safe_Release(*iter);
				iter++;
			}
			m_vecCatapults.clear();
			m_bCanTalk = true;

			MINIMAP		minidesc;
			ZeroMemory(&minidesc, sizeof(MINIMAP));
			minidesc.name = MIN_WINONA;
			minidesc.pointer = this;
			LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), CurrentLevelndex, TEXT("MiniMap_Icon"), &minidesc);
		}
		else
		{
			m_bCanTalk = true;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(40.f, 0.5f, 27.f));
			Clear_Activated();
			Reset_Target();
			if (m_iCurrentLevelndex == LEVEL_GAMEPLAY)
			{
				MINIMAP		minidesc;
				ZeroMemory(&minidesc, sizeof(MINIMAP));
				minidesc.name = MIN_WINONA;
				minidesc.pointer = this;
				LEVEL CurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MiniMap_Icon"), CurrentLevelndex, TEXT("MiniMap_Icon"), &minidesc);
			}
			
		}

		

		m_iPreLevelIndex = m_iCurrentLevelndex;
	}

	if (m_iCurrentLevelndex == LEVEL_LOADING)
	{
		m_iPreLevelIndex = m_iCurrentLevelndex;
		return false;
	}

	if (m_iCurrentLevelndex != LEVEL_GAMEPLAY && !m_bOwner)
	{
		m_bCanTalk = false;
		m_iPreLevelIndex = m_iCurrentLevelndex;
		return false;
	}


	return true;
}

DIR_STATE CWinona::Check_Direction(void)
{//내 자신의 Look에서 3번 변환한 값
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
	_float3 vTargetLook = m_vTargetPos - Get_Pos();

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

void CWinona::Find_Priority()
{

	int i = rand() % 5;
	switch (i)
	{
	case 1:
		Find_Friend();
		break;
	case 2:
		Find_Berry();
		break;
	case 0:
	case 3:
	case 4:
		Set_RandPos(0.f);
		break;
	}
}

void CWinona::Find_Friend()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Monster"));

	if (list_Obj == nullptr)
		return;

	_uint iIndex = 0;
	Reset_Target();
	m_pTarget = nullptr;
	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if ((*iter_Obj) != nullptr && !dynamic_cast<CMonster*>(*iter_Obj)->Get_Aggro()
			&& dynamic_cast<CMonster*>(*iter_Obj)->Get_MonsterID() == CMonster::MONSTER_ID::PIG)
		{
			if (m_pTarget == nullptr)
			{
				m_pTarget = *iter_Obj;
				m_bSelectAct = false;
			}

			_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
				+ (Get_Pos().y - (*iter_Obj)->Get_Position().y)*(Get_Pos().y - (*iter_Obj)->Get_Position().y)
				+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

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
		else
		{
			++iIndex;
			iter_Obj++;
			continue;
		}
	}
	Safe_AddRef(m_pTarget);
	Safe_Release(pGameInstance);
}

void CWinona::Find_Enemy()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Monster"));

	if (list_Obj == nullptr)
		return;

	_uint iIndex = 0;
	Reset_Target();
	m_pTarget = nullptr;
	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if ((*iter_Obj) != nullptr && dynamic_cast<CMonster*>(*iter_Obj)->Get_Aggro())
		{
			_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
				+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

			if (fCmpDir > m_fDetectRange)
			{
				++iIndex;
				iter_Obj++;
				continue;
			}

			if (m_pTarget == nullptr)
			{
				m_pTarget = *iter_Obj;
				m_bSelectAct = false;
			}

			_float fTargetDir = (Get_Pos().x - (m_pTarget)->Get_Position().x)*(Get_Pos().x - (m_pTarget)->Get_Position().x)
				+ (Get_Pos().z - (m_pTarget)->Get_Position().z)*(Get_Pos().z - (m_pTarget)->Get_Position().z);

			if (fCmpDir < fTargetDir)
			{
				m_pTarget = *iter_Obj;
			}

			++iIndex;
			iter_Obj++;
		}
		else
		{
			++iIndex;
			iter_Obj++;
			continue;
		}
	}

	if (m_pTarget == nullptr)
	{
		m_bFightMode = false;
		Safe_Release(pGameInstance);
		return;
	}
	Safe_AddRef(m_pTarget);
	Safe_Release(pGameInstance);
}

void CWinona::Find_Berry()
{//후에 Primary_queue로 각 레이어들중에서 가장 가까운 객체를 m_pTarget
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(m_iCurrentLevelndex, TEXT("Layer_Object"));

	if (list_Obj == nullptr)
		return;

	_uint iIndex = 0;
	Reset_Target();
	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		if ((*iter_Obj) != nullptr && dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_CanInteract()
			&& dynamic_cast<CInteractive_Object*>(*iter_Obj)->Get_InteractName() == INTERACTOBJ_ID::BOULDER)
		{

			if (m_pTarget == nullptr)
			{
				m_pTarget = *iter_Obj;
				m_bSelectAct = false;
			}

			_float fCmpDir = (Get_Pos().x - (*iter_Obj)->Get_Position().x)*(Get_Pos().x - (*iter_Obj)->Get_Position().x)
				+ (Get_Pos().y - (*iter_Obj)->Get_Position().y)*(Get_Pos().y - (*iter_Obj)->Get_Position().y)
				+ (Get_Pos().z - (*iter_Obj)->Get_Position().z)*(Get_Pos().z - (*iter_Obj)->Get_Position().z);

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
		else
		{
			++iIndex;
			iter_Obj++;
			continue;
		}
	}
	if (m_pTarget == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_AddRef(m_pTarget);
	Safe_Release(pGameInstance);
}

void CWinona::Find_Player()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Reset_Target();
	m_pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	Safe_AddRef(m_pTarget);
	Safe_Release(pGameInstance);
}

void CWinona::CatapultCheck(void)
{
	for (auto& iter = m_vecCatapults.begin(); iter != m_vecCatapults.end();)
	{
		if (*iter == nullptr)
			iter = m_vecCatapults.erase(iter);
		else
		{
			iter++;
		}
	}

	if (m_vecCatapults.size() >= 3)
	{
		m_bCanSkill = false;
	}
}

CWinona * CWinona::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{	
	CWinona* pInstance = new CWinona(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWendy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWinona::Clone(void * pArg)
{
	CWinona* pInstance = new CWinona(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWendy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWinona::Free()
{
	__super::Free();

	Safe_Release(BehaviorTree);
}
