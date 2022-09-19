#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CCollider_Cube;
END

BEGIN(Client)
class CStatue final : public CGameObject
{
public:
	enum STATE
	{
		HEALTHY,
		DAMAGED,
		BROKEN,
		MAX
	};

private:
	CStatue(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStatue(const CStatue& rhs);
	virtual ~CStatue() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
	 HRESULT SetUp_Components(void* pArg = nullptr);
	 void WalkingTerrain();
	 void SetUp_BillBoard();
private: /*For TextureCom */
	 void Change_Frame();
	 void Change_Motion();
	 HRESULT Change_Texture(const _tchar * LayerTag);
	 HRESULT Texture_Clone();
private:
	STATE m_eState = HEALTHY;
	STATE m_ePreState = MAX;
	const _tchar* m_TimerTag = TEXT("");
	OBJINFO m_tInfo;
	_bool m_bIsDestroyed = false;
	SHADER_STATE m_eShaderID = SHADER_IDLE_ALPHATEST;

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	vector<CTexture*> m_vecTexture;
public:
	static CStatue* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END