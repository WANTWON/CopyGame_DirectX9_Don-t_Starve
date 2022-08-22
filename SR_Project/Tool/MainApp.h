#pragma once

#include "Tool_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CGameInstance;
END


BEGIN(Tool)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = nullptr;

private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;


private:
	HRESULT Ready_Prototype_Component();
	
public:
	static CMainApp* Create();
	virtual void Free();
};

END

