#pragma once

enum eTextFormat { TEXT_LEFT, TEXT_CENTER, TEXT_RIGHT };

#define MARGIN_EXTEND			2
#define SIZE_EXTEND				9
#define GROUP_EXPAND_TOP_GAP	19
#define GROUP_EXPAND_LEFT_GAP	12
#define DEF_SCROLL_SIZE			18

struct sColumnInfo
{
	CString		strText;
	int			nWidth;
	eTextFormat	eFormat;
};


struct sSubItemInfo
{
	sSubItemInfo()
	{
		subItemColor	= RGB(255, 255, 255);
		bUseColor		= FALSE;
	}
	int			nSubItemIndex;
	BOOL		bUseColor;
	COLORREF	subItemColor;
	CString		strText;
};


struct sItemInfo
{
	sItemInfo()
	{
		itemColor = RGB(255, 255, 255);
		bSelected = FALSE;
	}

	int			nItemIndex;
	BOOL		bSelected;
	BOOL		bExpand;
	COLORREF	itemColor;
	CPtrArray	arrSubItems;
};

struct sGroupInfo
{
	sGroupInfo()
	{
		bDrawExpand = TRUE;
	}
	int			nGroupIndex;
	CString		strText;
	BOOL		bExpand;
	BOOL		bSelected;
	BOOL		bDrawExpand;
	CPtrArray	arrItems;

	int	GetSize()
	{
		return ( int )arrItems.GetCount();
	}
};