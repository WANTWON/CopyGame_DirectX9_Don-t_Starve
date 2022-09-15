#pragma once
#include "Client_Defines.h"
#include "Interactive_Object.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider_Rect;
END

BEGIN(Client)

class CNPC abstract : public CInteractive_Object
{
public:
	enum NPC_STATE { IDLE, MOVE, INTERACT, ATTACK, STATE_END };
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
	virtual HRESULT SetUp_Components(void* pArg = nullptr) =0;

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
	_float3 Get_Pos(void);
	//Set
	void	Reset_Target(void) { m_pTarget = nullptr; }
	void	Set_IsArrive(_bool _bArrive) { m_bArrive = _bArrive; }
	void	Reset_InteractTime(void) { m_fInteractTIme = 0.f; }
	void	Set_Activate(NPC_STATE _eState, _bool _bAct) { m_NPCStates[_eState] = _bAct; }
public:/*for Actions*/
	virtual void Interact(_uint Damage = 0) = 0;
	virtual HRESULT Drop_Items() = 0;

	virtual void	Move(_float _fTimeDelta) = 0;
	virtual void	Idle(_float _fTimeDelta) = 0;
	virtual void	Interaction(_float _fTimedelta) = 0;
	virtual void	Select_Target(_float _fTimeDelta) = 0;
	virtual void	Set_RandPos(_float _fTimeDelta) = 0;

public: //mange map
	NPC_STATE	Find_Activated(void);
	void		Init_Map(void);
protected:
	void Update_InteractTime(_float _fTimeDelta) { m_fInteractTIme += _fTimeDelta; }

protected:
	CGameObject*	m_pTarget = nullptr;

	//for TargetPos
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
	_bool					m_bActivate = false;
	map<NPC_STATE, _bool>	m_NPCStates;
	//Level
	LEVEL					m_iCurrentLevelndex;

	DIR_STATE				m_eCur_Dir;
	DIR_STATE				m_ePre_Dir;

	//NPCTYPE
	NPCID					m_eNPCID;

	//NPC_STATE
	NPC_STATE				m_eState;
	NPC_STATE				m_ePreState;

	//time
	//_float					m_fInteractTime = 0.f;
public:
	virtual CGameObject* Clone(void* pArg = nullptr) =0;
	virtual void Free() override;

};

END