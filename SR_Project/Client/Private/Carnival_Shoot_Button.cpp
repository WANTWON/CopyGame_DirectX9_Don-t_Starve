#include "stdafx.h"
#include "..\Public\Carnival_Shoot_Button.h"
#include "GameInstance.h"
#include "Player.h"
#include "CameraManager.h"
#include "Carnival_Shooter.h"
#include "Shooting_Target.h"
#include "Level_Maze.h"

CCarnival_Shoot_Button::CCarnival_Shoot_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarnival_Shoot_Button::CCarnival_Shoot_Button(const CCarnival_Shoot_Button & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCarnival_Shoot_Button::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnival_Shoot_Button::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::COOKPOT;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	m_eShaderID = SHADER_IDLE;

	return S_OK;
}

int CCarnival_Shoot_Button::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarnival_Shoot_Button::Late_Tick(_float fTimeDelta)
{
	// __super::Late_Tick(fTimeDelta); // CInteractiveObject are RENDER_NONALPHABLEND, but this need to be RENDER_ALPHABLEND

	SetUp_BillBoard();

	Compute_CamDistance(Get_Position());
	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == true)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	if (m_pColliderCom)
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	Change_Camera();
	Change_Motion();
	Change_Frame(fTimeDelta);
}

HRESULT CCarnival_Shoot_Button::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCarnival_Shoot_Button::Interact(_uint Damage)
{

	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_MAZE, TEXT("Layer_Shooter"));

	dynamic_cast<CCarnival_Shooter*>(pGameObject)->Set_bShoot(true);
	m_eState = PRESS;
	m_bInteract = false;
}


HRESULT CCarnival_Shoot_Button::SetUp_Components(void* pArg)
{
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

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

void CCarnival_Shoot_Button::Change_Camera()
{
	if (m_bFinished)
		return;

	if (CGameInstance::Get_Instance()->Is_In_Frustum(Get_Position(), m_fRadius) == false)
		return;
	
	CCameraManager* pCameraManager = CCameraManager::Get_Instance();
	if (Check_EveryTarget_Complete() == true)
	{
		CLevel* pLevel = CLevel_Manager::Get_Instance()->Get_CurrentLevel();
		dynamic_cast<CLevel_Maze*>(pLevel)->Set_PuzzleSolved(true);
		m_bFinished = true;
		if (pCameraManager->Get_CamState() == CCameraManager::CAM_TARGET)
		{
			CGameInstance::Get_Instance()->StopSound(SOUND_BGM);
			_tchar	szFullPath[MAX_PATH] = TEXT("Bell_Win_Carnivalgame_Herding_Station_DST_%02d.wav");
			_uint i = rand() % 7 + 1;
			wsprintf(szFullPath, szFullPath, i);
			CGameInstance::Get_Instance()->PlaySounds(szFullPath, SOUND_OBJECT, 0.5f);
			
			CCamera* pCamera = pCameraManager->Get_CurrentCamera();
			dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(false);
		}
		return;
	}

	CGameInstance* pGameInstace = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstace);
	CGameObject* pGameObject = pGameInstace->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	_float vDistance = D3DXVec3Length(&(Get_Position() - pGameObject->Get_Position()));

	if (vDistance < 1.f)
	{
		pCameraManager->Set_CamState(CCameraManager::CAM_TARGET);
		CGameInstance::Get_Instance()->PlayBGM(TEXT("DST_PirateMonkeys_accordian_V2 (1).wav"), 0.4f);
		CGameObject* pShooter = pGameInstace->Get_Object(LEVEL_MAZE, TEXT("Layer_Shooter"));
		_float3 vShooterPos = pShooter->Get_Position();
		vShooterPos.z += 2.f;
		CCamera* pCamera = pCameraManager->Get_CurrentCamera();
		dynamic_cast<CCameraTarget*>(pCamera)->Set_OffSetDistance(_float3(0.f, 7.f, -6.f));
		dynamic_cast<CCameraTarget*>(pCamera)->Set_Position(vShooterPos);
		dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(true);
	}
	else
	{
		if (pCameraManager->Get_CamState() == CCameraManager::CAM_TARGET)
		{
			CCamera* pCamera = pCameraManager->Get_CurrentCamera();
			dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(false);
		}
	}

	Safe_Release(pGameInstace);
}

_bool CCarnival_Shoot_Button::Check_EveryTarget_Complete()
{
	CGameInstance* pGameInstace = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstace);
	list<CGameObject*>* pGameObjects = pGameInstace->Get_ObjectList(LEVEL_MAZE, TEXT("Layer_Shooting"));
	Safe_Release(pGameInstace);
	if (pGameObjects == nullptr)
		return false;

	for (auto& iter : *pGameObjects)
	{
		if (dynamic_cast<CShooting_Target*>(iter)->Get_Complete() == false)
			return false;

	}

	return true;
}

HRESULT CCarnival_Shoot_Button::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 15;

	TextureDesc.m_iEndTex = 5;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Shooting_Button_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 2;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Press"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Shooting_Button_Press"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	return S_OK;
}

void CCarnival_Shoot_Button::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case PRESS:
		if (m_pTextureCom->MoveFrame(m_TimerTag, false) == true)
		{
			m_eState = IDLE;
			m_bInteract = true;
		}
		break;
	case IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	}
}

void CCarnival_Shoot_Button::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case PRESS:
			CGameInstance::Get_Instance()->PlaySounds(TEXT("carnivalgame_shooter_place.wav"), SOUND_OBJECT, 0.4f);
			Change_Texture(TEXT("Com_Texture_Press"));
			break;
		case IDLE:
			Change_Texture(TEXT("Com_Texture_Idle"));
			break;
		}

		m_ePreState = m_eState;
	}
}




CCarnival_Shoot_Button* CCarnival_Shoot_Button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarnival_Shoot_Button* pInstance = new CCarnival_Shoot_Button(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCarnival_Shoot_Button"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarnival_Shoot_Button::Clone(void* pArg)
{
	CCarnival_Shoot_Button* pInstance = new CCarnival_Shoot_Button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarnival_Shoot_Button"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarnival_Shoot_Button::Free()
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