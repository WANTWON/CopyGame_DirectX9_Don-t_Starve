// MyForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MyForm.h"

// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

#include "GameInstance.h"
#include "Terrain.h"
#include "Tool_Defines.h"
#include "MainApp.h"
#include "House.h"
#include "WoodWall.h"
#include "DecoObject.h"
#include "Trap.h"
typedef struct Terraintag
{
	CVIBuffer_Terrain::TERRAINDESC TerrainDesc;
	vector<_float3>		vecPos;
}TERRAIN;



CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
	, m_iMousePoint(0)
	, m_iTerrainTextureNum(0)
	, m_iNumVerticesX(0)
	, m_iNumVerticesZ(0)
	, m_fTerrainSizeX(0)
	, m_fTerrainSizeZ(0)
	, m_fTextureSize(0)
{

}

CMyForm::~CMyForm()
{
}

HOUSETYPE CMyForm::GetHouseType()
{
	if (m_HouseType[0].GetCheck() == TRUE)
		return PIGHOUSE;

	if (m_HouseType[1].GetCheck() == TRUE)
		return SPIDERHOUSE;

	if (m_HouseType[2].GetCheck() == TRUE)
		return BOARONSPAWNER;

	if (m_HouseType[3].GetCheck() == TRUE)
		return MAZESPAWNER;

	return HOUSE_END;
}

DECOTYPE CMyForm::GetDecoType()
{

	if (m_Decotype[0].GetCheck() == TRUE)
		return FLOORFIRE;

	if (m_Decotype[1].GetCheck() == TRUE)
		return TORCH;

	if (m_Decotype[2].GetCheck() == TRUE)
		return FLIES;

	return DECO_END;
}

TRAPTYPE CMyForm::GetTrapType()
{
	if (m_TrapType[0].GetCheck() == TRUE)
		return TRAP_STAR;

	if (m_TrapType[1].GetCheck() == TRUE)
		return TRAP_PLANT;

	return TRAP_MAX;
}

Tool::OBJECTID CMyForm::Get_ObjectID()
{
	for (_uint i = 0; i < 15; i++)
	{
		if (m_Radio[i].GetCheck() == TRUE)
			return Tool::OBJECTID(i);
	}

	return OBJ_END;
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_iMousePoint);
	DDX_Text(pDX, IDC_EDIT2, m_iTerrainTextureNum);
	DDV_MinMaxInt(pDX, m_iTerrainTextureNum, 0, 23);
	DDX_Control(pDX, IDC_SPIN1, m_TextureControl);
	DDX_Text(pDX, IDC_EDIT7, m_iNumVerticesX);
	DDV_MinMaxInt(pDX, m_iNumVerticesX, 0, 999);
	DDX_Text(pDX, IDC_EDIT8, m_iNumVerticesZ);
	DDV_MinMaxInt(pDX, m_iNumVerticesZ, 0, 999);
	DDX_Text(pDX, IDC_EDIT5, m_fTerrainSizeX);
	DDX_Text(pDX, IDC_EDIT6, m_fTerrainSizeZ);
	DDX_Text(pDX, IDC_EDIT9, m_fTextureSize);
	DDX_Control(pDX, IDC_RADIO1, m_Radio[0]);
	DDX_Control(pDX, IDC_RADIO2, m_Radio[1]);
	DDX_Control(pDX, IDC_RADIO3, m_Radio[2]);
	DDX_Control(pDX, IDC_RADIO4, m_Radio[3]);
	DDX_Control(pDX, IDC_RADIO5, m_Radio[4]);
	DDX_Control(pDX, IDC_RADIO6, m_Radio[5]);
	DDX_Control(pDX, IDC_RADIO9, m_Radio[6]);
	DDX_Control(pDX, IDC_RADIO10, m_Radio[7]);
	DDX_Control(pDX, IDC_RADIO11, m_Radio[8]);
	DDX_Control(pDX, IDC_RADIO12, m_Radio[9]);
	DDX_Control(pDX, IDC_RADIO13, m_Radio[10]);
	DDX_Control(pDX, IDC_RADIO19, m_Radio[11]);
	DDX_Control(pDX, IDC_RADIO20, m_Radio[12]);
	DDX_Control(pDX, IDC_RADIO23, m_Radio[13]);
	DDX_Control(pDX, IDC_RADIO24, m_Radio[14]);
	DDX_Control(pDX, IDC_RADIO7, m_HouseType[0]);
	DDX_Control(pDX, IDC_RADIO8, m_HouseType[1]);
	DDX_Control(pDX, IDC_RADIO14, m_HouseType[2]);
	DDX_Control(pDX, IDC_RADIO18, m_HouseType[3]);
	DDX_Control(pDX, IDC_CHECK1, m_WallSideButton);
	DDX_Control(pDX, IDC_RADIO15, m_Decotype[0]);
	DDX_Control(pDX, IDC_RADIO16, m_Decotype[1]);
	DDX_Control(pDX, IDC_RADIO17, m_Decotype[2]);
	DDX_Control(pDX, IDC_CHECK2, m_bVerticalTerrain);
	DDX_Control(pDX, IDC_RADIO21, m_TrapType[0]);
	DDX_Control(pDX, IDC_RADIO22, m_TrapType[1]);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CMyForm::OnTextureControl)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyForm::OnUpdateData)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyForm::OnSaveButton)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyForm::OnLoadButton)
END_MESSAGE_MAP()


// CMyForm 진단입니다.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기입니다.



void CMyForm::OnTextureControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (pNMUpDown->iDelta < 0)
	{
		m_iTerrainTextureNum++;
		if (m_iTerrainTextureNum == 19)
			m_iTerrainTextureNum = 0;
	}
	else if (pNMUpDown->iDelta > 0 && m_iTerrainTextureNum != 0)
	{
		m_iTerrainTextureNum--;
	}
		
	SetDlgItemInt(IDC_EDIT2, m_iTerrainTextureNum);
}


void CMyForm::OnUpdateData()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(FALSE);
}


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UpdateData(TRUE);

	 m_iTerrainTextureNum = 0;
	 m_iNumVerticesX = 2;
	 m_iNumVerticesZ = 2;
	 m_fTerrainSizeX = 1;
	 m_fTerrainSizeZ = 1;
	 m_fTextureSize = 30;

	 m_Radio[0].SetCheck(TRUE);
	 m_HouseType[0].SetCheck(TRUE);
	 m_Decotype[0].SetCheck(TRUE);
	 UpdateData(FALSE);
}


void CMyForm::OnSaveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	Tool::OBJECTID eObjectID = Get_ObjectID();

	switch (eObjectID)
	{
	case Tool::OBJ_TERRAIN:
		Save_Terrain();
		break;
	case  Tool::OBJ_ROCK:
		Save_Rock();
		break;
	case Tool::OBJ_BERRYBUSH:
		Save_BerryBush();
		break;
	case Tool::OBJ_TREE:
		Save_Tree();
		break;
	case Tool::OBJ_GRASS:
		Save_Grass();
		break;
	case Tool::OBJ_HOUSE:
		Save_House();
		break;
	case Tool::OBJ_PIG:
		Save_Pig();
		break;
	case Tool::OBJ_SPIDER:
		Save_Spider();
		break;
	case Tool::OBJ_WALL:
		Save_Wall();
		break;
	case Tool::OBJ_CARROT:
		Save_Carrot();
		break;
	case Tool::OBJ_DECO:
		Save_DecoObject();
		break;
	case Tool::OBJ_DIRT:
		Save_Dirt();
		break;
	case Tool::OBJ_TRAP:
		Save_Trap();
		break;
	case Tool::OBJ_CARNIVAL:
		Save_Carnival_Card();
		break;
	case Tool::OBJ_EGG:
		Save_Carnival_Egg();
		break;
	}
	
}


void CMyForm::OnLoadButton()
{
	
	Tool::OBJECTID eObjectID = Get_ObjectID();

	switch (eObjectID)
	{
	case Tool::OBJ_TERRAIN:
		Load_Terrain();
		break;
	case  Tool::OBJ_ROCK:
		Load_Rock();
		break;
	case Tool::OBJ_BERRYBUSH:
		Load_BerryBush();
		break;
	case Tool::OBJ_TREE:
		Load_Tree();
		break;
	case Tool::OBJ_GRASS:
		Load_Grass();
		break;
	case Tool::OBJ_HOUSE:
		Load_House();
		break;
	case Tool::OBJ_PIG:
		Load_Pig();
		break;
	case Tool::OBJ_SPIDER:
		Load_Spider();
		break;
	case Tool::OBJ_WALL:
		Load_Wall();
		break;
	case Tool::OBJ_CARROT:
		Load_Carrot();
		break;
	case Tool::OBJ_DECO:
		Load_DecoObject();
		break;
	case Tool::OBJ_DIRT:
		Load_Dirt();
		break;
	case Tool::OBJ_TRAP:
		Load_Trap();
		break;
	case Tool::OBJ_CARNIVAL:
		Load_Carnival_Card();
		break;
	case Tool::OBJ_EGG:
		Load_Carnival_Egg();
		break;
	}
	


}

void CMyForm::Save_Terrain()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Terrain");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			dynamic_cast<CTerrain*>(iter)->Save_Data(hFile, dwByte);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);

}

void CMyForm::Load_Terrain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);


		/* 여기서 선언한 것 만큼 해줘야함...*/
		//for(int i = 0; i< iNum; ++i )
		//{
		//_tchar			szFullPath[MAX_PATH] = TEXT("");
		//wsprintf(szFullPath, TEXT("Prototype_Component_VIBuffer_Terrain_Load%d"), i);

		_tchar			szFullPath0[MAX_PATH] = TEXT("Prototype_Component_VIBuffer_Terrain_Load0");
		pGameInstance->Add_Prototype(LEVEL_STATIC, szFullPath0, CMainApp::Get_Instance()->CreateTerrain(hFile, dwByte));
		pGameInstance->Add_GameObjectLoad(TEXT("Prototype_GameObject_Terrain"), LEVEL_STATIC, L"Layer_Terrain", szFullPath0);

	}

	Safe_Release(pGameInstance);

}

void CMyForm::Save_Grass()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Grass");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Grass()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_STATIC, TEXT("Layer_Grass"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_BerryBush()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_BerryBush");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_BerryBush()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Berry_Bush"), LEVEL_STATIC, TEXT("Layer_BerryBush"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_Rock()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Rock");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Rock()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Boulder"), LEVEL_STATIC, TEXT("Layer_Rock"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_Tree()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Tree");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Tree()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tree"), LEVEL_STATIC, TEXT("Layer_Tree"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_House()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_House");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		CHouse::HOUSEDECS HouseDesc;
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &dynamic_cast<CHouse*>(iter)->Get_Desc(), sizeof(CHouse::HOUSEDECS), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_House()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		CHouse::HOUSEDECS HouseDesc;
		_uint iNum = 0;
		

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(HouseDesc), sizeof(CHouse::HOUSEDECS), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_STATIC, TEXT("Layer_House"), &HouseDesc);
		}
	}
}

void CMyForm::Save_Pig()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Pig");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Pig()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pig"), LEVEL_STATIC, TEXT("Layer_Pig"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_Spider()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Spider");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Spider()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Spider"), LEVEL_STATIC, TEXT("Layer_Spider"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_Wall()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Wall");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		CWoodWall::WALLDESC WallDesc;
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WallDesc.etype = CWoodWall::WALL_ROCK;
			WallDesc.vecPosition = (iter)->Get_Position();
			WallDesc.eDir = dynamic_cast<CWoodWall*>(iter)->Get_DirType(); 
			WriteFile(hFile, &WallDesc, sizeof(CWoodWall::WALLDESC), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Wall()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		CWoodWall::WALLDESC WallDesc;
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(WallDesc), sizeof(CWoodWall::WALLDESC), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WoodWall"), LEVEL_STATIC, TEXT("Layer_Wall"), &WallDesc);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_Carrot()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Carrot");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Carrot()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carrot"), LEVEL_STATIC, TEXT("Layer_Carrot"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_DecoObject()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Deco");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		CDecoObject::DECODECS DecoDesc;
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &dynamic_cast<CDecoObject*>(iter)->Get_Desc(), sizeof(CDecoObject::DECODECS), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_DecoObject()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		CDecoObject::DECODECS DecoDesc;
		_uint iNum = 0;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(DecoDesc), sizeof(CDecoObject::DECODECS), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DecoObject"), LEVEL_STATIC, TEXT("Layer_Deco"), &DecoDesc);
		}
	}
}

void CMyForm::Save_Dirt()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Dirt");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Dirt()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dirt"), LEVEL_STATIC, TEXT("Layer_Dirt"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_Trap()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Trap");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		CTrap::TRAPDESC TrapDesc;
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &dynamic_cast<CTrap*>(iter)->Get_Desc(), sizeof(CTrap::TRAPDESC), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Trap()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;

		CTrap::TRAPDESC TrapDesc;
		_uint iNum = 0;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(TrapDesc), sizeof(CTrap::TRAPDESC), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Trap"), LEVEL_STATIC, TEXT("Layer_Trap"), &TrapDesc);
		}
	}
}

void CMyForm::Save_Carnival_Card()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Card");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Carnival_Card()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Carnival_Card"), LEVEL_STATIC, TEXT("Layer_Card"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}

void CMyForm::Save_Carnival_Egg()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(FALSE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	list<CGameObject*>* plistClone = pGameInstance->Get_ObjectList(LEVEL_STATIC, L"Layer_Egg");
	if (nullptr == plistClone)
		return;

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = plistClone->size();

		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);;
		if (0 == hFile)
			return;

		/* 첫줄은 object 리스트의 size 받아서 갯수만큼 for문 돌리게 하려고 저장해놓음*/
		WriteFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (auto& iter : *plistClone)
		{
			WriteFile(hFile, &(iter)->Get_Position(), sizeof(_float3), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
	Safe_Release(pGameInstance);
}

void CMyForm::Load_Carnival_Egg()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CFileDialog FileDialog(TRUE, L"dat", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Data Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	if (IDOK == FileDialog.DoModal())
	{
		HANDLE hFile = 0;
		_ulong dwByte = 0;
		_float3 ObjectPos(0, 0, 0);
		_uint iNum = 0;
		//vector<_tchar*> vecPath;


		hFile = CreateFile(FileDialog.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		/* 타일의 개수 받아오기 */
		ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < iNum; ++i)
		{
			ReadFile(hFile, &(ObjectPos), sizeof(_float3), &dwByte, nullptr);
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CCarnival_Egg"), LEVEL_STATIC, TEXT("Layer_Egg"), ObjectPos);
		}
	}

	Safe_Release(pGameInstance);
}
