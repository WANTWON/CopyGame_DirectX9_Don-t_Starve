#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Maze final : public CLevel
{
private:
	CLevel_Maze(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Maze() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Object(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar * pLayerTag);

public: //test
	void Start_Camera_Motion();
	void Update_Camera_Motion();
	void Update_Floor_Motion();
	void Update_Fence_Motion();
	void Set_NextLevel(_bool _dd) { m_bNextLevel = _dd; }
	void Set_PastLevel(_bool _dd) { m_bPastLevel = _dd; }
	void Set_Flowerpicked(_bool type) { m_bFlowerPicked = type; m_dwTime = GetTickCount(); }
	
public:
	_bool Get_PuzzleSolved() { return m_bPuzzleSolved; }
	void Set_PuzzleSolved(_bool type) { m_bPuzzleSolved = type; }
private:
	_bool m_bNextLevel = false;
	_bool m_bPastLevel = false;
	DWORD m_dwTime;
	_bool m_bFirst = false;
	_bool m_bTargetCam = false;
	_bool m_bFlowerPicked = false;
	_bool m_bPlayerCam = false;
	_bool m_bPuzzleSolved = false;
	_bool m_bPuzzleStart[6] = { false };
public:
	static CLevel_Maze* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;

};

END