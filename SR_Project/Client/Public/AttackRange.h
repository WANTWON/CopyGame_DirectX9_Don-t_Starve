#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider_Manager;
END

BEGIN(Client)

class CAttackRange : public CGameObject
{ //Picker& Range

public:
	CAttackRange(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAttackRange(const CAttackRange& rhs);
	virtual ~CAttackRange() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private://Init
	HRESULT Init_Data(void);
	HRESULT Texture_Clone(void);

public: /*Get& Set*/
	void Set_Scale(_float3 _vScale);
	void Set_IsShow(_bool _bShow) { m_bShow = _bShow; }
	void Set_IsCorrect(_bool _bCorrect) { m_bIsCorrect = _bCorrect; }
	void Set_TargetPos(_float3 _vPos) { m_vTargetPos = _vPos; }
	void Set_Pos(_float3 _vPos); 

	_bool Get_IsShow(void) { return m_bShow; }
	_bool Get_IsCorrect(void) { return m_bIsCorrect; }
private:
	HRESULT Check_Picker();

	void WalkingTerrain();
	void SetUp_BillBoard();
private:/*For Components*/
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	//CCollider*				m_pColliderCom = nullptr;

private: //values	
	_bool					m_bIsPicker = false;
	const _tchar*			m_TimerTag = TEXT("");
	/////
	_bool					m_bIsCorrect = false;
	_float3					m_vRangeScale;

	_bool					m_bShow = true;

	_float3					m_vTargetPos;
public:
	static CAttackRange* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END