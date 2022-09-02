#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CMonster abstract : public CGameObject
{
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

protected: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CCollider* m_pColliderCom = nullptr;

	vector<CTexture*> m_vecTexture;

protected:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) = 0;
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

protected: /*For TextureCom */
	virtual HRESULT Texture_Clone() = 0;
	HRESULT Change_Texture(const _tchar* LayerTag);
	virtual void Change_Frame() = 0;
	virtual void Change_Motion() = 0;

protected: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

protected:
	const _tchar* m_TimerTag = TEXT("");
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
	_bool m_bHit = false;
	DWORD m_dwIdleTime = GetTickCount();
	DWORD m_dwWalkTime = GetTickCount();
	DWORD m_dwAttackTime = GetTickCount();
	DWORD m_dwDeathTime = GetTickCount();

protected:
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