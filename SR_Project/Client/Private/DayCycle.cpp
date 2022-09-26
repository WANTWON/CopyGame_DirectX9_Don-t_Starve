#include "stdafx.h"
#include "..\Public\DayCycle.h"
#include "GameInstance.h"
#include "Level_Manager.h"


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
		CGameInstance* pInstance = CGameInstance::Get_Instance();
		switch (m_eDayState)
		{
		case Client::DAY_MORNING:
			
			
			m_eDayState = DAY_DINNER;
			if (CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY ||
				CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex() == LEVEL_HUNT)
			{
				pInstance->PlaySounds(TEXT("changetodinner.wav"), SOUND_UI, 0.3f);
			}
				
			NotifyObserver();
			break;
		case Client::DAY_DINNER:
		{
			

			if (CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY ||
				CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex() == LEVEL_HUNT)
			{
				_uint random = rand() % 2 + 1;
				if (random == 1)
					pInstance->PlaySounds(TEXT("changetonight.wav"), SOUND_UI, 0.3f);
				else
					pInstance->PlaySounds(TEXT("changetonight2.wav"), SOUND_UI, 0.3f);
			}
				
			
			m_eDayState = DAY_NIGHT;
			NotifyObserver();
			break;
		}
			
			
		case Client::DAY_NIGHT:
			
			if (CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY ||
				CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex() == LEVEL_HUNT)
			{
				pInstance->PlaySounds(TEXT("changetomorning.wav"), SOUND_UI, 0.5f);
			}

			m_eDayState = DAY_MORNING;
			NotifyObserver();
			break;
		}
		m_dwTime = GetTickCount();

	}
}

void CDayCycle::RegisterObserver(CObserver * observer, _uint iCycleIndex)
{
	m_Observer[iCycleIndex].push_back(observer);
}


void CDayCycle::RemoveObserver(CObserver * observer, _uint iCycleIndex)
{
	std::vector<CObserver *>::iterator iter;
	iter = find(m_Observer[iCycleIndex].begin(), m_Observer[iCycleIndex].end(), observer);
	if (iter != m_Observer[iCycleIndex].end())
	{
		m_Observer[iCycleIndex].erase(iter);
	}
}

void CDayCycle::AllRemoveObserver()
{
	for (_uint iCycleIndex = CYCLE_STATIC; iCycleIndex < CYCLE_END; ++iCycleIndex)
	{
		if ((CYCLE_GROUP)iCycleIndex == CYCLE_STATIC)
			continue;

		for (auto& iter = m_Observer[iCycleIndex].begin(); iter != m_Observer[iCycleIndex].end();)
		{
			iter = m_Observer[iCycleIndex].erase(iter);
		}

		m_Observer[iCycleIndex].clear();
	}

	
}

void CDayCycle::NotifyObserver()
{
	for (_uint iCycleIndex = CYCLE_STATIC; iCycleIndex < CYCLE_END; ++iCycleIndex)
	{
		for (auto& iter : m_Observer[iCycleIndex])
		{
			iter->Update(m_eDayState);
		}
	}
}

void CDayCycle::Free()
{
	//__super::Free();
	for (_uint iCycleIndex = CYCLE_STATIC; iCycleIndex < CYCLE_END; ++iCycleIndex)
	{
		m_Observer[iCycleIndex].clear();
	}
}
