#pragma once
#include "Client_Defines.h"
#include "Transform.h"
#include "Interactive_Object.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Manager;
END

BEGIN(Client)

class CSkeleton final : public CInteractive_Object
{
private:
	CSkeleton(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkeleton(const CSkeleton& rhs);
	virtual ~CSkeleton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) override;
private: /*For TextureCom */
	virtual HRESULT Texture_Clone() override;
	virtual void Change_Frame(_float fTimeDelta) override;
	virtual void Change_Motion() override;

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

public:
	static CSkeleton* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;





};

END