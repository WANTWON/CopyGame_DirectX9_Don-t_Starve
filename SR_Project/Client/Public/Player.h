#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
public:

	enum class ACTION_STATE { IDLE, MOVE, ATTACK, MINING, CHOP, ACTION_END };
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
		KEY_END
	}INTERACTKEY;


public:
	typedef struct tagPlayerStat
	{
		_float fSpeed = 0.f;
		_float fMaxHealth = 100.f;
		_float fCurrentHealth = fMaxHealth;
		_float fMaxMental = 100.f;
		_float fCurrentMental = fMaxMental;
		_float fMaxHungry = 100.f;
		_float fCurrentHungry = 100.f;
		_float fAtk = 20.f;
		_float fArmor = 0.f;
	}PLAYERSTAT;

	/*typedef struct tagActData {
	_int iKeyNum;
	_bool bIsKeyDown;
	}ACTDATA;*/

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
	_float3 Get_Pos();
	_float3 Get_State(CTransform::STATE  eState);
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }
	void Move_to_PickingPoint(_float fTimedelta);
	void Set_PickingPoint(_float3 PickingPoint) { m_vPickingPoint = PickingPoint; m_bPicked = true; m_bInputKey = false; m_bArrive = false; };

public: /*Get&Set*/
		//Gets


		//Sets
	void	Set_HP(_float _fHP) { m_tStat.fCurrentHealth += _fHP; }
	void	Set_Atk(_float _fAtk) { m_tStat.fAtk += _fAtk; }
	void	Set_Hungry(_float _fHungry) { m_tStat.fCurrentHungry += _fHungry; }
	void	Set_Mental(_float _fMental) { m_tStat.fCurrentMental += _fMental; }
	void	Set_Speed(_float _fSpeed) { m_tStat.fSpeed += _fSpeed; }
	void	Set_Armor(_float _fArmor) { m_tStat.fArmor += _fArmor; }
	void	Set_WeaponType(WEAPON_TYPE _eWeapon) { m_eWeaponType = _eWeapon; }

private:/*Setup*/
	HRESULT SetUp_Components();
	HRESULT SetUp_KeySettings();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

private: /**Actions*/
	void GetKeyDown(_float _fTimeDelta);
	bool ResetAction(_float _fTimeDelta);
	//Idle
	void Move_Idle(_float _fTimeDelta);
	//Move
	void Move_Up(_float _fTimeDelta);
	void Move_Right(_float _fTimeDelta);
	void Move_Down(_float _fTimeDelta);
	void Move_Left(_float _fTimeDelta);
	//Actions
	void Attack(_float _fTimeDelta);
	void Mining(_float _fTimeDelta);
	void Chop(_float _fTimeDelta);

	void Multi_Action(_float _fTimeDelta); //멀티키
										   //Passive
	void Decrease_Stat(void); //일정시간마다 Hunger 감소

	void Detect_Enemy(void);
	//Test
	void Test_Func(_int _iNum);
	void Test_Debug(_float fTimeDelta);
	//void Test_Attack()
private: /*For TextureCom */
	HRESULT Texture_Clone();
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /* For TransformCom*/
	void SetUp_BillBoard();



private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	vector<CTexture*>       m_vecTexture;

private: /*State*/
	DIR_STATE				m_eDirState = DIR_STATE::DIR_DOWN;
	DIR_STATE				m_ePreDirState = DIR_STATE::DIR_DOWN;
	ACTION_STATE			m_eState = ACTION_STATE::IDLE;
	ACTION_STATE			m_ePreState = ACTION_STATE::IDLE;
	WEAPON_TYPE				m_eWeaponType = WEAPON_TYPE::WEAPON_HAND;

private: /*others*/
	const _tchar*	m_TimerTag = TEXT("");
	_float			m_fTerrain_Height = 0.f;

	/* for Picking Test */
	_float3			m_vPickingPoint;
	_bool			m_bPicked = false;
	_bool			m_bArrive = false;
	_bool			m_bInputKey = false;

	map<INTERACTKEY, _int> m_KeySets;

	PLAYERSTAT		m_tStat;

private: //Test
	class CEquip_Animation*	m_Equipment = nullptr;
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone_Load(const _tchar* VIBufferTag, void* pArg = nullptr) override;
	virtual void Free() override;
};

END