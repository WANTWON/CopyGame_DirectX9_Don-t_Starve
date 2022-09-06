#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pKey_Manager(CKeyMgr::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pSound_Manager(CSound_Manager::Get_Instance())
{
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pKey_Manager);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pSound_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, LPDIRECT3DDEVICE9* ppOut)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pObject_Manager ||
		nullptr == m_pInput_Device	||
		nullptr == m_pPicking		||
		nullptr == m_pSound_Manager)
		return E_FAIL;

	/* 그래픽 디바이스를 초기화한다. */
	if (FAILED(m_pGraphic_Device->InitDevice(GraphicDesc, ppOut)))
		return E_FAIL;

	/* 입력 디바이스를 초기화한다. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드 디바이스를 초기화한다. */
	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	/* 픽킹 초기화 */
	if (FAILED(m_pPicking->Initialize(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, *ppOut)))
		return E_FAIL;

	/* 컨테이너의 공간을 확보해둔다. */
	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;


	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager ||
		nullptr == m_pInput_Device )
		return;

	m_pInput_Device->Update();

	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);

	m_pPicking->Tick();

	m_pLevel_Manager->Late_Tick(fTimeDelta);
	m_pObject_Manager->Late_Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return;

	m_pComponent_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
}

void CGameInstance::Render_Begin(void)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Render_Begin();
}

void CGameInstance::Render_End(HWND hWnd)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Render_End(hWnd);
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);	
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Update(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update(pTimerTag);
}

void CGameInstance::Set_ZeroTimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->SetZero_TimeDelta(pTimerTag);
}

void CGameInstance::Delete_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Delete_Timer(pTimerTag);
}

HRESULT CGameInstance::Open_Level(unsigned int iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObject(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_GameObjectLoad(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, const _tchar * VIBUfferTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectLoad(pPrototypeTag, iLevelIndex, pLayerTag, VIBUfferTag, pArg);
}

CGameObject * CGameInstance::Get_Object(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Objects(iLevelIndex, pLayerTag, iIndex);
}

list<CGameObject*>* CGameInstance::Get_ObjectList(_uint iSceneID, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ObjectList(iSceneID, pLayerTag);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

void CGameInstance::Clear_Layer(_uint iLevelIndex, const _tchar * LayerTag)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear_Layer(iLevelIndex, LayerTag);
}


HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

bool CGameInstance::Key_Pressing(int _Key)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Key_Pressing(_Key);
}

bool CGameInstance::Key_Up(int _Key)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Key_Up(_Key);
}

bool CGameInstance::Key_Down(int _Key)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Key_Down(_Key);
}

void CGameInstance::PlaySound(TCHAR * pSoundKey, const _uint & eID, const float & fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->PlaySoundW(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(TCHAR * pSoundKey, const float & fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(const _uint & eID)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(const _uint & eID, const float & fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

int CGameInstance::VolumeUp(const _uint & eID, const _float & _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->VolumeUp(eID, _vol);
}

int CGameInstance::VolumeDown(const _uint & eID, const _float & _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->VolumeDown(eID, _vol);
}

int CGameInstance::Pause(const _uint & eID)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->Pause(eID);
}

void CGameInstance::Release_Engine()
{

	CGameInstance::Get_Instance()->Destroy_Instance();

	CSound_Manager::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();

	CKeyMgr::Get_Instance()->Destroy_Instance();

	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}
