#pragma once
#include "Client_Defines.h"
#include "UnInteractive_Object.h"
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
class CCarnival_Shooter final : public CUnInteractive_Object
{
public:
	enum STATE { IDLE, SHOOT, MAX };
private:
	CCarnival_Shooter(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarnival_Shooter(const CCarnival_Shooter& rhs);
	virtual ~CCarnival_Shooter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;
	virtual void Set_Texture() override;

	void Set_bShoot(_bool type) { m_bShoot = type; type ? m_eState = SHOOT : m_eState = IDLE; }
	void SetUp_BillBoard();
private:
	HRESULT SetUp_Components(void* pArg);
	virtual void Change_Frame() override;
	virtual void Change_Motion() override;
	virtual HRESULT Texture_Clone()override;

private:
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;
	_bool m_bShoot = false;
	_bool m_bReady = false;
public:
	static CCarnival_Shooter* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END