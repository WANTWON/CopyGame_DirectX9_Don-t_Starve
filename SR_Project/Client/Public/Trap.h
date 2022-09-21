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
class CTrap final : public CGameObject
{
public:
	enum TRAPSTATE { STATE_IDLE_READY, STATE_TRAP, STATE_IDLE_TRAP, STATE_RESET, STATE_MAX };
	enum TRAPTYPE { TRAP_STAR, TRAP_PLANT, TRAP_MAX };

	typedef struct TrapDescription
	{
		TRAPTYPE eType = TRAP_MAX;
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
	}TRAPDESC;

private:
	CTrap(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTrap(const CTrap& rhs);
	virtual ~CTrap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

private:
	HRESULT SetUp_Components(void* pArg);

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	vector<CTexture*> m_vecTexture;
private: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);
	void Change_Motion();
	void Change_Frame(_float fTimeDelta);
	
public:
	TRAPDESC Get_Desc() { return m_tTrapDesc; }
	void Check_Collision();
	void Set_ShaderID();
private:
	const _tchar* m_TimerTag = TEXT("Trap");
	SHADER_STATE m_eShaderID = SHADER_IDLE_ALPHATEST;
	
	TRAPDESC m_tTrapDesc;
	TRAPSTATE m_eState = STATE_IDLE_READY;
	TRAPSTATE m_ePreState = STATE_MAX;

	_uint m_iDamage = 10.f;
	_bool m_bDidDamage = false;
	_float m_fTrapTimer = 0.f;

public:
	static CTrap* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END