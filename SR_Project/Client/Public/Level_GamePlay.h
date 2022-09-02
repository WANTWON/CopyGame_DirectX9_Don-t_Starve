#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Object(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Portal(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MainInventory(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Equipment_back(const _tchar* pLayerTag);
	HRESULT Ready_Layer_PlayerStatUI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MainToolbox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WeaponToolbox(const _tchar* pLayerTag);
	
	
public:
	static CLevel_GamePlay* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;


public: //test
	void Set_NextLevel(_bool _dd) { m_bNextLevel = _dd; }

	_bool m_bNextLevel = false;
};

END

