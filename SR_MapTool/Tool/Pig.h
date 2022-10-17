#pragma once
#include "Tool_Defines.h"
#include "Monster.h"
#include "Transform.h"

BEGIN(Tool)
class CPig final : public CMonster
{
	enum STATE
	{
		IDLE,
		WALK,
		RUN,
		ATTACK,
		HIT,
		DIE,
		MAX
	};

private:
	CPig(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPig(const CPig& rhs);
	virtual ~CPig() = default;

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

private:
	STATE m_eState = STATE::IDLE;
	STATE m_ePreState = STATE::MAX;

private:
	virtual _bool IsDead() override;

public:
	static CPig* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr);
	virtual void Free() override;
};
END