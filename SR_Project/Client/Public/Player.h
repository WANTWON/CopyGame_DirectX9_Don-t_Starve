#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
//class CCollider;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
public:

	enum class ACTION_STATE { IDLE, MOVE, ATTACK, MINING, CHOP, WEEDING, EAT, PICKUP, DAMAGED, BUILD, PORTAL, DEAD, REVIVE, ANGRY, ACTION_END };
	typedef enum class InteractionKey {
		KEY_LBUTTON, //VK_LBUTTON
		KEY_RBUTTON, //VK_RBUTTON
		KEY_UP,    //W
		KEY_RIGHT, //D
		KEY_DOWN, //S
		KEY_LEFT, //A
		KEY_MAP,    //VK_TAB
		KEY_ATTACK, //'F'
		KEY_ACTION, //VK_SPACE
		KEY_INVEN1, //1
		KEY_INVEN2,
		KEY_INVEN3,
		KEY_INVEN4,
		KEY_INVEN5,
		KEY_INVEN6,
		KEY_INVEN7,
		KEY_INVEN8,
		KEY_INVEN9,
		KEY_INVEN0,//0
		KEY_MENU, //VK_ESCAPE
		KEY_DEBUG,//VK_OEM_6 (]) 
		KEY_CAMLEFT, //q
		KEY_CAMRIGHT, //e
		KEY_CAMFPSMODE, // F1
		KEY_CAMTPSMODE, //F2
		KEY_ENTER,		//VK_RETURN
		KEY_END
	}INTERACTKEY;


public:
	typedef struct tagPlayerStat
	{
		_float fSpeed = 0.f;
		_float fMaxHealth = 100.f;
		_float fCurrentHealth = fMaxHealth / 2;
		_float fMaxMental = 100.f;
		_float fCurrentMental = fMaxMental / 2;
		_float fMaxHungry = 100.f;
		_float fCurrentHungry = 100.f;
		_float fAtk = 20.f;
		_float fArmor = 0.f;
	}PLAYERSTAT;

	/*typedef struct tagActData {
	_int iKeyNum;
	_bool bIsKeyDown;
	}ACTDATA;*/
	
	typedef struct tagSkillDesc {
		_float	fAtkRange;
		_float	fAtkScale;
		_int	iCnt;
		_float	fMaxCoolTime;
		_float	fCurrent_CoolTime;
		_bool	bSkillUsed;
	}SKILLDESC;


private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;
	
public:
	virtual _float Take_Damage(float fDamage, void* DamageType, CGameObject* DamageCauser) override;


public:/*Picking*/
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }
	void Move_to_PickingPoint(_float fTimedelta);
	void Set_PickingPoint(_float3 PickingPoint, _bool _bIsBuildTrigger = false) { m_vPickingPoint = PickingPoint; m_bPicked = true; m_bInputKey = false; m_bArrive = false; m_bAutoMode = true; m_bBuildTrigger = _bIsBuildTrigger; };


public: /*Get&Set*/
		//Gets
	_float3 Get_Pos();
	_float3 Get_Look();
	_float3 Get_Right();
	PLAYERSTAT Get_Player_Stat() { return m_tStat; }

	_bool	Get_IsBuild(void) { return m_bIsBuild; }
	_float3 Get_PickingPoint(void) { return m_vPickingPoint; }
	_bool	Get_SkillShow(void);

	//Sets
	void	Set_HP(_float _fHP) { m_tStat.fCurrentHealth += _fHP; }
	void	Set_Atk(_float _fAtk) { m_tStat.fAtk += _fAtk; }
	void	Set_Hungry(_float _fHungry) { m_tStat.fCurrentHungry += _fHungry; }
	void	Set_Mental(_float _fMental) { m_tStat.fCurrentMental += _fMental; }
	void	Set_Speed(_float _fSpeed) { m_tStat.fSpeed += _fSpeed; }
	void	Set_Armor(_float _fArmor) { m_tStat.fArmor += _fArmor; }
	void	Set_Sleeping(_bool _bSleeping) { m_bSleeping = _bSleeping; }
	void	Set_WeaponType(WEAPON_TYPE _eWeapon) { m_eWeaponType = _eWeapon; }
	void	Set_Position(_float3 Position);
	void	Set_IsBuild(_bool _bIsBuild) { m_bIsBuild = _bIsBuild; }
	void	Set_bMove(_bool _Move) { m_bMove = _Move; }
public:
	void	Set_PickingTarget(_float3 TargetPicking) { m_vTargetPicking = TargetPicking; }
public:
	void	Add_ActStack(ACTION_STATE _ACT_STATE) { m_ActStack.push(_ACT_STATE); m_bAutoMode = true; /*m_bMove = false; */}
	//Test
	void Check_Target(_bool _bDead) { if (_bDead) m_pTarget = nullptr; }
private:/*Setup*/
	HRESULT SetUp_Components();
	HRESULT SetUp_KeySettings();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	//Test
	HRESULT Test_Setup();
	void Init_Data();
private: /**Actions*/
	void GetKeyDown(_float _fTimeDelta);
	
	//Idle
	bool ResetAction(_float _fTimeDelta);
	void Move_Idle(_float _fTimeDelta);
	//Move
	void	Move_Up(_float _fTimeDelta);
	void	Move_Right(_float _fTimeDelta);
	void	Move_Down(_float _fTimeDelta);
	void	Move_Left(_float _fTimeDelta);
	//Actions
	void	Attack(_float _fTimeDelta);
	void	Mining(_float _fTimeDelta);
	void	Chop(_float _fTimeDelta);
	void	Cutting_Grass(_float _fTimeDelta);
	void	Eatting(_float _fTimeDelta);
	void	Pickup(_float _fTimeDelta);
	void	Damaged(_float _fTimeDelta);
	void	Jump(_float _fTimeDelta);
	void	Dead(_float _fTimeDelta);
	void	Revive(_float _fTimeDelta);
	void	Building(_float _fTImeDelta);
	void	Angry(_float _fTimeDelta);


	void Multi_Action(_float _fTimeDelta); //

	//Skill
	void Throw_Bomb(_float _fTimeDelta);
	void Ice_Spike(_float _fTimeDelta);
	void Sand_Mines(_float _fTimeDelta);
	void Teleport(_float _fTimeDelta);
	 //Passive
	void	Decrease_Stat(void); //
	void	Create_Bullet(void);
	void	Detect_Enemy(void);
	_bool	Check_Interact_End(void);
	void	Find_Priority();
	_bool	Check_Dead();
	void	Sleep_Restore();

	void	Cooltime_Update(_float _fTimeDelta);
	//ActStack
	void	Tick_ActStack(_float fTimeDelta);
	void	Clear_ActStack();

	//Debug
	void Test_Debug(_float fTimeDelta);
	void Debug_Render(void);
	//Test
	void Test_Func(_int _iNum);
	
	void Test_Detect(_float fTImeDelta);
	
	void RangeCheck(_float _fTimeDelta);

	void Turn_OriginMat(_bool _bIsRight);
	//Interact Check
	ACTION_STATE Select_Interact_State(INTERACTOBJ_ID _eObjID);
	

private: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /* For TransformCom*/
	void SetUp_BillBoard();
	void WalkingTerrain();

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	//CCollider*				m_pColliderCom = nullptr;
	//vector<CTexture*>       m_mapTexture;
	map<const _tchar*, CTexture*> m_mapTexture;
private: /*State*/
	DIR_STATE				m_eDirState = DIR_STATE::DIR_DOWN;
	DIR_STATE				m_ePreDirState = DIR_STATE::DIR_DOWN;
	ACTION_STATE			m_eState = ACTION_STATE::IDLE;
	ACTION_STATE			m_ePreState = ACTION_STATE::IDLE;
	WEAPON_TYPE				m_eWeaponType = WEAPON_TYPE::WEAPON_HAND;

private: /*others*/
	const _tchar*	m_TimerTag = TEXT("");
	_float			m_fTerrain_Height = 0.f;

	map<INTERACTKEY, _int> m_KeySets;

	PLAYERSTAT		m_tStat;

	class CEquip_Animation*	m_Equipment = nullptr;

	/*CamMode*/
	_bool			m_bIsFPS = false;
	_uint			m_iCameraMode;

private:/* for Picking Test */
	_float3			m_vPickingPoint;
	_bool			m_bPicked = false;
	_bool			m_bArrive = false;
	_bool			m_bInputKey = false;

private: /*for Debug*/	
	_float fTimeAcc = 0.f;

	_bool					m_bDebugKey = false;
	CVIBuffer_Rect*			m_pDebugBufferCom = nullptr;
	CTransform*				m_pDebugTransformCom = nullptr;
	CTexture*				m_pDebugTextureCom = nullptr;
private: /*for Auto*/
	CGameObject*			m_pTarget = nullptr;

	_bool					m_bAutoMode = false;
	stack<ACTION_STATE>		m_ActStack;

	_bool					m_bMove = true;

	//AtkRange
	vector<SKILLDESC>		m_vecSkillDesc;
	_float					m_fAtkRange = 0.f;
	class CAttackRange*		m_pPicker = nullptr;
	class CAttackRange*		m_pRange = nullptr;

	//for Skills
	_float4x4				m_pOriginMatrix;
	_uint					iCnt = 0;

	_bool					m_bSleeping = false;
	DWORD					m_dwSleepTime = GetTickCount();

	_float					m_fAtkScale = 6.2f;

	//Dead
	_bool					m_bGhost = false;
	_float					m_fReviveTime = 0.f;
	//MovePortal
	_bool					m_bInPortal = false;

	//Building
	_bool					m_bIsBuild = false;
	_bool					m_bBuildTrigger = false;
	_float					m_fBuildTime = 0.f;
private: // Test
	_float3					m_vTargetPicking;
	LEVEL					m_iCurrentLevelndex; //현재 레벨에 따라 불렛 생성 레벨이 다르기 때문에
	LEVEL					m_iPreLevelIndex;
	

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END