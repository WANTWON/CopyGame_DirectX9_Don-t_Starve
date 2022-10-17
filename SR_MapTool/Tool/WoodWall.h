#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Tool)
class CWoodWall final : public CGameObject
{
public:
	enum WALLTYPE { WALL_WOOD, WALL_ROCK, WALL_END };
	enum WALLDIR { SIDE, FRONT, WALL_DIREND};
	enum STATE
	{
		HEALTHY,
		DAMAGED,
		BROKEN,
		MAX
	};


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
	WALLDIR Get_DirType() { return m_eWallDesc.eDir; }

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void SetUp_BillBoard();
	HRESULT Change_Texture(const _tchar* LayerTag);
	

private: /*For TextureCom */
	virtual HRESULT Texture_Clone();

private:
	STATE m_eState = HEALTHY;
	STATE m_ePreState = MAX;
	WALLDESC m_eWallDesc;

	const _tchar* m_TimerTag = TEXT("");
	OBJINFO m_tInfo;
	_uint m_iDamage = 0;
	_float3 m_vecOutPos;

	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;

	vector<CTexture*> m_vecTexture;

public:
	static CWoodWall* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END