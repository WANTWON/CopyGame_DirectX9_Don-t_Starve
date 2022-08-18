#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	LEVEL				m_eNextLevel = LEVEL_END;
	class CLoader*		m_pLoader = nullptr;

public:
	static CLevel_Loading* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevel);
	virtual void Free() override;
};

END

