// GroupListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GroupListCtrl.h"

#pragma warning(disable:4100)

LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam);
// CGroupListCtrl

IMPLEMENT_DYNAMIC(CGroupListCtrl, CWnd)

CGroupListCtrl::CGroupListCtrl()
{
	//gdi+ 설정
	GdiplusStartupInput gdiplus;

	GdiplusStartup(&m_gpToken, &gdiplus, NULL);

	//m_colorGroupLine		= RGB(100, 100, 100);
	m_colorGroupLine		= RGB( 79, 93, 115 );
	m_colorBKLine			= RGB(150, 150, 150);
	m_colorColumn			= RGB(180, 180, 180);
	m_colorColumnSelect		= RGB(240, 240, 240);
	m_colorItemLine			= RGB(160, 160, 160);	// 표 테두리
	m_colorSelectGroup		= RGB(230, 230, 230);
	//m_colorSelectItem		= Color(150, 150, 150, 100);	// 이놈 바꿔야 색상 바뀐다.
	m_colorSelectItem		= Color(100, 220, 220, 220);

	m_bColumnGradation		= FALSE;
	m_nColumnHeight			= 25;
	m_nColumnFontSize		= 15;
	m_bColumnTracking		= FALSE;
	m_nColumnTrackingIndex	= -1;
	m_nExpandGroupIndex		= -1;
	m_nSelectColumn			= -1;
	m_nMinColumnWidth		= 10;
	m_nGroupHeight			= 35; // m_nGroupHeight			= 35; //
	m_nItemHeight			= 25;
	//m_nGroupFontSize		= 14;
	m_nGroupFontSize		= 12;
	m_bItemLine				= TRUE;
	m_bSubItemLine			= TRUE;
	m_eItemFormat			= TEXT_CENTER;
	m_pVerticalScroll		= NULL;
	m_pHorizontalScroll		= NULL;
	m_pEditCtrl				= NULL;
	m_nCurEditGroupIndex	= -1;
	m_nCurEditItemIndex		= -1;
	m_nCurEditSubItemIndex	= -1;
	m_nScrollBarId			= -1;
	m_strFontName			= "Arial";
	m_bMultiLine			= FALSE;
	m_bClickCtrl			= FALSE;
	m_bClickShift			= FALSE;
	m_nStartIndex			= 0;
	m_nEndIndex				= 0;
	m_nUNQID				= 0;

	m_colorGroupLinePen.CreatePen(PS_SOLID, 2, m_colorGroupLine);
	m_colorItemLinePen.CreatePen(PS_SOLID, 1, m_colorItemLine);
	m_ColumnPen.CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
	m_colorBKLinePen.CreatePen(PS_SOLID, 1, m_colorBKLine);
	m_Font.CreateFont(m_nGroupFontSize, 0, 0, 0, FW_BOLD, 0, 0, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, m_strFontName);
}

CGroupListCtrl::~CGroupListCtrl()
{
	RemoveAllColumn();
	RemoveAllItems();
	RemoveScroll();
	RemoveEditCtrl();

	GdiplusShutdown(m_gpToken);
	m_colorGroupLinePen.DeleteObject();
	m_colorItemLinePen.DeleteObject();
	m_ColumnPen.DeleteObject();
	m_colorBKLinePen.DeleteObject();
	m_Font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CGroupListCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void CGroupListCtrl::RemoveEditCtrl()
{
	if (m_pEditCtrl != NULL)
	{
		delete m_pEditCtrl;
		m_pEditCtrl = NULL;
	}
}

void CGroupListCtrl::RemoveScroll()
{
	if (m_pVerticalScroll != NULL)
	{
		delete m_pVerticalScroll;
		m_pVerticalScroll = NULL;
	}

	if (m_pHorizontalScroll != NULL)
	{
		delete m_pHorizontalScroll;
		m_pHorizontalScroll = NULL;
	}
}

void CGroupListCtrl::RemoveAllItems()
{
	for ( int i = 0 ; i < m_arrGroupInfo.GetCount() ; ++i )
	{
		sGroupInfo* pGroupInfo = ( sGroupInfo* )m_arrGroupInfo.GetAt( i );

		if ( pGroupInfo )
		{
			for ( int j = 0 ; j < pGroupInfo->GetSize() ; ++j )
			{
				sItemInfo* pItemInfo = ( sItemInfo* )pGroupInfo->arrItems.GetAt( j );

				for ( int k = 0 ; k < pItemInfo->arrSubItems.GetSize() ; ++k )
				{
					sSubItemInfo* pSubItemInfo = ( sSubItemInfo* )pItemInfo->arrSubItems.GetAt( k );

					if ( pSubItemInfo )
					{
						delete pSubItemInfo;

						pSubItemInfo = NULL;
					}
				}

				pItemInfo->arrSubItems.RemoveAll();

				if (pItemInfo != NULL)
				{
					delete pItemInfo;

					pItemInfo = NULL;
				}
			}

			pGroupInfo->arrItems.RemoveAll();

			delete pGroupInfo;

			pGroupInfo = NULL;
		}
	}

	m_arrGroupInfo.RemoveAll();

	if ( GetSafeHwnd() )
	{
		InvalidateNoScroll();
	}
}

void CGroupListCtrl::RemoveAllColumn()
{
	sColumnInfo *pInfo = NULL;

	for (int i = 0; i < m_arrColumnInfo.GetCount(); i++)
	{
		pInfo = (sColumnInfo *)m_arrColumnInfo.GetAt(i);

		if (pInfo != NULL)
		{
			delete pInfo;
			pInfo = NULL;
		}
	}
	
	m_arrColumnInfo.RemoveAll();

	if (GetSafeHwnd())
		InvalidateNoScroll();
}


BOOL CGroupListCtrl::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CGroupListCtrl::OnPaint()
{
	CPaintDC	dc(this); // device context for painting
	
	CDC			memDC;
	CBitmap		bitmap;
	CRect		rcClient;
	HGDIOBJ		gdiObj;
	int			nBkMode;
	
	GetClientRect(&rcClient);

	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.right, rcClient.bottom);
	
	gdiObj = (HGDIOBJ)memDC.SelectObject(bitmap);

	nBkMode = memDC.SetBkMode(TRANSPARENT);

	memDC.FillSolidRect(rcClient, RGB(255, 255, 255));
	
	DrawItems(&memDC);

	DrawColumn(&memDC);

	DrawBkLine(&memDC);
	
	SettingScrollBar();

	dc.BitBlt(0, 0, rcClient.right, rcClient.bottom, &memDC, 0, 0, SRCCOPY);

	memDC.SetBkMode(nBkMode);
	memDC.SelectObject(gdiObj);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}

void CGroupListCtrl::DrawItems(CDC *pDC)
{
	sGroupInfo   *pGroupInfo = NULL;
	CRect      rcGroup;
	CRect      rcClient;
//	CPen      pen, *pOldPen = NULL;
//	CFont      font, *pOldFont   = NULL;
//
//	font.CreateFont(m_nGroupFontSize, 0, 0, 0, FW_BOLD, 0, 0, 0,
//		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, /*_T("Segoe UI") */ m_strFontName );
//
//	pen.CreatePen(PS_SOLID, 2, m_colorGroupLine);
//
//	pOldFont=pDC->SelectObject(&font);
	pDC->SelectObject(&m_Font);

	GetClientRect(&rcClient);

	for (int nGroupIndex = 0; nGroupIndex < m_arrGroupInfo.GetCount(); nGroupIndex++)
	{
		pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

		if (pGroupInfo != NULL)
		{
			GetGroupRect(nGroupIndex, rcGroup);

			if (rcGroup.top > rcClient.bottom)
				continue;  

			if (pGroupInfo->bExpand == TRUE || m_nExpandGroupIndex == nGroupIndex)
				DrawItems(nGroupIndex, pDC, pGroupInfo, rcGroup);

			if (pGroupInfo->bSelected == TRUE)
				pDC->FillSolidRect(CRect(rcGroup.left, rcGroup.top + 1, rcGroup.right, rcGroup.bottom), m_colorSelectGroup);

			pDC->SelectObject(&m_colorGroupLinePen);
//			pDC->SelectObject(&pen);

			pDC->MoveTo(rcGroup.left, rcGroup.bottom);
			pDC->LineTo(rcGroup.right, rcGroup.bottom);

//			pDC->SelectObject(pOldPen);

			if (pGroupInfo->bDrawExpand == TRUE)
			{
				DrawExpand(pDC, CRect(rcGroup.left + GROUP_EXPAND_LEFT_GAP, rcGroup.top + GROUP_EXPAND_TOP_GAP, rcGroup.right, rcGroup.bottom), 
					pGroupInfo->bExpand);
			}

			if (m_nGroupHeight == 0)
			{
				pDC->DrawText(pGroupInfo->strText, CRect(rcGroup.left + 30, rcGroup.bottom, rcGroup.right - 30, rcGroup.bottom + m_nItemHeight), 
					DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			}
			else
			{
				pDC->DrawText(pGroupInfo->strText, CRect(rcGroup.left + 30, rcGroup.bottom - 25, rcGroup.right - 30, rcGroup.bottom), 
					DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			}
		}
	}


// 	pDC->SelectObject(pOldFont);
// 	font.DeleteObject();
// 	pen.DeleteObject();
}

void CGroupListCtrl::GetGroupRect(int nGroupIndex, CRect &rect)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return;

	sGroupInfo	*pGroupInfo		= NULL;
	sItemInfo   *pItemInfo		= NULL;
	int			nTop			= m_nColumnHeight;
	int			nExpandCnt		= 0;
	int			nVerticalPos	= 0;
	int			nHorizontalPos	= 0;

	CRect		rcClient;
	

	if (m_pVerticalScroll != NULL)
		nVerticalPos = GetScrollPos(m_pVerticalScroll);

	if (m_pHorizontalScroll != NULL)
		nHorizontalPos = GetScrollPos(m_pHorizontalScroll);

	GetClientRect(&rcClient);

	for (int i = 0; i < nGroupIndex; i++)
	{
		pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(i);
		
		if (pGroupInfo != NULL)
		{
			if (pGroupInfo->bExpand == TRUE && m_nExpandGroupIndex != i)
			{
				nTop += (pGroupInfo->GetSize() * m_nItemHeight);
			}
			else if (m_nExpandGroupIndex == i)
			{
				for (int nItemIndex=0; nItemIndex<pGroupInfo->arrItems.GetCount(); nItemIndex++)
				{
					pItemInfo = (sItemInfo *)pGroupInfo->arrItems.GetAt(nItemIndex);

					if (pItemInfo != NULL)
					{
						if (pItemInfo->bExpand == TRUE)
							nExpandCnt++;
					}
				}

				nTop += (nExpandCnt * m_nItemHeight);
			}
		}
	}
	

	rect.SetRect(-nHorizontalPos, nTop + (m_nGroupHeight * nGroupIndex) - nVerticalPos, 
		rcClient.right - nHorizontalPos, nTop + (m_nGroupHeight * nGroupIndex) + m_nGroupHeight - nVerticalPos);
}

void CGroupListCtrl::DrawItems(int nGroupIndex, CDC *pDC, sGroupInfo *pGroupInfo, CRect rcGroup)
{
	sItemInfo		*pItemInfo		= NULL;
	sSubItemInfo	*pSubItemInfo	= NULL;
//	CPen		pen, *pOldPen		= NULL;

	CRect		rcItem;
	CRect		rcColumn;
	CRect		rcClient;
	int			nStartIndex			= 0;
	Graphics	grs(pDC->GetSafeHdc());

// 	pen.CreatePen(PS_SOLID, 1, m_colorItemLine);
// 
// 	pDC->SelectObject(&pen);

	GetClientRect(&rcClient);

	if (pGroupInfo != NULL)
	{
		if (rcGroup.bottom < 0)
			nStartIndex = ((rcGroup.bottom * -1) / m_nItemHeight);

		for (int nItemIndex=nStartIndex; nItemIndex<min(pGroupInfo->arrItems.GetCount(), nStartIndex + rcClient.bottom/m_nItemHeight) + 1; nItemIndex++)
		{
			if ( nItemIndex >= pGroupInfo->arrItems.GetSize() )
				continue;

			pItemInfo = (sItemInfo *)pGroupInfo->arrItems.GetAt(nItemIndex);

			if (pItemInfo != NULL)
			{
				if (m_nExpandGroupIndex != -1 && pItemInfo->bExpand == FALSE)
					continue;

				GetItemRect(nGroupIndex, nItemIndex, rcItem);
					
				if (rcItem.top > rcClient.bottom || rcItem.bottom < rcClient.top) //TODO. ween8 scroll 위치
					continue; 

				pDC->FillSolidRect(CRect(rcItem.left, rcItem.top + 1, rcItem.right, rcItem.bottom), pItemInfo->itemColor);

				if (m_bItemLine == TRUE)
				{
					pDC->MoveTo(0, rcItem.bottom);
					pDC->LineTo(rcItem.right, rcItem.bottom);
				}

				for (int i=0; i<m_arrColumnInfo.GetCount(); i++)
				{
					GetColumnRect(i, rcColumn);

					if (i >= pItemInfo->arrSubItems.GetCount())
						continue;

					pSubItemInfo = (sSubItemInfo *)pItemInfo->arrSubItems.GetAt(i);

					if (pSubItemInfo != NULL)
					{
						if (pSubItemInfo->bUseColor == TRUE)
							pDC->FillSolidRect(CRect(rcColumn.left + 1, rcItem.top + 1, rcColumn.right, rcItem.bottom), pSubItemInfo->subItemColor);

						if ( TRUE == m_bMultiLine && pSubItemInfo->strText.Find( _T( "\n" ) ) >= 0 )
						{
							pDC->DrawText(	pSubItemInfo->strText, CRect(rcColumn.left + 3, rcItem.top, rcColumn.right-3, rcItem.bottom), 
											GetTextFormat(m_eItemFormat) | DT_CENTER | DT_WORDBREAK  );
						}
						else
						{
							pDC->DrawText(	pSubItemInfo->strText, CRect(rcColumn.left + 3, rcItem.top, rcColumn.right-3, rcItem.bottom), 
											GetTextFormat(m_eItemFormat) | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS );
						}
					}

					if (m_bSubItemLine == TRUE)
					{
						pDC->MoveTo(rcColumn.right, rcItem.top		+ 5);
						pDC->LineTo(rcColumn.right, rcItem.bottom	- 5);
					}
				}

				if (pItemInfo->bSelected == TRUE)
				{
					SolidBrush      brushDeep(m_colorSelectItem);
					grs.FillRectangle(&brushDeep, rcItem.left, rcItem.top + 1, rcItem.right - rcItem.left, rcItem.bottom - (rcItem.top + 1));
				}
				//pDC->FillSolidRect(CRect(rcItem.left, rcItem.top + 1, rcItem.right, rcItem.bottom), m_colorSelectItem);

				

			}
		}
	}

//	pDC->SelectObject(pOldPen);
}

void CGroupListCtrl::SetSubItemColor(int nGroupIndex, int nItemIndex, int nSubItemIndex, COLORREF subItemColor, BOOL bUseColor)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return;

	sGroupInfo		*pGroupInfo		= NULL;
	sItemInfo		*pItemInfo		= NULL;
	sSubItemInfo	*pSubItemInfo	= NULL;

	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo != NULL)
	{
		if (nItemIndex < 0 || nItemIndex >= pGroupInfo->arrItems.GetCount())
			return;

		pItemInfo = (sItemInfo *)pGroupInfo->arrItems.GetAt(nItemIndex);

		if (pItemInfo != NULL && nSubItemIndex >= 0 && pItemInfo->arrSubItems.GetCount() > nSubItemIndex)
		{
			pSubItemInfo = (sSubItemInfo *)pItemInfo->arrSubItems.GetAt(nSubItemIndex);

			if (pSubItemInfo != NULL)
			{
				pSubItemInfo->subItemColor = subItemColor;
				pSubItemInfo->bUseColor = bUseColor;
			}
		}
	}

	InvalidateNoScroll();
}
void CGroupListCtrl::SetItemColor(int nGroupIndex, int nItemIndex, COLORREF itemColor)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return;

	sGroupInfo		*pGroupInfo		= NULL;
	sItemInfo		*pItemInfo		= NULL;
	
	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo != NULL)
	{
		if (nItemIndex < 0 || nItemIndex >= pGroupInfo->arrItems.GetCount())
			return;

		pItemInfo = (sItemInfo *)pGroupInfo->arrItems.GetAt(nItemIndex);

		if (pItemInfo != NULL)
		{
			pItemInfo->itemColor = itemColor;
		}
	}

	InvalidateNoScroll();
}
CString	CGroupListCtrl::GetItemText(int nGroupIndex, int nItemIndex, int nSubItemIndex)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return _T("");

	if (nSubItemIndex < 0 || nSubItemIndex >= m_arrColumnInfo.GetCount())
		return _T("");

	sGroupInfo		*pGroupInfo		= NULL;
	sItemInfo		*pItemInfo		= NULL;
	sSubItemInfo	*pSubItemInfo	= NULL;

	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (nItemIndex < 0 || nItemIndex >= pGroupInfo->arrItems.GetCount())
		return _T("");

	pItemInfo = (sItemInfo *)pGroupInfo->arrItems.GetAt(nItemIndex);
	
	if (pItemInfo == NULL)
		return _T("");

	if (pItemInfo->arrSubItems.GetCount() <= nSubItemIndex)
		return _T("");

	pSubItemInfo = (sSubItemInfo *)pItemInfo->arrSubItems.GetAt(nSubItemIndex);

	if (pSubItemInfo != NULL)
		return pSubItemInfo->strText;

	return _T("");
}
void CGroupListCtrl::SetItemText(int nGroupIndex, int nItemIndex, int nSubItemIndex, CString strText)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return;

	if (nSubItemIndex < 0 || nSubItemIndex >= m_arrColumnInfo.GetCount())
		return;

	sGroupInfo		*pGroupInfo		= NULL;
	sItemInfo		*pItemInfo		= NULL;
	sSubItemInfo	*pSubItemInfo	= NULL;
	BOOL			bAlreadyData	= FALSE;

	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo != NULL)
	{
		if (nItemIndex < 0 || nItemIndex >= pGroupInfo->arrItems.GetCount())
			return;

		pItemInfo = (sItemInfo *)pGroupInfo->arrItems.GetAt(nItemIndex);

		if (pItemInfo != NULL)
		{
			for (int i=0; i<pItemInfo->arrSubItems.GetCount(); i++)
			{
				pSubItemInfo = (sSubItemInfo *)pItemInfo->arrSubItems.GetAt(i);

				if (pSubItemInfo != NULL)
				{
					if (pSubItemInfo->nSubItemIndex == nSubItemIndex)
					{
						pSubItemInfo->strText = strText;
						bAlreadyData = TRUE;
						break;
					}
				}
			}
		
			//ween8 서브아이템이 두개이상 빈경우 수정
			if (bAlreadyData == FALSE)
			{
				pSubItemInfo = new sSubItemInfo;
				
				pSubItemInfo->nSubItemIndex = nSubItemIndex;
				pSubItemInfo->strText		= strText;

				pItemInfo->arrSubItems.Add(pSubItemInfo);
			}
		}
	}

	InvalidateNoScroll();
}

void	CGroupListCtrl::SetItemSelected(int nGroupIndex, int nItemIndex)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return;

	sGroupInfo		*pGroupInfo = NULL;
	sItemInfo		*pItemInfo = NULL;

	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo != NULL)
	{
		if (nItemIndex < 0 || nItemIndex >= pGroupInfo->arrItems.GetCount())
			return;

		pItemInfo = (sItemInfo *)pGroupInfo->arrItems.GetAt(nItemIndex);

		if (pItemInfo != NULL)
		{
			pItemInfo->bSelected = TRUE;
		}
	}

	InvalidateNoScroll();
}

void CGroupListCtrl::RedrawNoScroll()
{
	CRect	rect;
	int		nVerGap = 0;
	int		nHorGap = 0;

	GetClientRect(&rect);

	if (m_pVerticalScroll != NULL && m_pVerticalScroll->GetSafeHwnd())
	{
		if (m_pVerticalScroll->IsWindowVisible())
			nVerGap = DEF_SCROLL_SIZE;
	}

	if (m_pHorizontalScroll != NULL && m_pHorizontalScroll->GetSafeHwnd())
	{
		if (m_pHorizontalScroll->IsWindowVisible())
			nHorGap = DEF_SCROLL_SIZE;
	}

	RedrawWindow(CRect(rect.left, rect.top, rect.right - nVerGap, rect.bottom - nHorGap), NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
}

void CGroupListCtrl::InvalidateNoScroll()
{
	CRect	rect;
	int		nVerGap = 0;
	int		nHorGap = 0;

	GetClientRect(&rect);

	if ( m_pVerticalScroll->GetSafeHwnd() && m_pVerticalScroll != NULL )
	{
		if (m_pVerticalScroll->IsWindowVisible())
			nVerGap = DEF_SCROLL_SIZE;
	}

	if (m_pHorizontalScroll != NULL)
	{
		if ( m_pHorizontalScroll->GetSafeHwnd() && m_pHorizontalScroll->IsWindowVisible() )
			nHorGap = DEF_SCROLL_SIZE;
	}
	InvalidateRect(CRect(rect.left, rect.top, rect.right - nVerGap, rect.bottom - nHorGap));
}

void CGroupListCtrl::GetColumnRect(int nColumnIndex, CRect &rect)
{
	if (nColumnIndex < 0 || nColumnIndex >= m_arrColumnInfo.GetCount())
		return;

	sColumnInfo		*pColumnInfo	= NULL;
	int				nLeft			= 0;
	int				nVerticalPos	= 0;
	int				nHorizontalPos	= 0;

	if (m_pVerticalScroll != NULL)
		nVerticalPos = GetScrollPos(m_pVerticalScroll);

	if (m_pHorizontalScroll != NULL)
		nHorizontalPos = GetScrollPos(m_pHorizontalScroll);

	for (int i=0; i<m_arrColumnInfo.GetCount(); i++)
	{
		pColumnInfo = (sColumnInfo *)m_arrColumnInfo.GetAt(i);
		
		if (pColumnInfo != NULL)
		{
			if (i == nColumnIndex)
				break;

			nLeft += pColumnInfo->nWidth;
		}
	}
	
	pColumnInfo = (sColumnInfo *)m_arrColumnInfo.GetAt(nColumnIndex);
	rect.SetRect(nLeft - nHorizontalPos, 0, nLeft + pColumnInfo->nWidth - nHorizontalPos, m_nColumnHeight);
}

void CGroupListCtrl::GetItemRect(int nGroupIndex, int nItemIndex, CRect &rect)
{
	CRect rectGroup;
	GetGroupRect(nGroupIndex, rectGroup);
	
	rect.SetRect(rectGroup.left, rectGroup.bottom + (nItemIndex * m_nItemHeight), rectGroup.right, rectGroup.bottom + (nItemIndex * m_nItemHeight) + m_nItemHeight);
}

void CGroupListCtrl::GetSubItemRect(int nGroupIndex, int nItemIndex, int nSubItemRect, CRect &rect)
{
	CRect rcItem;
	CRect rcColumn;

	GetItemRect(nGroupIndex, nItemIndex, rcItem);
	GetColumnRect(nSubItemRect, rcColumn);

	rect.SetRect(rcColumn.left, rcItem.top, rcColumn.right, rcItem.bottom);
}

void CGroupListCtrl::DrawColumn(CDC* pDC)
{
	CRect		rcClient;
	sColumnInfo	*pInfo			= NULL;
// 	CFont		font, *pOldFont	= NULL;
// 	CPen		pen, *pOldPen   = NULL;
	CRect		rectColumn;

	GetClientRect(&rcClient);

// 	font.CreateFont(m_nColumnFontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 
// 		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, _T("Arial"));    
// 
// 	pen.CreatePen(PS_SOLID, 1, RGB(100, 100, 100));

	if (m_bColumnGradation == TRUE)
	{
		DrawGradation(pDC, RGB(240, 240, 240), m_colorColumn, CRect(0, 0, rcClient.right, m_nColumnHeight), m_nColumnHeight);

		if (m_nSelectColumn != -1)
		{
			GetColumnRect(m_nSelectColumn, rectColumn);

			DrawGradation(pDC, m_colorColumn, RGB(240, 240, 240), CRect(rectColumn.left, 0, rectColumn.right, m_nColumnHeight), m_nColumnHeight);
		}
	}
	else
	{
		pDC->FillSolidRect(0, 0, rcClient.right, m_nColumnHeight, m_colorColumn);

		if (m_nSelectColumn != -1)
		{
			GetColumnRect(m_nSelectColumn, rectColumn);

			pDC->FillSolidRect(rectColumn.left, 0, rectColumn.right-rectColumn.left, rectColumn.bottom, m_colorColumnSelect);
		}
	}

// 	pOldFont = pDC->SelectObject(&font);
// 	pOldPen = pDC->SelectObject(&pen);

	for (int i = 0; i < m_arrColumnInfo.GetCount(); i++)
	{
		pInfo = (sColumnInfo *)m_arrColumnInfo.GetAt(i);

		if (pInfo != NULL)
		{
			GetColumnRect(i, rectColumn);

			pDC->DrawText(pInfo->strText, CRect(rectColumn.left + 5, (m_nColumnHeight - m_nColumnFontSize)/2, 
				rectColumn.left + pInfo->nWidth - 5, (m_nColumnHeight - m_nColumnFontSize)/2 + m_nColumnFontSize + 1), 
				GetTextFormat(pInfo->eFormat) | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

			
			pDC->MoveTo(rectColumn.right, 4);
			pDC->LineTo(rectColumn.right, m_nColumnHeight - 4);
		}
	}
	
	pDC->MoveTo(0, m_nColumnHeight);
	pDC->LineTo(rcClient.right, m_nColumnHeight);

// 	pDC->SelectObject(pOldFont);
// 	pDC->SelectObject(pOldPen);
}

UINT CGroupListCtrl::GetTextFormat(eTextFormat eFormat)
{
	switch (eFormat)
	{
	case TEXT_LEFT:
		return DT_LEFT;
		break;

	case TEXT_CENTER: 
		return DT_CENTER;
		break;

	case TEXT_RIGHT:
		return DT_RIGHT;
		break;
	}

	return DT_LEFT;
}

//column gradation 설정
void CGroupListCtrl::SetColumnGradation(BOOL bGradation)
{
	m_bColumnGradation = bGradation;
}

//back ground line draw
void CGroupListCtrl::DrawBkLine(CDC *pDC)
{
//	CPen	pen, *pOldPen = NULL;
	CRect	rcClient;

// 	pen.CreatePen(PS_SOLID, 1, m_colorBKLine);
// 
// 	pOldPen = pDC->SelectObject(&pen);
	
	GetClientRect(&rcClient);
	
	pDC->MoveTo(0, 0);
	pDC->LineTo(rcClient.right-1, 0);
	pDC->LineTo(rcClient.right-1, rcClient.bottom-1);
	pDC->LineTo(0, rcClient.bottom-1);
	pDC->LineTo(0, 0);
	
//	pDC->SelectObject(pOldPen);
}

void CGroupListCtrl::InsertColumn(sColumnInfo info)
{
	sColumnInfo *pInfo = NULL;
	
	pInfo = new sColumnInfo;

	pInfo->eFormat	= info.eFormat;
	pInfo->nWidth	= info.nWidth;
	pInfo->strText	= info.strText;

	m_arrColumnInfo.Add(pInfo);
}

void CGroupListCtrl::InsertColumn(CString strText, int nWidth, eTextFormat eFormat)
{
	sColumnInfo *pInfo = NULL;

	pInfo = new sColumnInfo;

	pInfo->eFormat	= eFormat;
	pInfo->nWidth	= nWidth;
	pInfo->strText	= strText;

	m_arrColumnInfo.Add(pInfo);
}

void CGroupListCtrl::SetBkLineColor(COLORREF colorBKLine)
{
	m_colorBKLine = colorBKLine;
}

void CGroupListCtrl::SetColumnHeight(UINT nHeight)
{
	m_nColumnHeight = nHeight;
}


/*/##############################################################################
### \brief		 영역  gradation 처리
### \param		 CDC * pDC			:	출력할 DC
### \param		 COLORREF rgbBegin	:	시작 color
### \param		 COLORREF rgbEnd	:	종료 color
### \param		 CRect rect			:	그려질 영역
### \param		 int nPos			:	그려질 영역 height
### \return		 void				:	
#################################################################################*/
void CGroupListCtrl::DrawGradation(CDC * pDC, COLORREF rgbBegin, COLORREF rgbEnd, CRect rect, int nPos)
{
	COLORREF rgbReturn = 0;
	int byRBegin = GetRValue(rgbBegin);
	int byGBegin = GetGValue(rgbBegin);
	int byBBegin = GetBValue(rgbBegin);

	int byR = GetRValue(rgbBegin);
	int byG = GetGValue(rgbBegin);
	int byB = GetBValue(rgbBegin);

	int byR_diff = 0;
	int byG_diff = 0;
	int byB_diff = 0;

	byR_diff = byRBegin - GetRValue(rgbEnd);
	byG_diff = byGBegin - GetGValue(rgbEnd);
	byB_diff = byBBegin - GetBValue(rgbEnd);

	CPen* pNewPen = NULL;
	CPen* pOldPen = NULL;

	int byOffset = 0;
	int nPercent = 0;
	for(int i = 0; i < nPos; ++i)
	{
		pNewPen = new CPen(PS_SOLID, 1, RGB(byR, byG, byB));
		rgbReturn = RGB(byR, byG, byB);
		pOldPen = pDC->SelectObject(pNewPen);

		pDC->MoveTo(rect.left, rect.top+i);

		pDC->LineTo(rect.right, rect.top+i);

		nPercent = i * 100 / nPos;

		byOffset = byR_diff * nPercent / 100;
		byR = byRBegin - byOffset;
		byOffset = byG_diff * nPercent / 100;
		byG = byGBegin - byOffset;
		byOffset = byB_diff * nPercent / 100;
		byB = byBBegin - byOffset;

		pDC->SelectObject(pOldPen);
		delete pNewPen;
	}
}

void CGroupListCtrl::CheckSelectColumn(CPoint point)
{
	CRect rectColumn;

	for (int i=0; i<m_arrColumnInfo.GetCount(); i++)
	{
		GetColumnRect(i, rectColumn);

		if (rectColumn.PtInRect(point) == TRUE)
		{
			m_nSelectColumn = i;
			break;
		}
	}
}

void CGroupListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nExpandGroupIndex != -1)
		return;

	SetCapture();
	
	if (IsPtInResizeRect(point, &m_nColumnTrackingIndex) == TRUE)
	{
		SetCursor(LoadCursor(0, IDC_SIZEWE));
		m_ptClick			= point;
		m_bColumnTracking	= TRUE;
	}


	sGroupInfo *pGroupInfo = NULL;
	sItemInfo  *pItemInfo  = NULL;

	CRect		rcGroup;
	CRect		rcItem;
	CRect		rcGroupExpand;
	CRect		rcClient;
	int			nStartIndex = 0;
		
	GetClientRect(&rcClient);

	m_bClickShift = ((GetKeyState(VK_SHIFT) & 0x8000) != 0);
	m_bClickCtrl = ((GetKeyState(VK_CONTROL) & 0x8000) != 0);

	for (int nGroupIndex=0; nGroupIndex<m_arrGroupInfo.GetCount(); nGroupIndex++)
	{
		pGroupInfo	= (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);
		nStartIndex	= 0;

		if (pGroupInfo != NULL)
		{
			pGroupInfo->bSelected = FALSE;
			GetGroupRect(pGroupInfo->nGroupIndex, rcGroup);

			//group 의 expand 선택시
			if (rcGroup.PtInRect(point) == TRUE)
			{
				pGroupInfo->bSelected = TRUE;				
			}
			
			rcGroupExpand.SetRect(rcGroup.left + GROUP_EXPAND_LEFT_GAP, rcGroup.top + GROUP_EXPAND_TOP_GAP, 
				rcGroup.left + GROUP_EXPAND_LEFT_GAP + SIZE_EXTEND, rcGroup.top + GROUP_EXPAND_TOP_GAP + SIZE_EXTEND);
			
			//group 의 expand 클릭시
			if (rcGroupExpand.PtInRect(point) == TRUE)
			{
				SetCursor(LoadCursor(0, IDC_WAIT));
				if (pGroupInfo->arrItems.GetCount() < 20)
				{
					m_nExpandGroupIndex = pGroupInfo->nGroupIndex;
					
					for (int nItemIndex = pGroupInfo->GetSize() -1 ; nItemIndex>=0; nItemIndex--)
					{
						pItemInfo = (sItemInfo  *)pGroupInfo->arrItems.GetAt(nItemIndex);

						if (pItemInfo != NULL)
							pItemInfo->bExpand = pGroupInfo->bExpand;
					}
				}
			}
		
			if (rcGroup.bottom < 0)
				nStartIndex = ((rcGroup.bottom * -1) / m_nItemHeight);
		
			if (pGroupInfo->bExpand == TRUE)
			{
				//for (int nItemIndex=pGroupInfo->arrItems.GetCount()-1; nItemIndex>=0; nItemIndex--)
				for (int nItemIndex=min(pGroupInfo->GetSize()-1, nStartIndex + rcClient.bottom/m_nItemHeight); nItemIndex>=nStartIndex; nItemIndex--)
				{
					pItemInfo = (sItemInfo  *)pGroupInfo->arrItems.GetAt(nItemIndex);

					if (pItemInfo != NULL)
					{					
						// JSK
						if ( !m_bClickCtrl && !m_bClickShift)
							pItemInfo->bSelected = FALSE;

						GetItemRect(pGroupInfo->nGroupIndex, nItemIndex, rcItem);

						if (rcItem.PtInRect(point) == TRUE)
						{
							// JSK
							if (!m_bClickCtrl && !m_bClickShift)
							{
								pItemInfo->bSelected = TRUE;

								m_nStartIndex = nItemIndex;

								CString strSelectedIndex;

								strSelectedIndex.Format("%d", nItemIndex);

								CWnd* pWnd = GetParent();

								//그냥 클릭
								// 								if (pWnd)
								// 									::SendMessage(pWnd->GetSafeHwnd(), WM_GROUP_SELECT, (WPARAM)strSelectedIndex.operator LPCTSTR(), m_nScrollBarId);
								if (pWnd)
									::SendMessage(pWnd->GetSafeHwnd(),
									WM_GROUP_SELECT,
									(WPARAM)strSelectedIndex.operator LPCTSTR(),
									GetUNQID());
							}
							else if (m_bClickCtrl)
							{
								pItemInfo->bSelected = !pItemInfo->bSelected;
							}
							else if (m_bClickShift)
							{
								m_nEndIndex = nItemIndex;								
							}
						}

						if (m_bClickShift)
						{
							if (nItemIndex >= m_nStartIndex && nItemIndex <= m_nEndIndex)
							{
								pItemInfo->bSelected = TRUE;
							}
						}
					}

					if (m_nExpandGroupIndex != -1)
					{
						pItemInfo->bExpand = !pItemInfo->bExpand;
						RedrawNoScroll();
						
						if (pGroupInfo->arrItems.GetCount() >= 10 && pGroupInfo->arrItems.GetCount() < 20)
							Sleep(40);
						else if (pGroupInfo->arrItems.GetCount() >= 0 && pGroupInfo->arrItems.GetCount() < 10)
							Sleep(40);
					}
				}
			}
			else
			{
				//for (int nItemIndex=0; nItemIndex<pGroupInfo->arrItems.GetCount(); nItemIndex++)
				for (int nItemIndex=nStartIndex; nItemIndex<min(pGroupInfo->arrItems.GetCount(), nStartIndex + rcClient.bottom/m_nItemHeight); nItemIndex++)
				{
					pItemInfo = (sItemInfo  *)pGroupInfo->arrItems.GetAt(nItemIndex);

					if (pItemInfo != NULL)
					{
						pItemInfo->bSelected = FALSE;
						GetItemRect(pGroupInfo->nGroupIndex, nItemIndex, rcItem);

						if (rcItem.PtInRect(point) == TRUE)
							pItemInfo->bSelected = TRUE;
					}

					if (m_nExpandGroupIndex != -1)
					{
						pItemInfo->bExpand = !pItemInfo->bExpand;
						RedrawNoScroll();
						
						if (pGroupInfo->arrItems.GetCount() >= 10 && pGroupInfo->arrItems.GetCount() < 20)
							Sleep(20);
						else if (pGroupInfo->arrItems.GetCount() >= 0 && pGroupInfo->arrItems.GetCount() < 10)
							Sleep(30);

					}
				}
			}
			
			if (rcGroupExpand.PtInRect(point) == TRUE)
			{
				pGroupInfo->bExpand = !pGroupInfo->bExpand;
				SetCursor(LoadCursor(0, IDC_ARROW));
			}

			m_nExpandGroupIndex = -1;
		}
	}

	if (m_bClickShift || m_bClickCtrl)
	{
		CList< int, int > listSelected;

		for (int nGroupIndex = 0; nGroupIndex < m_arrGroupInfo.GetCount(); nGroupIndex++)
		{
			pGroupInfo = ( sGroupInfo* )m_arrGroupInfo.GetAt( nGroupIndex );

			for (int nItemIndex = min(pGroupInfo->GetSize() - 1, nStartIndex + rcClient.bottom / m_nItemHeight); nItemIndex >= nStartIndex; nItemIndex--)
			{
				pItemInfo = ( sItemInfo* )pGroupInfo->arrItems.GetAt(nItemIndex);

				if (pItemInfo->bSelected )
					listSelected.AddTail(nItemIndex);
			}
		}

		CString strSelectedIndex;

		auto Pos = listSelected.GetTailPosition();

		while (Pos)
		{
			CString str;

			str.Format("%d", listSelected.GetPrev(Pos));

			strSelectedIndex += str;
			strSelectedIndex += ",";
		}

		CWnd* pWnd = GetParent();

		// 		if (pWnd)
		// 			::SendMessage(pWnd->GetSafeHwnd(), WM_GROUP_SELECT, (WPARAM)strSelectedIndex.operator LPCTSTR(), 0);

		//Ctrl+클릭(선택해제 가능) or Shfit+클릭(다중선택) 했을때
		if (pWnd)
			::SendMessage(pWnd->GetSafeHwnd(),
			WM_GROUP_SELECT,
			(WPARAM)strSelectedIndex.operator LPCTSTR(),
			GetUNQID());
	}	
	
	InitEditCtrl();
	//column 선택체크
	CheckSelectColumn(point);
	
	SettingScrollBar();

	InvalidateNoScroll();

	CWnd::OnLButtonDown(nFlags, point);
}

void CGroupListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	m_bColumnTracking		= FALSE;
	m_nColumnTrackingIndex	= -1;
	m_nSelectColumn			= -1;
	m_ptClick.SetPoint(0, 0);

	SettingScrollBar();

	InvalidateNoScroll();

	CWnd::OnLButtonUp(nFlags, point);
}

void	CGroupListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	CWnd* pWnd = GetParent();

	if (pWnd)
		::SendMessage(pWnd->GetSafeHwnd(), WM_GROUP_RCLICK, 0, 0);

	CWnd::OnRButtonDown(nFlags, point);
}

void CGroupListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (IsPtInResizeRect(point) == TRUE || m_bColumnTracking == TRUE)
		SetCursor(LoadCursor(0, IDC_SIZEWE));
	else
		SetCursor(LoadCursor(0, IDC_ARROW));

	if (m_bColumnTracking == TRUE && m_nColumnTrackingIndex > -1 && m_arrColumnInfo.GetCount() > m_nColumnTrackingIndex)
	{
		sColumnInfo *pInfo = (sColumnInfo *)m_arrColumnInfo.GetAt(m_nColumnTrackingIndex);

		if (pInfo != NULL)
		{
			pInfo->nWidth += (point.x - m_ptClick.x);

			if (pInfo->nWidth < m_nMinColumnWidth)
			{
				pInfo->nWidth = m_nMinColumnWidth;
			}
			else
			{
				m_ptClick.x = point.x;
			}
			
			
			InvalidateNoScroll();
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CGroupListCtrl::SetMinColumnWidth(int nMinColumnWidth)
{
	m_nMinColumnWidth = nMinColumnWidth;
}

BOOL CGroupListCtrl::IsPtInResizeRect(CPoint point, int *pTrackingIndex)
{
	sColumnInfo *pInfo	= NULL;
	int			nIndex	= -1;

	CRect		rect;
	CRect		rectColumn;

	BOOL		bRectIn = FALSE;
	
	for (int i = 0; i < m_arrColumnInfo.GetCount(); i++)
	{
		pInfo = (sColumnInfo*)m_arrColumnInfo.GetAt(i);

		if (pInfo != NULL)
		{
			GetColumnRect(i, rectColumn);

			rect.left	= rectColumn.right-2;
			rect.top	= 0;
			rect.right	= rect.left + 4;
			rect.bottom	= m_nColumnHeight;

			if (rect.PtInRect(point) == TRUE)
			{
				nIndex = i;
				bRectIn = TRUE;
				break;
			}
		}
	}
	
	if (pTrackingIndex != NULL)
		*pTrackingIndex = nIndex;

	return bRectIn;
}

void CGroupListCtrl::SetItemHeight(int nItemHeight)
{
	m_nItemHeight = nItemHeight;
	InvalidateNoScroll();
}

void CGroupListCtrl::SetGroupHeight(int nGroupHeight)
{
	m_nGroupHeight = nGroupHeight;
	InvalidateNoScroll();
}

void	CGroupListCtrl::SetFontName( CString strFontName )
{
	m_strFontName = strFontName;
}

void	CGroupListCtrl::SetFontSizeEx( int nFontSize )
{
	m_nGroupFontSize = nFontSize;
}

int CGroupListCtrl::InsertItem(int nGroupIndex, CString strText)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return -1;

	sGroupInfo *pGroupInfo   = NULL;
	sItemInfo  *pItemInfo   = NULL;

	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo == NULL)
		return -1;

	pItemInfo = new sItemInfo;
	int nItemIndex      = pGroupInfo->GetSize();

	pGroupInfo->arrItems.Add(pItemInfo);


	for (int i=0; i<m_arrColumnInfo.GetCount(); i++)
		SetItemText(nGroupIndex, nItemIndex, i, _T(""));

	InvalidateNoScroll();
	return pGroupInfo->GetSize() - 1;
}


int CGroupListCtrl::InsertItem(int nGroupIndex, sItemInfo itemInfo)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount())
		return -1;

	sGroupInfo *pGroupInfo   = NULL;
	sItemInfo  *pItemInfo   = NULL;

	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo == NULL)
		return -1;

	pItemInfo = new sItemInfo;
	int nItemIndex      = pGroupInfo->GetSize();

	pGroupInfo->arrItems.Add(pItemInfo);

	for (int i=0; i<m_arrColumnInfo.GetCount(); i++)
		SetItemText(nGroupIndex, nItemIndex, i, _T(""));

	InvalidateNoScroll();

	return pGroupInfo->GetSize() - 1;
}




int	CGroupListCtrl::InsertGroup(sGroupInfo *pInfo)
{
	sGroupInfo *pGroupInfo = NULL;

	pGroupInfo = new sGroupInfo;

	pGroupInfo->bExpand		= pInfo->bExpand;
	pGroupInfo->nGroupIndex = ( int )m_arrGroupInfo.GetSize();
	pGroupInfo->strText		= pInfo->strText;
	pGroupInfo->bDrawExpand = pInfo->bDrawExpand;

	m_arrGroupInfo.Add(pGroupInfo);
	InvalidateNoScroll();
	return pGroupInfo->nGroupIndex;
}

int	CGroupListCtrl::InsertGroup(CString strText)
{
	sGroupInfo *pGroupInfo = NULL;

	pGroupInfo = new sGroupInfo;

	pGroupInfo->bExpand		= TRUE;
	pGroupInfo->nGroupIndex = ( int )m_arrGroupInfo.GetSize();
	pGroupInfo->strText		= strText;

	m_arrGroupInfo.Add(pGroupInfo);
	InvalidateNoScroll();
	return pGroupInfo->nGroupIndex;
}

void CGroupListCtrl::SetItemLine(BOOL bItemLine)
{
	m_bItemLine = bItemLine;
}

void CGroupListCtrl::SetSubItemLine(BOOL bSubItemLine)
{
	m_bSubItemLine = bSubItemLine;
}
void CGroupListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	
	if (m_nExpandGroupIndex != -1)
		return;
	
	sGroupInfo *pGroupInfo = NULL;
	sItemInfo  *pItemInfo  = NULL;

	CRect		rcGroup;
	CRect		rcItem;
	CRect		rcGroupExpand;

	for (int nGroupIndex=0; nGroupIndex<m_arrGroupInfo.GetCount(); nGroupIndex++)
	{
		pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

		if (pGroupInfo != NULL)
		{
			GetGroupRect(pGroupInfo->nGroupIndex, rcGroup);

			rcGroupExpand.SetRect(rcGroup.left + GROUP_EXPAND_LEFT_GAP, rcGroup.top + GROUP_EXPAND_TOP_GAP, 
				rcGroup.left + GROUP_EXPAND_LEFT_GAP + SIZE_EXTEND, rcGroup.top + GROUP_EXPAND_TOP_GAP + SIZE_EXTEND);

			if (rcGroupExpand.PtInRect(point) == TRUE)
				return;

			
			if (rcGroup.PtInRect(point) == TRUE)
			{
				SetCursor(LoadCursor(0, IDC_WAIT));

				if (pGroupInfo->arrItems.GetCount() < 15)
				{
					m_nExpandGroupIndex = pGroupInfo->nGroupIndex;

					for (int nItemIndex=pGroupInfo->GetSize()-1; nItemIndex>=0; nItemIndex--)
					{
						pItemInfo = (sItemInfo  *)pGroupInfo->arrItems.GetAt(nItemIndex);

						if (pItemInfo != NULL)
							pItemInfo->bExpand = pGroupInfo->bExpand;
					}
				}
			}
			


			if (pGroupInfo->bExpand == TRUE)
			{
				for (int nItemIndex=pGroupInfo->GetSize()-1; nItemIndex>=0; nItemIndex--)
				{
					pItemInfo = (sItemInfo  *)pGroupInfo->arrItems.GetAt(nItemIndex);

					if (pItemInfo != NULL)
					{
						GetItemRect(pGroupInfo->nGroupIndex, nItemIndex, rcItem);

						//아이템 안에 더블클릭
						if (rcItem.PtInRect(point) == TRUE)
						{
							//TODO . item double click 시 처리
							for (int i=0; i<m_arrColumnInfo.GetCount(); i++)
							{
								CRect rcSubItem;
								GetSubItemRect(pGroupInfo->nGroupIndex, nItemIndex, i, rcSubItem);

								if (rcSubItem.PtInRect(point) == TRUE)
								{
									if (m_pEditCtrl != NULL)
									{
										m_nCurEditGroupIndex	= pGroupInfo->nGroupIndex;
										m_nCurEditItemIndex		= nItemIndex;
										m_nCurEditSubItemIndex	= i;
										m_pEditCtrl->SetWindowText(GetItemText(pGroupInfo->nGroupIndex, nItemIndex, i));
										m_pEditCtrl->MoveWindow(rcSubItem.left + 1, rcSubItem.top + 1, rcSubItem.right - rcSubItem.left - 1, rcSubItem.bottom - rcSubItem.top - 1);
										m_pEditCtrl->ShowWindow(SW_SHOW);
										m_pEditCtrl->SetFocus();
										break;
									}
									
								}
							}
							
						}

					}

					if (m_nExpandGroupIndex != -1)
					{
						pItemInfo->bExpand = !pItemInfo->bExpand;
						RedrawNoScroll();
						if (pGroupInfo->arrItems.GetCount() >= 10 && pGroupInfo->arrItems.GetCount() < 20)
							Sleep(20);
						else if (pGroupInfo->arrItems.GetCount() >= 0 && pGroupInfo->arrItems.GetCount() < 10)
							Sleep(30);
					}
				}
			}
			else
			{
				for (int nItemIndex=0; nItemIndex<pGroupInfo->arrItems.GetCount(); nItemIndex++)
				{
					pItemInfo = (sItemInfo  *)pGroupInfo->arrItems.GetAt(nItemIndex);

					if (pItemInfo != NULL)
					{
						GetItemRect(pGroupInfo->nGroupIndex, nItemIndex, rcItem);

						if (rcItem.PtInRect(point) == TRUE)
						{
							//TODO . item double click 시 처리
						}
					}

					if (m_nExpandGroupIndex != -1)
					{
						pItemInfo->bExpand = !pItemInfo->bExpand;
						RedrawNoScroll();
						
						if (pGroupInfo->arrItems.GetCount() >= 10 && pGroupInfo->arrItems.GetCount() < 20)
							Sleep(20);
						else if (pGroupInfo->arrItems.GetCount() >= 0 && pGroupInfo->arrItems.GetCount() < 10)
							Sleep(30);
					}
				}
			}

			m_nExpandGroupIndex = -1;


			if (rcGroup.PtInRect(point) == TRUE)
			{
				pGroupInfo->bExpand = !pGroupInfo->bExpand;
				SetCursor(LoadCursor(0, IDC_ARROW));
			}
		}
	}

	InvalidateNoScroll();
	CWnd::OnLButtonDblClk(nFlags, point);
}




/*/##############################################################################
### \brief		확장 아이콘 출력
#################################################################################*/
void CGroupListCtrl::DrawExpand(CDC *pDC, CRect rect, BOOL bExpand)
{

	CRect rtExtend(rect);

	rtExtend.right  = rtExtend.left + SIZE_EXTEND;
	rtExtend.top    = rect.top;
	rtExtend.bottom = rtExtend.top + SIZE_EXTEND;

	COLORREF oldBackground = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
	pDC->FillSolidRect(&rtExtend, ::GetSysColor(COLOR_WINDOW));

	// draw border
	CBrush brush(RGB(150, 150, 150));
	pDC->FrameRect(&rtExtend, &brush);

	CPen pen(PS_SOLID, 1, RGB(128, 70, 70));
	CPen * pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(rtExtend.left+MARGIN_EXTEND, rtExtend.CenterPoint().y);
	pDC->LineTo(rtExtend.right-MARGIN_EXTEND, rtExtend.CenterPoint().y);

	if (bExpand == FALSE)
	{
		pDC->MoveTo(rtExtend.CenterPoint().x, rtExtend.top+MARGIN_EXTEND);
		pDC->LineTo(rtExtend.CenterPoint().x, rtExtend.bottom-MARGIN_EXTEND);
	}


	pDC->SelectObject(pOldPen);
	pDC->SetBkColor(oldBackground);

}

int CGroupListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CGroupListCtrl::CreateEditCtrl(UINT nEditID)
{
	if (nEditID == 0)
		return FALSE;

	m_pEditCtrl = new CEdit;

	if (m_pEditCtrl != NULL)
	{
		if (FALSE == m_pEditCtrl->Create(WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, nEditID))
		{
			TRACE(_T("CGroupListCtrl::CreateEditCtrl create fail!"));
			return FALSE;
		}
	}

	m_pEditCtrl->ShowWindow(SW_HIDE);

	return TRUE;
}

BOOL CGroupListCtrl::CreateScrollBar(UINT nVerticalScrollID, UINT nHorizontalScrollID)
{
	if (nVerticalScrollID == 0 && nHorizontalScrollID == 0)
		return FALSE;

	m_pVerticalScroll		= new CScrollBar;
	m_pHorizontalScroll		= new CScrollBar;

	if (m_pVerticalScroll != NULL && nVerticalScrollID != 0)
	{
		if (FALSE == m_pVerticalScroll->Create(WS_VISIBLE | WS_CHILD | SB_VERT, CRect(0, 0, 0, 0), this, nVerticalScrollID))
		{
			TRACE(_T("CGroupListCtrl::CreateScrollBar VerticalScrool create fail!"));
			return FALSE;
		}
	}

	if (m_pHorizontalScroll != NULL && nHorizontalScrollID != 0)
	{
		if (FALSE == m_pHorizontalScroll->Create(WS_VISIBLE | WS_CHILD | SB_HORZ, CRect(0, 0, 0, 0), this, nHorizontalScrollID))
		{
			TRACE(_T("CGroupListCtrl::CreateScrollBar HorizontalScroll create fail!"));
			return FALSE;
		}
	}

	m_nScrollBarId = nVerticalScrollID;
	
	return TRUE;
}
void CGroupListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
			m_pVerticalScroll->MoveWindow(cx - DEF_SCROLL_SIZE, 0, DEF_SCROLL_SIZE, cy);
	}

	if (m_pHorizontalScroll != NULL)
	{
		if (m_pHorizontalScroll->GetSafeHwnd())
			m_pHorizontalScroll->MoveWindow(0, cy - DEF_SCROLL_SIZE, cx - DEF_SCROLL_SIZE, DEF_SCROLL_SIZE);
	}

	SettingScrollBar();
}

int CGroupListCtrl::GetAllColumnWidth()
{
	int			nWidth = 0;
	sColumnInfo *pInfo = NULL;

	for (int i = 0; i < m_arrColumnInfo.GetCount(); i++)
	{
		pInfo = (sColumnInfo *)m_arrColumnInfo.GetAt(i);

		if (pInfo != NULL)
			nWidth += pInfo->nWidth;
	}

	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
			nWidth += DEF_SCROLL_SIZE;
	}

	return nWidth;
}

int CGroupListCtrl::GetAllListHeight()
{
	int         nHeight = 0;
	sGroupInfo   *pInfo   = NULL;

	for (int i=0; i<m_arrGroupInfo.GetCount(); i++)
	{
		pInfo = (sGroupInfo*)m_arrGroupInfo.GetAt(i);

		if (pInfo != NULL)
		{
			if (pInfo->bExpand == TRUE)
				nHeight += (pInfo->GetSize() * m_nItemHeight);
		}

		nHeight += m_nGroupHeight;
	}

	nHeight += m_nGroupHeight;
	nHeight += m_nColumnHeight;
	if (m_pHorizontalScroll != NULL && m_pHorizontalScroll->GetSafeHwnd() != NULL && m_pHorizontalScroll->IsWindowVisible() == TRUE)
		nHeight += DEF_SCROLL_SIZE;

	return nHeight;
}

int CGroupListCtrl::GetScrollPos(CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL)
		return 0;

	SCROLLINFO si = {0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;

	::GetScrollInfo(pScrollBar->GetSafeHwnd(), SB_CTL, &si);

	return si.nPos;
}

int CGroupListCtrl::GetScrollTrackPos(CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL)
		return 0;

	SCROLLINFO si = {0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;

	::GetScrollInfo(pScrollBar->GetSafeHwnd(), SB_CTL, &si);

	return si.nTrackPos;
}

void CGroupListCtrl::SettingScrollBar()
{
	CRect		rect;

	GetClientRect(&rect);

	int			nWidth	= max(GetAllColumnWidth(), rect.right);
	int			nHeight	= max(GetAllListHeight(), rect.bottom);
	int			nVerGap	= 0;


	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	
	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
		{
			if(rect.Height() < nHeight)
			{
				si.nMin		= 0;
				si.nMax		= nHeight; 
				si.nPage	= rect.Height();
				si.nPos		= GetScrollPos(m_pVerticalScroll);

				m_pVerticalScroll->SetScrollInfo(&si, TRUE);
				m_pVerticalScroll->ShowScrollBar(TRUE);
			}
			else
			{
				// JSK
				si.nMin = 0;
				si.nMax = nHeight;
				si.nPage = rect.Height();
				si.nPos = GetScrollPos(m_pVerticalScroll);

				m_pVerticalScroll->SetScrollInfo(&si, TRUE);

				m_pVerticalScroll->ShowScrollBar(FALSE);
			}
				
		}
	}
	
	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
		{
			if (m_pVerticalScroll->IsWindowVisible())
				nVerGap = DEF_SCROLL_SIZE;
		}
	}

	if (m_pHorizontalScroll != NULL)
	{
		if (m_pHorizontalScroll->GetSafeHwnd())
		{
			if(rect.Width() - nVerGap < nWidth)
			{
				si.nMin		= 0;
				si.nMax		= nWidth;
				si.nPage	= rect.Width() - nVerGap;
				si.nPos		= GetScrollPos(m_pHorizontalScroll);

				m_pHorizontalScroll->SetScrollInfo(&si, TRUE);
				m_pHorizontalScroll->ShowScrollBar(TRUE);
			}
			else
				m_pHorizontalScroll->ShowScrollBar(FALSE);
		}
	}

	//InitEditCtrl();
}




void CGroupListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nScrollPos = GetScrollPos(m_pVerticalScroll);

	switch (nSBCode)
	{
	case SB_PAGEDOWN:   
	case SB_LINEDOWN:
		nScrollPos += m_nItemHeight;
		break;

	case SB_LINEUP:
	case SB_PAGEUP:   
		nScrollPos -= m_nItemHeight;
		break;
		
	case SB_THUMBPOSITION: 
	case SB_THUMBTRACK:    
		nScrollPos = GetScrollTrackPos(m_pVerticalScroll);
		break;

	case SB_ENDSCROLL:
		return;
	}
	
	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
			m_pVerticalScroll->SetScrollPos(nScrollPos);
	}

	InitEditCtrl();

	InvalidateNoScroll();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CGroupListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nScrollPos = GetScrollPos(m_pHorizontalScroll);

	switch (nSBCode)
	{

	case SB_LINELEFT:
	case SB_PAGELEFT:   
		nScrollPos -= m_nItemHeight;
		break;
	case SB_PAGERIGHT:   
	case SB_LINERIGHT:
		nScrollPos += m_nItemHeight;
		break;

	case SB_THUMBPOSITION: 
	case SB_THUMBTRACK:    
		nScrollPos = GetScrollTrackPos(m_pHorizontalScroll);
		break;

	case SB_ENDSCROLL:
		return;
	}

	if (m_pHorizontalScroll != NULL)
	{
		if (m_pHorizontalScroll->GetSafeHwnd())
			m_pHorizontalScroll->SetScrollPos(nScrollPos);
		
	}

	InitEditCtrl();

	InvalidateNoScroll();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}



/*/##############################################################################
### \brief		mouse wheel callback
#################################################################################*/
void CGroupListCtrl::SetMouseWheel(short zDelta)
{
	int nScrollPos = GetScrollPos(m_pVerticalScroll);

	if (zDelta == -120)
		nScrollPos += m_nItemHeight;
	else if (zDelta == 120)
		nScrollPos -= m_nItemHeight;


	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
			m_pVerticalScroll->SetScrollPos(nScrollPos);
	}

	InitEditCtrl();


	InvalidateNoScroll();
}

void CGroupListCtrl::InitEditCtrl()
{
	if (m_pEditCtrl != NULL)
		m_pEditCtrl->ShowWindow(SW_HIDE);

	m_nCurEditGroupIndex	= -1;
	m_nCurEditItemIndex		= -1;
	m_nCurEditSubItemIndex	= -1;

}

void CGroupListCtrl::SetVScrollPos(int nGroupIndex, int nItemIndex)
{
	if (nGroupIndex < 0 || nGroupIndex >= m_arrGroupInfo.GetCount() || m_pVerticalScroll == NULL)
		return;

	int nScrollPos = 0;
	sGroupInfo *pGroupInfo = NULL;

	nScrollPos += (m_nGroupHeight * (nGroupIndex + 1));


	for (int i=0; i<nGroupIndex; i++)
	{
		pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(i);

		nScrollPos += (pGroupInfo->GetSize() * (m_nItemHeight));
	}

	pGroupInfo = (sGroupInfo *)m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo->arrItems.GetCount() <= nItemIndex)
		return;

	nScrollPos += (nItemIndex * m_nItemHeight);

	m_pVerticalScroll->SetScrollPos(nScrollPos);

	InvalidateNoScroll();

}

BOOL CGroupListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			CString strText;			

			if (m_pEditCtrl != NULL && m_nCurEditGroupIndex != -1 && m_nCurEditItemIndex != -1 && m_nCurEditSubItemIndex != -1)
			{
				m_pEditCtrl->GetWindowText(strText);

				SetItemText(m_nCurEditGroupIndex, m_nCurEditItemIndex, m_nCurEditSubItemIndex, strText);
			}

			InitEditCtrl();
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			InitEditCtrl();
			return TRUE;
		}		
	}
	else if (pMsg->message == WM_MOUSEWHEEL)
	{
		SetMouseWheel((short)HIWORD(pMsg->wParam));
	}

	return CWnd::PreTranslateMessage(pMsg);
}

int CGroupListCtrl::GetGroupInfoSize()
{
	return ( int )m_arrGroupInfo.GetSize();
}

int CGroupListCtrl::GetItemCount( int nGroupIndex )
{
	//error
	if (m_arrGroupInfo.GetCount() <= nGroupIndex)
		return 0;

	sGroupInfo* pGroupInfo = ( sGroupInfo* )m_arrGroupInfo.GetAt(nGroupIndex);

	if (pGroupInfo == NULL)
		return 0;

	return pGroupInfo->GetSize();
}

void	CGroupListCtrl::SetMultiLine( BOOL bMultiLine )
{
	m_bMultiLine = bMultiLine;
}

void CGroupListCtrl::SetUNQID(const int nID)
{
	m_nUNQID = nID;
}

int CGroupListCtrl::GetUNQID()
{
	return m_nUNQID;
}