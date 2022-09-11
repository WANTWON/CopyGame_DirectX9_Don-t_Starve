#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
END

BEGIN(Client)
class CInteractive_Object abstract : public CGameObject
{
protected:
	CInteractive_Object(LPDIRECT3DDEVICE9 pGraphic_Device);
	CInteractive_Object(const CInteractive_Object& rhs);
	virtual ~CInteractive_Object() = default;

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
	CCollider_Rect* m_pColliderCom = nullptr;

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
	INTERACTOBJ_ID m_eInteract_OBJ_ID;
	_uint m_iDamage = 0;
	_bool m_bInteract = true;
	_bool m_bPicking = false;
	_float3 m_vecOutPos;
	_bool m_bIsDestroyed = false;

public:
	INTERACTOBJ_ID Get_InteractName() { return m_eInteract_OBJ_ID; }
	_bool Get_CanInteract() { return m_bInteract; }
	_uint Set_Damage(_uint _iDamage) { m_iDamage = 0; }
	_bool Get_Destroyed() { return m_bIsDestroyed; }
	_bool Get_Dead() { return m_bDead; }
	virtual void Interact(_uint Damage = 0) = 0;
	virtual HRESULT Drop_Items() = 0;
	virtual void Destroy() { m_bIsDestroyed = true; }

public:
	virtual void Free() override;
};
END