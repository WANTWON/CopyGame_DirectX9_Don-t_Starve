#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END