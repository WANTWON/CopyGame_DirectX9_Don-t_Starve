#include "stdafx.h"
#include "..\Public\BT_NPC.h"

CBT_NPC::CBT_NPC(CNPC * _Actor)
	:Actor(_Actor)
{
	Safe_AddRef(Actor);
}

void CBT_NPC::Initialize()
{
	Init_Map();
	Init_Nodes();
}

void CBT_NPC::Init_Nodes()
{
	//Init Root 
	Root = new CSequenceNode;

	//Init Selector
	Selector_OnTarget = new CSelectorNode;
	//Init Sequence
	Sequence_NonTarget = new CSequenceNode;
	//Sequence_OnTarget = new CSequenceNode;
	//Init Decorator
	BTTask_HasTarget = new CBTTask_HasTarget;

	//Init Leaf
	BTTask_SetRandPos = new CBTTask_SetRandPos;
	BTTask_Idle = new CBTTask_Idle;
	BTTask_Move = new CBTTask_Move;
	BTTask_Interact = new CBTTask_Interact;
	BTTask_SelectTarget = new CBTTask_SelectTarget;


	//Add in Root
	Root->Add_Node(BTTask_HasTarget);
	//Add in HasTarget_Deco
	BTTask_HasTarget->Set_DecoratorNodes(Selector_OnTarget, Sequence_NonTarget);
	//Add InSequence_NoneTarget 
	Sequence_NonTarget->Add_Node(BTTask_Idle);
	Sequence_NonTarget->Add_Node(BTTask_Move);
	Sequence_NonTarget->Add_Node(BTTask_SetRandPos);
	Sequence_NonTarget->Add_Node(BTTask_SelectTarget);
	//----------------------
	Selector_OnTarget->Add_Node(BTTask_Interact);
	Selector_OnTarget->Add_Node(BTTask_Move);
}

void CBT_NPC::Init_Map()
{
	for (_uint i = 0; i < (_uint)CNPC::NPC_STATE::STATE_END; ++i)
	{
		m_mapNPCStatus.emplace((CNPC::NPC_STATE)i, STATUS::NOT_STARTED);
	}
}

void CBT_NPC::Tick(_float _fTimeDelta)
{
	if(Root->Excute(Actor, _fTimeDelta) == STATUS::SUCCESS)
	{
		return;
	}
}

CNPC::NPC_STATE CBT_NPC::Find_Running()
{
	for(auto& iter : m_mapNPCStatus)
	{
		if (STATUS::RUNNING == iter.second)
			return iter.first;
	}

	return CNPC::NPC_STATE::STATE_END;
}

void CBT_NPC::Free()
{

	Root->Free();
	Safe_Release(Actor);
}
