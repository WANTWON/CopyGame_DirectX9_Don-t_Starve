
//class CEquip_Animation

#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CCollider_Manager;
class CShader;
END

BEGIN(Client)
class CEquip_Animation : public CGameObject
{
public:

	enum class ACTION_STATE { IDLE, MOVE, ATTACK, BUILD, ACTION_END };
protected:
	CEquip_Animation(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEquip_Animation(const CEquip_Animation& rhs);
	virtual ~CEquip_Animation() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:/*Get&Set*/
	void	Set_WeaponType(WEAPON_TYPE _eWeapon) { m_eWeaponType = _eWeapon; }
	void	Set_Directon(DIR_STATE _eDirState) { m_eDirState = _eDirState; }
	void	Set_ActionState(_uint iNum) { m_eState = ACTION_STATE(iNum); }
	void	Set_TargetPos(_float3 _TargetPos) { m_MovePos = _TargetPos; }
protected:
public:

private:/*Setup*/
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();


private:
	void Select_Animation();
	void Direction_Check();
	void Set_Offset();
private: /*For TextureCom */
	HRESULT Texture_Clone(void);
	HRESULT Change_Texture(const _tchar* LayerTag);

private: /* For TransformCom*/
	void SetUp_BillBoard();

protected://value

protected: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CTexture*>       m_vecTexture;
protected: /*STATE*/
	ACTION_STATE			m_eState = ACTION_STATE::IDLE;
	ACTION_STATE			m_ePreState = ACTION_STATE::IDLE;
	DIR_STATE				m_eDirState = DIR_STATE::DIR_DOWN;
	DIR_STATE				m_ePreDirState = DIR_STATE::DIR_DOWN;
	WEAPON_TYPE				m_eWeaponType = WEAPON_TYPE::WEAPON_SWORD;
	SHADER_STATE			m_eShaderID = SHADER_IDLE_ALPHATEST;
protected:/*others*/
	const _tchar*	m_TimerTag = TEXT("");
	_float3			m_MovePos;
	_bool			m_bIsAttack = false;
public:
	static CEquip_Animation* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END