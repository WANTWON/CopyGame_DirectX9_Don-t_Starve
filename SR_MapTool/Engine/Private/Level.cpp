#include "..\Public\Level.h"

CLevel::CLevel(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{
}

void CLevel::Late_Tick(_float fTimeDelta)
{
}

void CLevel::Free()
{
	Safe_Release(m_pGraphic_Device);
}
