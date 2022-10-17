#include "stdafx.h"
#include "..\Public\Mouse.h"

IMPLEMENT_SINGLETON(CMouse)

CMouse::CMouse()
	: m_iboxindex(20), m_Itemtype(ITEM_END), m_Equipment_Name(ITEMNAME_END)
{
}



void CMouse::Clear(_uint iLevelIndex)
{
	

}


void CMouse::Free()
{


}
