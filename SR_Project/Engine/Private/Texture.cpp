#include "..\Public\Texture.h"

#include "GameInstance.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
	, m_iNumTextures(rhs.m_iNumTextures)
{
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

//Texture.png
//Texture%d.png

HRESULT CTexture::Initialize_Prototype(TYPE eType, const _tchar * pTextureFilePath, _uint iNumTexture)
{
	m_iNumTextures = iNumTexture;

	_tchar	szFullPath[MAX_PATH] = TEXT("");

	for (_uint i = 0; i < m_iNumTextures; ++i)
	{
		IDirect3DBaseTexture9* pTexture = nullptr;

		wsprintf(szFullPath, pTextureFilePath, i);

		HRESULT hr = eType == TYPE_DEFAULT ? D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture) : D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pTexture);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{

	if (pArg != nullptr)
	{
		memcpy(&m_TextureDesc, pArg, sizeof(TEXTUREDESC));
		m_TextureDesc.m_iCurrentTex = m_TextureDesc.m_iStartTex;
	}

	return S_OK;
}

HRESULT CTexture::Bind_OnGraphicDev(_uint iIndex)
{
	if (iIndex >= m_iNumTextures)
		return E_FAIL;

	return m_pGraphic_Device->SetTexture(0, m_Textures[iIndex]);
}

HRESULT CTexture::Bind_OnGraphicDev_Debug()
{
	return m_pGraphic_Device->SetTexture(0, 0);
}

bool CTexture::MoveFrame(const _tchar* TimerTag, _bool bLoop)
{
	//CGameInstance::Get_Instance()->Update(TimerTag);
	//m_fTimeAcc += CGameInstance::Get_Instance()->Get_TimeDelta(TimerTag);

	m_fTimeAcc += CGameInstance::Get_Instance()->Get_TimeDelta(TEXT("Timer_60"));

	// Looping Animation
	if (bLoop)
	{ 
		if (m_fTimeAcc > 1.f / m_TextureDesc.m_fSpeed)
		{
			m_TextureDesc.m_iCurrentTex++;

			if (m_TextureDesc.m_iCurrentTex >= m_TextureDesc.m_iEndTex)
				m_TextureDesc.m_iCurrentTex = m_TextureDesc.m_iStartTex;

			//CGameInstance::Get_Instance()->Set_ZeroTimeDelta(TimerTag);
			m_fTimeAcc = 0.f;

			return false;
		}
	}
	// One-Time Animation
	else
	{
		if (m_fTimeAcc > 1.f / m_TextureDesc.m_fSpeed)
		{
			if (m_TextureDesc.m_iCurrentTex < m_TextureDesc.m_iEndTex)
				m_TextureDesc.m_iCurrentTex++;
			else
				return true;

			//CGameInstance::Get_Instance()->Set_ZeroTimeDelta(TimerTag);
			m_fTimeAcc = 0.f;

			return false;
		}
	}
}

void CTexture::Set_Frame(int iStartTex, int iEndTex, int iSpeed)
{
	m_TextureDesc.m_iStartTex = iStartTex;
	m_TextureDesc.m_iEndTex = iEndTex;
	m_TextureDesc.m_fSpeed = (_float)iSpeed;
	m_TextureDesc.m_iCurrentTex = m_TextureDesc.m_iStartTex;
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eType, const _tchar * pTextureFilePath, _uint iNumTexture)
{
	CTexture*	pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(eType, pTextureFilePath, iNumTexture)))
	{
		ERR_MSG(TEXT("Failed to Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void* pArg)
{
	CTexture*	pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	m_Textures.clear();
}
