#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Spear final : public CVIBuffer
{
private:
	CVIBuffer_Rect_Spear(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Rect_Spear(const CVIBuffer_Rect_Spear& rhs);
	virtual ~CVIBuffer_Rect_Spear() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_Rect_Spear* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END