#pragma once

#include "Engine_Defines.h"

/* 레퍼런스 카운트를 관리(증가, 감소) 한다. */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	CBase();
	virtual ~CBase() = default;

public:
	unsigned long AddRef(); /* 레퍼런스카운트를 증가시킨다. */
	unsigned long Release(); /* 레퍼런스카운트를 감소시킨다. or 삭제한다. */

private:
	unsigned long			m_dwRefCnt = 0;

public:
	virtual void Free() = 0;
};

END

