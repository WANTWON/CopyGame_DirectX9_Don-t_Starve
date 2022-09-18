#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Boss final : public CLevel
{
private:
	CLevel_Boss(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Boss() = default;

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
	void Set_PastLevel(_bool _dd) { m_bPastLevel = _dd; }
	void Set_PortalMake(_bool _dd) { m_bPortalMake = _dd; }

private:
	_bool m_bNextLevel = false;
	_bool m_bPastLevel = false;
	DWORD m_dwTime;
	_bool m_bFirst = false;
	_bool m_bTargetCam = false;
	_bool m_bPortalMake = false;
	_float m_fTimeAcc = 0.f;
public:
	static CLevel_Boss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;

};

END