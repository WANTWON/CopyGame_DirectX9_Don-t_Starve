#include "stdafx.h"
#include "..\Public\DecoObject.h"
#include "GameInstance.h"
#include "FloorGrateEruption.h"
#include "Player.h"
#include "Level_GamePlay.h"

CDecoObject::CDecoObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDecoObject::CDecoObject(const CDecoObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDecoObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecoObject::Initialize(void* pArg)
{
	ZeroMemory(&m_DecoDesc, sizeof(DECODECS));
	memcpy(&m_DecoDesc, (DECODECS*)pArg, sizeof(DECODECS));


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(&m_DecoDesc.vInitPosition)))
		return E_FAIL;

	switch (m_DecoDesc.m_eState)
	{
	case DECOTYPE::FLOORFIRE:
		m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
		m_pTransformCom->Turn(_float3(1, 0, 0), 90.f);
		m_pTransformCom->Turn(_float3(0, 1, 0), m_DecoDesc.fRotate);
		m_fRadius = 0.01f;
		break;
	case DECOTYPE::FLOOR_EFFECT:
		m_pTransformCom->Set_Scale(1.f, 1.5f, 1.f);
		break;
	case DECOTYPE::TORCH:
		m_pTransformCom->Set_Scale(0.8f, 2.8f, 1.f);
		m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
		m_fRadius = m_pTransformCom->Get_Scale().y *0.5f;
		break;
	case DECOTYPE::FLIES:
		m_pTransformCom->Set_Scale(1.5f, 2.f, 1.f);
		break;
	case DECOTYPE::FLOOR:
	{
		if (m_DecoDesc.fRotate == 1.f)
			m_pTransformCom->Set_Scale(7.f, 7.f, 1.f);
		else if (m_DecoDesc.fRotate == 2.f)
			m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
		else if (m_DecoDesc.fRotate == 3.f)
			m_pTransformCom->Set_Scale(7.f, 7.f, 1.f);
		else if(m_DecoDesc.fRotate == 4.f)
			m_pTransformCom->Set_Scale(3.f, 3.f, 3.f);
		else if (m_DecoDesc.fRotate == 5.f)
			m_pTransformCom->Set_Scale(10.f, 2.f, 1.f);
		else
			m_pTransformCom->Set_Scale(7.f, 7.f, 1.f);

		m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();
		m_pTransformCom->Turn(_float3(1, 0, 0), 90.f);
		m_fRadius = 0.002f;
		break;
	}
	case DECOTYPE::PARTY:
	{
		m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
		m_fRadius = 2.f;
		break;
	}
	case DECOTYPE::SPARKLE:
		m_pTransformCom->Set_Scale(.5f, .5f, 1.f);
		break;
	case DECOTYPE::FIREFLIES:
		m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
		break;
	}

	return S_OK;
}

int CDecoObject::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	__super::Tick(fTimeDelta);

	if (m_DecoDesc.m_eState == FLOOR && m_DecoDesc.fRotate == 4)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Add_CollisionGroup(CCollider_Manager::COLLISION_PUSH, this);
	}

	WalkingTerrain();
	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CDecoObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_DecoDesc.m_eState != FLOORFIRE && m_DecoDesc.m_eState != FLOOR )
		SetUp_BillBoard();

	switch (m_DecoDesc.m_eState)
	{
	case DECOTYPE::FLOORFIRE:
		FloorUpdate();
		Check_Eruption(fTimeDelta);
		break;
	}

	if (nullptr != m_pRendererCom)
	{
		if (m_DecoDesc.m_eState == DECOTYPE::FLOOR_EFFECT || m_DecoDesc.m_eState == DECOTYPE::SPARKLE || m_DecoDesc.m_eState == DECOTYPE::FIREFLIES)
		{
			m_eShaderID = SHADER_IDLE;
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
			Compute_CamDistance(Get_Position());
		}
		else
		{
			m_eShaderID = SHADER_IDLE;
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}

		if (m_DecoDesc.m_eState != DECOTYPE::SPARKLE && m_DecoDesc.m_eState != DECOTYPE::FIREFLIES)
			Set_ShaderID();
	}
	
	Compute_CamDistance(Get_Position());

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update_ColliderBox(m_CollisionMatrix);

	// Change Motion only if FIREFLIES
	if (m_DecoDesc.m_eState == DECOTYPE::FIREFLIES)
	{
		if (m_eState != m_ePreState)
		{
			switch (m_eState)
			{
			case DECOSTATE::LOOP:
				Change_Texture(TEXT("Com_Texture_Loop"));
				break;
			case DECOSTATE::PRE:
				Change_Texture(TEXT("Com_Texture_Pre"));
				break;
			case DECOSTATE::POST:
				Change_Texture(TEXT("Com_Texture_Post"));
				break;
			}

			if (m_eState != m_ePreState)
				m_ePreState = m_eState;
		}
	}

	MoveFrame(fTimeDelta);
	
}


void CDecoObject::Set_ShaderID()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	CGameObject* pGameObject = CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	if (pGameObject->Get_Dead())
		m_eShaderID = SHADER_DEAD;
	else if (dynamic_cast<CPlayer*>(pGameObject)->Get_WeaponType() == WEAPON_LIGHT)
		m_eShaderID = SHADER_DARKWITHLIGHT;
	else if (iLevel == LEVEL_MAZE)
		m_eShaderID = SHADER_DARK;
	else if (iLevel == LEVEL_BOSS)
		m_eShaderID = SHADER_FIRE;
	else
		m_eShaderID = SHADER_DAYCYClE;

}

HRESULT CDecoObject::Render()
{
	
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

	m_pVIBufferCom->Render();
	m_pShaderCom->End();
	
#ifdef _DEBUG
	if (g_ColliderRender &&  m_pColliderCom != nullptr)
	{
		m_pTextureCom->Bind_OnGraphicDev_Debug();
		m_pColliderCom->Render_ColliderBox();
	}
#endif // _DEBUG


	return S_OK;
}

void CDecoObject::FloorUpdate()
{
	if (m_bCreate)
		return;

	DECODECS DecoDesc;
	DecoDesc.m_eState = CDecoObject::DECOTYPE::FLOOR_EFFECT;
	DecoDesc.vInitPosition = Get_Position();
	CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_BOSS, TEXT("Layer_Deco"), &DecoDesc);

	m_bCreate = true;
}

void CDecoObject::MoveFrame(_float fTimeDelta)
{
	switch (m_DecoDesc.m_eState)
	{
	case FLOORFIRE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case FLOOR_EFFECT:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case TORCH:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case FLIES:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case FLOOR:
		m_pTextureCom->MoveFrame(m_TimerTag, false);
		break;
	case PARTY:
	case SPARKLE:
		if (m_pTextureCom->MoveFrame(m_TimerTag, false) == true)
			m_bDead = true;
		break;
	case FIREFLIES:
		switch (m_eState)
		{
		case DECOSTATE::LOOP:
			if (m_fFirefliesTimer > 5.f)
				m_eState = DECOSTATE::POST;
			else
				m_fFirefliesTimer += fTimeDelta;

			m_pTextureCom->MoveFrame(m_TimerTag);
			break;
		case DECOSTATE::PRE:
			if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
				m_eState = DECOSTATE::LOOP;
			break;
		case DECOSTATE::POST:
			if ((m_pTextureCom->MoveFrame(m_TimerTag, false)) == true)
			{
				m_bDead = true;

				// Decrease FirefliesCounter in GameplayLevel
				CLevel_Manager* pLevelManager = CLevel_Manager::Get_Instance();
				CLevel_GamePlay* pGameplayLevel = dynamic_cast<CLevel_GamePlay*>(pLevelManager->Get_CurrentLevel());
				if (!pGameplayLevel)
					return;
				pGameplayLevel->Decrease_Fireflies();
			}
			break;
		}
		break;
	}	
}

HRESULT CDecoObject::Change_Texture(const _tchar * LayerTag)
{
	if (FAILED(__super::Change_Component(LayerTag, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pTextureCom->Set_ZeroFrame();

	return S_OK;
}

HRESULT CDecoObject::SetUp_Components(void* pArg)
{

	if (m_DecoDesc.m_eState == DECOTYPE::FLOOR_EFFECT || m_DecoDesc.m_eState == DECOTYPE::SPARKLE || m_DecoDesc.m_eState == DECOTYPE::FIREFLIES)
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static_Blend"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}

	/* For.Com_Texture */
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_fSpeed = 60.f;
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iCurrentTex = 0;

	switch (m_DecoDesc.m_eState)
	{
	case DECOTYPE::FLOORFIRE:
		TextureDesc.m_iEndTex = 43;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_FloorDeco"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case DECOTYPE::FLOOR_EFFECT:
		TextureDesc.m_iEndTex = 32;
		TextureDesc.m_fSpeed = rand() % 20 + 10;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_FloorParticle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case DECOTYPE::TORCH:
		TextureDesc.m_iEndTex = 4;
		TextureDesc.m_fSpeed = 30.f;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_BOSS, TEXT("Prototype_Component_Texture_Torch"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case DECOTYPE::FLIES:
		TextureDesc.m_iEndTex = 54;
		TextureDesc.m_fSpeed = 40.f;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_HUNT, TEXT("Prototype_Component_Texture_Flies"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case DECOTYPE::FLOOR:
		
		Set_FloorDecoTexture();
		break;
	case DECOTYPE::PARTY:
		TextureDesc.m_iEndTex = 14;
		TextureDesc.m_fSpeed = 20.f;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Party"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		m_vecTexture.push_back(m_pTextureCom);
		break;
	case DECOTYPE::SPARKLE:
		TextureDesc.m_iEndTex = 53;
		TextureDesc.m_fSpeed = 40.f;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sparkle_Effect"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		break;
	case DECOTYPE::FIREFLIES:
		TextureDesc.m_iEndTex = 100;
		TextureDesc.m_fSpeed = 20.f;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Loop"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fireflies_Loop"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 51;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Pre"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fireflies_Pre"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
		TextureDesc.m_iEndTex = 50;
		if (FAILED(__super::Add_Components(TEXT("Com_Texture_Post"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fireflies_Post"), (CComponent**)&m_pTextureCom, &TextureDesc)))
			return E_FAIL;
	}

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
	TransformDesc.fRotationPerSec = D3DXToRadian(1.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.5f;
	CollRectDesc.fRadiusX = 0.2f;
	CollRectDesc.fRadiusZ = 0.2f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.f;
	CollRectDesc.fOffsetZ = 0.f;

	switch (m_DecoDesc.m_eState)
	{
		case DECOTYPE::TORCH:
			if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
				return E_FAIL;
		break;
		case DECOTYPE::FLOOR:
			if (m_DecoDesc.fRotate == 4.f)
			{
				CollRectDesc.fRadiusY = 0.3f;
				CollRectDesc.fRadiusX = 0.3f;
				CollRectDesc.fRadiusZ = 0.3f;
				if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
					return E_FAIL;
			}
			break;
	}

	return S_OK;
}


void CDecoObject::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);	// Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);		// Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	if(m_DecoDesc.m_eState != TORCH)
		m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

}

void CDecoObject::WalkingTerrain()
{
	LEVEL iLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(iLevel, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;
	CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(iLevel, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), m_fRadius);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CDecoObject::Check_Eruption(_float fTimeDelta)
{
	// Spawn Eruption (Warning already Spawned)
	if (m_bShouldErupt)
	{
		if (m_fEruptionTime < 1.5f)
			m_fEruptionTime += fTimeDelta;
		else
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			if (!pGameInstance)
				return;

			CFloorGrateEruption::ERUPTIONDESC tEruptionDesc;
			tEruptionDesc.eState = CFloorGrateEruption::ERUPTION_STATE::ERUPTION;
			tEruptionDesc.vInitPosition = Get_Position();

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Floor_Grate_Eruption"), LEVEL_BOSS, TEXT("Layer_Effect"), &tEruptionDesc)))
				return;

			m_bShouldErupt = false;
			m_fEruptionTime = 0.f;
		}
	}
	// Spawn Warning
	else
	{
		// Every 1 to 3 seconds there is a random chance of Eruption (Warning)
		if (m_fWarningTime < m_fRandomWarningLimit)
			m_fWarningTime += fTimeDelta;
		else
		{
			if (!m_bIsWarningTimeChosen)
			{
				m_fRandomWarningLimit = rand() % 3 + 1;
				m_bIsWarningTimeChosen = true;
			}
			else
			{
				m_fWarningTime = 0.f;

				_bool bShouldWarn = rand() % 5; // (0 ... 4)

				// If bShouldErupt == 0 : Erupt (25% Chance)
				if (bShouldWarn == 0)
				{
					CGameInstance* pGameInstance = CGameInstance::Get_Instance();
					if (!pGameInstance)
						return;

					CFloorGrateEruption::ERUPTIONDESC tEruptionDesc;
					tEruptionDesc.eState = CFloorGrateEruption::ERUPTION_STATE::WARNING;
					tEruptionDesc.vInitPosition = Get_Position();

					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Floor_Grate_Eruption"), LEVEL_BOSS, TEXT("Layer_Effect"), &tEruptionDesc)))
						return;

					m_bShouldErupt = true;
					m_bIsWarningTimeChosen = false;
				}
			}
		}
	}
}

void CDecoObject::Set_FloorDecoTexture()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_iCurrentTex = 0;
	TextureDesc.m_iEndTex = 15;
	TextureDesc.m_fSpeed = 40.f;


	if (m_DecoDesc.fRotate == 0.f)
	{
		__super::Add_Components(TEXT("Com_Texture"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_MazeFloor_Place1"), (CComponent**)&m_pTextureCom, &TextureDesc);
		m_vecTexture.push_back(m_pTextureCom);
	}
		
	if (m_DecoDesc.fRotate == 1.f)
	{
		__super::Add_Components(TEXT("Com_Texture"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_MazeFloor_Place2"), (CComponent**)&m_pTextureCom, &TextureDesc);
		m_vecTexture.push_back(m_pTextureCom);

	}

	if (m_DecoDesc.fRotate == 2.f)
	{
		__super::Add_Components(TEXT("Com_Texture"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_MazeFloor_Place3"), (CComponent**)&m_pTextureCom, &TextureDesc);
		m_vecTexture.push_back(m_pTextureCom);

	}

	if (m_DecoDesc.fRotate == 3.f)
	{
		__super::Add_Components(TEXT("Com_Texture"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_MazeFloor_Place4"), (CComponent**)&m_pTextureCom, &TextureDesc);
		m_vecTexture.push_back(m_pTextureCom);

	}

	if (m_DecoDesc.fRotate == 4.f)
	{
		__super::Add_Components(TEXT("Com_Texture"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_MazeFloor_Place5"), (CComponent**)&m_pTextureCom, &TextureDesc);
		m_vecTexture.push_back(m_pTextureCom);

	}

	if (m_DecoDesc.fRotate == 5.f)
	{
		TextureDesc.m_iEndTex = 10;
		__super::Add_Components(TEXT("Com_Texture"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_MazeFloor_Place6"), (CComponent**)&m_pTextureCom, &TextureDesc);
		m_vecTexture.push_back(m_pTextureCom);

	}
		
}

CDecoObject* CDecoObject::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDecoObject* pInstance = new CDecoObject(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDecoObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecoObject::Clone(void* pArg)
{
	CDecoObject* pInstance = new CDecoObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDecoObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecoObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	for (auto& iter : m_vecTexture)
		Safe_Release(iter);

	m_vecTexture.clear();
}