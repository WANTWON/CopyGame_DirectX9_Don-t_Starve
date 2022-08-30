#include "..\Public\Layer.h"


CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			int iEvent = pGameObject->Tick(fTimeDelta);
			if (iEvent == OBJ_DEAD)
			{
				Safe_Release(pGameObject);
			}
				
		}
			
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(fTimeDelta);
	}
}

CGameObject * CLayer::Get_Object(_uint iIndex)
{
	if (m_GameObjects.size() <= iIndex)
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(pComponentTag);
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}

