#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END