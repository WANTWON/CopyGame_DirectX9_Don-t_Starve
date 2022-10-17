#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "Observer.h"

BEGIN(Client)

class CLevel_Hunt final : public CLevel, public CObserver
{
private:
	CLevel_Hunt(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Hunt() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Object(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar * pLayerTag);

public: //test
	void Start_Camera_Motion();
	void Set_NextLevel(_bool _dd) { m_bNextLevel = _dd; }
	void Play_Music();
	virtual void Update(_uint eDayState) override;
	_bool m_bNextLevel = false;

private:
	DWORD m_dwTime;
	_bool m_bFirst = false;
	_bool m_bTargetCam = false;;
	_float m_fMusicTime;
	_bool  m_bMusicStart = true;
	DAY_STATE m_eDayState = DAY_MORNING;
public:
	static CLevel_Hunt* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;

};

END

