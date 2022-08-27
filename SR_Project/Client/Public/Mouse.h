#pragma once

#include "Client_Defines.h"

#include "Base.h"
#include "MainInventory_front.h"
#include "Equipment_front.h"
#include "Pont.h"

/*  생성된 게임오브젝트들을 모아서 보관한다. : 사용자가 나누고 싶은 기준에 따라. */


class CMouse final : public CBase
{
	DECLARE_SINGLETON(CMouse)
private:
	CMouse();
	virtual ~CMouse() = default;

public:
	/*HRESULT Reserve_Container(_uint iNumLevels);
	HRESULT Add_GameObject(_uint iLevelIndex, void* pArg = nullptr);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);*/
	void Clear(_uint iLevelIndex);


public:
	//class CGameObject* Find_Objects(_uint iLevelIndex, const _tchar* pLayerTag);

	ITEMNAME Get_Item_name(void) { return m_Item_Name; }
	ITEMNAME Get_Prev_Item_name(void) { return m_Prev_Item_Name; }
	void Set_Item_name(ITEMNAME& itemname) { m_Item_Name = itemname; }
	void Set_Prev_Item_name(ITEMNAME& itemname) { m_Prev_Item_Name = itemname; }
	bool Get_picked(void) { return m_bpicked; }
	void Set_picked(bool tof) { m_bpicked = tof; }
	_uint Get_index() { return m_iboxindex; }
	void Set_index(_uint indexnum) { m_iboxindex = indexnum; }
	

private: /* 내가 구분하고 싶은 대로 그룹을 지어 레벨별로 객체를 추가한다. */
	_uint										m_iboxindex = 20;
	ITEMNAME m_Item_Name = ITEMNAME_END;
	ITEMNAME m_Prev_Item_Name = ITEMNAME_END;
	bool m_bpicked = false;
	


public:
	virtual void Free() override;
};