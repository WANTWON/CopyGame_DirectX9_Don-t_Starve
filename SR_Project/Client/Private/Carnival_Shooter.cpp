#include "stdafx.h"
#include "..\Public\Carnival_Shooter.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "Player.h"
#include "Level_Maze.h"
#include "Level_Manager.h"
#include "Bullet.h"

CCarnival_Shooter::CCarnival_Shooter(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUnInteractive_Object(pGraphic_Device)
{
	ZeroMemory(&m_tInfo, sizeof(OBJINFO));
}

CCarnival_Shooter::CCarnival_Shooter(const CCarnival_Shooter & rhs)
	: CUnInteractive_Object(rhs)
{
}

HRESULT CCarnival_Shooter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarnival_Shooter::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eState = IDLE;
	m_pTransformCom->Set_Scale(1.5f, 2.f, 1.f);
	m_CollisionMatrix = m_pTransformCom->Get_WorldMatrix();

	return S_OK;
}


int CCarnival_Shooter::Tick(_float fTimeDelta)
{
	

	__super::Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return OBJ_NOEVENT;
}

void CCarnival_Shooter::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	
	if (!m_bReady)
	{
		_float3 vPosition = Get_Position();
		vPosition.z -= 2.f;

		BULLETDATA BulletData;
		ZeroMemory(&BulletData, sizeof(BulletData));
		BulletData.vLook = _float3(0.f,1.f,0.f);
		BulletData.eDirState = DIR_END;
		BulletData.bIsPlayerBullet = true;
		BulletData.eWeaponType = CARNIVAL_ARROW;
		BulletData.vPosition = vPosition;
		CGameInstance::Get_Instance()->Add_GameObject(TEXT("Prototype_GameObject_Bullet"), LEVEL_MAZE, TEXT("Layer_Bullet"), &BulletData);
		m_bReady = true;
	}


	Change_Motion();
	Change_Frame();

	Set_ShaderID();

}

HRESULT CCarnival_Shooter::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG

	if (g_ColliderRender &&  m_pColliderCom != nullptr)
	{
		if (CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct() == false)
		{
			m_pTextureCom->Bind_OnGraphicDev_Debug();
			m_pColliderCom->Render_ColliderBox();
		}
	}

#endif // _DEBUG

	return S_OK;
}

void CCarnival_Shooter::Set_Texture()
{
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_pTextureCom->Get_Frame().m_iCurrentTex));
}


void CCarnival_Shooter::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

HRESULT CCarnival_Shooter::SetUp_Components(void* pArg)
{
	
	/* For.Com_Texture */
	Texture_Clone();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Static"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider*/
	CCollider_Cube::COLLRECTDESC CollRectDesc;
	ZeroMemory(&CollRectDesc, sizeof(CCollider_Cube::COLLRECTDESC));
	CollRectDesc.fRadiusY = 0.5f;
	CollRectDesc.fRadiusX = 0.5f;
	CollRectDesc.fRadiusZ = 0.1f;
	CollRectDesc.fOffSetX = 0.f;
	CollRectDesc.fOffSetY = 0.f;
	CollRectDesc.fOffsetZ = 0.f;


	if (FAILED(__super::Add_Components(TEXT("Com_Collider_Cube"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"), (CComponent**)&m_pColliderCom, &CollRectDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);
	TransformDesc.InitPos = *(_float3*)pArg;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}




HRESULT CCarnival_Shooter::Texture_Clone()
{
	CTexture::TEXTUREDESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(CTexture::TEXTUREDESC));
	TextureDesc.m_iStartTex = 0;
	TextureDesc.m_fSpeed = 40;

	TextureDesc.m_iEndTex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooter_Idle"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);

	TextureDesc.m_iEndTex = 4;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_SHOOT"), LEVEL_MAZE, TEXT("Prototype_Component_Texture_Shooter_Shoot"), (CComponent**)&m_pTextureCom, &TextureDesc)))
		return E_FAIL;
	m_vecTexture.push_back(m_pTextureCom);
	return S_OK;
}

void CCarnival_Shooter::Change_Frame()
{
	switch (m_eState)
	{
	case Client::CCarnival_Shooter::IDLE:
		m_pTextureCom->MoveFrame(m_TimerTag);
		break;
	case Client::CCarnival_Shooter::SHOOT:
		if (m_pTextureCom->MoveFrame(m_TimerTag, false) == true)
		{
			m_bReady = false;
			m_eState = IDLE;
		}
		break;
	default:
		break;
	}
}

void CCarnival_Shooter::Change_Motion()
{
	if (m_eState != m_ePreState)
	{
		switch (m_eState)
		{
		case Client::CCarnival_Shooter::IDLE:
			Change_Texture(TEXT("Com_Texture_IDLE"));
			break;
		case Client::CCarnival_Shooter::SHOOT:
			Change_Texture(TEXT("Com_Texture_SHOOT"));
			break;
		}

		if (m_eState != m_ePreState)
			m_ePreState = m_eState;

	}
}



CCarnival_Shooter* CCarnival_Shooter::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCarnival_Shooter* pInstance = new CCarnival_Shooter(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCarnival_Shooter"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarnival_Shooter::Clone(void* pArg)
{
	CCarnival_Shooter* pInstance = new CCarnival_Shooter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCarnival_Shooter "));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarnival_Shooter::Free()
{
	__super::Free();

}