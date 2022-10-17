#pragma once

/* 화면에 그려져야할 객체들을 모아서 그리는 순서대로 보관한다. 순서대로 렌더함수를 호출해준다. */
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {RENDER_PRIORITY, RENDER_NONALPHABLEND, RENDER_ALPHABLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Render_GameObjects();

private:
	list<class CGameObject*>				m_GameObjects[RENDER_END];
	typedef list<class CGameObject*>		GAMEOBJECTS;

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();
public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free();
};

END