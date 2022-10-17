#pragma once

namespace Engine
{
	typedef struct FaceIndices16
	{
		unsigned short _0, _1, _2;
	}FACEINDICES16;

	typedef struct FaceIndices32
	{
		unsigned long _0, _1, _2;
	}FACEINDICES32;


	typedef struct LineIndices16
	{
		unsigned short _0, _1;
	}LINEINDICES16;

	typedef struct LineIndices32
	{
		unsigned long _0, _1;
	}LINEINDICES32;


	typedef struct tagVertexTexture
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR2		vTexture;
	}VTXTEX;

	typedef struct tagVertexCubeTexture
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vTexture;
	}VTXCUBETEX;


	typedef struct tagVertexColor
	{

	}VTXCOL;


	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		HWND			hWnd;
		WINMODE			eWinMode;

	}GRAPHIC_DESC;
}
