#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(LPDIRECT3DDEVICE9 pGraphic_Device);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	HRESULT Initialize_Prototype(const _tchar* pShaderFilePath);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT End();

public:
	HRESULT Set_RawValue(D3DXHANDLE hParameter, const void* pData, _uint iLength);
	HRESULT Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture);

private:
	LPD3DXEFFECT			m_pEffect = nullptr;

public:
	static CShader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pShaderFilePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END