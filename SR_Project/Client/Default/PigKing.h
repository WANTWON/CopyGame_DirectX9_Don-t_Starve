#pragma once
#include "Client_Defines.h"
#include "Interactive_Object.h"
#include "Transform.h"

class CPigKing : public CInteractive_Object
{
	enum STATE
	{
		IDLE,
		COINTOSS,
		UNIMPRESSED,
		MAX
	};

private:
	CPigKing(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPigKing(const CPigKing& rhs);
	virtual ~CPigKing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

protected: /* For.Components */
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CCollider* m_pColliderCom = nullptr;

	vector<CTexture*> m_vecTexture;

private:
	HRESULT SetUp_Components(void* pArg = nullptr);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);
	void Change_Frame();
	void Change_Motion();

private: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

private:
	const _tchar* m_TimerTag = TEXT("");
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;

private:
	virtual void Interact(_uint Damage = 0) override;
	void Find_Target();
	HRESULT Drop_Items();
	_bool IsDead();

public:
	static CPigKing* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
	virtual void Free() override;
};