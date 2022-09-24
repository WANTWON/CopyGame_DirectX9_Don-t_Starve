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
class CDecoObject final : public CGameObject
{
public:
	enum DECOTYPE { FLOORFIRE, FLOOR_EFFECT, TORCH, FLIES, FLOOR, PARTY, SPARKLE, DECO_END };

	typedef struct DecoTag
	{
		DECOTYPE m_eState = DECO_END;
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
		_float	fRotate = 1.f;
	}DECODECS;

private:
	CDecoObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDecoObject(const CDecoObject& rhs);
	virtual ~CDecoObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	void FloorUpdate();
	void MoveFrame();

public:
	DECODECS Get_Desc() { return m_DecoDesc; }
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }

private:
	HRESULT SetUp_Components(void* pArg);
	void Set_ShaderID();

private: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CCollider_Cube* m_pColliderCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
	vector<CTexture*> m_vecTexture;
private:
	void Check_Eruption(_float fTimeDelta);
	void Set_FloorDecoTexture();
private:
	DECODECS m_DecoDesc;
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;

	_bool m_bCreate = false;

	// Eruption Variables
	_bool m_bShouldErupt = false;
	_bool m_bIsWarningTimeChosen = false;
	_float m_fWarningTime = 0.f;
	_float m_fRandomWarningLimit = 0.f;
	_float m_fEruptionTime = 0.f;
	SHADER_STATE m_eShaderID = SHADER_IDLE;
public:
	static CDecoObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END