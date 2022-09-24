#pragma once
#include "Client_Defines.h"
#include "Subject.h"

BEGIN(Engine)
class CObserver;
class CSubject;
END

BEGIN(Client)
class CDayCycle : public CSubject
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

private:
	//void NotifyObserver();

private:
	vector<CObserver*> m_Observer;
	DAY_STATE m_eDayState;
	DWORD m_dwTime;

public:
	virtual void Free() override;
};

END