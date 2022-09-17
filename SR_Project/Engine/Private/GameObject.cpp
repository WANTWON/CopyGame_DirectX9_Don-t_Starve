#include "..\Public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

int CGameObject::Tick(_float fTimeDelta)
{

	return OBJ_NOEVENT;
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}



//void CGameObject::Apply_Damage(_float fDamage, CGameObject * DamagedObj, void * AttackType)
//{
//}
//
//void CGameObject::Apply_Damage_Multi(_float fDamage, vector<CGameObject*>& vecDamagedObj, void * AttackType)
//{
//}


void CGameObject::Compute_CamDistance(_float3 WorldPos)
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr , &ViewMatrix);

	_float3 vCamPosition = *(_float3*)&ViewMatrix.m[3][0];

	m_fCamDistance = D3DXVec3Length(&(vCamPosition - WorldPos));

}

_float CGameObject::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	/*
	HP -= fDamage;
	DamageType = DamageType

	*/
	return 0.f;
}


HRESULT CGameObject::Add_Components(const _tchar * pComponentTag, _uint iLevelIndex, const _tchar * pPrototypeTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CGameObject::Change_Component(const _tchar * pComponentTag, CComponent ** ppOut)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)	
		Safe_Release(Pair.second);
	m_Components.clear();	

	Safe_Release(m_pGraphic_Device);
}
