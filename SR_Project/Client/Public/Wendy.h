#pragma once
#include "Client_Defines.h"
#include "Transform.h"
#include "Interactive_Object.h"
#include "BT_NPC.h"
#include "NPC.h"
BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)
class CWendy : public CNPC
{
public:
private:
	CWendy(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWendy(const CWendy& rhs);
	virtual ~CWendy() = default;

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
	virtual void Change_Frame() override;
	virtual void Change_Motion() override;

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;

private:/*Func for BT*/
	CBT_NPC* BehaviorTree = nullptr;
public:/*for Actions*/
	virtual void	Move(_float _fTimeDelta) override;
	virtual void	Idle(_float _fTimeDelta) override;
	virtual void	Select_Target(_float _fTimeDelta) override;
	virtual void	Set_RandPos(_float _fTimeDelta) override;
	
public:
	DIR_STATE Check_Direction(void);
private:
	virtual void Find_Priority() override;

private:


public:
	static CWendy* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


END
