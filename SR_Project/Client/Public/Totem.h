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
class CTotem final : public CGameObject
{
public:
	enum TOTEM_TYPE 
	{ 
		DEFENSE, 
		HEAL, 
		MAX_TYPE 
	};

	enum TOTEM_STATE
	{
		PLACE,
		IDLE,
		HIT,
		BREAK,
		ACTIVE,
		MAX_STATE
	};

	typedef struct TotemDescription
	{
		TOTEM_TYPE eType = TOTEM_TYPE::MAX_TYPE;
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
	}TOTEMDESC;

private:
	CTotem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTotem(const CTotem& rhs);
	virtual ~CTotem() = default;

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
	void WalkingTerrain();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /*For TextureCom */
	virtual HRESULT Texture_Clone();
	virtual void Change_Motion();
	virtual void Change_Frame();

private:
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	SHADER_STATE    m_eShaderID = SHADER_IDLE;
	vector<CTexture*> m_vecTexture;

public:
	virtual _float Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser) override;

private:
	void AI_Behaviour(_float fTimeDelta);

public:
	TOTEMDESC Get_TotemDesc() { return m_tTotemDesc; }

private:
	const _tchar* m_TimerTag = TEXT("");
	OBJINFO m_tInfo;
	TOTEMDESC m_tTotemDesc;
	TOTEM_STATE m_eState = TOTEM_STATE::PLACE;
	TOTEM_STATE m_ePreState = TOTEM_STATE::MAX_STATE;

public:
	void Set_IsActive(_bool bIsActive = false) { m_bIsActive = bIsActive; }

private:
	_bool m_bIsHit = false;
	_bool m_bIsActive = false;
	_bool m_bShouldDestroy = false;

public:
	static CTotem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END