#pragma once
#include"Behavior_Tree.h"
#include "NPC.h"

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
class CBT_NPC : public CBase
{
public:
	typedef CNPC::NPC_STATE	NPCSTATE;
public:
	CBT_NPC(CNPC* _Actor);
	virtual ~CBT_NPC()= default;

public:
	void Initialize();
	void Init_Nodes();
	void Init_Map();
	void Tick(_float _fTimeDelta);
	//void Late_Tick(_float _fTimeDelta);
public:
	CNPC::NPC_STATE Find_Running();

public:/*for Behavior Tree*/
	CSequenceNode* Root = nullptr;
public:/*Selector Nodes*/
	CSelectorNode* Selector_OnTarget = nullptr;
public:/*Sequence Nodes*/
	CSequenceNode* Sequence_NonTarget = nullptr;
	//CSequenceNode* Sequence_OnTarget = nullptr;
public:/*If_Nodes*/
	CBTTask_HasTarget* BTTask_HasTarget = nullptr;
	
public:/*Leaf Nodes*/
	CBTTask_SetRandPos* BTTask_SetRandPos = nullptr;
	CBTTask_Idle* BTTask_Idle = nullptr;
	CBTTask_Move* BTTask_Move = nullptr;
	CBTTask_Interact* BTTask_Interact = nullptr;
	CBTTask_SelectTarget* BTTask_SelectTarget = nullptr;

private:
	CNPC* Actor = nullptr;


	map<NPCSTATE, STATUS> m_mapNPCStatus;
public:
	virtual void Free() override;
};


//Actions
class CBTTask_SetRandPos : public CNode
{
public:

	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		dynamic_cast<CNPC*>(_Obj)->Set_RandPos(_fTimeDelta);
		return STATUS::SUCCESS;
	}

};

class CBTTask_Idle : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		_float fRemainTime = 4.f;
		if (fRemainTime < dynamic_cast<CNPC*>(_Obj)->Get_InteractiveTime())
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::IDLE, false);
			dynamic_cast<CNPC*>(_Obj)->Reset_InteractTime();
			return STATUS::SUCCESS;
		}
		else if (dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::IDLE) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::IDLE, true);
			dynamic_cast<CNPC*>(_Obj)->Idle(_fTimeDelta);
			return STATUS::RUNNING;
		}
	}
};

class CBTTask_Move : public CNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		if (dynamic_cast<CNPC*>(_Obj)->Get_IsArrive() && dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::MOVE) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::MOVE, false);
			return STATUS::SUCCESS;
		}
		else if(dynamic_cast<CNPC*>(_Obj)->Find_Activated() == CNPC::STATE_END
			|| dynamic_cast<CNPC*>(_Obj)->Get_Activated(CNPC::MOVE) == true)
		{
			dynamic_cast<CNPC*>(_Obj)->Set_Activate(CNPC::MOVE, true);
			dynamic_cast<CNPC*>(_Obj)->Move(_fTimeDelta);
			return STATUS::RUNNING;
		}
		else
		{
			return STATUS::FAIL;
		}
		//elseif Exception
	}
};

class CBTTask_Interact : public CNode
{
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
			dynamic_cast<CNPC*>(_Obj)->Interact(_fTimeDelta);
			return STATUS::RUNNING;
		}
	}
};

class CBTTask_SelectTarget : public CNode
{
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		dynamic_cast<CNPC*>(_Obj)->Select_Target(_fTimeDelta);
		return STATUS::SUCCESS;
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

END