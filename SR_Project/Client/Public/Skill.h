#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CShader;
END

BEGIN(Client)

class CSkill :
	public CPawn
{
public:
	enum class SKILL_TYPE
	{
		HEAL,
		ICE_BLAST,
		ELEC,
		SPARK,
		BURST,
		CRACKLE,
		CRACKLE_HIT,
		REVIVE,
		SKILL_END
	};

	typedef struct tagSkill_Desc
	{
		SKILL_TYPE	eSkill;
		DIR_STATE	eDirState;
		_float3		vTargetPos;
		_float3		vPosition;
		_float3		vLook;
		_float3		vScale;
		CGameObject* pTarget;
		//_float4x4	m_mat;
	}SKILL_DESC;
private:
	CSkill(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkill(const CSkill& rhs);
	virtual ~CSkill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_DebugComponents(void* pArg = nullptr);
	void SetUp_BillBoard();
	void WalkingTerrain();
	HRESULT Change_Texture(const _tchar* LayerTag);
	_bool Compare_Terrain(void);
private://Init
	HRESULT Init_Data(void);
	HRESULT Texture_Clone(void);

private:
	void	Excute(_float _fTimeDelta);
	void	Activate_Check(_float _fTimeDelta);
	void	Dead_Check(_float _fTimeDelta);

private:/*Act Skills*/
	void Ice_Blast(_float _fTimeDelta);
	void Heal(_float _fTimeDelta);
	void Elec(_float _fTimeDelta);
	void Spark(_float _fTimeDelta);
	void Burst(_float _fTimeDelta);
	void Crackle(_float _fTimeDelta);
	void Crackle_Hit(_float _fTimeDelta);
	void Revive(_float _fTimeDelta);
private:
	const _tchar*		m_TimerTag = TEXT("");

	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider_Cube*		m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	SHADER_STATE		m_eShaderID = SHADER_IDLE;
	/* For.Debug */
	CVIBuffer_Rect*		m_pVIDebugBufferCom = nullptr;

	vector<CTexture*>	m_vecTexture;

private:
	SKILL_DESC			m_tSkillDesc;
	_float				m_fDamage = 20.f;
	_float				m_fAccDeadTimer = 0.f;
	_float				m_fDamageTimger = 0.f; //jisokDamage

	_bool				m_bTrigger = false;
	_bool				m_bActivated = false;
	_bool				m_bRender = true;
	//Reverse
	_uint			m_iReversTex = 0;
public:
	static CSkill* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END