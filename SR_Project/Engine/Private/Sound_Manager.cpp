#include "..\Public\Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
	: CBase()
	, m_pSystem(nullptr)
{
	ZeroMemory(m_pChannelArr, sizeof(m_pChannelArr));
}

HRESULT CSound_Manager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	//FMOD_System_Create(&m_pSystem);
	FMOD::System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	//FMOD_System_Init(m_pSystem, MAXCHANNEL, FMOD_INIT_NORMAL, NULL);
	m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	if (m_pSystem == nullptr)
		return E_FAIL;

	LoadSoundFile();

	return S_OK;
}

void CSound_Manager::PlaySound(TCHAR * pSoundKey, const _uint& eID, const float& fVolume)
{
	std::map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = std::find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[eID]);



	//if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	//{
	//	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);
	//}

	m_pChannelArr[eID]->setVolume(fVolume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	m_pSystem->update();
	//FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlayBGM(TCHAR * pSoundKey, const float& fVolume)
{
	std::map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = std::find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;
	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[0]);

	//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[0]);

	m_pChannelArr[0]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[0]->setVolume(fVolume);
	//FMOD_Channel_SetMode(m_pChannelArr[0], FMOD_LOOP_NORMAL);

	//FMOD_Channel_SetVolume(m_pChannelArr[0], fVolume);
	m_pSystem->update();
	//FMOD_System_Update(m_pSystem);
}

void CSound_Manager::StopSound(const _uint& eID)
{
	m_pChannelArr[eID]->stop();
	//FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < SOUND_MAX; ++i)
	{
		m_pChannelArr[i]->stop();
		//FMOD_Channel_Stop(m_pChannelArr[i]);
	}
}

void CSound_Manager::SetChannelVolume(const _uint& eID, const float& fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	//FMOD_System_Update(m_pSystem);
	m_pSystem->update();
}

int CSound_Manager::VolumeUp(const _uint & eID, const _float & _vol)
{
	if (m_volume < SOUND_MAX) {
		m_volume += _vol;
	}

	m_pChannelArr[eID]->setVolume(m_volume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSound_Manager::VolumeDown(const _uint & eID, const _float & _vol)
{
	if (m_volume > SOUND_MIN) {
		m_volume -= _vol;
	}

	m_pChannelArr[eID]->setVolume(m_volume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSound_Manager::Pause(const _uint & eID)
{
	m_bPause = !m_pChannelArr[eID]->getPaused(&m_bPause);
	m_pChannelArr[eID]->setPaused(m_bPause);

	//FMOD_Channel_SetPaused(m_pChannelArr[eID], m_bPause);

	return 0;
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../../Client/Bin/Resources/Sounds/*.*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Client/Bin/Resources/Sounds/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		// ../Sound/"
		strcpy_s(szFullPath, szCurPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		//FMOD_SOUND* pSound = nullptr; 
		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);
		//FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, (int)iLength, pSoundKey, (int)iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();
	//FMOD_System_Update(m_pSystem);

	_findclose(handle);
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		Mypair.second->release();
		//FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
	//FMOD_System_Release(m_pSystem);
	//FMOD_System_Close(m_pSystem);
}
