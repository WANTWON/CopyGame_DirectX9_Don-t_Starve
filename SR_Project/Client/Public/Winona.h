#pragma once
#include "BT_NPC.h"
#include "NPC.h"
BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Manager;
END

BEGIN(Client)

class CWinona : public CNPC
{
public:
	CWinona(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWinona(const CWinona& rhs);
	virtual ~CWinona() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public://Get&Set
	void Add_Catapult(class CCatapult* _pCatapult) { m_vecCatapults.push_back(_pCatapult); }
	void Decreas_Catapult(class CCatapult* _pCatapult);

private:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) override;
private: /*For TextureCom */
	virtual HRESULT Texture_Clone() override;
	virtual void Change_Frame(_float fTimeDelta) override;
	virtual void Change_Motion() override;

	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;
public:
	virtual void Interact(_uint Damage = 0) override;
	virtual HRESULT Drop_Items() override;
	virtual	void	Make_Interrupt(CPawn* pCauser, _uint _InterruptNum);
private:/*Func for BT*/
	CBT_NPC* BehaviorTree = nullptr;
public:/*for Actions*/
	virtual void	Move(_float _fTimeDelta) override;
	virtual void	Idle(_float _fTimeDelta) override;
	virtual void	Interaction(_float _fTimedelta) override;
	virtual void	Talk(_float _fTimeDelta) override;
	virtual void	Dance(_float _fTimeDelta) override;
	virtual void	Attack(_float _fTimeDelta) override;
	virtual void	Interrupted(_float _fTimeDelta) override;
	virtual void	Skill(_float _fTimeDelta) override;
	virtual _bool	Hit(_float _fTimeDelta) override;
	virtual _bool	Dead(_float _fTimeDelta) override;

	virtual void	Select_Target(_float _fTimeDelta) override;
	virtual void	Set_RandPos(_float _fTimeDelta) override;
	virtual _bool	Get_Target_Moved(_float _fTimeDelta, _uint _iTarget) override;
	virtual _bool	Detect_Enemy() override;
private:
	void	Revive_Berry(_float _fTimeDelta);
	void	Talk_Player(_float _fTimeDelta);
	void	Talk_Friend(_float _fTimeDelta);
	void	Create_Bullet(_float _fTimeDelta);
	void	Create_Catapult(_float _fTimeDelta);

	void	MoveWithOwner(_float _fTimeDelta);
	void	MoveWithoutOwner(_float _fTimeDelta);
	void	SetPosForSkill(_float _fTimeDelta);
	_bool	Setup_LevelChange(_float _fTimeDelta);
public:
	DIR_STATE Check_Direction(void);
private://FindPriority
	virtual void Find_Priority() override;
	void		Find_Friend();
	void		Find_Enemy();
	void		Find_Berry();
	void		Find_Player();
	//athors
	void		CatapultCheck(void);
private:
	vector<class CCatapult*>	m_vecCatapults;

public:
	static CWinona* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END