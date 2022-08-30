#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CSpider final : public CGameObject
{
	enum class DIR 
	{ 
		DIR_UP,
		DIR_DOWN,
		DIR_RIGHT, 
		DIR_LEFT, 
		MAX
	};
	enum class STATE
	{
		IDLE,
		MOVE,
		ATTACK,
		TAUNT,
		HIT,
		DIE,
		MAX
	};

private:
	CSpider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSpider(const CSpider& rhs);
	virtual ~CSpider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CCollider* m_pColliderCom = nullptr;

	vector<CTexture*> m_vecTexture;

private:
	HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);
	void Change_Frame();
	void Change_Motion();

private: /* For TransformCom*/
	void SetUp_BillBoard();
	
private:
	_float m_fTerrain_Height = 0.f;
	const _tchar* m_TimerTag = TEXT("");
	DIR m_eDir = DIR::DIR_DOWN;
	DIR	m_ePreDir = DIR::MAX;
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;
	OBJINFO m_tInfo;
	CGameObject* m_pTarget = nullptr;
	_float m_fDistanceToTarget = 0.f;
	_float m_fAggroRadius = 3.f;
	_bool m_bAggro = false;
	_float m_fAttackRadius = .5f;
	_bool m_bIsAttacking = false;
	DWORD m_dwAttackTime = GetTickCount();
	_bool m_bHit = false;
	DWORD m_dwDeathTime = GetTickCount();

private:
	void AI_Behaviour(_float fTimeDelta);
	void Find_Target();
	void Follow_Target(_float fTimeDelta);
	void Interact(_uint iDamage = 0);
	HRESULT Drop_Items();
	_bool IsDead();

public:
	static CSpider* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
	virtual void Free() override;
};
END