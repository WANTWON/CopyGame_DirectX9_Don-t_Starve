#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CShader;
END


BEGIN(Client)
class CUnInteractive_Object abstract : public CGameObject
{
protected:
	CUnInteractive_Object(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUnInteractive_Object(const CUnInteractive_Object& rhs);
	virtual ~CUnInteractive_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Set_Texture() = 0;

protected: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CTexture*> m_vecTexture;

protected:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) = 0;
	void Set_ShaderID();
protected: /*For TextureCom */
	virtual HRESULT Texture_Clone() { return S_OK;  };
	HRESULT Change_Texture(const _tchar* LayerTag);
	virtual void Change_Frame() {};
	virtual void Change_Motion() {};

protected: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

protected:
	const _tchar* m_TimerTag = TEXT("");
	OBJINFO m_tInfo;
	_uint m_iDamage = 0;
	SHADER_STATE m_eShaderID = SHADER_IDLE_ALPHATEST;
public:
	virtual void Free() override;
};
END