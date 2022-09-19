#pragma once
#include "Client_Defines.h"
#include "UnInteractive_Object.h"
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
class CWoodWall final : public CUnInteractive_Object
{
public:
	enum WALLTYPE { WALL_WOOD, WALL_ROCK, WALL_MAZE, WALL_BOSS, WALL_PUZZLE, WALL_END };
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
	virtual void Set_Texture() override;
private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr);
	void SetUp_BillBoard();
	void SetUp_FPSBillBoard();
	void WalkingTerrain();
	HRESULT Change_Texture(const _tchar* LayerTag);

	void Set_ShaderID();

private: /*For TextureCom */
	virtual HRESULT Texture_Clone();
	virtual void Change_Frame(_float fTimeDelta);
	virtual void Change_Motion();
	

private:
	void Check_GrowShrink();
	void Check_PuzzleSolved();

private:
	STATE m_eState = HEALTHY;
	STATE m_ePreState = MAX;
	FENCESTATE m_eFenceState = IDLE;
	FENCESTATE m_ePreFenceState = FENCE_MAX;
	WALLDESC m_eWallDesc;
	_float3 m_vecOutPos;

public:
	static CWoodWall* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END