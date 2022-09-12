#include "stdafx.h"
#include "..\Public\NPC.h"
#include "GameInstance.h"


CNPC::CNPC(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CInteractive_Object(pGraphic_Device)
{
}

CNPC::CNPC(const CNPC & rhs)
	:CInteractive_Object(rhs)
{
}

HRESULT CNPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	//Test
	m_eObjID = OBJID::OBJ_NPC;
	m_eInteract_OBJ_ID = INTERACTOBJ_ID::NPC;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

	Init_Map();

	return S_OK;
}

int CNPC::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Update_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_iCurrentLevelndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();
	
	return OBJ_NOEVENT;
}

void CNPC::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CNPC::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CNPC::Find_Priority()
{
}

_float3 CNPC::Get_Pos(void)
{
	return (m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}



void CNPC::Free()
{
	__super::Free();
}

CNPC::NPC_STATE CNPC::Find_Activated(void)
{
	for (auto& iter : m_NPCStates)
	{
		if (iter.second == true)
		{
			return iter.first;
		}
	}

	return NPC_STATE::STATE_END;
}

void CNPC::Init_Map(void)
{
	for (_uint i = 0; i < (_uint)CNPC::NPC_STATE::STATE_END; ++i)
	{
		m_NPCStates.emplace((CNPC::NPC_STATE)i, false);
	}
}
