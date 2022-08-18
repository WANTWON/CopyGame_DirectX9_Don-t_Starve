#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE {IDLE, DOWN, UP, RIGHT, LEFT, PLAYER_END};
private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	_float3 Get_Pos();
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /* For TransformCom*/
	void SetUp_BillBoard();


private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	vector<CTexture*>       m_vecTexture;


private:
	STATE			m_eState = IDLE;
	STATE			m_ePastState = DOWN;
	const _tchar*	m_TimerTag = TEXT("");

	_bool			m_bInverseScale = false;
	_float			m_fTerrain_Height = 0.f;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END