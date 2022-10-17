#include "stdafx.h"
#include "..\Public\BT_NPC.h"

CBT_NPC::CBT_NPC(CNPC * _Actor)
	:Actor(_Actor)
{
	//Safe_AddRef(Actor);
}

void CBT_NPC::Initialize()
{
	//Init_Map();
	Init_Nodes();
}

void CBT_NPC::Init_Nodes()
{
	//Init Root 
	Root = new CSelectorNode(TEXT("Root"));
	//Init Selector
	Selector_OnTarget = new CSelectorNode(TEXT("Ontarget"));

	Selector_OnOwner = new CSelectorNode(TEXT("OnOwner"));
	Selector_NonOwner = new CSelectorNode(TEXT("NonOwner"));
	Selector_NonFight = new CSelectorNode(TEXT("NonFight"));
	Selector_Fight = new CSelectorNode(TEXT("Fight"));
	Selector_NonTarget = new CSelectorNode(TEXT("Nontarget"));
	//Init Sequence
	Sequence_Interact_Actor = new CSequenceNode(TEXT("Interact_Actor"));
	Sequence_Fight = new CSequenceNode(TEXT("Fight"));
	Sequence_Attack = new CSequenceNode(TEXT("Attack"));
	Sequence_Skill = new CSequenceNode(TEXT("Skill1"));
	Sequence_MoveToPos = new CSequenceNode(TEXT("MoveToPos"));
	Sequence_MoveToOwner = new CSequenceNode(TEXT("MoveToOwner"));
	Sequence_Idle = new CSequenceNode(TEXT("Idle"));
	Sequence_SelectAct = new CSequenceNode(TEXT("SelectAct"));
	Sequence_DeadCheck = new CSequenceNode(TEXT("DeadCheck"));
	Sequence_HitCheck = new CSequenceNode(TEXT("HitCheck"));
	Sequence_Dead = new CSequenceNode(TEXT("Dead"));
	Sequence_Hit = new CSequenceNode(TEXT("Hit"));

	//Init Decorator
	BTTask_HasTarget = new CBTTask_HasTarget;
	BTTask_IsActor = new CBTTask_IsActor;
	BTTask_HasOwner = new CBTTask_HasOwner;
	BTTask_IsFirstCall = new CBTTask_IsFirstCall;
	BTTask_IsFightMode = new CBTTask_IsFightMode;

	BTTask_IsSkillRange = new CBTTask_TargetSkillRange;
	BTTask_IsAtkRange = new CBTTask_TargetAtkRange;
	//Init Leaf
	//BTTask_SetRandPos = new CBTTask_SetRandPos; //delete
	BTTask_Idle = new CBTTask_Idle;
	BTTask_Move = new CBTTask_Move;
	BTTask_Interact = new CBTTask_Interact;
	BTTask_SelectTarget = new CBTTask_SelectTarget;
	BTTask_Talk = new CBTTask_Talk;
	//BTTask_Dance = new CBTTask_Dance; //delete
	//BTTask_Arrive = new CBTTask_IsArrive; //delete
	BTTask_Interrupt = new CBTTask_Interrupt;

	//BTTask_SetGoalPos = new CBTTask_SetGoalPos; //delete
	BTTask_Attack = new CBTTask_Attack;
	BTTask_GetCanAttack = new CBTTask_GetCanAttack;
	BTTask_TargetIsDead = new CBTTask_TargetIsDead;
	BTTask_TargetHited = new CBTTask_Target_Hited;
	//BTTask_Fail = new CBTTask_Fail; //delete
	BTTask_GetCanSkill = new CBTTask_GetCanSkill;
	BTTask_Skill = new CBTTask_Skill;
	BTTask_DetectEnemy = new CBTTask_DetectEnemy;
	BTTask_Wait = new CBTTask_Wait;
	BTTask_IsMove_Owner = new CBTTask_TargetMoved(0);
	BTTask_IsMove_Pos = new CBTTask_TargetMoved(4);
	BTTask_CanSelectAct = new CBTTask_GetSelectAct;
	BTTask_Reset = new CBTTask_Clear;
	BTTask_Hit = new CBTTask_Hit;
	BTTask_IsHit = new CBTTask_IsHit;
	BTTask_IsDead = new CBTTask_IsDead;
	BTTask_Dead = new CBTTask_Dead;
	//Add in Root
	Root->Add_Node(BTTask_Interrupt);
	Root->Add_Node(Sequence_Dead);
	Root->Add_Node(Sequence_Hit);
	Root->Add_Node(BTTask_HasOwner);

	//Sequence_Dead
	Sequence_Dead->Add_Node(BTTask_IsDead);
	Sequence_Dead->Add_Node(BTTask_Dead);
	//Sequence_Hit
	Sequence_Hit->Add_Node(BTTask_IsHit);
	Sequence_Hit->Add_Node(BTTask_Hit);
	// HasOwner
	BTTask_HasOwner->Set_DecoratorNodes(Selector_OnOwner, Selector_NonOwner);

	//--Selector_OnOwner--
	Selector_OnOwner->Add_Node(Sequence_MoveToOwner);
	Selector_OnOwner->Add_Node(BTTask_IsFightMode);
	//Sequence_MoveToOwner
	Sequence_MoveToOwner->Add_Node(BTTask_IsMove_Owner);
	Sequence_MoveToOwner->Add_Node(BTTask_Move);
	//Deco_FightMode
	BTTask_IsFightMode->Set_DecoratorNodes(Sequence_Fight, Selector_NonFight);
	//--Sequence_FightMode---
	Sequence_Fight->Add_Node(BTTask_DetectEnemy);
	Sequence_Fight->Add_Node(Selector_Fight);
	Selector_Fight->Add_Node(Sequence_Skill);
	Selector_Fight->Add_Node(Sequence_Attack);
	Selector_Fight->Add_Node(BTTask_Idle);
	//------Sequence_Skill
	Sequence_Skill->Add_Node(BTTask_GetCanSkill);
	Sequence_Skill->Add_Node(BTTask_IsSkillRange);
	//IsSkillRange
	BTTask_IsSkillRange->Set_DecoratorNodes(BTTask_Skill, BTTask_Move);
	//--Sequence_Attack---
	Sequence_Attack->Add_Node(BTTask_GetCanAttack);
	Sequence_Attack->Add_Node(BTTask_IsAtkRange);
	//IsTargetAttakRange_Deco
	BTTask_IsAtkRange->Set_DecoratorNodes(BTTask_Attack, BTTask_Move);

	//-----Selector_NonFight----
	Selector_NonFight->Add_Node(BTTask_IsFirstCall);
	Selector_NonFight->Add_Node(BTTask_Idle);
	//Firstcall_Deco
	BTTask_IsFirstCall->Set_DecoratorNodes(Sequence_Interact_Actor, nullptr);

	//----------Selector_NonOwner--------
	Selector_NonOwner->Add_Node(Sequence_SelectAct);
	Selector_NonOwner->Add_Node(BTTask_HasTarget);
	//Sequence_SelectAct
	Sequence_SelectAct->Add_Node(BTTask_CanSelectAct);
	Sequence_SelectAct->Add_Node(BTTask_Idle);
	//Sequence_SelectAct->Add_Node(BTTask_Wait);
	Sequence_SelectAct->Add_Node(BTTask_SelectTarget);
	//Add in HasTarget_Deco
	BTTask_HasTarget->Set_DecoratorNodes(Selector_OnTarget, Selector_NonTarget);
	//----Selector_OnTarget----
	Selector_OnTarget->Add_Node(Sequence_DeadCheck);
	Selector_OnTarget->Add_Node(Sequence_HitCheck);
	Selector_OnTarget->Add_Node(Sequence_MoveToPos);
	Selector_OnTarget->Add_Node(BTTask_IsActor);
	//Sequence_DeadCheck
	Sequence_DeadCheck->Add_Node(BTTask_TargetIsDead);
	Sequence_DeadCheck->Add_Node(BTTask_Reset);
	//Sequence_HitCheck
	Sequence_HitCheck->Add_Node(BTTask_TargetHited);
	Sequence_HitCheck->Add_Node(BTTask_Reset);
	//Sequence_MoveToPos
	Sequence_MoveToPos->Add_Node(BTTask_IsMove_Pos);
	Sequence_MoveToPos->Add_Node(BTTask_Move);
	//BTTask_IsActor
	BTTask_IsActor->Add_Node(BTTask_Reset);
	BTTask_IsActor->Add_Node(Sequence_Interact_Actor);
	BTTask_IsActor->Add_Node(BTTask_Interact);
	//----------Sequence_OnActor-------------
	Sequence_Interact_Actor->Add_Node(BTTask_Talk);
	Sequence_Interact_Actor->Add_Node(BTTask_Reset);
	//Sequence_Interact_Actor->Add_Node(BTTask_Dance);

	//Selector_NonTarget
	Selector_NonTarget->Add_Node(Sequence_MoveToPos);
	Selector_NonTarget->Add_Node(Sequence_Idle);
	//Sequence_Idle
	Sequence_Idle->Add_Node(BTTask_Idle);
	Sequence_Idle->Add_Node(BTTask_Wait);
	Sequence_Idle->Add_Node(BTTask_Reset);
}

void CBT_NPC::Tick(_float _fTimeDelta)
{
	if(Root->Excute(Actor, _fTimeDelta) == STATUS::SUCCESS)
	{
		return;
	}
}

void CBT_NPC::Free()
{
	Safe_Release(Root);
}
