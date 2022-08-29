#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "KeyMgr.h"
#include "Inventory.h"
#include "Equip_Animation.h"
#include "Bullet.h"
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

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Equipment"), LEVEL_GAMEPLAY, TEXT("Layer_Equip"), nullptr)))
		return E_FAIL;

	m_Equipment = (CEquip_Animation*)pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Equip"));

	Safe_Release(pGameInstance);

	//Test
	Change_Texture(TEXT("Com_Texture_Idle_Down"));

	return S_OK;
}

int CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	GetKeyDown(fTimeDelta);
	Move_to_PickingPoint(fTimeDelta);
	WalkingTerrain();

	Create_Bullet();

	m_Equipment->Set_TargetPos(Get_Pos());
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return OBJ_NOEVENT;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Add_CollisionGroup(CCollider::COLLISION_PLAYER, this);

	
	Test_Debug(fTimeDelta);
	//���Ŀ� ������ ���������?�÷��̾ �����۰� �������?ȹ���ϴ� ��ȣ�ۿ��� ��ģ�Ŀ� �κ��丮�� �����°� �Ʒ��ڵ� �״��?���ø� �۵��մϴ�!!
	//#include "Inven.h" �����Ͻð�
	/*CInventory_Manager*			pInventory_Manager = CInventory_Manager::Get_Instance();

	auto Maininvenlist = pInventory_Manager->Get_Inven_list();

	if (m_pColliderCom->Collision_with_Group(CCollider::COLLISION_ITEM, this) && (GetKeyState(VK_SPACE) < 0))
	{
	for (auto iter = Maininvenlist->begin(); iter != Maininvenlist->end();)
	{
	if (!(*iter)->get_check())
	{
	(*iter)->set_texnum(2); //���Ŀ� ������enum ��������ʹ�?���ڴ��?���ϴ� ������ �־��ּ���
	(*iter)->set_check(true);

	return;
	}
	else
	++iter;

	}*/

}


_float3 CPlayer::Get_Pos()
{
	return (m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

_float3 CPlayer::Get_Look()
{
	return (m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CPlayer::Move_to_PickingPoint(_float fTimedelta)
{
	/* Picking ����Ʈ�� �������� �ʰų�, �̹� �����ߴٸ� return*/
	if (m_bPicked == false || m_bArrive == true || m_bInputKey == true)
		return;

	//m_pTransformCom->Follow_Target(m_vPickingPoint, _float3(0,1,0));
	m_pTransformCom->LookAt(m_vPickingPoint);
	m_pTransformCom->Go_Straight(fTimedelta);

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
	//Test
	Debug_Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Player"))))
		return E_FAIL;

	m_TimerTag = TEXT("Timer_Player");

	Safe_Release(pGameInstance);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
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
	TransformDesc.InitPos = _float3(10.f, 2.f, 5.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if(Test_Setup())
		return E_FAIL;

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

	m_KeySets[INTERACTKEY::KEY_CAMLEFT] = 'q';

	m_KeySets[INTERACTKEY::KEY_CAMRIGHT] = 'e';

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

HRESULT CPlayer::Test_Setup()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_DebugBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pDebugBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	TransformDesc.InitPos = _float3(10.f, 2.f, 5.f);

	if (FAILED(__super::Add_Components(TEXT("Com_DebugTransform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pDebugTransformCom, &TransformDesc)))
		return E_FAIL;

	m_pDebugTransformCom->Set_Scale(0.5f, 0.5f, 1.f);
	//TEXTURE
	CTexture::TEXTUREDESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 1;
	TextureDesc.m_fSpeed = 60;
	/*Debug*/
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Debug"), LEVEL_STATIC, TEXT("Prototype_Component_DebugLine"), (CComponent**)&m_pDebugTextureCom, &TextureDesc)))
		return E_FAIL;

	Set_Radius(0.25f);

	return S_OK;
}

void CPlayer::GetKeyDown(_float _fTimeDelta)
{
	if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_DEBUG]))
	{
		m_bDebugKey = !m_bDebugKey;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_INVEN1]))
	{
		Test_Func(1);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_INVEN2]))
	{
		Test_Func(2);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_INVEN3]))
	{
		Test_Func(3);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_INVEN4]))
	{
		Mining(_fTimeDelta);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_INVEN5]))
	{
		Chop(_fTimeDelta);
	}


	if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_UP]))
	{
		m_bInputKey = true;
		m_bPicked = false;
		Move_Up(_fTimeDelta);
		
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_RIGHT]))
	{
		m_bInputKey = true;
		m_bPicked = false;
		Move_Right(_fTimeDelta);
	
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_DOWN]))
	{
		m_bInputKey = true;
		m_bPicked = false;
		Move_Down(_fTimeDelta);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_LEFT]))
	{
		m_bInputKey = true;
		m_bPicked = false;
		Move_Left(_fTimeDelta);
	
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing(m_KeySets[INTERACTKEY::KEY_ATTACK]))
	{
		m_bInputKey = true;
		m_bPicked = false;
		Attack(_fTimeDelta);	
	}
	else if (CKeyMgr::Get_Instance()->Key_Down(m_KeySets[INTERACTKEY::KEY_ACTION]))
	{
		Multi_Action(_fTimeDelta);
	}
	else
	{
		if (!m_bPicked)
		{
			Move_Idle(_fTimeDelta);
		}
	}
}

bool CPlayer::ResetAction(_float _fTimeDelta)
{
	if (m_eState == ACTION_STATE::ATTACK || m_eState == ACTION_STATE::CHOP || m_eState == ACTION_STATE::MINING)
	{
		if (m_pTextureCom->Get_Frame().m_iCurrentTex == m_pTextureCom->Get_Frame().m_iEndTex - 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void CPlayer::Move_Idle(_float _fTimeDelta)
{//���ǹ����� �����ʸ��콺�� �׼� ���ൿ�ȿ��� ���ϵ��� �ؾ���.

	if (!ResetAction(_fTimeDelta))
		return;

	//if(m_eState == ACTION_STATE::ATTACK || m_eState == ACTION_STATE::AXE || m_eState == ACTION_STATE:: MINING)
	m_eState = ACTION_STATE::IDLE;
	m_Equipment->Set_ActionState((_uint)ACTION_STATE::IDLE);
	if (m_ePreState != m_eState)
	{
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::IDLE);
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
		m_ePreState = m_eState;
	}
}

void CPlayer::Move_Up(_float _fTimeDelta)
{
	if (m_bInputKey)
	{
		m_pTransformCom->Go_Straight(_fTimeDelta, m_fTerrain_Height);
	}
	

	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_UP;
	if (m_ePreState != m_eState
		|| m_ePreDirState != m_eDirState)
	{

		Change_Texture(TEXT("Com_Texture_Run_Up"));
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
		m_pTransformCom->Go_Right(_fTimeDelta, m_fTerrain_Height);
	}
	
	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_RIGHT;
	if (m_ePreState != m_eState || m_ePreDirState != m_eDirState)
	{
		if (m_ePreDirState != m_eDirState)
		{
			m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
			//m_bInverseScale = false;
		}
		/*	if (m_bInverseScale)
		{
		m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
		m_bInverseScale = false;
		}*/
		Change_Texture(TEXT("Com_Texture_Run_Side"));
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
		m_pTransformCom->Go_Backward(_fTimeDelta, m_fTerrain_Height);
	}
	
	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_DOWN;

	if (m_ePreState != m_eState || m_ePreDirState != m_eDirState)
	{
		Change_Texture(TEXT("Com_Texture_Run_Down"));
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
		m_pTransformCom->Go_Right(_fTimeDelta, m_fTerrain_Height);
	}	
	m_eState = ACTION_STATE::MOVE;
	m_eDirState = DIR_STATE::DIR_LEFT;

	if (m_ePreState != m_eState || m_ePreDirState != m_eDirState)
	{

		if (m_ePreDirState != m_eDirState)
		{
			m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
			//m_bInverseScale = false;
		}
		/*if (!m_bInverseScale)
		{
		m_pTransformCom->Set_Scale(-1.f, 1.f, 1.f);
		m_bInverseScale = true;
		}*/
		Change_Texture(TEXT("Com_Texture_Run_Side"));

		m_ePreState = m_eState;
		m_ePreDirState = m_eDirState;

		m_Equipment->Set_Directon(m_eDirState);
		m_Equipment->Set_ActionState((_uint)ACTION_STATE::MOVE);
	}
}

void CPlayer::Attack(_float _fTimeDelta)
{
	m_eState = ACTION_STATE::ATTACK;

	//���� ���� Ȯ���ڵ� �����?

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
}

void CPlayer::Multi_Action(_float _fTimeDelta)
{
	Find_Priority();

	if (m_pTarget != nullptr)
	{
		Set_PickingPoint(m_pTarget->Get_Position());
	}

}

void CPlayer::Create_Bullet()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	
	
	if (m_eState == ACTION_STATE::ATTACK)
	{
		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		BulletData.eDirState = m_eDirState;
		BulletData.eWeaponType = m_eWeaponType;
		BulletData.vPosition = Get_Pos();
		switch (m_eWeaponType)
		{
		case WEAPON_TYPE::WEAPON_HAND:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 10)
			{	
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), LEVEL_GAMEPLAY, TEXT("Bullet"), &BulletData)))
					return;
			}
				
			break;
		case WEAPON_TYPE::WEAPON_SWORD:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 20)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), LEVEL_GAMEPLAY, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		case WEAPON_TYPE::WEAPON_STAFF:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 20)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), LEVEL_GAMEPLAY, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		case WEAPON_TYPE::WEAPON_DART:
			if (m_pTextureCom->Get_Frame().m_iCurrentTex == 17)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), LEVEL_GAMEPLAY, TEXT("Bullet"), &BulletData)))
					return;
			}
			break;
		}
	}
}

void CPlayer::Detect_Enemy(void)
{
	/*�ݰ� ���� ��ü ������Ʈ�� �˻��ϱ�. */
}

void CPlayer::Find_Priority()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* list_Obj = pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Object"));

	_uint iIndex = 0;

	for (auto& iter_Obj = list_Obj->begin(); iter_Obj != list_Obj->end();)
	{
		//���߿� Detect����& ���?���� ������ ��.
		if (iIndex == 0)
		{
			m_pTarget = *iter_Obj;
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


	Safe_Release(pGameInstance);
}

void CPlayer::Test_Func(_int _iNum)
{
	switch (_iNum)
	{
	case 1:
		m_eWeaponType = WEAPON_TYPE::WEAPON_SWORD;
		break;
	case 2:
		m_eWeaponType = WEAPON_TYPE::WEAPON_DART;
		break;
	case 3:
		m_eWeaponType = WEAPON_TYPE::WEAPON_STAFF;
		break;
	}
	m_Equipment->Set_WeaponType(m_eWeaponType);
}

void CPlayer::Test_Detect(_float fTImeDelta)
{
	
}

void CPlayer::Test_Debug(_float fTimeDelta)
{
	if (!m_bDebugKey)
		return;

	m_pDebugTransformCom->Set_State(CTransform::STATE_POSITION, Get_Pos());

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	fTimeAcc += fTimeDelta;
	if (fTimeAcc > 1.f && m_pColliderCom->Collision_with_Group(CCollider::COLLISION_MONSTER, this))
	{
		m_tStat.fCurrentHealth -= 5.f;

	}

	if (fTimeAcc > 2.f)
	{
		//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
		printf("%f\n ", m_tStat.fCurrentHealth);
		fTimeAcc = 0.f;
	}

}

void CPlayer::Debug_Render(void)
{
	if (!m_bDebugKey)
		return;

	m_pDebugTransformCom->Bind_OnGraphicDev();
	if (FAILED(m_pDebugTextureCom->Bind_OnGraphicDev(0)))
		return;

	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pDebugBufferCom->Render();

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

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
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Run_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 66;
	TextureDesc.m_fSpeed = 60;
	/*Idle*/
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Punch*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 24;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Punch_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Punch_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Punch_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Punch_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Attack_Weapon*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 36;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Weapon_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Weapon_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Weapon_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Weapon_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Attack_Dart*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dart_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dart_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dart_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Dart_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Mining*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 29;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Mining_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Mining_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Mining_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Mining_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	/*Chop*/
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iEndTex = 39;
	TextureDesc.m_fSpeed = 60;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Chop_Up"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Up"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Chop_Down"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Down"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Chop_Side"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Chop_Side"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

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
	if (m_eDirState == DIR_STATE::DIR_LEFT)
		return;
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CPlayer::WalkingTerrain()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
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

CGameObject * CPlayer::Clone_Load(const _tchar * VIBufferTag, void * pArg)
{
	return nullptr;
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

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}
