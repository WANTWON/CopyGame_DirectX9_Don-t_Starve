#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CShader;
END

BEGIN(Client)
class CWoodWall final : public CGameObject
{
public:
	enum WALLTYPE { WALL_WOOD, WALL_ROCK, WALL_MAZE, WALL_BOSS, WALL_END };
	enum WALLDIR { SIDE, FRONT, WALL_DIREND };
	enum STATE { HEALTHY, DAMAGED, BROKEN, MAX };
	enum FENCESTATE { IDLE, GROW, SHRINK, FENCE_MAX };

	typedef struct WallDesc
	{
		WALLTYPE etype = WALL_WOOD;
		WALLDIR eDir = WALL_DIREND;
		_float3 vecPosition;

	}WALLDESC;

private:
	CWoodWall(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWoodWall(const CWoodWall& rhs);
	virtual ~CWoodWall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void SetUp_BillBoard();
	void SetUp_FPSBillBoard();
	void WalkingTerrain();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /*For TextureCom */
	virtual HRESULT Texture_Clone();
	virtual void Change_Frame(_float fTimeDelta);
	virtual void Change_Motion();

private:
	void Check_GrowShrink();

private:
	SHADER_STATE m_eShaderID = SHADER_IDLE_ALPHATEST;
	STATE m_eState = HEALTHY;
	STATE m_ePreState = MAX;
	FENCESTATE m_eFenceState = IDLE;
	FENCESTATE m_ePreFenceState = FENCE_MAX;
	WALLDESC m_eWallDesc;

	const _tchar* m_TimerTag = TEXT("");
	OBJINFO m_tInfo;
	_uint m_iDamage = 0;
	_float3 m_vecOutPos;

	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

	vector<CTexture*> m_vecTexture;

public:
	static CWoodWall* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END