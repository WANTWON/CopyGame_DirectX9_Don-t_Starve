#pragma once
#include "Client_Defines.h"
#include "ObserverTarget.h"

BEGIN(Engine)
class CObserver;
class CObserverTarget;
END

BEGIN(Client)
class CDayCycle : public CObserverTarget
{
	DECLARE_SINGLETON(CDayCycle);

public:
	CDayCycle();
	virtual ~CDayCycle() = default;

public:
	void DayCycleTick();
	void AllRemoveObserver();

	virtual void RegisterObserver(CObserver *observer)override;
	virtual void RemoveObserver(CObserver *observer) override;
	virtual void NotifyObserver() override;

	DAY_STATE Get_DayState() { return m_eDayState; }

private:
	//void NotifyObserver();

private:
	vector<CObserver*> m_Observer;
	DAY_STATE m_eDayState = DAY_MORNING;
	DWORD m_dwTime;

public:
	virtual void Free() override;
};

END