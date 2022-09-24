#pragma once
#include "Pawn.h"
#include "Transform.h"
#include "Catapult.h"
BEGIN(Engine)
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CBattery_Tower :
	public CPawn
{
public:
	enum STATE
	{
		PLACE,
		ATTACK,
		DESTROY,
		STATE_END
	};

private:
	CBattery_Tower(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBattery_Tower(const CBattery_Tower& rhs);
	virtual ~CBattery_Tower() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:// Get&Set
	void	Set_Target(CGameObject* _pObj) { m_pTarget = _pObj; Safe_AddRef(_pObj); }
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
	void Destroy(_float _fTimeDelta);
	void Place(_float _fTimeDelta);

	void Create_Spark(_float _fTimeDelta);
	void Give_Burst(_float _fTimeDelta);
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
	SHADER_STATE    m_eShaderID = SHADER_IDLE;
	vector<CTexture*> m_vecTexture;

private:
	const _tchar*	m_TimerTag = TEXT("Battery_Tower");
	OBJINFO			m_tInfo;
	STATE			m_eState = PLACE;
	STATE			m_ePreState = STATE_END;

	_uint			m_iCurrentLevelndex = 0;
	_float3			m_vInitPos;
	vector<class CCatapult*>	m_vecCatapults;
	//for Attack
	_bool			m_bCanAttack = true;
	_float			m_fAttackMaxDelay = 1.f;
	_float			m_fAttackDelay = m_fAttackMaxDelay;
	CGameObject*	m_pTarget = nullptr;
	_float3			m_fSavePos;
	//for Life
	_float			m_fLifeTime = 10.f;
	_bool			m_bDestroyed = true;
	_uint			m_iCount = 6;
	_uint			m_iCountMax = 6;
	//Range
	_float			m_fAttakRange = 3.f;
	_float			m_fMaxRange = 15.f;
	_uint			m_iReversTex = 0;
public:
	void Interact(_uint Damage = 0);



public:
	static CBattery_Tower* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END