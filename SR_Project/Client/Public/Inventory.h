#pragma once

#include "Client_Defines.h"

#include "Base.h"
#include "MainInventory_front.h"
#include "MainInventory_back.h"
#include "Equipment_front.h"
#include "Pont.h"
#include "Playerhp_pont.h"
#include "Playerhunger_pont.h"
#include "PlayerMentality_pont.h"
#include "BagInventory.h"
#include "Mouse_item.h"
#include "ToolboxWeapon.h"
#include "ToolboxWeapon_back.h"
#include "ToolboxWeapon_front.h"
#include "ToolboxGather.h"
#include "ToolboxGather_back.h"
#include "ToolboxGather_front.h"
#include "Craftmain.h"
#include "Craftmain_back.h"
#include "Craftmain_front.h"
#include "Craftbutton.h"
/*  생성된 게임오브젝트들을 모아서 보관한다. : 사용자가 나누고 싶은 기준에 따라. */


class CInventory_Manager final : public CBase
{
	DECLARE_SINGLETON(CInventory_Manager)
private:
	CInventory_Manager();
	virtual ~CInventory_Manager() = default;

public:
	HRESULT Reserve_Container(_uint iNumLevels);
	HRESULT Add_GameObject(_uint iLevelIndex, void* pArg = nullptr);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

	list<CMainInventory_front*>* Get_Inven_list(void) { return&m_MainInventorylist; }
	list<CPont*>* Get_Pont_list(void) { return &m_Pontlist; }
	list<CEquipment_front*>* Get_Equipment_list(void) { return&m_Equipmentlist; }
	list<CPlayerhp_pont*>* Get_playerhp_Pont_list(void) { return &m_Hppontlist; }
	list<CPlayerhunger_pont*>* Get_playerhunger_Pont_list() { return &m_Hungerpontlist; }
	list<CPlayerMentality_pont*>* Get_playermentality_Pont_list() { return &m_Mentalitiypontlist; }
	list<CBagInventory*>* Get_BagInventory_list() {return &m_BagInventorylist;}
	list<CMainInventory_back*>* Get_back_Inven_list(void) { return &m_MainInventorybacklist; }
	list<CMouse_item*>* Get_Mouse_item_list(void) { return &m_Mouseitemlist; }
	list<CToolboxWeapon*>*Get_Toolboxweapon_list(void) { return &m_Toolboxweapon; }
	list<CToolboxWeapon_back*>*Get_Toolboxweapon_back_list(void) { return &m_Toolboxweapon_back; }
	list<CToolboxWeapon_front*>*Get_Toolboxweapon_front_list(void) { return &m_Toolboxweapon_front; }

	list<CToolboxGather*>*Get_Toolboxgather_list(void) { return &m_Toolboxgather; }
	list<CToolboxGather_back*>*Get_Toolboxgather_back_list(void) { return &m_Toolboxgather_back; }
	list<CToolboxGather_front*>*Get_Toolboxgather_front_list(void) { return &m_Toolboxgather_front; }

	list<CCraftmain*>*Get_Craftmain_list(void) { return &m_Craftmain; }
	list<CCraftmain_back*>*Get_Craftmainback_list(void) { return &m_Craftmainback; }
	list<CCraftmain_front*>*Get_Craftmainfront_list(void) { return &m_Craftmainfront; }

	list<CCraftbutton*>*Get_Craftbutton_list(void) { return &m_Craftbutton; }

	void sethp(_uint hp) { m_ihp = hp; }
	void sethuger(_uint hunger) { m_ihunger = hunger;}
	void setmentality(_uint mental) { m_mentality = mental; }

	void Use_bag();
	void Off_bag();

	void weapontool_on();
	void weapontool_off();

	void gathertool_on();
	void gathertool_off();

	void craft_on(MAKEWHAT item);
	void craft_off();



public:
	class CGameObject* Find_Objects(_uint iLevelIndex, const _tchar* pLayerTag);

private: /* 내가 구분하고 싶은 대로 그룹을 지어 레벨별로 객체를 추가한다. */
	_uint										m_iNumLevels = 0;
	/*map<const _tchar*, class CLayer*>*			m_pLayers = nullptr;
	typedef map<const _tchar*, class CLayer*>	LAYERS;*/
	list<CMainInventory_front*> m_MainInventorylist;//[INVEN_END]
	list<CMainInventory_back*> m_MainInventorybacklist;
	list<CEquipment_front*> m_Equipmentlist;//
	list<CPont*> m_Pontlist;//
	list<CPlayerhp_pont*> m_Hppontlist;//
	list<CPlayerhunger_pont*>m_Hungerpontlist;//
	list<CPlayerMentality_pont*>m_Mentalitiypontlist;//
	list<CBagInventory*>m_BagInventorylist;//
	list<CMouse_item*>m_Mouseitemlist;//
	list<CToolboxWeapon*>m_Toolboxweapon;//
	list<CToolboxWeapon_back*>m_Toolboxweapon_back;//
	list<CToolboxWeapon_front*>m_Toolboxweapon_front;//

	list<CToolboxGather*>m_Toolboxgather;
	list<CToolboxGather_back*>m_Toolboxgather_back;
	list<CToolboxGather_front*>m_Toolboxgather_front;

	
	list<CCraftmain*>m_Craftmain;
	list<CCraftmain_back*>m_Craftmainback;
	list<CCraftmain_front*>m_Craftmainfront;

	list<CCraftbutton*>	m_Craftbutton;


	typedef list<CMainInventory_front*> INVENLIST;

	_uint m_ihp = 7;
	_uint m_ihunger = 7;
	_uint m_mentality = 7;

	//private:
	//class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	//class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);


public:
	virtual void Free() override;
};