#include "stdafx.h"
#include "..\Public\DayCycle.h"
#include "GameInstance.h"


IMPLEMENT_SINGLETON(CDayCycle);

CDayCycle::CDayCycle()
{
	m_dwTime = GetTickCount();
}

void CDayCycle::DayCycleTick()
{
	_float Tickcount = GetTickCount();
	if (m_dwTime + 30000 < Tickcount)
	{
		switch (m_eDayState)
		{
		case Client::DAY_MORNING:
			m_eDayState = DAY_DINNER;
			NotifyObserver();
			break;
		case Client::DAY_DINNER:
		{
			_uint random = rand() % 2 + 1;

			CGameInstance* pInstance = CGameInstance::Get_Instance();
			if (random == 1)
				pInstance->PlaySounds(TEXT("changetonight.wav"), SOUND_UI, 0.9f);
			else
				pInstance->PlaySounds(TEXT("changetonight2.wav"), SOUND_UI, 0.9f);
			m_eDayState = DAY_NIGHT;
			NotifyObserver();
			break;
		}
			
			
		case Client::DAY_NIGHT:
			m_eDayState = DAY_MORNING;
			NotifyObserver();
			break;
		}
		m_dwTime = GetTickCount();

	}
}

void CDayCycle::RegisterObserver(CObserver * observer)
{
	m_Observer.push_back(observer);
}


void CDayCycle::RemoveObserver(CObserver * observer)
{
	std::vector<CObserver *>::iterator iter;
	iter = find(m_Observer.begin(), m_Observer.end(), observer);
	if (iter != m_Observer.end())
	{
		m_Observer.erase(iter);
	}
}

void CDayCycle::AllRemoveObserver()
{
	for (auto& iter  = m_Observer.begin() ; iter != m_Observer.end() ;)
	{
		iter = m_Observer.erase(iter);
	}

	m_Observer.clear();
}

void CDayCycle::NotifyObserver()
{
	for (auto& iter : m_Observer)
	{
		iter->Update(m_eDayState);
	}
}

void CDayCycle::Free()
{
	//__super::Free();
	m_Observer.clear();
}
