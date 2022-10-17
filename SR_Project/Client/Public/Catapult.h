#pragma once
#include "Transform.h"
#include "Interactive_Object.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)
class CCatapult :
	public CPawn
{
public:
	enum STATE
	{
		PLACE,
		IDLE,
		ATTACK,
		DESTROY,
		STATE_END
	};

	typedef struct tagCatapultDesc
	{
		CGameObject* pOwner;
		_float3		 vInitPos;
	}CATAPULTDESC;
private:
	CCatapult(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCatapult(const CCatapult& rhs);
	virtual ~CCatapult() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components(void* pArg = nullptr);
	void SetUp_BillBoard();
	void WalkingTerrain();
	HRESULT Change_Texture(const _tchar* LayerTag);
private: /*For TextureCom */
	HRESULT Texture_Clone(void);

private:
	void Update_Delay(_float _fTimeDelta);
	void Behavior(_float _fTimeDelta);

	void Attack(_float _fTimeDelta);
	void Idle(_float _fTimeDelta);
	void Destroy(_float _fTimeDelta);
	void Place(_float _fTimeDelta);
	void BurstMode(_float _fTimeDelta);
	void Play_Sound(_float _fTimeDelta);
private:
	_bool	Find_Enemy(_float _fTimeDelta);
	_bool	Check_Distance(CGameObject* _pObj);
private:
	CTexture*		m_pTextureCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	vector<CTexture*> m_vecTexture;

private:
	const _tchar*	m_TimerTag = TEXT("Catapult");
	OBJINFO			m_tInfo;
	CATAPULTDESC	m_tDesc;
	STATE			m_eState = PLACE;
	STATE			m_ePreState = STATE_END;
	
	_uint			m_iCurrentLevelndex = 0;

	//for Attack
	_bool			m_bBurst = false;
	_bool			m_bCanAttack = true;
	_float			m_fAtkMaxDelay = 4.f;
	_float			m_fAttackDelay = m_fAtkMaxDelay;
	
	CGameObject*	m_pTarget = nullptr;
	_float3			m_fSavePos;
	//for Life
	_float			m_fLifeTime = 30.f;
	_bool			m_bDestroyed = true;
	//Range
	_float			m_fMaxRange = 15.f;
	_float			m_fMinRange = 5.f;

	//forBurstMode
	_float			m_fBurst_Time = 5.f;
	_float			m_fBurst_AtckMaxDelay = 2.f;
	_float			m_fCreate_Effect_Time = 0.f;
public:
	void Interact(_uint Damage = 0);



public:
	static CCatapult* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END