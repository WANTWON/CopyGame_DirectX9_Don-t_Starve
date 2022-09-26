#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Observer.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel, public CObserver
{
private:
	CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual void Update(_uint eDayState) override;
public:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Object(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MainInventory(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Equipment_back(const _tchar* pLayerTag);
	HRESULT Ready_Layer_PlayerStatUI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MainToolbox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WeaponToolbox(const _tchar* pLayerTag);
	HRESULT Ready_LayerPont(const _tchar* pLayerTag);
	
	HRESULT Ready_LayerNPC(const _tchar* pLayerTag);
public:
	static CLevel_GamePlay* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;


public:
	void Set_NextLevel(_bool type) { m_bNextLevel = type; }
	void Set_BeargerAdd(_bool type) { m_bBeargerAdd = type; }

	DAY_STATE m_eDayState = DAY_MORNING;
	_float m_fMusicTime;
	_bool  m_bMusicStart = true;
	_bool m_bBeargerAdd = false;
	_float m_fTimeAcc = 0.f;
	_bool m_bNextLevel = false;

	void Decrease_Fireflies() { m_iFirefliesCounter--; }

	_float m_fFirefliesTimer = 0.f;
	_uint m_iFirefliesMax = 50;
	_uint m_iFirefliesCounter = 0;

};

END

