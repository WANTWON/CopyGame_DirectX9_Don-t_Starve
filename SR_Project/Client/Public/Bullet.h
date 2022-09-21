#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider_Cube;
class CShader;
END

BEGIN(Client)

class CBullet : public CPawn
{
public:
	CBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_DebugComponents(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private://Init
	HRESULT Init_Data(void);
	HRESULT Texture_Clone(void);

private://Passive
	void Excute(_float fTimeDelta);
	void AttackCheck(_float _fTimeDelta);
	void DeadCheck(_float _fTimeDelta);

private:/*Act Player`s Bullets*/
	void Red_Smoke(_float _fTimeDelta);
	void Bomb(void);
	void IceSpikes(_float _fTimeDelta);
	void IceMine(_float _fTimeDelta);
	void IceMines(_float _fTimeDelta);
	void Puff(_float _fTimeDelta);
	void Ice_Smoke(_float _fTimeDelta);
	void Ice_Blast(_float _fTimeDelta);
	void Rock(_float _fTimeDelta);
	void Carnival_Arrow(_float fTimeDelta);
	void Shoot_Carnival_Arrow(_float fTimeDelta);

private://ApplyDamage
	//virtual void	Apply_Damage(_float Damage, CGameObject* DamagedObj, void* AttackType = nullptr);
	//virtual void	Apply_Damage_Multi(_float fDamage, vector<CGameObject*>& vecDamagedObj, void* AttackType = nullptr);

	//Campare Terrain& Bullets
	_bool Compare_Terrain(void);
private:/*for Texture*/
	HRESULT Render_TextureState();
	HRESULT Change_Texture(const _tchar* LayerTag);
private:
	void SetUp_BillBoard();
private:/*For Components*/
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CCollider_Cube*				m_pColliderCom = nullptr;
	CShader*					m_pShaderCom = nullptr;

	vector<CTexture*> m_vecTexture;

	/* For.Debug */
	CVIBuffer_Rect* m_pVIDebugBufferCom = nullptr;

private: //values		
	//WEAPON_TYPE				m_eWeaponType = WEAPON_TYPE::WEAPON_HAND;
	const _tchar*			m_TimerTag = TEXT("");
	_bool					m_bIsAttacked = false;

	BULLETDATA				m_tBulletData;
	_float					m_fAccDeadTimer = 0.f;
	_float					m_fDamageTimger = 0.f; //jisokDamage

	_float					m_fDamage= 20.f;
	
	//isActivated?
	_bool					m_bActivated = false; //주로 행동용
	_bool					m_bActivated2 = false; 
	_bool					m_bActivated3 = false;
	//Test
	_float					m_fTime = 0.f; //각 객체들내부시간
	_float					m_fPower = 2.f;
	_float					m_MaxTime;

	_float					m_fAttackRange = 1.f;
	_float					m_fAngle = 310.0f;
	//for Anim
	_bool					m_bReversAnim = false;

	//for Carnival Bullet
	_bool					m_bShoot = false;
	_float3					m_vShootDir;

	

public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END