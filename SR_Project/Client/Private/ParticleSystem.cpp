#include "stdafx.h"
#include "..\Public\ParticleSystem.h"
#include "GameInstance.h"

CParticleSystem::CParticleSystem(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CParticleSystem::CParticleSystem(const CParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticleSystem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleSystem::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATEDESC));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(Ready_ParticleSystem()))
		return E_FAIL;

	if (FAILED(Ready_VIBuffer()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_StateDesc.vPosition);
	m_bStart = false;


	m_pTransformCom->Set_Scale(m_StateDesc.vParticleScale.x, m_StateDesc.vParticleScale.y, 0);


	return S_OK;
}

int CParticleSystem::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	/* 파티클 리스트에서 죽은 파티클들을 지운다*/
	if (FAILED(Kill_DeadParticles()))
		return E_FAIL;

	m_dCurrentDuration += fTimeDelta;
	/*만약 현재 시간이 DeadDuration 기간보다 적으면 파티클의 위치를 세팅해준다*/
	if (m_StateDesc.dDuration > m_dCurrentDuration)
	{
		switch (m_StateDesc.eType)
		{
		case Client::CParticleSystem::PARTICLE_ROCK:
			Particle_Rock_Mining(fTimeDelta);
			break;
		case Client::CParticleSystem::PARTICLE_LEAF:
			Particle_Leaf_Moving(fTimeDelta);
			break;
		default:
			break;
		}
	}

	/* 혹시 모든 파티클이 죽었다면 바로 Dead 처리*/
	if (m_bStart && Check_AllParticleDead())
	{
		m_bDead = true;
		return OBJ_DEAD;
	}

	//파티클 시스템의 드로잉 순서 (비효율적이지만 용책에서 알려준 구조)
	/*1. 모든 파티클을 갱신한다.(각 파티클들의 시간과 위치 업데이트)*/
	if (FAILED(Update_Particles(fTimeDelta)))
		return E_FAIL;

	/*2. 모든 살아 있는 파티클들을 버퍼로 복사한다 */
	if (FAILED(Update_VIBuffer()))
		return E_FAIL;

	/*3. 버텍스 버퍼를 그린다. 이건 렌더부분에서*/
	return OBJ_NOEVENT;
}

void CParticleSystem::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	SetUp_BillBoard();
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this)))
		return;
}

HRESULT CParticleSystem::Render()
{
	if (nullptr == m_pVBuffer)
		return E_FAIL;
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

	if (FAILED(Release_RenderState()))
		return E_FAIL;
	m_pShaderCom->End();

	return S_OK;
}

HRESULT CParticleSystem::Render_VIBuffer()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pVBuffer)
		return E_FAIL;

	m_pGraphic_Device->SetStreamSource(0, m_pVBuffer, 0, m_iStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumPrimitive);

	return S_OK;
}

HRESULT CParticleSystem::Particle_Rock_Mining(_float fTimeDelta)
{

	m_bStart = true;
	_bool emitParticle = false;
	_bool found = false;
//	float positionX, positionY, positionZ;
	int index, i, j;

	m_dCurrentSpawnTime += fTimeDelta;
	emitParticle = false;

	if (m_dCurrentSpawnTime > (m_StateDesc.dSpawnTime / m_StateDesc.fParticlePerSecond))
	{
		m_dCurrentSpawnTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_iCurrentParticleCount < (m_StateDesc.iMaxParticleCount - 1)))
	{

		m_iCurrentParticleCount++;

		_float3 vPosition = _float3(0,0,0);

		_float3 vVelocity =_float3((rand() % 3)*0.05f, (rand() % 3) * 0.05f, (rand() % 3) * 0.05f);

		if (rand() % 2 == 0)
			vVelocity.x *= -1;


		index = 0;
		found = false;
		while (!found)
		{
			if ((m_ParticleArray[index].bActive == false) || Compute_ViewZ(m_ParticleArray[index].vPosition) < Compute_ViewZ(vPosition))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		i = m_iCurrentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_ParticleArray[i].bActive = m_ParticleArray[j].bActive;
			m_ParticleArray[i].vPosition = m_ParticleArray[j].vPosition;
			m_ParticleArray[i].vVelocity = m_ParticleArray[j].vVelocity;
			m_ParticleArray[i].dCurrentLifeTime = m_ParticleArray[j].dCurrentLifeTime;
			i--;
			j--;
		}

		m_ParticleArray[i].bActive = true;
		m_ParticleArray[i].vPosition = vPosition;
		m_ParticleArray[i].vVelocity = vVelocity;
		m_ParticleArray[i].dCurrentLifeTime = 0;
	}

	return S_OK;
}

HRESULT CParticleSystem::Particle_Leaf_Moving(_float fTimeDelta)
{
	m_bStart = true;
	_bool emitParticle = false;
	_bool found = false;
	float positionX, positionY, positionZ;
	int index, i, j;

	m_dCurrentSpawnTime += fTimeDelta;
	emitParticle = false;

	if (m_dCurrentSpawnTime > (m_StateDesc.dSpawnTime / m_StateDesc.fParticlePerSecond))
	{
		m_dCurrentSpawnTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_iCurrentParticleCount < (m_StateDesc.iMaxParticleCount - 1)))
	{
		m_iCurrentParticleCount++;

		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.vParticleDeviation.x;
		positionY = 0;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.vParticleDeviation.z;
		_float3 vPosition = _float3(positionX, positionY, positionZ);

		_float3 vVelocity = m_StateDesc.vVelocity * (rand()%10*0.01f) * m_StateDesc.fVelocityDeviation;
		if(rand()%2 == 0)
			vVelocity.x *= -1;

		index = 0;
		found = false;
		while (!found)
		{
			if ((m_ParticleArray[index].bActive == false) || Compute_ViewZ(m_ParticleArray[index].vPosition) < Compute_ViewZ(vPosition))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		i = m_iCurrentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_ParticleArray[i].bActive = m_ParticleArray[j].bActive;
			m_ParticleArray[i].vPosition = m_ParticleArray[j].vPosition;
			m_ParticleArray[i].vVelocity = m_ParticleArray[j].vVelocity;
			m_ParticleArray[i].dCurrentLifeTime = m_ParticleArray[j].dCurrentLifeTime;
			i--;
			j--;
		}

		m_ParticleArray[i].bActive = true;
		m_ParticleArray[i].vPosition = vPosition;
		m_ParticleArray[i].vVelocity = vVelocity;
		m_ParticleArray[i].dCurrentLifeTime = 0;
	}

	return S_OK;
}

HRESULT CParticleSystem::SetUp_Components(void * pArg)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTexture::TEXTUREDESC  TextureDesc;
	TextureDesc.m_iEndTex = m_StateDesc.iTextureNum - 1;

	/* For Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), m_StateDesc.eTextureScene, m_StateDesc.pTextureKey, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Transform*/
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleSystem::Ready_ParticleSystem()
{
	m_ParticleArray = new PARTICLE_INFO[m_StateDesc.iMaxParticleCount];
	ZeroMemory(m_ParticleArray, sizeof(PARTICLE_INFO) * m_StateDesc.iMaxParticleCount);
	if (nullptr == m_ParticleArray)
		return E_FAIL;

	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		m_ParticleArray[i].bActive = false;
	}

	m_iCurrentParticleCount = 0;
	m_dCurrentSpawnTime = 0;
	return S_OK;
}

HRESULT CParticleSystem::Ready_VIBuffer()
{
	// For. Vertices
	// For. Vertices
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = m_StateDesc.iMaxParticleCount * 6; // 중복 정점 허용?
	m_iNumPrimitive = m_iNumVertices / 3;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0);

	m_Vertices = new VTXTEX[m_iNumVertices];
	if (nullptr == m_Vertices)
		return E_FAIL;
	ZeroMemory(m_Vertices, sizeof(VTXTEX) * m_iNumVertices);

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iStride * m_iNumVertices, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_DEFAULT, &m_pVBuffer, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleSystem::Update_Particles(_float fTimeDelta)
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		if (m_ParticleArray[i].bActive)
		{
			// 각 파티클들의 시간과 위치 업데이트
			switch (m_StateDesc.eType)
			{
			case Client::CParticleSystem::PARTICLE_ROCK:
				m_ParticleArray[i].dCurrentLifeTime += fTimeDelta;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;
				break;
			case Client::CParticleSystem::PARTICLE_LEAF:
				m_ParticleArray[i].dCurrentLifeTime += fTimeDelta;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;

				if (m_ParticleArray[i].dCurrentLifeTime == m_StateDesc.dParticleLifeTime*0.5)
					m_ParticleArray[i].vVelocity *= -1;
				break;
			case Client::CParticleSystem::PARTICLE_END:
				break;
			default:
				break;
			}
		}
	}
	return S_OK;
}

HRESULT CParticleSystem::Update_VIBuffer()
{
	ZeroMemory(m_Vertices, sizeof(VTXTEX) * m_iNumVertices);

	// For.Vertices
	VTXTEX*		pVertices = nullptr;
	int index = 0;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	_float4x4 ViewMatrix, InvViewMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&InvViewMatrix, nullptr, &ViewMatrix);

	//정점들의 위치를 제로 메모리 하고
	ZeroMemory((_float3*)InvViewMatrix.m[3], sizeof(_float3));

	//파티클 개수만큼 로컬의 좌표들을 월드좌표로 바꿔준다.
	for (_int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		/// 오른쪽 위 삼각형
		// Left Top
		_float3 LocalPos = _float3(-0.5f*m_StateDesc.vParticleScale.x, 0.5f*m_StateDesc.vParticleScale.y, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexture = _float2(0.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Right Top
		LocalPos = _float3(0.5f*m_StateDesc.vParticleScale.x, 0.5f*m_StateDesc.vParticleScale.y, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexture = _float2(1.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Right Bottom
		LocalPos = _float3(0.5f*m_StateDesc.vParticleScale.x, -0.5f*m_StateDesc.vParticleScale.y, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexture = _float2(1.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;

		/// 왼쪽 아래 삼각형
		// Left Top
		LocalPos = _float3(-0.5f*m_StateDesc.vParticleScale.x, 0.5f*m_StateDesc.vParticleScale.y, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexture = _float2(0.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Right Bottom
		LocalPos = _float3(0.5f*m_StateDesc.vParticleScale.x, -0.5f*m_StateDesc.vParticleScale.y, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexture = _float2(1.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// Left Bottom
		//pVertices[index].vPosition = _float3(m_ParticleArray[i].vPosition.x - m_vParticleScale.x, m_ParticleArray[i].vPosition.y - m_vParticleScale.y, m_ParticleArray[i].vPosition.z);
		LocalPos = _float3(-0.5f*m_StateDesc.vParticleScale.x, -0.5f*m_StateDesc.vParticleScale.y, 0);
		D3DXVec3TransformCoord(&LocalPos, &LocalPos, &InvViewMatrix);
		pVertices[index].vPosition = m_ParticleArray[i].vPosition + LocalPos;
		pVertices[index].vTexture = _float2(0.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;
	}

	m_pVBuffer->Unlock();

	return S_OK;
}

HRESULT CParticleSystem::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return S_OK;
}

HRESULT CParticleSystem::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	return S_OK;
}

HRESULT CParticleSystem::Kill_DeadParticles()
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		/* 파티클이 행동하고 있고,  내 Life Time이 파티클의 Dead LifeTime보다 클 경우*/
		if ((true == m_ParticleArray[i].bActive) && (m_StateDesc.dParticleLifeTime <= m_ParticleArray[i].dCurrentLifeTime))
		{
			m_ParticleArray[i].bActive = false; /*파티클의 행동을 멈춘다*/
			--m_iCurrentParticleCount; /*현재 살아 있는 파티클의 수를 낮춘다*/

			for (int j = i; j < m_StateDesc.iMaxParticleCount - 1; ++j) /*살아있는 파티클의 갯수만큼*/
			{
				/* 살아 있는 애들을 한칸씩 앞으로 끌고 오게 하기*/
				/* 그 뒤에 있는 애들은 어차피 살아 있는 갯수가 아니니께 신경 x*/
				m_ParticleArray[j].bActive = m_ParticleArray[j + 1].bActive;
				m_ParticleArray[j].vPosition = m_ParticleArray[j + 1].vPosition;
				m_ParticleArray[j].vVelocity = m_ParticleArray[j + 1].vVelocity;
				m_ParticleArray[j].dCurrentLifeTime = m_ParticleArray[j + 1].dCurrentLifeTime;
			}
		}
	}

	return S_OK;
}

_bool CParticleSystem::Check_AllParticleDead()
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		/* 하나라도 살아있다면 False반환*/
		if (m_ParticleArray[i].bActive)
			return false;
	}
	return true;
}

_float CParticleSystem::Compute_ViewZ(_float3 WorldPos)
{
	/*카메라의 월드 매트릭스의 좌표 받아오기*/
	_float4x4 ViewInvMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewInvMatrix);
	D3DXMatrixInverse(&ViewInvMatrix, nullptr, &ViewInvMatrix);
	_float3 vCamPosition = *(_float3*)&ViewInvMatrix.m[3][0];
	_float3	 vDistance = vCamPosition - WorldPos;
	return D3DXVec3Length(&vDistance);
}

void CParticleSystem::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CParticleSystem * CParticleSystem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CParticleSystem*	pInstance = new CParticleSystem(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CParticleSystem"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticleSystem::Clone(void * pArg)
{
	CParticleSystem*	pInstance = new CParticleSystem(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CParticleSystem"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticleSystem::Free()
{
	__super::Free();
	Safe_Delete_Array(m_ParticleArray);
	Safe_Delete_Array(m_Vertices);

	Safe_Release(m_pVBuffer);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
}
