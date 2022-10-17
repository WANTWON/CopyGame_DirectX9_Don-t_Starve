
// ToolView.h : CToolView 클래스의 인터페이스
//

#pragma once
#include "Terrain.h"

BEGIN(Engine)
class CGameInstance;
END


class CToolDoc;
class CToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// 특성입니다.
public:
	CToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

private:
	HRESULT Ready_Window_Size();
	HRESULT Ready_Layer_Terrain(const _tchar* LayerTag);
	HRESULT Ready_Layer_Grass(const _tchar* LayerTag);
	HRESULT Ready_Layer_Rock(const _tchar* LayerTag);
	HRESULT Ready_Layer_Tree(const _tchar* LayerTag);
	HRESULT Ready_Layer_BerryBush(const _tchar* LayerTag);
	HRESULT Ready_Layer_House(const _tchar* LayerTag);
	HRESULT Ready_Layer_Pig(const _tchar* LayerTag);
	HRESULT Ready_Layer_Spider(const _tchar* LayerTag);
	HRESULT Ready_Layer_Wall(const _tchar* LayerTag);
	HRESULT Ready_Layer_Carrot(const _tchar* LayerTag);
	HRESULT Ready_Layer_DecoObject(const _tchar* LayerTag);
	HRESULT Ready_Layer_Dirt(const _tchar* LayerTag);
	HRESULT Ready_Layer_Trap(const _tchar* LayerTag);
	HRESULT Ready_Layer_Card(const _tchar* LayerTag);
	HRESULT Ready_Layer_Egg(const _tchar* LayerTag);


public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	CGameInstance*		m_pGameInstance = nullptr;
	CTerrain*			m_pTerrain = nullptr;
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	HRESULT Erase_CurrentTerrain();
	HRESULT Rotate_Object_Plus();
	HRESULT Rotate_Object_Minus();
};

#ifndef _DEBUG  // ToolView.cpp의 디버그 버전
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

