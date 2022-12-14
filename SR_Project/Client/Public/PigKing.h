#pragma once
#include "Client_Defines.h"
#include "NPC.h"
#include "Transform.h"

class CPigKing : public CNPC
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
private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) override;
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
private: /*For TextureCom */
	virtual HRESULT Texture_Clone() override;
	virtual void Change_Frame(_float fTimeDelta) override;
	virtual void Change_Motion() override;


private:
	STATE m_eState = IDLE;
	STATE m_ePreState = MAX;
	_bool m_bFirstTalk = false;

	//Test
	_bool bTest = true;

public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;
	void Find_Target();
public:/*for Actions*/
	virtual void	Move(_float _fTimeDelta) override;
	virtual void	Idle(_float _fTimeDelta) override;
	virtual void	Interaction(_float _fTimedelta) override;
	virtual void	Talk(_float _fTimeDelta) override;
	virtual void	Dance(_float _fTimeDelta) override;

	virtual void	Select_Target(_float _fTimeDelta) override;
	virtual void	Set_RandPos(_float _fTimeDelta) override;

public:
	static CPigKing* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};