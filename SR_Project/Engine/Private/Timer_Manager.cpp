#include "..\Public\Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{

}

HRESULT CTimer_Manager::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr != Find_Timer(pTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(pTimerTag, pTimer);

	return S_OK;
}

_float CTimer_Manager::Get_TimeDelta(const _tchar * pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);
	if(nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::SetZero_TimeDelta(const _tchar * pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Set_ZeroTimerDelta();
}

void CTimer_Manager::Update(const _tchar * pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return;

	return pTimer->Update();
}

void CTimer_Manager::Delete_Timer(const _tchar * pTimerTag)
{
	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), CTag_Finder(pTimerTag));

	if (iter == m_Timers.end())
		return;

	Safe_Release(iter->second);
}

CTimer * CTimer_Manager::Find_Timer(const _tchar * pTimerTag)
{
	auto iter = find_if(m_Timers.begin(), m_Timers.end(), CTag_Finder(pTimerTag));

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;	
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();
}
