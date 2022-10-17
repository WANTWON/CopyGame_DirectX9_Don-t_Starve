#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
class CGameInstance;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CMainApp final : public CBase
{
	DECLARE_SINGLETON(CMainApp)
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize(HWND hWNd, HINSTANCE hInst);
	void Tick(_float fTimeDelta);
	HRESULT Render();

public:
	HRESULT Add_LayerTerrain(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerGrass(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerBerryBush(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerRock(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerTree(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerHouse(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerPig(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerSpider(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerWall(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerCarrot(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerDeco(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerDirt(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerTrap(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerCard(const _tchar* pLayerTag, void* pArg);
	HRESULT Add_LayerEgg(const _tchar* pLayerTag, void* pArg);


	LPDIRECT3DDEVICE9 Get_GraphicDevice() {return m_pGraphic_Device;}
	class CVIBuffer_Terrain* CreateTerrain(HANDLE hFile, _ulong& dwByte);

private:
	CGameInstance*			m_pGameInstance = nullptr;

private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CCollider*				m_pCollider = nullptr;


private:
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Texture();
	HRESULT Ready_GameObject();


	HRESULT Ready_Layer_Background(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);


	HRESULT SetUp_RenderState();
	

public:
	static CMainApp* Create();
	virtual void Free();
};

END

