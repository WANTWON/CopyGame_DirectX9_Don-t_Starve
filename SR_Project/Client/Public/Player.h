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
	enum class DIR_STATE { DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT, PLAYER_END};
	enum class ACTION_STATE{ IDLE, MOVE, MINING, AXE, ATTACK, ACTION_END};
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

	enum class WEAPON_TYPE{ WEAPON_HAND, WEAPON_SWORD, WEAPON_END};
public:
	typedef struct tagPlayerStat
	{
		_float fSpeed = 0.f;
		_float fMental = 0.f;
		_float fHungry = 0.f;
		_float fAtk = 0.f;
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
	void Set_TerrainY(_float TerrainY) { m_fTerrain_Height = TerrainY; }
	void Move_to_PickingPoint(_float fTimedelta);
	void Set_PickingPoint(_float3 PickingPoint) { m_vPickingPoint = PickingPoint; m_bPicked = true; m_bInputKey = false; m_bArrive = false; };

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


private:
	DIR_STATE				m_eDirState = DIR_STATE::DIR_DOWN;
	DIR_STATE				m_ePreDirState = DIR_STATE::DIR_DOWN;
	ACTION_STATE			m_eState = ACTION_STATE::IDLE;
	ACTION_STATE			m_ePreState = ACTION_STATE::IDLE;
	WEAPON_TYPE				m_eWeaponType = WEAPON_TYPE::WEAPON_HAND;

	const _tchar*	m_TimerTag = TEXT("");

	//_uint			m_iKeyNum;
	_bool			m_bInverseScale = false;
	_float			m_fTerrain_Height = 0.f;

	/* for Picking Test */
	_float3			m_vPickingPoint;
	_bool			m_bPicked = false;
	_bool			m_bArrive = false;
	_bool			m_bInputKey = false;

	map<INTERACTKEY, _int> m_KeySets;

private: 

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END