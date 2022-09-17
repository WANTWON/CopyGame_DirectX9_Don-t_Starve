#pragma once
#include "Client_Defines.h"
#include "Interactive_Object.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider_Rect;
class CShader;
END

BEGIN(Client)

class CNPC abstract : public CInteractive_Object
{
public:
	enum NPC_STATE { IDLE, MOVE, INTERACT, TALK, DANCE, ATTACK, STATE_END };
public:
	CNPC(LPDIRECT3DDEVICE9 pGraphic_Device);
	CNPC(const CNPC& rhs);
	virtual ~CNPC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT SetUp_Components(void* pArg = nullptr) = 0;

protected: /*For TextureCom */
	virtual HRESULT Texture_Clone() override = 0;
	virtual void Change_Frame() override = 0;
	virtual void Change_Motion() override = 0;

protected:
	virtual void Find_Priority();

public: /*Get & Set*/
	_bool	Get_HasTarget(void) { return m_pTarget != nullptr ? true : false; }
	_bool	Get_IsArrive(void) { return m_bArrive; }
	_float	Get_InteractiveTime(void) { return m_fInteractTIme; }
	_bool	Get_Activated(NPC_STATE _eState) { return m_NPCStates[_eState]; }
	CGameObject* Get_Target(void) { return m_pTarget; }
	_float3 Get_Pos(void);
	_bool	Get_NextAct(void) { return m_bNextAct; }
	_bool	Get_Interrupted(void) { return m_bInterrupted; }
	_uint	Get_TalkCnt(void) { return m_iTalkCnt; }
	_bool	Get_HasOwner(void) { if (m_bOwner && !m_bFightMode) { m_pTarget = m_pOwner; } return m_bOwner; }//Attack�߰��� &&!AttackMode �߰�
	_bool	Get_FirstCall(void) { return m_bFirstCall; }
	_bool	Get_CanAttack(void) { return m_bCanAttack; }
	_bool	Get_FightMode(void) { return m_bFightMode; }
	_bool	Get_TargetDead(void);
	_bool	Get_CloseToOwner(void);
	//Set
	void	Reset_Target(void) { m_pTarget = nullptr; }
	void	Set_IsArrive(_bool _bArrive) { m_bArrive = _bArrive; }
	void	Reset_InteractTime(void) { m_fInteractTIme = 0.f; }
	void	Set_Activate(NPC_STATE _eState, _bool _bAct) { m_NPCStates[_eState] = _bAct; }
	void	Set_Interrupt(_bool _bInterrupt) { m_bInterrupted = _bInterrupt; }
	void	Set_TalkCnt() { ++m_iTalkCnt; }
	void	Set_Accept(_bool _bAccept) { m_bAccept = _bAccept; }
	void	Set_HasOwner(_bool _bAccept) { m_bOwner = _bAccept; }
	void	Set_Position(_float3 _vPos) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos); }
	void	Set_MoveNum(_uint _iNum) { m_MoveNum = _iNum; }
public:/*for Actions*/
	virtual void Interact(_uint Damage = 0) = 0;
	virtual HRESULT Drop_Items() = 0;

	virtual void	Move(_float _fTimeDelta) = 0;
	virtual void	Idle(_float _fTimeDelta) = 0;
	virtual void	Interaction(_float _fTimedelta) = 0;
	virtual void	Talk(_float _fTimeDelta) = 0;
	virtual void	Dance(_float _fTimeDelta) = 0;
	virtual void	Select_Target(_float _fTimeDelta) = 0;
	virtual void	Set_RandPos(_float _fTimeDelta) = 0;
	virtual void	Attack(_float _fTimeDelta);


	virtual void	Interrupted(_float _fTimeDelta);
	virtual	void	Make_Interrupt(CPawn* pCauser, _uint _InterruptNum);
	virtual _bool	Get_Target_Moved(_float _fTimeDelta);
	virtual _bool	Set_TargetPos();
public: //mange map
	NPC_STATE	Find_Activated(void);
	void		Init_Map(void);
	void		Clear_Activated(void);
protected:
	void Update_InteractTime(_float _fTimeDelta) { m_fInteractTIme += _fTimeDelta; }
	void Update_Cooltime(_float _fTimeDelta);
	void Update_FightMode(_float _fTimeDelta);
protected:
	CGameObject*	m_pTarget = nullptr;
	CGameObject*	m_pOwner = nullptr;
	//for TargetPos
	_float	m_fOwnerRadius = 3.f;
	_float	m_fAtkRange = 3.f;
	_uint	m_MoveNum = 0;
	//For RandPos
	_float m_fPatrolRadius = 3.f;
	_float m_fPatrolPosX = 0.f;
	_float m_fPatrolPosZ = 0.f;
	_float3			m_vTargetPos;
	_bool			m_bArrive = true;
	_bool			m_bDirChanged = false;
	//for PlayerPos
	_float3			m_vPlayerPos;
	
	//InteractTime
	_float			m_fInteractTIme = 0.f;
	//
	_bool					m_bNextAct = false;
	_bool					m_bActivate = false;
	map<NPC_STATE, _bool>	m_NPCStates;
	//Level
	LEVEL					m_iCurrentLevelndex;
	LEVEL					m_iPreLevelIndex;
	DIR_STATE				m_eCur_Dir;
	DIR_STATE				m_ePre_Dir;

	//NPCTYPE
	NPCID					m_eNPCID;

	//NPC_STATE
	NPC_STATE				m_eState;
	NPC_STATE				m_ePreState;

	//Interrupted
	_bool					m_bInterrupted = false;
	_uint					m_iInterruptNum = 0;
	//Talk with Player
	_bool		m_bFirstCall = false;
	_uint		m_iTalkCnt = 0;
	_uint		m_iPreTalkCnt = 0;
	_bool		m_bAccept = false;

	_bool		m_bOwner = false;

	//Fight Mode
	_bool		m_bFightMode = false;
	_float		m_fModeTime = 0.f;
	_float		m_fAtk_Cur_CoolTime = 0.f;
	_float		m_fAtk_Max_CoolTime = 0.f;
	_bool		m_bCanAttack = false;
	//time
	//_float					m_fInteractTime = 0.f;
public:
	virtual CGameObject* Clone(void* pArg = nullptr) =0;
	virtual void Free() override;

};

END