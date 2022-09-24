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
END

BEGIN(Client)
class CTotemEffect final : public CGameObject
{
public:
	enum TOTEM_EFFECT_TYPE
	{
		DEFENSE,
		HEAL,
		PARTICLES,
		SHIELD,
		MAX_TYPE
	};

	typedef struct TotemEffectDescription
	{
		TOTEM_EFFECT_TYPE eType = TOTEM_EFFECT_TYPE::MAX_TYPE;
		CGameObject* pEffectTarget = nullptr;
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
	}TOTEMEFFECTDESC;

private:
	CTotemEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTotemEffect(const CTotemEffect& rhs);
	virtual ~CTotemEffect() = default;

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
	virtual void Change_Frame();

private:
	const _tchar* m_TimerTag = TEXT("");

	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CShader*	m_pShaderCom = nullptr;
	SHADER_STATE m_eShaderID = SHADER_IDLE;


	vector<CTexture*> m_vecTexture;

private:
	void Stick_ToTarget();

private:
	TOTEMEFFECTDESC m_tTotemEffectDesc;

public:
	static CTotemEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END