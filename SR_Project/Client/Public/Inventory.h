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
#include "ToolboxMain_back.h"
#include "ToolboxWeapon.h"
#include "ToolboxWeapon_back.h"
#include "ToolboxWeapon_front.h"

#include "ToolboxGather.h"
#include "ToolboxGather_back.h"
#include "ToolboxGather_front.h"

#include "ToolboxConstruct.h"
#include "ToolboxConstruct_back.h"
#include "ToolboxConstruct_front.h"

#include "ToolboxMaterial.h"
#include "ToolboxMaterial_back.h"
#include "ToolboxMaterial_front.h"

#include "Craftmain.h"
#include "Craftmain_back.h"
#include "Craftmain_front.h"
#include "Craftbutton.h"
#include "CraftPont.h"
#include "Quest.h"
#include "Questpont.h"
#include "Line.h"
#include "Talk.h"

#include "PotMain.h"
#include "Pot_back.h"
#include "Pot_front.h"
#include "Potbutton.h"
#include "CookPot.h"

#include "Mouse_iteminfo.h"
#include "Eatitem.h"
#include "Mouse_Monster.h"

#include "Daypont.h"
#include "Deadcountpont.h"
#include "Deadmain.h" 
#include "Daycountpont.h"

#include "Loadingscene.h"
#include "Monsterhp_pont.h"
#include "Skill_Icon.h"
#include "MiniMap_Icon.h" 
#include "MiniMap.h"
#include "Cardgame.h"
#include "Party.h"
#include "Partyhp.h"
#include "Endingscene.h"
/*  생성 게임오브젝트들을 모아서 보관한다. : 사용자가 나누고 싶은 기준에 따라. */


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
	list<CMouse_iteminfo*>* Get_Mouse_iteminfo_list(void) { return &m_Mouseiteminfolist; }

	list<CToolboxWeapon*>*Get_Toolboxweapon_list(void) { return &m_Toolboxweapon; }
	list<CToolboxWeapon_back*>*Get_Toolboxweapon_back_list(void) { return &m_Toolboxweapon_back; }
	list<CToolboxWeapon_front*>*Get_Toolboxweapon_front_list(void) { return &m_Toolboxweapon_front; }

	list<CToolboxGather*>*Get_Toolboxgather_list(void) { return &m_Toolboxgather; }
	list<CToolboxGather_back*>*Get_Toolboxgather_back_list(void) { return &m_Toolboxgather_back; }
	list<CToolboxGather_front*>*Get_Toolboxgather_front_list(void) { return &m_Toolboxgather_front; }

	list<CToolboxConstruct*>*Get_ToolboxConstruct_list(void) { return &m_Toolboxconstruct; }
	list<CToolboxConstruct_back*>*Get_ToolboxConstruct_back_list(void) { return &m_Toolboxconstruct_back; }
	list<CToolboxConstruct_front*>*Get_ToolboxConstruct_front_list(void) { return &m_Toolboxconstruct_front; }

	list<CToolboxMaterial*>*Get_ToolboxMaterial_list(void) { return &m_Toolboxmaterial; }
	list<CToolboxMaterial_back*>*Get_ToolboxMaterial_back_list(void) { return &m_Toolboxmaterial_back; }
	list<CToolboxMaterial_front*>*Get_ToolboxMaterial_front_list(void) { return &m_Toolboxmaterial_front; }

	list<CCraftmain*>*Get_Craftmain_list(void) { return &m_Craftmain; }
	list<CCraftmain_back*>*Get_Craftmainback_list(void) { return &m_Craftmainback; }
	list<CCraftmain_front*>*Get_Craftmainfront_list(void) { return &m_Craftmainfront; }

	list<CCraftbutton*>*Get_Craftbutton_list(void) { return &m_Craftbutton; }

	list<CCraftPont*>*Get_Craftpont_list(void) { return &m_Craftpont; }

	list<CQuest*>*Get_Quest_list(void) { return &m_Quest; }
	list<CQuestpont*>*Get_Questpont_list(void) { return &m_Questpont; }
	list<CLine*>*Get_Line_list(void) { return &m_Line; }
	list<CTalk*>*Get_Talk_list(void) { return &m_Talk; }

	list<CPotMain*>*Get_Pot_list(void) { return &m_Pot; }
	list<CPot_back*>*Get_Potback_list(void) { return &m_Potback; }
	list<CPot_front*>*Get_Potfront_list(void) { return &m_Potfront; }
	list<CPotbutton*>*Get_Potbutton_list(void) { return &m_Potbutton; }

	list<CEateffect*>*Get_Eatitem_list(void) { return &m_Eateffect; }

	list<CDaypont*>*Get_Daypont_list(void) { return &m_Daypont; }
	list<CDeadmain*>*Get_Deadmain_list(void) { return &m_Deadmain; }
	list<CDeadcountpont*>*Get_Deadcountpont_list(void) { return &m_Deadcountpont; }

	list<CDaycountpont*>* Get_Daycountpont_list(void) { return &m_Daycountpont; }

	list<CMouse_Monster*>*Get_Monsterinfo_list(void) { return &m_Mousemonsterinfolist; }
	list<CLoadingscene*>*Get_Loadingscene_list(void) { return &m_Loadingscenelist; }
	list<CMonsterhp_pont*>*Get_Monsterhp_list(void) { return &m_Monsterhppontlist; }
	list<CSkill_Icon*>*Get_Skill_list(void) { return &m_Skill; }

	list<CMiniMap*>* Get_Minimap_list(void) { return &m_Minimaplist; }
	list<CMiniMap_Icon*>* Get_Minimapicon_list(void) { return &m_Minimapiconlist; }

	list<CCardgame*>* Get_Cardgame_list(void) { return &m_Cardgamelist; }

	list<CParty*>* Get_Party_list(void) { return &m_Partylist; }
	list<CPartyhp*>* Get_Partyhp_list(void) { return &m_Partyhplist; }
	list<CEndingscene*>* Get_Ending_list(void) { return &m_Ending; }

	list<CToolboxMain_back*>* Get_toolboxmainback_list(void) {return &m_toolboxmain_back;}

	

	void sethp(_uint hp) { m_ihp = hp; }
	void sethuger(_uint hunger) { m_ihunger = hunger;}
	void setmentality(_uint mental) { m_mentality = mental; }

	void Use_bag();
	void Off_bag();

	void Use_pot(CCookPot* pCookPot);
	void Off_pot(CCookPot* pCookPot);

	void weapontool_on();
	void weapontool_off();

	void gathertool_on();
	void gathertool_off();

	void constructtool_on();
	void constructtool_off();

	void materialtool_on();
	void materialtool_off();

	void Cardgame();
	void Start_Cardgame();
	

	void Dead_on();
	void Dead_off();

	void craft_on(MAKEWHAT item);
	void craft_off();

	void update_craftpont();

	void update_questpont();

	void Set_PickingPot(_bool type) { m_PickingPot = type; }
	_bool Get_PickingPot() { return m_PickingPot; }

	void Iconreset(void) { m_Minimapiconlist.clear(); }

	_uint Get_Cardgamecount() { return icardgamecount; }

	void plus_cardgamecount() {++icardgamecount;}


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
	list<CMouse_iteminfo*>m_Mouseiteminfolist;

	list<CToolboxWeapon*>m_Toolboxweapon;//
	list<CToolboxWeapon_back*>m_Toolboxweapon_back;//
	list<CToolboxWeapon_front*>m_Toolboxweapon_front;//

	list<CToolboxGather*>m_Toolboxgather;
	list<CToolboxGather_back*>m_Toolboxgather_back;
	list<CToolboxGather_front*>m_Toolboxgather_front;

	list<CToolboxConstruct*>m_Toolboxconstruct;
	list<CToolboxConstruct_back*>m_Toolboxconstruct_back;
	list<CToolboxConstruct_front*>m_Toolboxconstruct_front;

	list<CToolboxMaterial*> m_Toolboxmaterial;
	list<CToolboxMaterial_back*>    m_Toolboxmaterial_back;
	list<CToolboxMaterial_front*> m_Toolboxmaterial_front;

	
	list<CCraftmain*>m_Craftmain;
	list<CCraftmain_back*>m_Craftmainback;
	list<CCraftmain_front*>m_Craftmainfront;

	list<CCraftbutton*>	m_Craftbutton;
	list<CCraftPont*> m_Craftpont;

	list<CQuest*> m_Quest;
	list<CQuestpont*> m_Questpont;
	list<CLine*> m_Line;
	list<CTalk*>m_Talk;

	list<CPotMain*> m_Pot;
	list<CPot_back*> m_Potback;
	list<CPot_front*> m_Potfront;
	list<CPotbutton*> m_Potbutton;

	list<CEateffect*>m_Eateffect;

	list<CDaypont*> m_Daypont;
	list<CDeadmain*>m_Deadmain;
	list<CDeadcountpont*>m_Deadcountpont;

	list<CDaycountpont*>m_Daycountpont;

	list<CMouse_Monster*>m_Mousemonsterinfolist;
	list<CLoadingscene*> m_Loadingscenelist;
	list<CMonsterhp_pont*>m_Monsterhppontlist;

	list<CSkill_Icon*> m_Skill;

	list<CMiniMap*>m_Minimaplist;
	list<CMiniMap_Icon*>m_Minimapiconlist;

	list<CCardgame*> m_Cardgamelist;

	list<CParty*> m_Partylist;
	list<CPartyhp*> m_Partyhplist;
	list<CEndingscene*> m_Ending;

	list<CToolboxMain_back*> m_toolboxmain_back;


	


	//typedef list<CMainInventory_front*> INVENLIST;

	_uint m_ihp = 7;
	_uint m_ihunger = 7;
	_uint m_mentality = 7;

	_uint icardgamecount = 0;
	
	_bool m_PickingPot = false;

	_bool m_bcardgameon = true;
	_bool m_first = true;


	//private:
	//class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	//class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);


public:
	virtual void Free() override;
};