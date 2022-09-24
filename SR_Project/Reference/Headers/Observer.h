#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CObserver abstract //: public CBase
{
protected:
	CObserver();
	virtual ~CObserver() = default;

public:
	virtual void Update(_uint eDayState) = 0;
	//virtual void Free() override = 0;
};

END