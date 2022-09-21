#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Interactive_Object.h"
#include "CarnivalCard.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CCarnivalMemory final : public CInteractive_Object
{
public:
	enum STATIONTYPE { STATION_MEMORY, STATION_BIRD, STATION_EGG, STATION_MAX };

	typedef struct StationTagDesc
	{
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
		STATIONTYPE eType = STATION_MAX;
	} STATIONDESC;

public:
	enum STATESTATION
	{
		HIT,
		IDLE_OFF,
		IDLE_ON,
		PLACE,
		TURN_OFF,
		TURN_ON,
		WIN,
		COMPLETE,
		MAX
	};

private:
	CCarnivalMemory(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCarnivalMemory(const CCarnivalMemory& rhs);
	virtual ~CCarnivalMemory() = default;

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

public: /*For Picking */
	virtual _bool Picking(_float3* PickingPoint) override;
	virtual void PickingTrue() override;

public:
	virtual void Interact(_uint Damage = 0) override;
	
	void Make_Guess(_bool bGuess) { m_vecGuesses.push_back(bGuess); }
	virtual HRESULT Drop_Items() override;

	// Memory Game
	void Start_Memory();
	void Play_Memory(_float fTimeDelta);
	void Start_Round(_float fTimeDelta);
	void Check_Guesses();
	void Reset_Round();
	_uint Get_Round_Good_Hint();
	_uint Get_Hungry_Max();
	void Set_FedGoal(_bool bFed) { bFed ? m_iFedGoal-- : m_iFedGoal++; }

	// Bird Game
	void Start_Bird();
	void Play_Bird(_float fTimeDelta);

	//Egg-Game
	void Start_Egg();
	void Play_Egg(_float fTimeDelta);
	_bool Check_Clear();
	void Add_NewEgg();
	void Set_EggCount() { m_iEggCount--; m_eState = COMPLETE; }

private:
	STATIONDESC m_tStationDesc;
	STATESTATION m_eState = IDLE_OFF;
	STATESTATION m_ePreState = MAX;

	// Game Variables
	vector<CCarnivalCard*> m_vecCards;
	vector<_uint> m_vecHints;
	_bool m_bCanPlay = false;

	// Memory Variables
	_bool m_bIsRoundActive = false;
	_uint m_iTurnCount = 5;
	vector<_bool> m_vecGuesses;
	_bool m_bShouldResetRound = false;
	_float m_fGameTimer = 0.f;
	_float m_fIdleOnTimer = 0.f;
	_float m_fWinTimer = 0.f;

	// Bird Variables
	_uint m_iFedGoal = 15;

	// Egg Variables
	DWORD m_dwEggSpawnTime;
	_int m_iEggCount = 0;
	_float m_fMinDistance = 2.f;
	_float m_fMaxDistance = 5.f;
	_bool m_bStart = false;
	_bool m_bIsWin = false;


public:
	static CCarnivalMemory* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END