#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)
class CHouse final : public CGameObject
{
public:
	enum HOUSETYPE { PIGHOUSE, SPIDERHOUSE, HOUSE_END };

	typedef struct HouseTag
	{
		HOUSETYPE m_eState = HOUSE_END;
		_float3 vInitPosition = _float3( 0.f,0.f,0.f );

	}HOUSEDECS;

private:
	CHouse(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHouse(const CHouse& rhs);
	virtual ~CHouse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	_float3 Get_Pos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	HOUSEDECS Get_Desc() { return m_HouseDesc; }
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();
private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Terrain* m_pVIBufferCom = nullptr;

private:
	HOUSEDECS m_HouseDesc;
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;

public:
	static CHouse* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar * VIBufferTag, void * pArg = nullptr) override;
	virtual void Free() override;
};
END