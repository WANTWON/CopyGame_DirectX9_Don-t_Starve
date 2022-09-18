#pragma once
#include"Behavior_Tree.h"
#include "NPC.h"
#include "Monster.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider_Rect;
END

BEGIN(Client)


class CBTTask_HasTarget;
class CBTTask_SetRandPos;
class CBTTask_Idle;
class CBTTask_Move;
class CBTTask_Interact;
class CBTTask_SelectTarget;
class CBTTask_IsArrive;
class CBTTask_Talk;
class CBTTask_Dance;
class CBTTask_IsActor;
class CBTTask_Interrupt;
class CBTTask_HasOwner;

class CBTTask_IsFightMode;
class CBTTask_TargetMoved;
class CBTTask_IsFirstCall;
class CBTTask_SetGoalPos;
class CBTTask_GetCanAttack;
class CBTTask_Attack;
class CBTTask_IsFightMode;
class CBTTask_Fail;
class CBTTask_TargetIsDead;
class CBTTask_Target_Hited;
class CBTTask_Owner_Close;
class CBTTask_TargetAtkRange;
class CBTTask_TargetSkillRange;
class CBTTask_GetCanSkill;
class CBTTask_Skill;
//class CBTTask_FollowTarget;
class CBT_NPC : public CBase
{
public:
	typedef CNPC::NPC_STATE	NPCSTATE;
public:
	CBT_NPC(CNPC* _Actor);
private:
	virtual ~CBT_NPC() = default;

public:
	void Initialize();
	void Init_Nodes();
	//void Init_Map();
	void Tick(_float _fTimeDelta);
	//void Late_Tick(_float _fTimeDelta);
public:
	//CNPC::NPC_STATE Find_Running();
	//void Clear_Status(void);
public:/*for Behavior Tree*/
	CSequenceNode* Root = nullptr;
public:/*Selector Nodes*/
	CSelectorNode* Selector_OnTarget = nullptr;
	CSelectorNode* Selector_Main = nullptr;
	CSelectorNode* Selector_OnOwner = nullptr;
	CSelectorNode* Selector_NonFight = nullptr;
	CSelectorNode* Selector_Attack = nullptr;
	CSelectorNode* Selector_Skill = nullptr;
public:/*Sequence Nodes*/
	CSequenceNode* Sequence_NonTarget = nullptr;
	CSequenceNode* Sequence_Interact_Actor = nullptr;
	CSequenceNode* Sequence_OnTarget = nullptr;
	CSequenceNode* Sequence_Attack = nullptr; 
	CSequenceNode* Sequence_MoveToTarget = nullptr;
	CSequenceNode* Sequence_Fight = nullptr;
	CSequenceNode*Sequence_Skill = nullptr;
	//CSequenceNode* Sequence_OnTarget = nullptr;
public:/*If_Nodes*/
	CBTTask_HasTarget* BTTask_HasTarget = nullptr;
	CBTTask_HasTarget* BTTask_HasEnemy = nullptr;
	CBTTask_IsActor* BTTask_IsActor = nullptr;
	CBTTask_HasOwner* BTTask_HasOwner = nullptr;
	CBTTask_TargetMoved* BTTask_IsTargetMove = nullptr;
	CBTTask_TargetSkillRange* BTTask_IsSkillRange = nullptr;
	CBTTask_TargetAtkRange* BTTask_IsAtkRange = nullptr;
	CBTTask_IsFirstCall* BTTask_IsFirstCall = nullptr;
	CBTTask_IsFightMode* BTTask_IsFightMode = nullptr;
	CBTTask_TargetMoved* BTTask_IsOwner_Closed = nullptr; //CBTTask_Owner_Close class 삭제할것
public:/*Leaf Nodes*/
	CBTTask_SetRandPos* BTTask_SetRandPos = nullptr;
	CBTTask_Idle* BTTask_Idle = nullptr;
	CBTTask_Move* BTTask_Move = nullptr;
	CBTTask_Interact* BTTask_Interact = nullptr;
	CBTTask_SelectTarget* BTTask_SelectTarget = nullptr;
	CBTTask_Dance* BTTask_Dance = nullptr;
	CBTTask_Talk* BTTask_Talk = nullptr;
	CBTTask_IsArrive* BTTask_Arrive = nullptr;
	CBTTask_Interrupt* BTTask_Interrupt = nullptr;
	
	CBTTask_SetGoalPos* BTTask_SetGoalPos = nullptr;
	CBTTask_Attack* BTTask_Attack = nullptr;
	CBTTask_GetCanAttack* BTTask_GetCanAttack = nullptr;
	CBTTask_TargetIsDead* BTTask_TargetIsDead = nullptr;
	CBTTask_Target_Hited* BTTask_TargetHited = nullptr;
	CBTTask_GetCanSkill* BTTask_GetCanSkill = nullptr;
	CBTTask_Skill* BTTask_Skill = nullptr;
	//Default
	CBTTask_Fail* BTTask_Fail = nullptr;
private:
	CNPC* Actor = nullptr;
	//map<NPCSTATE, STATUS> m_mapNPCStatus;
public:
	virtual void Free() override;
};


//Actions
class CBTTask_SetRandPos : public CNode
{
public:

	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		//cout << "Rand Pos" << endl;
		dynamic_cast<CNPC*>(_Obj)->Set_RandPos(_fTimeDelta);
		return STATUS::SUCCESS;
	}

};

class CBTTask_Idle : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		_float fRemainTime = 2.f;
		if (fRemainTime < dynamic_cast<CNPC*>(_Obj)->Get_InteractiveTime()
			&& dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::IDLE) == true)
		{
			//cout << "Idle End" << endl;
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::IDLE, false);
			dynamic_cast<CNPC*>(_Obj)->Reset_InteractTime();
			return STATUS::SUCCESS;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::IDLE, true);
			//cout << "Idle Start" << endl;
			dynamic_cast<CNPC*>(_Obj)->Idle(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::IDLE) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Idle(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else
		{
			return STATUS::NOT_STARTED;
		}
	}
};

class CBTTask_Move : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_IsArrive()
			&& dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::MOVE) == true)
		{
			//cout << "Move End" << endl;
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::MOVE, false);
			return STATUS::SUCCESS;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END)
		{
			//cout << "Move Start" << endl;
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::MOVE, true);
			dynamic_cast<CNPC*>(_Obj)->Move(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::MOVE) == true
			&& dynamic_cast<CNPC*>(_Obj)->Get_IsArrive() == false)
		{
			dynamic_cast<CNPC*>(_Obj)->Move(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else
		{
			return STATUS::NOT_STARTED;
		}
		//elseif Exception
	}
};
// Object Interact
class CBTTask_Interact : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (!dynamic_cast<CNPC*>(_Obj)->Get_CanInteract() 
			&& dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::INTERACT) == true)
		{		
			dynamic_cast<CNPC*>(_Obj)->Reset_Target();
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::INTERACT, false);
			return STATUS::SUCCESS;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Get_IsArrive()
			&& dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::INTERACT) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::INTERACT, true);
			dynamic_cast<CNPC*>(_Obj)->Interaction(_fTimeDelta);
			return STATUS::RUNNING;
		}
	}
};

class CBTTask_Talk : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (!dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::TALK) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::TALK, false);

			if (dynamic_cast<CNPC*>(_Obj)->Get_NextAct())
			{
				dynamic_cast<CNPC*>(_Obj)->Set_Interact(true);
				return STATUS::SUCCESS;
			}
			else 
			{
				dynamic_cast<CNPC*>(_Obj)->Reset_Target();
				return STATUS::FAIL;
			}
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::TALK) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::TALK, true);
			dynamic_cast<CNPC*>(_Obj)->Talk(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else {
			return STATUS::NOT_STARTED;
		}
	}
};

class CBTTask_Dance : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (!dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::DANCE) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Reset_Target();
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::DANCE, false);
			return STATUS::SUCCESS;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::DANCE) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::DANCE, true);
			dynamic_cast<CNPC*>(_Obj)->Dance(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else {
			return STATUS::NOT_STARTED;
		}
	}
};
class CBTTask_IsArrive : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (static_cast<CNPC*>(_Obj)->Get_IsArrive())
		{
			//dynamic_cast<CNPC*>(_Obj)->Clear_Activated();
			if (static_cast<CNPC*>(_Obj)->Get_Activated(CNPC::MOVE))
			{
				static_cast<CNPC*>(_Obj)->Set_Activate(CNPC::MOVE, false);
			}

			return STATUS::SUCCESS;
		}
		else
		{
			return STATUS::FAIL;
		}

	}
};
class CBTTask_SelectTarget : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		dynamic_cast<CNPC*>(_Obj)->Select_Target(_fTimeDelta);
		if (dynamic_cast<CNPC*>(_Obj)->Get_HasTarget())
		{
			return STATUS::SUCCESS;
		}
		else
		{
			return STATUS::FAIL;
		}
	}
};

class CBTTask_Interrupt : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		
		if (dynamic_cast<CNPC*>(_Obj)->Get_Interrupted())
		{
			dynamic_cast<CNPC*>(_Obj)->Interrupted(_fTimeDelta);
			return STATUS::SUCCESS;
		}
		else
		{
			return STATUS::FAIL;
		}
	}
};

class CBTTask_SetGoalPos :public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{

		if (dynamic_cast<CNPC*>(_Obj)->Set_TargetPos())
		{
			return STATUS::SUCCESS;
		}
		else
		{
			return STATUS::FAIL;
		}
	}

};

class CBTTask_GetCanAttack : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{

		if (dynamic_cast<CNPC*>(_Obj)->Get_CanAttack())
		{
			return STATUS::SUCCESS;
		}
		else
		{
			return STATUS::FAIL;
		}
	}
};

class CBTTask_Attack : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (!dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::ATTACK) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Reset_Target();
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::ATTACK, false);
			return STATUS::SUCCESS;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::ATTACK) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::ATTACK, true);
			dynamic_cast<CNPC*>(_Obj)->Attack(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else {
			return STATUS::NOT_STARTED;
		}
	}
};

class CBTTask_GetCanSkill : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{

		if (dynamic_cast<CNPC*>(_Obj)->Get_CanSkill() 
			&& dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::SKILL) == true)
		{
			return STATUS::SUCCESS;
		}
		else
		{
			return STATUS::FAIL;
		}
	}
};

class CBTTask_Skill : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (!dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::SKILL) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Reset_Target();
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::SKILL, false);
			return STATUS::SUCCESS;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Get_CanInteract()
			&& dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::SKILL) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::SKILL, true);
			dynamic_cast<CNPC*>(_Obj)->Skill(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else {
			return STATUS::NOT_STARTED;
		}
	}
};

class CBTTask_TargetIsDead : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (static_cast<CNPC*>(_Obj)->Get_Target()->Get_Dead() == true
			|| static_cast<CNPC*>(_Obj)->Get_Target() == nullptr)
		{
			static_cast<CNPC*>(_Obj)->Clear_Activated();
			static_cast<CNPC*>(_Obj)->Reset_Target();
			return STATUS::SUCCESS;
		}
		else
		{
			return STATUS::FAIL;
		}
	}
};

class CBTTask_Target_Hited : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		OBJID eID = static_cast<CPawn*>(dynamic_cast<CNPC*>(_Obj)->Get_Target())->Get_ObjID();

		if (eID == OBJ_MONSTER)
		{
			if (static_cast<CMonster*>(dynamic_cast<CNPC*>(_Obj)->Get_Target())->Get_Hited())
			{
				static_cast<CNPC*>(_Obj)->Clear_Activated();
				static_cast<CNPC*>(_Obj)->Reset_Target();
				return STATUS::SUCCESS;
			}

		}
		else
		{
			return STATUS::FAIL;
		}
	}

};



//Decorator
class CBTTask_HasTarget : public CDecorator_If
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_HasTarget())
		{//Sequence
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}

};

class CBTTask_TargetAtkRange : public CDecorator_If
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (!dynamic_cast<CNPC*>(_Obj)->Get_Target_Moved(_fTimeDelta, 1))
		{
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}
};

class CBTTask_TargetSkillRange : public CDecorator_If
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (!dynamic_cast<CNPC*>(_Obj)->Get_Target_Moved(_fTimeDelta, 2))
		{
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}
};

class CBTTask_TargetMoved : public CDecorator_If
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_Target_Moved(_fTimeDelta, 0))
		{
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}
};

//Decorator
class CBTTask_IsActor : public CDecorator_If
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		OBJID eID = dynamic_cast<CPawn*>(dynamic_cast<CNPC*>(_Obj)->Get_Target())->Get_ObjID();
		
		if (eID == OBJID::OBJ_PLAYER)
		{//Sequence
			//return TrueNode->Excute(_Obj, _fTimeDelta);
			return Get_VecNodes()[1]->Excute(_Obj, _fTimeDelta);
		}
		else if (eID == OBJID::OBJ_MONSTER)
		{
			_bool bAggro = dynamic_cast<CMonster*>(dynamic_cast<CNPC*>(_Obj)->Get_Target())->Get_Aggro();
			if (bAggro)
			{
				return Get_VecNodes()[0]->Excute(_Obj, _fTimeDelta); //Attack
			}
			else
			{
				return Get_VecNodes()[1]->Excute(_Obj, _fTimeDelta);
			}
		}
		else if (eID == OBJ_OBJECT)
		{
			return Get_VecNodes()[2]->Excute(_Obj, _fTimeDelta); //Object_Interact
		}
	}
};

class CBTTask_IsFirstCall : public CDecorator_If
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_FirstCall())
		{
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return STATUS::FAIL;
		}
	}
};


class CBTTask_HasOwner : public CDecorator_If
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_HasOwner())
		{//Sequence
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}
};

class CBTTask_IsFightMode : public CDecorator_If
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_FightMode())
		{//Sequence
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return  FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}

};

class CBTTask_Owner_Close : public CDecorator_If
{

	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_CloseToOwner())
		{//Sequence
			return TrueNode->Excute(_Obj, _fTimeDelta);
		}
		else
		{
			return  FalseNode->Excute(_Obj, _fTimeDelta);
		}
	}
};

class CBTTask_Fail : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		return STATUS::FAIL;
	}

};

END