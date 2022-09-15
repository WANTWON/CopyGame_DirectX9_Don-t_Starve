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
END

BEGIN(Client)
class CFloorGrateEruption final : public CGameObject
{
public:
	enum ERUPTION_STATE
	{
		WARNING,
		ERUPTION,
		MAX
	};

	typedef struct EruptionDescription
	{
		ERUPTION_STATE eState = MAX;
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
	}ERUPTIONDESC;

private:
	CFloorGrateEruption(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFloorGrateEruption(const CFloorGrateEruption& rhs);
	virtual ~CFloorGrateEruption() = default;

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

	vector<CTexture*> m_vecTexture;

private:
	const _tchar* m_TimerTag = TEXT("");
	OBJINFO m_tInfo;
	ERUPTIONDESC m_tEruptionDesc;
	ERUPTION_STATE m_eState = MAX;
	ERUPTION_STATE m_ePreState = MAX;
	_bool m_bDidDamage = false;

private:
	void Check_Collision();

public:
	static CFloorGrateEruption* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END