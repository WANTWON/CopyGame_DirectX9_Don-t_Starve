#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Tool)
class CDecoObject final : public CGameObject
{
public:
	enum DECOTYPE { FLOORFIRE, FLOOR_EFFECT, TORCH, FLIES, DECO_END };

	typedef struct DecoTag
	{
		DECOTYPE m_eState = DECO_END;
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
		_float	fRotate = 0.f;
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
	void Add_Rotate() { m_DecoDesc.fRotate++; if (m_DecoDesc.m_eState == FLOORFIRE)m_pTransformCom->Turn(_float3(0, 1, 0), 1.f); }
	void Minus_Rotate() { m_DecoDesc.fRotate--; if(m_DecoDesc.m_eState == FLOORFIRE)m_pTransformCom->Turn(_float3(0, 1, 0), -1.f); }

public:
	void FloorUpdate();

public:
	DECODECS Get_Desc() { return m_DecoDesc; }
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /* For TransformCom*/
	void SetUp_BillBoard();

private: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	DECODECS m_DecoDesc;
	const _tchar* m_TimerTag = TEXT("");
	_float m_fTerrain_Height = 0.f;

	_int m_MonsterMaxCount = 0;
	_float m_fSpawnTime = 999.f;
	_bool m_bCreate = false;
	DWORD m_dwTime = GetTickCount();

public:
	static CDecoObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END