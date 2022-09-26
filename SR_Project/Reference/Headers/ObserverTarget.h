#pragma once
#include "Base.h"
#include "Observer.h"


BEGIN(Engine)
class CObserver;
class ENGINE_DLL CObserverTarget abstract : public CBase
{
protected:
	CObserverTarget();
	virtual ~CObserverTarget() = default;

public:
	virtual void RegisterObserver(CObserver *observer, _uint iLevelIndex) = 0;
	virtual void RemoveObserver(CObserver *observer, _uint iLevelIndex) = 0;
	virtual void NotifyObserver() = 0;

public:
	virtual void Free() override;
};

END