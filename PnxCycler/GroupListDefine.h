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
	int			nSubItemIndex = -1;
	BOOL		bUseColor=FALSE;
	COLORREF	subItemColor;
	int			nBattery = 0;
	CString		strText="";

};


struct sItemInfo
{
	sItemInfo()
	{
		itemColor = RGB(255, 255, 255);
		bSelected = FALSE;
		bReserveCommand = false;
		bMultiSchedule = false;
	}

	int			nItemIndex;
	BOOL		bSelected;
	BOOL		bExpand;

	bool		bReserveCommand;
	bool		bMultiSchedule;

	COLORREF	itemColor;
	CPtrArray	arrSubItems;
};

struct sGroupInfo
{
	sGroupInfo()
	{
		bDrawExpand = TRUE;
	}
	int			nGroupIndex = -1;
	CString		strText;
	BOOL		bExpand = FALSE;
	BOOL		bSelected = FALSE;
	BOOL		bDrawExpand = FALSE;
	BOOL		bBattery = FALSE;
	CPtrArray	arrItems;

	int	GetSize()
	{
		return ( int )arrItems.GetCount();
	}
};