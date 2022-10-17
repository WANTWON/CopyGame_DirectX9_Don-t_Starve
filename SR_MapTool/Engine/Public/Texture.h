#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TYPE { TYPE_DEFAULT, TYPE_CUBEMAP, TYPE_END };

	typedef struct TextureInfo
	{
		int m_iStartTex = 0;
		int m_iCurrentTex = 0;
		int m_iEndTex = 0;
		_float m_fSpeed = 1;
	}TEXTUREDESC;


private:
	CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	HRESULT Initialize_Prototype(TYPE eType, const _tchar* pTextureFilePath, _uint iNumTexture);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_OnGraphicDev(_uint iIndex = 0);

public:
	void MoveFrame(const _tchar* TimerTag);
	void Set_Frame(int iStartTex, int iEndTex, int iSpeed);
	void Set_ZeroFrame() { m_TextureDesc.m_iCurrentTex = 0; }
	TEXTUREDESC& Get_Frame() { return m_TextureDesc;}

private:
	_uint										m_iNumTextures = 0;
	vector<IDirect3DBaseTexture9*>				m_Textures;
	TEXTUREDESC									m_TextureDesc;
	_float		m_fTimeAcc = 0.0f;

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eType, const _tchar* pTextureFilePath, _uint iNumTexture = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END