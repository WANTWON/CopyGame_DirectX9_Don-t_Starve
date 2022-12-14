#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CShader;
END

BEGIN(Client)

class CPawn abstract : public CGameObject
{
protected:
	CPawn(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPawn(const CPawn& rhs);
	virtual ~CPawn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_ShaderID(_bool bIsAlphaBlend = false);
public:
	OBJID	Get_ObjID(void) { return m_eObjID; }
	SHADER_STATE m_eShaderID = SHADER_DAYCYClE;
protected:
	OBJID m_eObjID;
	_bool m_bPickingTrue = false;

public:
	virtual void Free() override;
};

END