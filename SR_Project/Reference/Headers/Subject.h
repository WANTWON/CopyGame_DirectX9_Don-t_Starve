#pragma once
#include "Base.h"
#include "Observer.h"
BEGIN(Engine)

class CObserver;
class ENGINE_DLL CSubject abstract : public CBase
{
protected:
	CSubject() {};
	virtual ~CSubject() = default;

public:
	virtual void RegisterObserver(CObserver *observer) = 0;
	virtual void RemoveObserver(CObserver *observer) = 0;
	virtual void NotifyObserver() = 0;

public:
	virtual void Free() override {};
};

END