#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CObserver abstract 
{
protected:
	CObserver() {};
	~CObserver() {};

public:
	virtual void Update(_uint eDayState) = 0;
};

END