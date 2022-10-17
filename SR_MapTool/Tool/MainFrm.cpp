
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"
#include "ToolView.h"
#include "MyForm.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

HWND g_Main_hWnd;
HINSTANCE g_Main_hInst;

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

Tool::OBJECTID CMainFrame::Get_ObjectID()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return Tool::OBJECTID();

	return pFormView->Get_ObjectID();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
void CMainFrame::Get_ClientRect(UINT iRow, UINT iCol, RECT * pRect)
{
	CWnd* pView = m_MainSplitter.GetPane(iRow, iCol);
	if (nullptr == pView)
		return;

	pView->GetClientRect(pRect);
}


int CMainFrame::Get_TerrainTextureNum()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return -1;

	return pFormView->Get_TerrainTexture();
}
int CMainFrame::Get_NumVerticesX()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return -1;

	return pFormView->Get_NumVerticesX();
}
int CMainFrame::Get_NumVerticesZ()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return -1;

	return pFormView->Get_NumVerticesZ();
}
int CMainFrame::Get_TerrainSizeX()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return -1;

	return pFormView->Get_TerrainSizeX();
}
int CMainFrame::Get_TerrainSizeZ()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return -1;

	return pFormView->Get_TerrainSizeZ();
}
float CMainFrame::Get_TextureSize()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return -1;

	return pFormView->Get_TextureSize();
}
bool CMainFrame::Get_WallSide_BeChecked()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return false;

	return pFormView->Get_WallSide_BeChecked();
}
HOUSETYPE CMainFrame::Get_HouseType()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return HOUSE_END;

	return pFormView->GetHouseType();
}
DECOTYPE CMainFrame::GetDecoType()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return DECO_END;

	return pFormView->GetDecoType();
}
TRAPTYPE CMainFrame::GetTrapType()
{
	CMyForm* pFormView = dynamic_cast<CMyForm*>(m_MainSplitter.GetPane(0, 1));
	if (nullptr == pFormView)
		return TRAP_MAX;

	return pFormView->GetTrapType();
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	g_Main_hWnd = GetSafeHwnd();
	g_Main_hInst = AfxGetInstanceHandle();

	m_MainSplitter.CreateStatic(this, 1, 2);

	m_MainSplitter.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(g_iToolViewCX, g_iToolViewCY), pContext);
	m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CMyForm), CSize(g_iSideViewCX, g_iSideViewCY), pContext);

	m_MainSplitter.SetColumnInfo(0, 1080, 500);
	/*m_pMainView = (CToolView*)m_MainSplitter.GetPane(0, 1);
	m_pMyFormView = (CMyForm*)m_MainSplitter.GetPane(0, 0);*/

	return TRUE;
}


void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	m_MainSplitter.DestroyWindow();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
