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
class CHouse final : public CGameObject
{
public:
	enum HOUSETYPE { PIGHOUSE, SPIDERHOUSE, BOARONSPAWNER, MAZESPAWNER, HOUSE_END };

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
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

private:
	void Spawn_Spider(_float fTimeDelta);
	void Spawn_RandomMonster(_float fTimeDelta);
public:
	void Spawn_Boaron(_float fTimeDelta);

private:
	HOUSEDECS m_HouseDesc;
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;

	_int m_MonsterMaxCount = 0;
	_float m_fSpawnTime = 999.f;
	DWORD m_dwTime = GetTickCount();
	SHADER_STATE m_eShaderID = SHADER_IDLE_ALPHATEST;
public:
	static CHouse* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END