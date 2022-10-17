#pragma once

#include "afxcmn.h"
#include "afxwin.h"
// CMyForm 폼 뷰입니다.
#include "Tool_Defines.h"
class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	int Get_TerrainTexture() { return m_iTerrainTextureNum; }
	int Get_NumVerticesX() { return m_iNumVerticesX; }
	int Get_NumVerticesZ() { return m_iNumVerticesZ; }
	int Get_TerrainSizeX() { return m_fTerrainSizeX; }
	int Get_TerrainSizeZ() { return m_fTerrainSizeZ; }
	float Get_TextureSize() { return m_fTextureSize; }
	bool Get_WallSide_BeChecked() { return m_WallSideButton.GetCheck(); }
	bool Get_VerticalTerrain() { return m_bVerticalTerrain.GetCheck(); }
	HOUSETYPE GetHouseType();
	DECOTYPE  GetDecoType();
	TRAPTYPE GetTrapType();
	Tool::OBJECTID Get_ObjectID();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_iMousePoint;
	int m_iTerrainTextureNum = 0; 
	CSpinButtonCtrl m_TextureControl;
	int m_iNumVerticesX = 2;
	int m_iNumVerticesZ =2;
	int m_fTerrainSizeX = 1;
	int m_fTerrainSizeZ =1;
	float m_fTextureSize = 30;

public:
	afx_msg void OnTextureControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateData();
	virtual void OnInitialUpdate();
	afx_msg void OnSaveButton();
	afx_msg void OnLoadButton();

public:
	void Save_Terrain();
	void Load_Terrain();
	void Save_Grass();
	void Load_Grass();
	void Save_BerryBush();
	void Load_BerryBush();
	void Save_Rock();
	void Load_Rock();

	void Save_Tree();
	void Load_Tree();

	void Save_House();
	void Load_House();

	void Save_Pig();
	void Load_Pig();

	void Save_Spider();
	void Load_Spider();
	
	void Save_Wall();
	void Load_Wall();

	void Save_Carrot();
	void Load_Carrot();

	void Save_DecoObject();
	void Load_DecoObject();

	void Save_Dirt();
	void Load_Dirt();

	void Save_Trap();
	void Load_Trap();

	void Save_Carnival_Card();
	void Load_Carnival_Card();

	void Save_Carnival_Egg();
	void Load_Carnival_Egg();

	CButton m_Radio[15];
	CButton m_HouseType[4];
	CButton m_WallSideButton;
	CButton m_Decotype[3];
	CButton m_bVerticalTerrain;
	CButton m_TrapType[2];
};


