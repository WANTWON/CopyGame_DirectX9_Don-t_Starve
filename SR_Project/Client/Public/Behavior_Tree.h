#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include <algorithm>

BEGIN(Client)
enum class STATUS { NOT_STARTED, SUCCESS, FAIL, RUNNING, STATUS_END};


//Node
class CNode : public CBase
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) = 0;
public:
	virtual void Free() override
	{}
};

//CompositeNode
class CCompositeNode abstract : public CNode
{

public:
	vector<CNode*> Get_VecNodes(void) { return m_vecNodes; }
	void Add_Node(CNode* _pNode) { m_vecNodes.push_back(_pNode); }

protected:
	vector<CNode*> Get_ShuffeldNodes(void) const {
		vector<CNode*> vecTemp = m_vecNodes;
		random_shuffle(vecTemp.begin(), vecTemp.end());
		return vecTemp;
	}
private:
	vector<CNode*> m_vecNodes;
public:
	virtual void Free() override
	{
		for (auto& iter : m_vecNodes)
		{
			Safe_Release(iter);
			iter = nullptr;
		}
		m_vecNodes.clear();
	}
};

//SequenceNode
class CSequenceNode : public CCompositeNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		for (CNode* iter : Get_VecNodes())
		{
			STATUS eResult = iter->Excute(_Obj, _fTimeDelta);

			if (eResult == STATUS::FAIL)
			{
				return STATUS::FAIL;
			}
			else if (eResult == STATUS::RUNNING)
			{
				return STATUS::RUNNING;
			}
		}
		return STATUS::SUCCESS;
	}
public:
	virtual void Free() override
	{
		__super::Free();
	}
};

//Selector
class CSelectorNode : public CCompositeNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		for (CNode* iter : Get_VecNodes())
		{

			STATUS eResult = iter->Excute(_Obj, _fTimeDelta);
			if (eResult == STATUS::SUCCESS)
			{
				return  STATUS::SUCCESS;
			}
			else if (eResult == STATUS::RUNNING)
			{
				return STATUS::RUNNING;
			}
		}
		return STATUS::FAIL;
	}
public:
	virtual void Free() override
	{
		__super::Free();
	}
};

//RandomSelector
class CRandomSelector : public CCompositeNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) override
	{
		for (CNode* iter : Get_ShuffeldNodes())
		{
			STATUS eResult = iter->Excute(_Obj, _fTimeDelta);

			if (eResult == STATUS::SUCCESS)
			{
				return  STATUS::SUCCESS;
			}
			else if (eResult == STATUS::RUNNING)
			{
				return STATUS::RUNNING;
			}
		}
		return STATUS::FAIL;
	}
public:
	virtual void Free() override
	{
		__super::Free();
	}
};
//Decorator_If
class CDecorator_If : public CCompositeNode
{
public:
	virtual STATUS Excute(CGameObject* _Obj, _float _fTimeDelta) = 0;

	void	Set_DecoratorNodes(CNode* _TrueNode, CNode* _FalseNode)
	{
		TrueNode = _TrueNode;
		FalseNode = _FalseNode;
	}
protected:
	CNode* TrueNode = nullptr;
	CNode* FalseNode = nullptr;
public:
	virtual void Free() override
	{
		__super::Free();
	}
};

END

