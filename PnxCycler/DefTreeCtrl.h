#pragma once


#define DEF_TREE_ITEM_HEIGHT	22
#define DEF_CHILD_GAP			25
#define DEF_LEFT_GAP			5
#define DEF_RIGHT_GAP			5
#define DEF_ICON_SIZE			16

#define MARGIN_EXTEND			2
#define SIZE_EXTEND				9
#define SIZE_EXTEND_IMAGE		16

#define DEF_TREE_SCROLL_SIZE	18

#include <list>
using namespace std;


//////////////////////////////////////////////////////////////////////////
//Tree Item ¿Ã∫•∆Æ

#define WM_TREE_LBTN_CLICK		WM_USER + 3001
#define WM_TREE_LBTN_DBCLICK	WM_USER + 3002
#define WM_TREE_RBTN_CLICK		WM_USER + 3003

//////////////////////////////////////////////////////////////////////////




struct stTreeItem
{
	stTreeItem()
	{
		pParentItem = NULL;
		bExpand		= TRUE;	
		bSelect		= FALSE;
		colorItem	= RGB(255, 255, 255);
		pData		= NULL;
		pos			= NULL;
		bDraw		= TRUE;
	}

	void	RemoveAll()
	{
		while (!listChilds.IsEmpty())
			delete listChilds.RemoveHead();
	}

	POSITION			pos;
	void				*pData;
	BOOL				bExpand;
	BOOL				bSelect;
	BOOL				bDraw;
	CString				strText;
	CString				strIconPath;
	COLORREF			colorItem;
	stTreeItem			*pParentItem;

	CList<stTreeItem*, stTreeItem*>	listChilds;
};