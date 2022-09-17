#include "..\Public\Shader.h"

CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
{
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar * pShaderFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Device, pShaderFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->Begin(nullptr, 0);
	m_pEffect->BeginPass(iPassIndex);

	return S_OK;
}

HRESULT CShader::End()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->EndPass();
	m_pEffect->End();

	return S_OK;
}

HRESULT CShader::Set_RawValue(D3DXHANDLE hParameter, const void * pData, _uint iLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	//m_pEffect->CommitChanges();

	return m_pEffect->SetRawValue(hParameter, pData, 0, iLength);
}

HRESULT CShader::Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetTexture(hParameter, pTexture);
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pShaderFilePath)
{
	CShader*	pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath)))
	{
		ERR_MSG(TEXT("Failed to Created : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone(void* pArg)
{
	CShader*	pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
