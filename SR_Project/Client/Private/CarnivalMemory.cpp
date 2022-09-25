#include "stdafx.h"
#include "CarnivalMemory.h"
#include "GameInstance.h"
#include "Player.h"
#include "Item.h"
#include "PickingMgr.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Inventory.h"
#include "DecoObject.h"
#include "Carnival_Egg.h"
#include "CameraManager.h"
#include "Level_Maze.h"

CCarnivalMemory::CCarnivalMemory(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarnivalMemory::CCarnivalMemory(const CCarnivalMemory & rhs)
	: CInteractive_Object(rhs)
{
}

HRESULT CCarnivalMemory::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnivalMemory::Initialize(void* pArg)
{
	ZeroMemory(&m_tStationDesc, sizeof(STATIONDESC));
	memcpy(&m_tStationDesc, (STATIONDESC*)pArg, sizeof(STATIONDESC));

	if (FAILED(__super::Initialize(m_tStationDesc.vInitPosition)))
		return E_FAIL;

	m_eObjID = OBJID::OBJ_OBJECT;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::COOKPOT;

	if (m_tStationDesc.eType == STATIONTYPE::STATION_MEMORY)
		m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_BIRD)
		m_pTransformCom->Set_Scale(1.3f, 1.3f, 1.f);
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_EGG)
	{
		m_pTransformCom->Set_Scale(1.3f, 2.5f, 1.f);
		m_eState = PLACE;
		m_dwEggSpawnTime = GetTickCount();
	}

	return S_OK;
}

int CCarnivalMemory::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bConstruct)
	{
		_float3 vPickingPos = CPickingMgr::Get_Instance()->Get_PickingPos();
		vPickingPos.y += m_fRadius;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPickingPos);
	}

	if (m_tStationDesc.eType == STATIONTYPE::STATION_MEMORY)
		Play_Memory(fTimeDelta);
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_BIRD)
		Play_Bird(fTimeDelta);
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_EGG)
		Play_Egg(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CameraChange();
	return OBJ_NOEVENT;
}

void CCarnivalMemory::Late_Tick(_float fTimeDelta)
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

	if (!m_bPicking && !CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
	{
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);
		m_bPicking = true;
	}

	Change_Motion();
	Change_Frame(fTimeDelta);
	
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));


	if (pGameObject->Get_Dead())
		m_eShaderID = SHADER_DEAD;
	else 
		m_eShaderID = SHADER_DARK;
}

HRESULT CCarnivalMemory::Render()
{

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix, PlayerMatrix;

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return E_FAIL;

	PlayerMatrix = *D3DXMatrixTranspose(&WorldMatrix, &pTransform_Terrain->Get_WorldMatrix());

	_float3  fPlayerPosition = pTarget->Get_Position();
	m_pShaderCom->Set_RawValue("g_PlayerPosition", &fPlayerPosition, sizeof(_float3));
	m_pShaderCom->Set_RawValue("g_PlayerWorldMatrix", &PlayerMatrix, sizeof(_float4x4));

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCarnivalMemory::Interact(_uint Damage)
{
	if (m_tStationDesc.eType == STATIONTYPE::STATION_MEMORY)
	{
		m_vCamSettingDistanceOffset = _float3(0.f, 5.f, -5.f);
		m_vCamSettingPosOffset = _float3(0.f, 0.f, 2.f);
		m_fCameraSettingDistance = 5.f;
		m_eState = STATESTATION::HIT;

		// Play Sound
		CGameInstance::Get_Instance()->PlaySounds(TEXT("Turn_On_Carnivalgame_Memory.wav"), SOUND_ID::SOUND_OBJECT, .8f);

		Start_Memory();
	}
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_BIRD)
	{
		m_fCameraSettingDistance = 5.f;
		m_vCamSettingPosOffset = _float3(0.f, 0.f, 1.f);
		m_vCamSettingDistanceOffset = _float3(0.f, 7.f, -6.f);
		m_eState = STATESTATION::TURN_ON;

		// Play Sound
		CGameInstance::Get_Instance()->PlaySounds(TEXT("carnival_feedbirds_station_turnon.wav"), SOUND_ID::SOUND_OBJECT, .8f);

		Start_Bird();
	}
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_EGG)
	{
		m_fCameraSettingDistance = 5.f;
		m_vCamSettingPosOffset = _float3(0.f, 0.f, 1.f);
		m_vCamSettingDistanceOffset = _float3(0.f, 7.f, -7.f);
		m_eState = STATESTATION::TURN_ON;
		Start_Egg();
		m_bStart = true;
	}

	m_bInteract = false;
	CameraChange();
}

void CCarnivalMemory::CameraChange()
{
	if (m_bIsWin || m_bInteract == true)
		return;

	CCameraManager* pCameraManager = CCameraManager::Get_Instance();
	CGameInstance* pGameInstace = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstace);
	CGameObject* pGameObject = pGameInstace->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	_float vDistance = D3DXVec3Length(&(Get_Position() - pGameObject->Get_Position()));

	if (vDistance < m_fCameraSettingDistance)
	{
		pCameraManager->Set_CamState(CCameraManager::CAM_TARGET);
		_float3 vZoomPos = Get_Position();
		vZoomPos -= m_vCamSettingPosOffset;
		CCamera* pCamera = pCameraManager->Get_CurrentCamera();
		dynamic_cast<CCameraTarget*>(pCamera)->Set_OffSetDistance(m_vCamSettingDistanceOffset);
		dynamic_cast<CCameraTarget*>(pCamera)->Set_Position(vZoomPos);
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

void CCarnivalMemory::Start_Memory()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();

	list<CGameObject*>* lObjects = pGameInstance->Get_ObjectList(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Object"));
	if (!lObjects)
		return;

	_uint iHintCounter = 0;

	// Populate Card Vector
	for (auto& iter = lObjects->begin(); iter != lObjects->end(); ++iter)
	{
		CCarnivalCard* pCard = dynamic_cast<CCarnivalCard*>(*iter);
		if (pCard && pCard->Get_Desc().eType == CCarnivalCard::TYPE::CARD)
		{
			pCard->Set_Memory(this);

			m_vecCards.push_back(pCard);
			m_vecHints.push_back(iHintCounter);
			iHintCounter++;
		}
	}

	// Turn On Cards
	for (auto pCard : m_vecCards)
		if (pCard)
			pCard->Turn_On();

	m_bCanPlay = true;
}

void CCarnivalMemory::Play_Memory(_float fTimeDelta)
{
	if (m_bCanPlay)
	{
		if (!m_bIsRoundActive && m_fGameTimer > 3.f)
			Start_Round(fTimeDelta);
		else if (m_bIsRoundActive)
		{
			Check_Guesses();
			
			if (m_bShouldResetRound)
				Reset_Round();
		}
		else
			m_fGameTimer += fTimeDelta;
	}
	else
		m_fGameTimer += fTimeDelta;
}

void CCarnivalMemory::Start_Round(_float fTimeDelta)
{
	random_shuffle(m_vecHints.begin(), m_vecHints.end());
	_uint iGoodHints = Get_Round_Good_Hint();

	for (auto i = 0; i < m_vecCards.size(); ++i)
	{
		if (m_vecCards[i])
		{
			m_vecCards[i]->Give_Hint(m_vecHints[i] < iGoodHints ? true : false);
			m_vecCards[i]->Set_Interact(true);
		}
	}

	// Play Sound
	_tchar szFileName[MAX_PATH] = TEXT("");
	wsprintf(szFileName, TEXT("Begin_Round_Carnivalgame_Memory_%02d.wav"), rand() % 3 + 1);
	CGameInstance::Get_Instance()->PlaySounds(szFileName, SOUND_ID::SOUND_OBJECT, .8f);

	m_bIsRoundActive = true;
}

void CCarnivalMemory::Check_Guesses()
{ 
	for (auto bGuess : m_vecGuesses)
	{
		if (!bGuess)
		{
			m_bShouldResetRound = true;

			// Lose Round
			m_eState = STATESTATION::HIT;
			m_vecGuesses.clear();
			return;
		}
	}

	if (m_vecGuesses.size() >= Get_Round_Good_Hint())
	{
		m_bShouldResetRound = true;

		// Win Round
		m_eState = STATESTATION::TURN_ON;
		m_vecGuesses.clear();
		m_iTurnCount--;

		// Win Game
		if (m_iTurnCount == 0)
		{
			CLevel* pLevel = CLevel_Manager::Get_Instance()->Get_CurrentLevel();
			dynamic_cast<CLevel_Maze*>(pLevel)->Set_PuzzleSolved(true);

			m_eState = STATESTATION::WIN;
			m_bCanPlay = false;
			m_bIsWin = true;
			CCameraManager* pCameraManager = CCameraManager::Get_Instance();
			if (pCameraManager->Get_CamState() == CCameraManager::CAM_TARGET)
			{
				CCamera* pCamera = pCameraManager->Get_CurrentCamera();
				dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(false);
			}
			// Spawn Confetti Effect
			CDecoObject::DECODECS DecoDesc;
			DecoDesc.m_eState = CDecoObject::PARTY;
			DecoDesc.vInitPosition = Get_Position();
			DecoDesc.vInitPosition.y += 1.5f;
			CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);
		}
	}
}

void CCarnivalMemory::Reset_Round()
{
	m_bIsRoundActive = false;
	m_bShouldResetRound = false;
	m_fGameTimer = 0.f;

	for (auto& pCard : m_vecCards)
		pCard->Set_Interact(false);
}

_uint CCarnivalMemory::Get_Round_Good_Hint()
{
	switch (m_iTurnCount)
	{
	case 5:
	case 4:
		return 1;
	case 3:
	case 2:
		return 2;
	case 1:
		return 3;
	}
}

_uint CCarnivalMemory::Get_Hungry_Max()
{
	if (m_iFedGoal <= 5)
		return 3;
	else if (m_iFedGoal <= 10)
		return 2;
	else if (m_iFedGoal <= 15)
		return 1;
}

void CCarnivalMemory::Start_Bird()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();

	list<CGameObject*>* lObjects = pGameInstance->Get_ObjectList(pLevelManager->Get_CurrentLevelIndex(), TEXT("Layer_Object"));
	if (!lObjects)
		return;

	// Populate Card Vector
	for (auto& iter = lObjects->begin(); iter != lObjects->end(); ++iter)
	{
		CCarnivalCard* pCard = dynamic_cast<CCarnivalCard*>(*iter);
		if (pCard && pCard->Get_Desc().eType == CCarnivalCard::TYPE::BIRD)
		{
			m_vecCards.push_back(pCard);
			pCard->Set_Memory(this);
			pCard->Turn_On();
			pCard->Set_Interact(true);
		}
	}

	m_bCanPlay = true;
}

void CCarnivalMemory::Play_Bird(_float fTimeDelta)
{
	if (!m_bCanPlay)
		return;

	// Play Game
	if (m_iFedGoal > 0)
	{
		// Check how many Hungry Birds there are
		_uint iHungryCounter = 0;
		for (auto& pBird : m_vecCards)
		{
			_bool bIsHungry = pBird->Get_IsHungry();
			if (bIsHungry)
				iHungryCounter++;
		}

		// Iterate through Birds and set them Hungry
		for (auto& pBird : m_vecCards)
		{
			// Already Maximum of Birds are Hungry at the same time
			if (iHungryCounter >= Get_Hungry_Max())
				return;

			// Set Birds Hungry
			_bool bIsHungry = pBird->Check_Hungry(fTimeDelta);
			if (bIsHungry)
			{
				iHungryCounter++;
			}
		}
	}
	else
	{
		// Game Won
		if (m_bCanPlay)
		{
			CLevel* pLevel = CLevel_Manager::Get_Instance()->Get_CurrentLevel();
			dynamic_cast<CLevel_Maze*>(pLevel)->Set_PuzzleSolved(true);

			// Spawn Confetti Effect
			CDecoObject::DECODECS DecoDesc;
			DecoDesc.m_eState = CDecoObject::PARTY;
			DecoDesc.vInitPosition = Get_Position();
			DecoDesc.vInitPosition.y += 1.f;
			CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_MAZE, TEXT("Layer_Deco"), &DecoDesc);

			for (auto& pBird : m_vecCards)
			{
				pBird->Set_GameWon();
				pBird->Set_Interact(false);

				m_eState = STATESTATION::TURN_OFF;
			}

			m_bIsWin = true;
			CCameraManager* pCameraManager = CCameraManager::Get_Instance();
			if (pCameraManager->Get_CamState() == CCameraManager::CAM_TARGET)
			{
				CCamera* pCamera = pCameraManager->Get_CurrentCamera();
				dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(false);
			}
			m_bCanPlay = false;
		}
	}
}

void CCarnivalMemory::Start_Egg()
{


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->PlayBGM(TEXT("DST_Carnival_EggRide3_V2.wav"),0.3f);
	HANDLE		hFile = CreateFile(TEXT("../Bin/Resources/Data/Egg_Stage3.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;
	
	
	_ulong dwByte = 0;
	_float3 vPosition(0, 0, 0);
	_uint iNum = 0;
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(vPosition), sizeof(_float3), &dwByte, nullptr);
		vPosition.z += 1.f;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CCarnival_Egg"), LEVEL_MAZE, TEXT("Layer_Egg"), &vPosition);
	}

	CloseHandle(hFile);

	_tchar	szFullPath[MAX_PATH] = TEXT("Place%d_Carnival_Eggrides_DST_01.wav");
	_uint i = rand() % 3 + 1;
	wsprintf(szFullPath, szFullPath, i);
	pGameInstance->PlaySounds(szFullPath, SOUND_OBJECT, 0.5f);

	list<CGameObject*>* lObjects = pGameInstance->Get_ObjectList(LEVEL_MAZE, TEXT("Layer_Egg"));
	if (!lObjects)
		return;

	// Populate Card Vector
	for (auto& iter = lObjects->begin(); iter != lObjects->end(); ++iter)
	{
		CCarnival_Egg* pEgg = dynamic_cast<CCarnival_Egg*>(*iter);
		pEgg->Set_Memory(this);
		m_iEggCount++;

	}
}

void CCarnivalMemory::Play_Egg(_float fTimeDelta)
{
	if (m_eState == WIN)
		return;

	if (Check_Clear() && m_bIsWin && m_bStart)
		m_eState = WIN;

	if (m_dwEggSpawnTime + 20000 < GetTickCount() && !m_bIsWin)
	{
		Add_NewEgg();
		m_dwEggSpawnTime = GetTickCount();
	}
		
}

_bool CCarnivalMemory::Check_Clear()
{
	if (!m_bStart || m_bInteract == true )
		return false;

	CCameraManager* pCameraManager = CCameraManager::Get_Instance();
	if (m_iEggCount <= 0)
	{
		if (pCameraManager->Get_CamState() == CCameraManager::CAM_TARGET)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->StopSound(SOUND_BGM);

			_tchar	szFullPath[MAX_PATH] = TEXT("Bell_Win_Carnivalgame_Herding_Station_DST_%02d.wav");
			_uint i = rand() % 7 + 1;
			wsprintf(szFullPath, szFullPath, i);
			pGameInstance->PlaySounds(szFullPath, SOUND_OBJECT, 0.5f);
			
			CCamera* pCamera = pCameraManager->Get_CurrentCamera();
			dynamic_cast<CCameraTarget*>(pCamera)->Set_PositionMode(false);

			CLevel* pLevel = CLevel_Manager::Get_Instance()->Get_CurrentLevel();
			dynamic_cast<CLevel_Maze*>(pLevel)->Set_PuzzleSolved(true);
		}
		m_bIsWin = true;
		return true;
	}
		

	return false;
}

void CCarnivalMemory::Add_NewEgg()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float3 vPosition = Get_Position();
	_bool isXMinus = rand() % 2 == 0 ? true : false;
	_bool isZMinus = rand() % 2 == 0 ? true : false;

	if (isXMinus)
		vPosition.x += (((rand() % 1000)*0.001f) / float(m_fMaxDistance - m_fMinDistance)) + m_fMinDistance;
	else
		vPosition.x -= (((rand() % 1000)*0.001f) / float(m_fMaxDistance - m_fMinDistance)) + m_fMinDistance;

	if (isZMinus)
		vPosition.z += (((rand() % 1000)*0.001f) / float(m_fMaxDistance - m_fMinDistance)) + m_fMinDistance;
	else
		vPosition.z -= (((rand() % 1000)*0.001f) / float(m_fMaxDistance - m_fMinDistance)) + m_fMinDistance;

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CCarnival_Egg"), LEVEL_MAZE, TEXT("Layer_Egg"), vPosition);

	_tchar	szFullPath[MAX_PATH] = TEXT("Place%d_Carnival_Eggrides_DST_01.wav");
	_uint i = rand() % 3 + 1;
	wsprintf(szFullPath, szFullPath, i);
	pGameInstance->PlaySounds(szFullPath, SOUND_OBJECT, 0.5f);
	
	list<CGameObject*>* lObjects = pGameInstance->Get_ObjectList(LEVEL_MAZE, TEXT("Layer_Egg"));
	if (!lObjects)
		return;

	_uint iIndex = lObjects->size() -1;

	CGameObject* pGameObject = pGameInstance->Get_Object(LEVEL_MAZE, TEXT("Layer_Egg"), iIndex);
	dynamic_cast<CCarnival_Egg*>(pGameObject)->Set_Memory(this);
	m_iEggCount++;
	//CCarnival_Egg* pEgg = dynamic_cast<CCarnival_Egg*>(*iter);
	//pEgg->Set_Memory(this);
	
	
}

HRESULT CCarnivalMemory::Drop_Items()
{
	return S_OK;
}

HRESULT CCarnivalMemory::SetUp_Components(void* pArg)
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

HRESULT CCarnivalMemory::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));

	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 30;

	if (m_tStationDesc.eType == STATIONTYPE::STATION_MEMORY)
	{
		TextureDesc.m_iEndTex = 8;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Hit"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Hit"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Idle_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 40;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Idle_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 26;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Place"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 14;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Turn_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 24;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Turn_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 20;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Win"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Memory_Win"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_BIRD)
	{
		TextureDesc.m_iEndTex = 0;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Nest_Idle_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 32;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Nest_Idle_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 26;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Place"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Nest_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 26;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Nest_Turn_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 25;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Nest_Turn_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}
	else if (m_tStationDesc.eType == STATIONTYPE::STATION_EGG)
	{
		TextureDesc.m_iEndTex = 3;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_EggMom_Idle_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 15;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Idle_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_EggMom_Idle_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 22;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Place"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_EggMom_Place"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 11;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_Off"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_EggMom_Turn_Off"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 12;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Turn_On"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_EggMom_Turn_On"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		TextureDesc.m_iEndTex = 16;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Win"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_EggMom_Complete"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
	}

	return S_OK;
}

void CCarnivalMemory::Change_Frame(_float fTimeDelta)
{
	switch (m_eState)
	{
	case HIT:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATESTATION::IDLE_OFF;
		break;
	case IDLE_OFF:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case IDLE_ON:
		m_pTextureCom->MoveFrame(m_TimerTag);
		
		if (m_tStationDesc.eType == STATIONTYPE::STATION_MEMORY)
		{
			if (m_fIdleOnTimer > 2.f)
			{
				m_fIdleOnTimer = 0.f;
				m_eState = STATESTATION::TURN_OFF;
			}
			else
				m_fIdleOnTimer += fTimeDelta;
		}
		
		break;
	case PLACE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATESTATION::IDLE_OFF;
		break;
	case TURN_OFF:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATESTATION::IDLE_OFF;
		break;
	case TURN_ON:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATESTATION::IDLE_ON;
		break;
	case WIN:
	{
		if (m_eState == STATION_EGG)
		{
			if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
				m_eState = STATESTATION::TURN_OFF;
		}
		else
		{
			m_pTextureCom->MoveFrame(m_TimerTag);

			if (m_fWinTimer > 2.f)
			{
				m_fWinTimer = 0.f;
				m_eState = STATESTATION::TURN_OFF;
			}
			else
				m_fWinTimer += fTimeDelta;
		}
		break;
	}	
	case COMPLETE:
		if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			m_eState = STATESTATION::IDLE_ON;
		break;
	}
}

void CCarnivalMemory::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case HIT:
			Change_Texture(TEXT("Com_Texture_Hit"));
			break;
		case IDLE_OFF:
			Change_Texture(TEXT("Com_Texture_Idle_Off"));
			break;
		case IDLE_ON:
			Change_Texture(TEXT("Com_Texture_Idle_On"));
			break;
		case PLACE:
			Change_Texture(TEXT("Com_Texture_Place"));
			break;
		case TURN_OFF:
			Change_Texture(TEXT("Com_Texture_Turn_Off"));
			break;
		case TURN_ON:
			Change_Texture(TEXT("Com_Texture_Turn_On"));
			break;
		case WIN:
			Change_Texture(TEXT("Com_Texture_Win"));
			break;
		case COMPLETE:
		{
			_tchar szFullPath[MAX_PATH] = TEXT("Turn_On_Carnivalgame_Herding_Station_DST_%02d.wav");
			_uint i = rand() % 4 + 1;
			wsprintf(szFullPath, szFullPath, i);
			CGameInstance::Get_Instance()->PlaySounds(szFullPath, SOUND_OBJECT, 0.5f);
			Change_Texture(TEXT("Com_Texture_Win"));
			break;
		}		
		}

		m_ePreState = m_eState;
	}
}

_bool CCarnivalMemory::Picking(_float3 * PickingPoint)
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

void CCarnivalMemory::PickingTrue()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Up(VK_LBUTTON))
	{
	}

	cout << "Collision Carnival Memory : " << m_vecOutPos.x << " " << m_vecOutPos.y << " " << m_vecOutPos.z << endl;

	Safe_Release(pGameInstance);
}

CCarnivalMemory* CCarnivalMemory::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarnivalMemory* pInstance = new CCarnivalMemory(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCarnivalMemory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarnivalMemory::Clone(void* pArg)
{
	CCarnivalMemory* pInstance = new CCarnivalMemory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarnivalMemory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarnivalMemory::Free()
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