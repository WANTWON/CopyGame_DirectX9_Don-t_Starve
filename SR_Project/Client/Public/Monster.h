#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CTexture;
class CCollider_Cube;
END

BEGIN(Client)
class CMonster abstract : public CPawn
{
public:
	enum MONSTER_ID{ PIG, SPIDER, SPIDER_WARRIOR, BEARGER, BOARON, BOARRIOR, ID_END};
protected:
	CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public: // Get& Set
	_bool Get_Aggro(void) { return m_bAggro; }
	MONSTER_ID Get_MonsterID(void) { return m_eMonsterID; }
protected: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	vector<CTexture*> m_vecTexture;

	/* For.Debug */
	//CVIBuffer_Rect* m_pVIDebugBufferCom = nullptr;

protected:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) = 0;
	virtual HRESULT SetUp_DebugComponents(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

protected: /*For TextureCom */
	virtual HRESULT Texture_Clone() = 0;
	HRESULT Change_Texture(const _tchar* LayerTag);
	virtual void Change_Frame(_float fTimeDelta = 0.f) = 0;
	virtual void Change_Motion() = 0;

protected: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

protected:
	const _tchar* m_TimerTag = TEXT("");
	DIR_STATE m_eDir = DIR_STATE::DIR_DOWN;
	DIR_STATE m_ePreDir = DIR_STATE::DIR_END;
	_uint m_iDirOffset = 0;
	OBJINFO m_tInfo;
	CGameObject* m_pTarget = nullptr;
	_float m_fDistanceToTarget = 0.f;
	_float m_fAggroRadius = 3.f;
	_float m_fPatrolRadius = 3.f;
	_float m_fPatrolPosX = 0.f;
	_float m_fPatrolPosZ = 0.f;
	_float m_fAttackRadius = .5f;
	_bool m_bAggro = false;
	_bool m_bIsAttacking = false;
	_bool m_bDidDamage = false;
	_bool m_bHit = false;
	_bool m_bPicking = false;
	_bool m_bSend = false;
	DWORD m_dwIdleTime = GetTickCount();
	DWORD m_dwWalkTime = GetTickCount();
	DWORD m_dwAttackTime = GetTickCount();
	DWORD m_dwDeathTime = GetTickCount();
	_bool m_bDeadAnimExpired = false;
	MONSTER_ID	m_eMonsterID;
protected:
	DIR_STATE Get_Unprocessed_Dir(DIR_STATE eDir);
	DIR_STATE Get_Processed_Dir(DIR_STATE eDir);
	void Calculate_Direction(_float3 vTargetPos);
	virtual void AI_Behaviour(_float fTimeDelta) { };
	virtual void Find_Target() { };
	virtual void Follow_Target(_float fTimeDelta) { };
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser);
	virtual HRESULT Drop_Items() { return S_OK; };
	virtual _bool IsDead() = 0;
public:
	virtual void Free() override;
};
END