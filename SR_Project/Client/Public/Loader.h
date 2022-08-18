#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 다음레벨을 위한 데이터를 준비한다. */

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLoader() = default;

public:
	LEVEL Get_NextLevelID() const {
		return m_eNextLevel;
	}

	CRITICAL_SECTION Get_CriticalSection() {
		return m_CriticalSection;
	}

	_bool Get_Finished() const {
		return m_isFinished;
	}

	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

public:
	HRESULT Initialize(LEVEL eNextLevel);

	HRESULT Loading_ForLogoLevel();
	HRESULT Loading_ForGamePlayLevel();




private:
	LEVEL					m_eNextLevel = LEVEL_END;
	_bool					m_isFinished = false;
	_tchar					m_szLoadingText[MAX_PATH] = TEXT("");
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;

private:
	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_CriticalSection;


public:
	static CLoader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevel);
	virtual void Free();
};

END