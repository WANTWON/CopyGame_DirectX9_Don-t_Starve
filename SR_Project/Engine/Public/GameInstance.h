#pragma once

#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Component_Manager.h"
#include "Input_Device.h"
#include "KeyMgr.h"
#include "Picking.h"
#include "Sound_Manager.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, LPDIRECT3DDEVICE9* ppOut);
	void Tick_Engine(_float fTimeDelta);

	void Clear(_uint iLevelIndex);


public: /* For.Graphic_Device */
	void		Render_Begin(void);
	void		Render_End(HWND hWnd = 0);

public: /* For. Input_Device */
	_char Get_DIKState(_uchar eKeyID);
	_char Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Get_TimeDelta(const _tchar* pTimerTag);
	void Update(const _tchar* pTimerTag);
	void Set_ZeroTimeDelta(const _tchar* pTimerTag);
	void Delete_Timer(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(unsigned int iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObjectLoad(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, const _tchar* VIBUfferTag, void* pArg = nullptr);

	class CGameObject* Get_Object(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex =0);
	list<CGameObject*>* Get_ObjectList(_uint iSceneID, const _tchar * pLayerTag);
	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* For. Key Manager*/
	bool		Key_Pressing(int _Key);
	bool		Key_Up(int _Key);		// 누르고 있다가 뗐을 때
	bool		Key_Down(int _Key);		// 눌렀을 때

public: /* For. Sound Manager */
	void PlaySound(TCHAR* pSoundKey, const _uint& eID, const float& fVolume);
	void PlayBGM(TCHAR * pSoundKey, const float& fVolume);
	void StopSound(const _uint& eID);
	void StopAll();
	void SetChannelVolume(const _uint& eID, const float& fVolume);
	int  VolumeUp(const _uint& eID, const _float& _vol);
	int  VolumeDown(const _uint& eID, const _float& _vol);
	int  Pause(const _uint& eID);

public: /* For. Collider Manager */
	HRESULT Add_CollisionGroup(CCollider::COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);
	HRESULT Update_ColliderGroup();

	_bool Collision_with_Group(CCollider::COLLISION_GROUP eGroup, class CGameObject* pGameObject);
	_bool Collision_Check_Group_Multi(CCollider::COLLISION_GROUP eGroup, vector<class CGameObject*>& vecDamagedObj, class CGameObject* pDamageCauser);

public:
	static void Release_Engine();

private:
	CGraphic_Device*				m_pGraphic_Device = nullptr;
	CInput_Device*					m_pInput_Device = nullptr;
	CLevel_Manager*					m_pLevel_Manager = nullptr;
	CObject_Manager*				m_pObject_Manager = nullptr;
	CTimer_Manager*					m_pTimer_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CKeyMgr*						m_pKey_Manager = nullptr;
	CPicking*						m_pPicking = nullptr;
	CSound_Manager*					m_pSound_Manager = nullptr;
	CCollider*						m_pCollider_Manager = nullptr;

public:
	virtual void Free() override;
};

END