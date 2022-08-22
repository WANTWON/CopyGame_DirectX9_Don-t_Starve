#pragma once

namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const unsigned int	g_iSideViewCX = 400;
	const unsigned int	g_iSideViewCY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

}

extern HWND		g_ToolhWnd;
extern HINSTANCE g_ToolhInst;

using namespace Tool;

