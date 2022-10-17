#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Vertex.h"
#include "CameraDynamic.h"
#include "VIBuffer_Terrain.h"
#include "Tree.h"
#include "BerryBush.h"
#include "Boulder.h"
#include "Grass.h"
#include "House.h"
#include "Pig.h"
#include "Spider.h"
#include "WoodWall.h"
#include "Carrot.h"
#include "DecoObject.h"
#include "Carnival_Egg.h"

#include "Dirt.h"
#include "Trap.h"
#include "CarnivalCard.h"

IMPLEMENT_SINGLETON(CMainApp)

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize(HWND hWNd, HINSTANCE hInst)
{
	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

	GRAPHIC_DESC		Graphic_Desc;
	ZeroMemory(&Graphic_Desc, sizeof(GRAPHIC_DESC));

	Graphic_Desc.hWnd = hWNd;
	Graphic_Desc.iWinSizeX = g_iToolViewCX;
	Graphic_Desc.iWinSizeY = g_iToolViewCY;
	Graphic_Desc.eWinMode = GRAPHIC_DESC::MODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(hInst, LEVEL_END, Graphic_Desc, &m_pGraphic_Device)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Texture()))
		return E_FAIL;

	if (FAILED(Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Background(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;


	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(fTimeDelta);
	//m_pCollider->Update_ColliderGroup();

}

HRESULT CMainApp::Render()
{
	if (m_pRenderer == nullptr)
		return S_OK;

	m_pGameInstance->Render_Begin();

	m_pRenderer->Render_GameObjects();

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Add_LayerTerrain(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerGrass(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerBerryBush(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Berry_Bush"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerRock(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boulder"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerTree(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tree"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerHouse(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerPig(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pig"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerSpider(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Spider"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerWall(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WoodWall"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerCarrot(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carrot"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerDeco(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerDirt(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dirt"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerTrap(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Trap"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerCard(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Card"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Add_LayerEgg(const _tchar * pLayerTag, void * pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CCarnival_Egg"), LEVEL_STATIC, pLayerTag, pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Terrain * CMainApp::CreateTerrain(HANDLE hFile, _ulong & dwByte)
{
	return CVIBuffer_Terrain::Create(m_pGraphic_Device, hFile, dwByte);
}


HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_Component_VIBuffer_Terrain*/
	CVIBuffer_Terrain::TERRAINDESC		TerrainDesc;
	ZeroMemory(&TerrainDesc, sizeof(CVIBuffer_Terrain::TERRAINDESC));
	TerrainDesc.m_iNumVerticesX = 30;
	TerrainDesc.m_iNumVerticesZ = 30;
	TerrainDesc.m_fTextureSize = 30.f;
	TerrainDesc.m_fSizeX = 1;
	TerrainDesc.m_fSizeZ = 1;
	TerrainDesc.m_iTextureNum = 2;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, TerrainDesc))))
		return E_FAIL;

	ZeroMemory(&TerrainDesc, sizeof(CVIBuffer_Terrain::TERRAINDESC));
	TerrainDesc.m_iNumVerticesX = 50;
	TerrainDesc.m_iNumVerticesZ = 50;
	TerrainDesc.m_fTextureSize = 30.f;
	TerrainDesc.m_fSizeX = 1;
	TerrainDesc.m_fSizeZ = 1;
	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_BaseFlane"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, TerrainDesc))))
		return E_FAIL;

	

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), m_pCollider= CCollider::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Dirt*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dirt"),
		CDirt::Create(m_pGraphic_Device))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Texture()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Terrain/tile%03d.png"), 23))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Flies"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Flies/Idle_%03d.png"), 55))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CarnivalGame_Card_Reveal_Good"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/CarnivalGame_Card/Reveal_Good/Reveal_Good_%03d.png"), 19))))
		return E_FAIL;

	/*Add Texture Carnival EggGame*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carnival_Egg_Idle_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Carnival_Egg/Egg/Idle/Down/Idle_%03d.png"), 6))))
		return E_FAIL;


#pragma region Add_Texture_Trap
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Idle_Ready"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Star/Idle_Ready/Idle_Ready_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Idle_Trap"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Star/Idle_Trap/Idle_Trap_%03d.png"), 17))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Trap"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Star/Trap/Trap_%03d.png"), 49))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star_Reset"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Star/Reset/Reset_%03d.png"), 17))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Idle_Ready"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Plant/Idle_Ready/Idle_Ready_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Idle_Trap"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Plant/Idle_Trap/Idle_Trap_%03d.png"), 3))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Trap"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Plant/Trap/Trap_%03d.png"), 24))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Plant_Reset"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Trap_Plant/Reset/Reset_%03d.png"), 16))))
		return E_FAIL;
#pragma endregion Add_Texture_Trap

	/*For. Prototype_Component_Texture_Carrot*/
#pragma region Add_Texture_Carrot
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Carrot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Carrot/Carrot_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Carrot
	/*For. Prototype_Component_Texture_Tree*/
#pragma region Add_Texture_Tree
	// Tall

	/*For.Prototype_Component_Texture_Dirt */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dirt"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Dirt/Dirt_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dirt_Empty"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Dirt/Empty_%03d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tree_Tall_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Idle/Idle_%03d.png"), 80))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tree_Tall_CHOP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Chop/Chop_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_RIGHT"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Fall_Right/Fall_Right_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tree_Tall_FALL_LEFT"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Fall_Left/Fall_Left_%03d.png"), 38))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tree_Tall_STUMP"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Evergreen/Tall/Stump/Stump_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Tree

	/*For. Prototype_Component_Texture_Grass*/
#pragma region Add_Texture_Grass
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Idle/Idle_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass_PICK"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Pick/Pick_%03d.png"), 15))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass_PICKED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Grass/Picked/Picked_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Grass

	/*For. Prototype_Component_Texture_Boulder*/
#pragma region Add_Texture_Boulder
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boulder_HEALTHY"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Boulder/Healthy/Healthy_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boulder_DAMAGED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Boulder/Damaged/Damaged_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boulder_BROKEN"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Boulder/Broken/Broken_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Boulder

	/*For. Prototype_Component_Texture_Berry_Bush*/
#pragma region Add_Texture_Berry_Bush
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Berry_Bush_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Idle/Idle_%03d.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Berry_Bush_PICK"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Pick/Pick_%03d.png"), 14))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Berry_Bush_PICKED"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Berry_Bush/Picked/Picked_%03d.png"), 1))))
		return E_FAIL;
#pragma endregion Add_Texture_Berry_Bush

	/*For. Prototype_Component_Texture_House*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pig_House"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Construct/PigHouse.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spider_House"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Construct/SpiderHouse.png"), 1))))
		return E_FAIL;

	/*For. Prototype_Component_Texture_Spider*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spider_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Spider/Idle/Idle_%03d.png"), 1))))
		return E_FAIL;

	/*For. Prototype_Component_Texture_Pig*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pig_Idle_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Monster/Pig/Idle_Down/Idle_Down_%03d.png"), 18))))
		return E_FAIL;

	/*For. Prototype_Component_Texture_Wall*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RockWall_HEALTHY"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/RockWall/Healthy/Healthy_%03d.png"), 3))))
		return E_FAIL;

	/*For.Prototype_Texture FloorDeco */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FloorDeco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Deco/FloorFire/Idle_%03d.png"), 44))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Torch"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Deco/Torch/Idle_%03d.png"), 5))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Spawner */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Spawner"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Object/Spawner/Spawner_%03d.png"), 2))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Ready_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CCarnival_Egg"),
		CCarnival_Egg::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Trap*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trap"),
		CTrap::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Carnival_Card*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Carnival_Card"),
		CCarnivalCard::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCameraDynamic::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vertex"),
		CVertex::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Tree */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Grass */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Boulder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boulder"),
		CBoulder::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Berry_Bush */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Berry_Bush"),
		CBerryBush::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_House */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_House"),
		CHouse::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Monster*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pig"),
		CPig::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For. Prototype_GameObject_Spider */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spider"),
		CSpider::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_WoodWall */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WoodWall"),
		CWoodWall::Create(m_pGraphic_Device))))
		return E_FAIL;

	/*For.Prototype_GameObject_Carrot */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Carrot"),
		CCarrot::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DecoObject"),
		CDecoObject::Create(m_pGraphic_Device))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Layer_Background(const _tchar* pLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_STATIC, pLayerTag)))
		//return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Vertex"), LEVEL_STATIC, TEXT("Layer_Vertex"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CCameraDynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCameraDynamic::CAMERADESC_DERIVED));

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float3(0.f, 5.f, -5.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iToolViewCX / g_iToolViewCY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 600.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f)*0.01f;

	if (FAILED(m_pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_STATIC, pLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	 //m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}


CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize(g_hWnd, g_hInst)))
	{
		ERR_MSG(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pGameInstance);
	
	CGameInstance::Release_Engine();
}

