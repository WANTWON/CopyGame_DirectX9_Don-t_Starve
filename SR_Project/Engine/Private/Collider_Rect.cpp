#include "..\Public\Collider_Rect.h"

CCollider_Rect::CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CCollider_Rect::CCollider_Rect(const CCollider_Rect & rhs)
	: CComponent(rhs)
{
}

HRESULT CCollider_Rect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider_Rect::Initialize(void * pArg)
{
	if (pArg)
		memcpy(&m_tCollRectDesc, pArg, sizeof(COLLRECTDESC));

	return S_OK;
}

CCollider_Rect * CCollider_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider_Rect::Clone(void * pArg)
{
	CCollider_Rect*	pInstance = new CCollider_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollider_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Rect::Free()
{
	__super::Free();
}
