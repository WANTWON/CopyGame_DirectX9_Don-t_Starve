#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Hunt final : public CLevel
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
	void Set_NextLevel(_bool _dd) { m_bNextLevel = _dd; }
	void Set_PastLevel(_bool _dd) { m_bPastLevel = _dd; }

	_bool m_bNextLevel = false;
	_bool m_bPastLevel = false;

private:
	DWORD m_dwTime;
	_bool m_bFirst = false;
	_bool m_bTargetCam = false;

public:
	static CLevel_Hunt* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;

};

END

