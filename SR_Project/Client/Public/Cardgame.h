#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCardgame final : public CGameObject
{
public:
	typedef struct CardDesc
	{
		_float3 pos = _float3(0.f, 0.f, 0.f);
		_int iNumber = 0;
	}CARDDESC;
private:
	CCardgame(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCardgame(const CCardgame& rhs);
	virtual ~CCardgame() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

	bool get_check_bag() { return m_bcheck_bag; }
	bool get_check() { return m_bcheck; }
	void set_check(bool tof) { m_bcheck = tof; }
	void set_check_bag(bool tof) { m_bcheck_bag = tof; }
	void set_texnum(ITEMNAME itemname) { texnum = itemname; }
	void set_pos(_float3 p) { m_CaedDesc.pos = p; }

	void set_first(bool tof) { m_bfirst = tof; }
	void SetUp_BillBoard();

	//void setcardback() { texnum = 5; }


	foreffect effectdesc;

	DWORD m_dwDeadtime = GetTickCount();


	int get_iNum() { return iNum; }

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	//CTransform*				m_pTransformCom1 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	CARDDESC					m_CaedDesc;
	_float4x4				m_ProjMatrix;
	_float					m_fX = 0, m_fY = 0, m_fSizeX, m_fSizeY;

	_float3 mypos = _float3(0.f, 0.f, 0.f);
	
	_float i = 1;
	_float3 vdir = _float3(0.f, 0.f, 0.f);

	_float4x4 m_WorldMatrix;

	

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CCardgame* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	void WalkingTerrain(); 
	virtual _bool Picking(_float3* PickingPoint) override;
	virtual void PickingTrue() override;

	_bool Get_frontcard() { return frontcard; }
	void Set_frontcard(bool tof) { frontcard = tof; }
	_uint Get_texnum() { return texnum; }
	_uint Get_texnum1() { return texnum1; }
	void Set_dead(bool tof) { m_bdead = tof; }
	

	void set_turn(bool tof) { turn = tof;  }
	void set_lastdance(bool tof) { m_blastdance = tof; }
	void set_wrong(bool tof) { m_bwrong = tof; }

	void set_goback(bool tof) { goback = tof; }
	

private:

	int iNum = 0;
	bool m_bcheck_bag = true;
	bool m_bcheck = false;
	bool m_Lclick = true;
	bool one = false;
	bool turn = false;
	bool bfirst = false;
	bool m_blastdance = false;

	_bool goback = true;
	_bool lastcard = false;

	_float j = 2.f;
	
	_float3 m_vecOutPos;

	_float ftime = 0.f;
	_float ftime2 = 0.f;
	_float ftime3 = 0.f; 
	_float ftime4 = 0.f;

	_uint texnum = 5;
	_uint texnum1 = 0;
	_uint random = 0;

	
	

	bool m_bfirst = false;
	bool m_bdead = false;
	_bool m_bPicking = false;
	_bool m_bPickingTrue = false;
    
	_bool m_bwrong = false;
	_bool frontcard = false;
	//bool m_bcheck = true;
};

END