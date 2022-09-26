#include "stdafx.h"
#include "..\Public\Cardgame.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "KeyMgr.h"
#include "PickingMgr.h"



CCardgame::CCardgame(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCardgame::CCardgame(const CCardgame & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCardgame::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCardgame::Initialize(void* pArg)
{
	if (pArg != nullptr)
		memcpy(&m_CaedDesc, pArg, sizeof(CARDDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(SetUp_Components()))
		return E_FAIL;


	iNum = m_CaedDesc.iNumber;   
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_CaedDesc.pos);

	//pos = { 40.f + (iNum*0.6f) , 0.5f, 25.f };

	texnum1 = iNum;
	/*if (iNum == 0 || iNum == 8)
		texnum1 = 0;

	else if (iNum == 1 || iNum == 7)
		texnum1 = 1;

	else if (iNum == 2 || iNum == 6)
		texnum1 = 2;

	else if (iNum == 3 || iNum == 5)
		texnum1 = 3;
	else
		texnum1 = 4;*/

	


	/*if(iNum >= 3 )

	pos = { 40.f + (iNum*0.5f) , 0.5f, 25.f };*/
	


	texnum = texnum1;
	//pos.x += 2.f;


	m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);
	//m_pTransformCom1->Set_Scale(0.5f, 0.5f, 1.f);

	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 0.5f);

	m_fRadius = 0.001f;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, effectdesc.pos);d


	m_WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	//m_pTransformCom->Set_State(m_WorldMatrix)

	//m_pTransformCom->set_world

	/*_float3 vRight;

	vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	*D3DXVec3Normalize(&vRight, &vRight);*/

	

	//pos += (vRight * 1.f);

	//	m_pTransformCom1->Set_State(CTransform::STATE_POSITION, pos);d
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(mypos.x - g_iWinSizeX * 0.5f, -pos.y + g_iWinSizeY * 0.5f, 0.f));

	

	return S_OK;
}

int CCardgame::Tick(_float fTimeDelta)
{

	
	if (m_bdead == true)
	{
		CPickingMgr::Get_Instance()->Out_PickingGroup(this);
		return OBJ_DEAD;
	}
	
	if (m_bcheck)
	{
		WalkingTerrain();


		/*if (CKeyMgr::Get_Instance()->Key_Up('6'))
			m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), fTimeDelta);

		if (CKeyMgr::Get_Instance()->Key_Up('7'))
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);*/


		//if (CKeyMgr::Get_Instance()->Key_Up('8'))
		//{
		//	bfirst = false;
		//	//ftime = fTimeDelta;
		//	turn = true;
		//}


		if (goback == true)
		{
			ftime4 += fTimeDelta;

			if (ftime4 >= 2.f)
			{
				texnum = 5;

				ftime4 = 0;

				goback = false;
			}

			

		}



		if (turn == true && 1.f > ftime && ftime > 0.5f && bfirst == false)
		{

			if (texnum == 5)
				texnum = texnum1;   //90도 회전..

			else if (texnum == texnum1)
				texnum = 5;


			bfirst = true;

		}






		if (turn == true && ftime > 1.f)
		{
			//texnum = 1;
			ftime = 0.f;
			turn = false;
			

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&m_WorldMatrix.m[0][0]);
			m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&m_WorldMatrix.m[1][0]);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&m_WorldMatrix.m[2][0]);
			if(texnum != 5)
				m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), 1.f);
		}



		if (turn == true)
		{
			ftime += fTimeDelta;
			m_pTransformCom->Turn(_float3(0.f, 0.f, 1.f), fTimeDelta);
		}



		
		


		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_CaedDesc.pos);
		//m_pTransformCom1->Set_State(CTransform::STATE_POSITION, pos);
	}


	








	return OBJ_NOEVENT;
}

void CCardgame::Late_Tick(_float fTimeDelta)
{
	if (m_bcheck == true)
	{

		__super::Late_Tick(fTimeDelta);
		//SetUp_BillBoard();
		//RECT		rcRect;
		//SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		//POINT		ptMouse;
		//GetCursorPos(&ptMouse);
		//ScreenToClient(g_hWnd, &ptMouse);

		//if (!PtInRect(&rcRect, ptMouse))
		//{
		//	m_fSizeX = 40;
		//	m_fSizeY = 40;
		//	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		if (m_blastdance == true)
		{
			ftime2 += fTimeDelta;

			if (ftime2 > 1.f)
			{
				m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 0.3f);
			}
			if (ftime2 > 2.f && !lastcard)
			{
				m_bcheck = false;

				foreffect		effectdesc;
				ZeroMemory(&effectdesc, sizeof(foreffect));
				Update_Position(m_CaedDesc.pos);
				effectdesc.pos = Get_Position();
				effectdesc.pos.y += 0.5f;
				effectdesc.whateffect = 1;
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CardEffect"), LEVEL_MAZE, TEXT("Layer_Cardeffect_bomb"), &effectdesc);
					
			}

			if (ftime2 > 2.f && lastcard)
			{
				m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 0.3f);
				texnum = 6;
				j += 0.02f;
				m_pTransformCom->Set_Scale(j, j, 1.f);
			}

			if (ftime2 > 5.f && lastcard)
			{
				m_bcheck = false;

				foreffect		effectdesc;
				ZeroMemory(&effectdesc, sizeof(foreffect));
				Update_Position(m_CaedDesc.pos);
				effectdesc.pos = Get_Position();
				effectdesc.pos.y += 0.5f;
				effectdesc.whateffect = 1;
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CardEffect"), LEVEL_MAZE, TEXT("Layer_Cardeffect_bomb"), &effectdesc);

			}

		}

		if (m_bwrong == true)
		{
			ftime3 += fTimeDelta;

			if (ftime3 > 1.f)
			{
				bfirst = false;

				turn = true;
				ftime3 = 0.f;
				m_bwrong = false;
			}
			

		}

		//	//ERR_MSG(L"�浹");
		//}



		/*if (m_bItem)
		{

		}*/

		


		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		if (!m_bPicking && !CPickingMgr::Get_Instance()->Get_Mouse_Has_Construct())
		{
			CPickingMgr::Get_Instance()->Add_PickingGroup(this);
			m_bPicking = true;
		}
	}


}

void CCardgame::WalkingTerrain()
{
	_uint iLevelIndex = CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (!pGameInstance)
		return;
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"), 0);
	if (!pVIBuffer_Terrain)
		return;
	CTransform*	pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Terrain"), TEXT("Com_Transform"), 0);
	if (!pTransform_Terrain)
		return;

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScale = m_pTransformCom->Get_Scale();
	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), m_fRadius);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}



_bool CCardgame::Picking(_float3 * PickingPoint)
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

void CCardgame::PickingTrue()
{

	m_bPickingTrue = true;
	


	if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON))
	{
		bfirst = false;
		
		turn = true;

		frontcard = true;

		if (CInventory_Manager::Get_Instance()->Get_Cardgamecount() == 8)
		{
			lastcard = true;
			m_blastdance = true;
		}
		
	}

	
}



HRESULT CCardgame::Render()
{
	if (m_bcheck == true)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(texnum)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
				return E_FAIL;


 m_pVIBufferCom->Render();



		if (FAILED(Release_RenderState()))
			return E_FAIL;


		



	}




	return S_OK;
}

HRESULT CCardgame::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cardgame"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(180.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;



	/*if (FAILED(__super::Add_Components(TEXT("Com_Transform1"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom1, &TransformDesc)))
		return E_FAIL;*/


	return S_OK;
}

HRESULT CCardgame::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 40);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);



	return S_OK;
}

void CCardgame::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);   // Get View Matrix
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      // Get Inverse of View Matrix (World Matrix of Camera)

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	/*m_pTransformCom1->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom1->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom1->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);*/
}

HRESULT CCardgame::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CCardgame * CCardgame::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCardgame*	pInstance = new CCardgame(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCardgame"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCardgame::Clone(void* pArg)
{
	CCardgame*	pInstance = new CCardgame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCardgame"));
		Safe_Release(pInstance);
	}

	CInventory_Manager::Get_Instance()->Get_Cardgame_list()->push_back(pInstance);

	

	return pInstance;
}


void CCardgame::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	//Safe_Release(m_pTransformCom1);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

