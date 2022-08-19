// SkinTreeCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SkinTreeCtrl.h"

#pragma warning(disable:4100)

// CSkinTreeCtrl

HHOOK	g_hTreeHook = NULL; 
CWnd	*g_pTreeThis  = NULL;

IMPLEMENT_DYNAMIC(CSkinTreeCtrl, CWnd)

CSkinTreeCtrl::CSkinTreeCtrl()
{
	m_nFontSize				= 15;
	m_colorSelectItem		= Color(100, 100, 50, 50);
	m_colorLine				= Color(180, 180, 180);
	m_pVerticalScroll		= NULL;
	m_pHorizontalScroll		= NULL;
	m_bBKLine				= TRUE;
	m_colorFont				= RGB(40, 40, 40);
	m_colorBK				= RGB(255, 255, 255);
}

CSkinTreeCtrl::~CSkinTreeCtrl()
{
	RemoveScroll();
	RemoveAllItems(&m_treeItems);

}


BEGIN_MESSAGE_MAP(CSkinTreeCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


/**********************************************************
@brief mouse wheel callback
@details 
@param zDelta 위로 움직였는지 아래로 움직였는지 체크 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::SetMouseWheel(short zDelta)
{
	int nScrollPos = GetScrollPos(m_pVerticalScroll);

	if (zDelta == -120)
		nScrollPos += DEF_TREE_ITEM_HEIGHT;
	else if (zDelta == 120)
		nScrollPos -= DEF_TREE_ITEM_HEIGHT;

	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
			m_pVerticalScroll->SetScrollPos(nScrollPos);
	}

	InvalidateNoScroll();
}

/**********************************************************
@brief 아이템 삭제
@details 
@param pos  삭제될 아이템
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::RemoveItem(POSITION pos)
{
	if (pos == NULL)
		return;

	stTreeItem *pTreeItem = NULL;

	pTreeItem = GetItem(&m_treeItems, pos);

	if (pTreeItem != NULL)
	{
		RemoveAllItems(&pTreeItem->listChilds);

		if (pTreeItem->pParentItem != NULL)
		{
			pTreeItem->pParentItem->listChilds.RemoveAt(pos);
		}
		else
		{
			m_treeItems.RemoveAt(pos);
		}

		delete pTreeItem;
		pTreeItem = NULL;
	}

	InvalidateNoScroll();
}

/**********************************************************
@brief 전체 아이템 삭제
@details 
@param pTreeItems 삭제될 아이템 리스트
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::RemoveAllItems(CList<stTreeItem*, stTreeItem*> *pTreeItems)
{
	if (pTreeItems == NULL)
		return;

	POSITION		pos				= pTreeItems->GetHeadPosition();
	stTreeItem		*pTreeItem		= NULL;

	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			continue;

		RemoveAllItems(&pTreeItem->listChilds);

		pTreeItem->listChilds.RemoveAll();

		delete pTreeItem;
		pTreeItem = NULL;
	}
	
	InvalidateNoScroll();
}

void	CSkinTreeCtrl::RemoveAllItems()
{
	RemoveAllItems(&m_treeItems);

	m_treeItems.RemoveAll();
}

/**********************************************************
@brief 스크롤 삭제
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::RemoveScroll()
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

/**********************************************************
@brief 아이템 정보 리턴
@details 
@param pos 아이템 포지션값 
@return 아이템 정보리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
stTreeItem*	CSkinTreeCtrl::GetItem(POSITION pos)
{
	return GetItem(&m_treeItems, pos);
}

/**********************************************************
@brief 아이템 정보 리턴
@details 
@param pTreeItems 이 리스트안에있는 데이터중 해당 아이템이 있을경우 정보 리턴
@param pos 정보를 가져올 아이템 pos
@return 아이템 정보리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
stTreeItem* CSkinTreeCtrl::GetItem(CList<stTreeItem*, stTreeItem*> *pTreeItems, POSITION pos)
{
	if (pTreeItems == NULL)
		return NULL;

	POSITION		posSearch	= pTreeItems->GetHeadPosition();
	stTreeItem		*pTreeItem	= NULL;
	stTreeItem		*pRetTreeItem	= NULL;

	while (posSearch)
	{
		if (pos == posSearch)
		{
			return pTreeItems->GetAt(posSearch);
		}

		pTreeItem = pTreeItems->GetNext(posSearch);

		pRetTreeItem = GetItem(&pTreeItem->listChilds, pos);

		if (pRetTreeItem != NULL)
			return pRetTreeItem;
	}

	return NULL;
}

/**********************************************************
@brief 아이템 추가
@details 
@param posParent 부모 포지션 
@param pTreeItem 추가할 아이템 정보 
@return  정상 추가 되면 포지션값 리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
POSITION CSkinTreeCtrl::InsertItem(POSITION posParent, stTreeItem *pTreeItem)
{
	if (pTreeItem == NULL)
		return NULL;

	POSITION pos = NULL;
	stTreeItem *pNewTreeItem = new stTreeItem;
	stTreeItem *pParentItem = GetItem(&m_treeItems, posParent);

	pNewTreeItem->bExpand		= pTreeItem->bExpand;
	pNewTreeItem->bSelect		= pTreeItem->bSelect;
	pNewTreeItem->strText		= pTreeItem->strText;
	pNewTreeItem->strIconPath	= pTreeItem->strIconPath;
	pNewTreeItem->colorItem		= pTreeItem->colorItem;

	//루트
	if (pParentItem == NULL)
	{
		pNewTreeItem->pParentItem	= NULL;

		pos = m_treeItems.AddTail(pNewTreeItem);

		pNewTreeItem->pos = pos;

		return pos;
	}
	else
	{
		if (pParentItem->bExpand == FALSE || pParentItem->bDraw == FALSE)
			pNewTreeItem->bDraw = FALSE;
	}

	pNewTreeItem->pParentItem	= pParentItem;

	pos = pParentItem->listChilds.AddTail(pNewTreeItem);

	pNewTreeItem->pos = pos;

	InvalidateNoScroll();
	
	return pos;

}


BOOL CSkinTreeCtrl::OnEraseBkgnd(CDC* pDC)
{

	return FALSE;//CWnd::OnEraseBkgnd(pDC);
}


/**********************************************************
@brief Expand 이미지 설정
@details 
@param strNormalImagePath 기본 이미지
@param strMoreImagePath  expand되었을때 이미지
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::SetExpandImagePath(CString strNormalImagePath, CString strMoreImagePath)
{
	m_strNormalImagePath = strNormalImagePath;
	m_strMoreImagePath = strMoreImagePath;
}

/**********************************************************
@brief 아이템의 부모 뎁스 구하는 함수
@details 
@param pTreeItem 구할 아이템포인터
@return 뎁스 카운트 리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
int CSkinTreeCtrl::GetParentCnt(stTreeItem *pTreeItem)
{
	if (pTreeItem == NULL)
		return 0;

	int	nCnt = 0;

	while (1)
	{
		if (pTreeItem->pParentItem == NULL)
			break;

		pTreeItem = pTreeItem->pParentItem;
		nCnt++;
	}

	return nCnt;
}

/**********************************************************
@brief Expand 드로잉
@details 
@param pDC DC
@param rect 그려질 위치
@param bExpand expand 여부
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::DrawExpand(CDC *pDC, CRect rect, BOOL bExpand)
{
	CRect rtExtend(rect);
	rtExtend.right  = rtExtend.left + SIZE_EXTEND;
	rtExtend.top    = rect.top;
	rtExtend.bottom = rtExtend.top + SIZE_EXTEND;
	
	if (IsUseExpandImage() == TRUE)
	{
		Graphics grs(pDC->GetSafeHdc());

		if (bExpand == TRUE)
		{
			Image img((CStringW)m_strNormalImagePath);

			grs.DrawImage(&img, rtExtend.left, rtExtend.top, SIZE_EXTEND_IMAGE, SIZE_EXTEND_IMAGE);
		}
		else
		{
			Image img((CStringW)m_strMoreImagePath);

			grs.DrawImage(&img, rtExtend.left, rtExtend.top, SIZE_EXTEND_IMAGE, SIZE_EXTEND_IMAGE);
		}
	}
	else
	{
		COLORREF oldBackground = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->FillSolidRect(&rtExtend, ::GetSysColor(COLOR_WINDOW));

		// draw border
		CBrush brush(RGB(150, 150, 150));
		pDC->FrameRect(&rtExtend, &brush);

		CPen  pen(PS_SOLID, 1, RGB(128, 70, 70));
		CPen  *pOldPen = pDC->SelectObject(&pen);

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
	

}

/**********************************************************
@brief 부모가 expand되었는지 리턴
@details 
@param pTreeItem 이 아이템의 부모
@return expand 여부 리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
BOOL CSkinTreeCtrl::IsParentExpand(stTreeItem *pTreeItem)
{
	if (pTreeItem == NULL)
		return FALSE;

	BOOL bExpand = TRUE;
	
	while (1)
	{
		if (pTreeItem->pParentItem == NULL)
			break;

		pTreeItem = pTreeItem->pParentItem;
		
		if (pTreeItem->bExpand == FALSE)
		{
			bExpand = FALSE;
			break;
		}
	}

	return bExpand;
}

/**********************************************************
@brief 전체 트리 아이템의 높이 리턴
@details 
@param &nTotalHeight 참조로 리턴 
@param pTreeItems 해당 리스트의 높이 리턴 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::GetTotalHeight(int &nTotalHeight, CList<stTreeItem*, stTreeItem*> *pTreeItems)
{
	POSITION	pos = pTreeItems->GetHeadPosition();
	stTreeItem	*pTreeItem   = NULL;
	stTreeItem	*pParentItem = NULL;
	int			nParentCnt = 0;

	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			break;

		nParentCnt = GetParentCnt(pTreeItem);

		pParentItem = pTreeItem->pParentItem;

		if ((pParentItem == NULL || IsParentExpand(pTreeItem) == TRUE) && pTreeItem->bDraw == TRUE)
			nTotalHeight += DEF_TREE_ITEM_HEIGHT;
		
		//재귀 함수
		GetTotalHeight(nTotalHeight, &pTreeItem->listChilds);
	}
}

/**********************************************************
@brief 전체 아이템들의 높이
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
int CSkinTreeCtrl::GetTotalHeight()
{
	int nTotalHeight = 0;

	GetTotalHeight(nTotalHeight, &m_treeItems);
	
	return nTotalHeight;
}


/**********************************************************
@brief Expand 영역에 클릭되었는지 리턴
@details 
@param point 클릭된 위치 
@return 클릭된 아이템 정보리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
stTreeItem* CSkinTreeCtrl::PtInItemExpand(CPoint point)
{
	int		nHeight = 0;

	return PtInItemExpand(&m_treeItems, point, nHeight);
}

/**********************************************************
@brief  Expand 영역에 클릭되었는지 리턴
@details 
@param pTreeItems 검색 할 리스트
@param point 클릭된 위치
@param nHeight 재귀로인해 nHeight 참조값으로 리턴
@return 클릭된 아이템 정보리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
stTreeItem* CSkinTreeCtrl::PtInItemExpand(CList<stTreeItem*, stTreeItem*> *pTreeItems, CPoint point, int &nHeight)
{
	POSITION	pos				= pTreeItems->GetHeadPosition();
	stTreeItem	*pTreeItem		= NULL;
	stTreeItem	*pParentItem	= NULL;
	int			nParentCnt		= 0;
	int			nVScrollPos		= GetScrollPos(m_pVerticalScroll);
	
	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			break;

		nParentCnt = GetParentCnt(pTreeItem);

		pParentItem = pTreeItem->pParentItem;

		if ((pParentItem == NULL || IsParentExpand(pTreeItem) == TRUE))
		{
			if (pTreeItem->listChilds.GetCount() > 0)
			{
				int nImageSize = IsUseExpandImage() == TRUE ? SIZE_EXTEND_IMAGE : SIZE_EXTEND;

				CRect rcExpand;
				rcExpand.left	= nParentCnt * DEF_CHILD_GAP + DEF_LEFT_GAP;
				rcExpand.top	= nHeight + (DEF_TREE_ITEM_HEIGHT - nImageSize)/2 - nVScrollPos;
				rcExpand.right	= rcExpand.left + nImageSize;
				rcExpand.bottom	= rcExpand.top + nImageSize;

				if (rcExpand.PtInRect(point) == TRUE)
					return pTreeItem;

			}
		
			nHeight += DEF_TREE_ITEM_HEIGHT;
		}

		//재귀 함수
		pTreeItem = PtInItemExpand(&pTreeItem->listChilds, point, nHeight);

		if (pTreeItem != NULL)
			return pTreeItem;
	}

	return NULL;
}

/**********************************************************
@brief 가장 첫번째로 그려질 아이템 리턴
@details 
@param pTreeItems 이 리스트 중 검색 
@param &nTop  재귀함수로 인한 참조 리턴
@return 가장 첫번째 아이템 정보 리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
stTreeItem* CSkinTreeCtrl::GetFirstDrawItem(CList<stTreeItem*, stTreeItem*> *pTreeItems, int &nTop)
{
	POSITION	pos				= pTreeItems->GetHeadPosition();
	stTreeItem	*pTreeItem		= NULL;
	stTreeItem	*pParentItem	= NULL;
	int			nParentCnt		= 0;
	int			nVScrollPos		= GetScrollPos(m_pVerticalScroll);
	CRect		rcClient;

	GetClientRect(&rcClient);

	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			break;

		nParentCnt = GetParentCnt(pTreeItem);

		pParentItem = pTreeItem->pParentItem;

		if (pParentItem == NULL || IsParentExpand(pTreeItem) == TRUE)
		{
			if (nVScrollPos <= nTop)
				return pTreeItem;

			nTop += DEF_TREE_ITEM_HEIGHT;
		}

		//재귀 함수
		pTreeItem = GetFirstDrawItem(&pTreeItem->listChilds, nTop);

		if (pTreeItem != NULL)
			return pTreeItem;
	}

	return NULL;
}

/**********************************************************
@brief 해당 포인트에 아이템이 선택되었다면 아이템 정보 리턴
@details 
@param pTreeItems 검색할 리스트 
@param point 클릭된 위치 
@param &nHeight 재귀를 위한 높이 참조
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
stTreeItem* CSkinTreeCtrl::PtInItem(CList<stTreeItem*, stTreeItem*> *pTreeItems, CPoint point, int &nHeight)
{
	POSITION	pos				= pTreeItems->GetHeadPosition();
	stTreeItem	*pTreeItem		= NULL;
	stTreeItem	*pParentItem	= NULL;
	int			nParentCnt		= 0;
	int			nVScrollPos		= GetScrollPos(m_pVerticalScroll);
	CRect		rcClient;

	GetClientRect(&rcClient);

	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			break;

		nParentCnt = GetParentCnt(pTreeItem);

		pParentItem = pTreeItem->pParentItem;

		if (pParentItem == NULL || IsParentExpand(pTreeItem) == TRUE)
		{
			CRect rcItem(rcClient);

			rcItem.top		= nHeight - nVScrollPos;
			rcItem.bottom	= nHeight + DEF_TREE_ITEM_HEIGHT - nVScrollPos;

			if (rcItem.PtInRect(point) == TRUE)
				return pTreeItem;

			nHeight += DEF_TREE_ITEM_HEIGHT;
		}

		//재귀 함수
		pTreeItem = PtInItem(&pTreeItem->listChilds, point, nHeight);

		if (pTreeItem != NULL)
			return pTreeItem;
	}

	return NULL;
}

/**********************************************************
@brief 해당 포인트에 아이템이 선택되었다면 아이템 정보 리턴
@details 
@param point 클릭된 포인트 
@return 아이템 정보 리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
stTreeItem* CSkinTreeCtrl::PtInItem(CPoint point)
{
	int		nHeight = 0;

	return PtInItem(&m_treeItems, point, nHeight);
}

/**********************************************************
@brief Expand 이미지 사용여부 리턴
@details 
@return 사용여부
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
BOOL CSkinTreeCtrl::IsUseExpandImage()
{
	return m_strNormalImagePath.IsEmpty() == TRUE || m_strMoreImagePath.IsEmpty() == TRUE ? FALSE : TRUE;
}

/**********************************************************
@brief 해당 리스트의 아이템 드로잉
@details 
@param pTreeItems  
@param nTop 리스트가 그려질 Top값 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::DrawItems(CDC *pDC, CList<stTreeItem*, stTreeItem*> *pTreeItems, int &nTop)
{
	if (pTreeItems == NULL)
		return;

	CFont		font, *pOldFont	= NULL;
	CRect		rcClient;
	POSITION	pos = pTreeItems->GetHeadPosition();
	stTreeItem	*pTreeItem   = NULL;
	stTreeItem	*pParentItem = NULL;
	Graphics	grs(pDC->GetSafeHdc());
	int			nVScrollPos = GetScrollPos(m_pVerticalScroll);
	int			nHScrollPos = GetScrollPos(m_pHorizontalScroll);

	font.CreateFont(m_nFontSize, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));    

	pOldFont = pDC->SelectObject(&font);

	GetClientRect(&rcClient);

	int nParentCnt = 0;

	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			break;

		nParentCnt = GetParentCnt(pTreeItem);

		pParentItem = pTreeItem->pParentItem;

		if ((pParentItem == NULL || IsParentExpand(pTreeItem) == TRUE) && pTreeItem->bDraw == TRUE)
		{
			CRect rcItem(nParentCnt * DEF_CHILD_GAP + DEF_LEFT_GAP*5 + DEF_ICON_SIZE - nHScrollPos, 
				nTop * DEF_TREE_ITEM_HEIGHT - nVScrollPos, 
				rcClient.right - DEF_RIGHT_GAP - GetScrollbarSize(m_pVerticalScroll), 
				(nTop + 1) * DEF_TREE_ITEM_HEIGHT - nVScrollPos);

			if (rcItem.bottom < 0)
			{
				nTop++;
				DrawItems(pDC, &pTreeItem->listChilds, nTop);
				continue;
			}
			else if (rcItem.top > rcClient.bottom)
			{
				break;
			}

			int nImageSize = IsUseExpandImage() == TRUE ? SIZE_EXTEND_IMAGE : SIZE_EXTEND;

			CRect rcExpand(nParentCnt * DEF_CHILD_GAP + DEF_LEFT_GAP - nHScrollPos, 
				nTop * DEF_TREE_ITEM_HEIGHT + (DEF_TREE_ITEM_HEIGHT - nImageSize)/2 - nVScrollPos, 
				rcClient.right - nHScrollPos, 
				rcClient.bottom - nVScrollPos);

			//아이템별 배경색상
			pDC->FillSolidRect(0, rcItem.top+1, rcClient.right, rcItem.bottom - rcItem.top, pTreeItem->colorItem);


			//Expand 표시 Draw
			if (pTreeItem->listChilds.GetCount() > 0 && IsParentExpand(pTreeItem) == TRUE)
				DrawExpand(pDC, rcExpand, pTreeItem->bExpand);

			//아이콘 Draw
			if (pTreeItem->strIconPath.IsEmpty() == FALSE)
			{
				Image img((CStringW)pTreeItem->strIconPath);
				grs.DrawImage(&img, 
					rcExpand.left + SIZE_EXTEND + DEF_LEFT_GAP + 2, 
					(nTop * DEF_TREE_ITEM_HEIGHT) + ((DEF_TREE_ITEM_HEIGHT / 2) - (DEF_ICON_SIZE / 2)) - nVScrollPos, 
					DEF_ICON_SIZE, 
					DEF_ICON_SIZE);
			}

			CSize sizeText = pDC->GetOutputTextExtent(pTreeItem->strText);
			
			pDC->DrawText(pTreeItem->strText, rcItem, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			
			if (m_bBKLine == TRUE)
			{
				Pen			penPlus(m_colorLine, 1);
				penPlus.SetDashStyle(DashStyleDot);
				grs.DrawLine(&penPlus, 0, rcItem.bottom, rcItem.right + DEF_RIGHT_GAP, rcItem.bottom);
			}
		
			//아이템 선택시 색상변경
			if (pTreeItem->bSelect == TRUE)
			{
				SolidBrush		brushDeep(m_colorSelectItem);
				grs.FillRectangle(&brushDeep, 0, rcItem.top, rcClient.right, rcItem.bottom - rcItem.top);
			}

			nTop++;
		}
		
		//pTreeItem->bDraw = TRUE;

		//재귀 함수
		DrawItems(pDC, &pTreeItem->listChilds, nTop);
	}

	pDC->SelectObject(pOldFont);

	font.DeleteObject();
}

/**********************************************************
@brief 아이템 셀렉트 해지
@details 
@param pTreeItems 해당 리스트의 아이템 셀렉트 해지
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::AllDeselectItems(CList<stTreeItem*, stTreeItem*> *pTreeItems)
{
	POSITION	pos = pTreeItems->GetHeadPosition();
	stTreeItem	*pTreeItem   = NULL;

	CRect		rcClient;

	GetClientRect(&rcClient);

	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			break;

		pTreeItem->bSelect = FALSE;

		//재귀 함수
		AllDeselectItems(&pTreeItem->listChilds);
	}
}


/**********************************************************
@brief 드로잉
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::OnPaint()
{
	CPaintDC	dc(this); // device context for painting
	CDC			memDC;
	CBitmap		bitmap, *pOldBitmap = NULL;
	CRect		rcClient;

	GetClientRect(&rcClient);

	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.right, rcClient.bottom);

	pOldBitmap = memDC.SelectObject(&bitmap);
	int nBKMode = memDC.SetBkMode(TRANSPARENT);

	memDC.SetTextColor(m_colorFont);

	memDC.FillSolidRect(rcClient, m_colorBK);

	int nTop = 0;
	SettingScrollBar();

	DrawItems(&memDC, &m_treeItems, nTop);

	dc.BitBlt(0, 0, rcClient.right, rcClient.bottom, &memDC, 0, 0, SRCCOPY);

	memDC.SetBkMode(nBKMode);
	memDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();

}

/**********************************************************
@brief 아이템의 child의 카운트 구함
@details 
@param &nChildCnt  child카운트 참조리턴
@param pTreeItem   구할 아이템
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::GetChildCount(int &nChildCnt, stTreeItem *pTreeItem)
{
	if (pTreeItem == NULL)
		return;

	POSITION	pos = pTreeItem->listChilds.GetHeadPosition();
	stTreeItem	*pChildItem = NULL;


	while (pos)
	{
		pChildItem = pTreeItem->listChilds.GetNext(pos);

		if (pChildItem == NULL)
			continue;

		GetChildCount(nChildCnt, pChildItem);

		nChildCnt++;
	}
}

/**********************************************************
@brief Draw시 Effect 설정
@details 
@param pTreeItem effect설정될 아이템
@param bDraw	그릴지 그리지 않을지 여부
@param nSleep 효과 시간
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::SetDrawEffect(stTreeItem *pTreeItem, BOOL bDraw, int nSleep)
{
	if (pTreeItem == NULL)
		return;

	POSITION	pos = pTreeItem->listChilds.GetTailPosition();
	stTreeItem	*pChildItem = NULL;

	
	while (pos)
	{
		pChildItem = pTreeItem->listChilds.GetPrev(pos);

		if (pChildItem == NULL)
			continue;

		SetDrawEffect(pChildItem, bDraw, nSleep);

		if (pTreeItem->bExpand == TRUE)
		{
			pChildItem->bDraw = bDraw;

			if (nSleep > 0)
			{
				RedrawNoScroll();
				Sleep((DWORD)nSleep);
			}
		}
		
	}
}

/**********************************************************
@brief 마우스 왼쪽버튼 클릭시 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	stTreeItem *pTreeItem = NULL;
	SetFocus();

	pTreeItem = PtInItemExpand(point);
	
	if (pTreeItem != NULL)
	{
		int nChildCnt = 0;
		int nSleepTime = 0;

		GetChildCount(nChildCnt, pTreeItem);

		if (nChildCnt <= 10)
			nSleepTime = 30;
		else if (nChildCnt > 10 && nChildCnt <= 30)
			nSleepTime = 20;
		else
			nSleepTime = 0;


		AllDeselectItems(&m_treeItems);
		pTreeItem->bSelect = TRUE;

		if (pTreeItem->bExpand == TRUE)
			SetDrawEffect(pTreeItem, FALSE, nSleepTime);

		pTreeItem->bExpand = !pTreeItem->bExpand;
		
		if (pTreeItem->bExpand == TRUE)
			SetDrawEffect(pTreeItem, TRUE, nSleepTime);

		InvalidateNoScroll();
	}
	else
	{
		pTreeItem = PtInItem(point);

		if (pTreeItem != NULL)
		{
			AllDeselectItems(&m_treeItems);

			pTreeItem->bSelect = TRUE;
			InvalidateNoScroll();
		}
	}

	if (GetParent() != NULL && pTreeItem != NULL)
		GetParent()->PostMessage(WM_TREE_LBTN_CLICK, (WPARAM)pTreeItem);
	
	SettingScrollBar();

	CWnd::OnLButtonDown(nFlags, point);
}

/**********************************************************
@brief 마우스 왼쪽버튼 더블클릭시 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	stTreeItem *pTreeItem = PtInItem(point);

	if (pTreeItem != NULL)
	{
		int nChildCnt = 0;
		int nSleepTime = 0;
		
		GetChildCount(nChildCnt, pTreeItem);

		if (nChildCnt <= 10)
			nSleepTime = 30;
		else if (nChildCnt > 10 && nChildCnt <= 20)
			nSleepTime = 20;
		else
			nSleepTime = 0;


		AllDeselectItems(&m_treeItems);
		pTreeItem->bSelect = TRUE;

		if (pTreeItem->bExpand == TRUE)
			SetDrawEffect(pTreeItem, FALSE, nSleepTime);

		pTreeItem->bExpand = !pTreeItem->bExpand;

		if (pTreeItem->bExpand == TRUE)
			SetDrawEffect(pTreeItem, TRUE, nSleepTime);

		InvalidateNoScroll();

		if (GetParent() != NULL)
			GetParent()->PostMessage(WM_TREE_LBTN_DBCLICK, (WPARAM)pTreeItem);
	}
	
	SettingScrollBar();

	CWnd::OnLButtonDblClk(nFlags, point);
}

/**********************************************************
@brief 스크롤바 생성
@details 
@param nVerticalScrollID   Vertical 스크롤 리소스 아이디
@param nHorizontalScrollID  Horizontal 스크롤 리소스 아이디
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
BOOL CSkinTreeCtrl::CreateScrollBar(UINT nVerticalScrollID, UINT nHorizontalScrollID)
{
	if (nVerticalScrollID == 0 && nHorizontalScrollID == 0)
		return FALSE;

	m_pVerticalScroll		= new CScrollBar;
	m_pHorizontalScroll		= new CScrollBar;

	if (m_pVerticalScroll != NULL && nVerticalScrollID != 0)
	{
		if (FALSE == m_pVerticalScroll->Create(WS_VISIBLE | WS_CHILD | SB_VERT, CRect(0, 0, 0, 0), this, nVerticalScrollID))
		{
			TRACE(_T("CSkinTreeCtrl::CreateScrollBar VerticalScrool create fail!"));
			return FALSE;
		}
	}

	if (m_pHorizontalScroll != NULL && nHorizontalScrollID != 0)
	{
		if (FALSE == m_pHorizontalScroll->Create(WS_VISIBLE | WS_CHILD | SB_HORZ, CRect(0, 0, 0, 0), this, nHorizontalScrollID))
		{
			TRACE(_T("CSkinTreeCtrl::CreateScrollBar HorizontalScroll create fail!"));
			return FALSE;
		}
	}

	return TRUE;
}

/**********************************************************
@brief 리스트에서 가장긴 텍스트의 넓이 구하는 함수
@details 
@param pTreeItems 구할 리스트 
@param &nLongTextWidth 넓이 참조로 리턴
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::GetLongTextWidth(CList<stTreeItem*, stTreeItem*> *pTreeItems, int &nLongTextWidth)
{
	POSITION	pos				= pTreeItems->GetHeadPosition();
	stTreeItem	*pTreeItem		= NULL;
	stTreeItem  *pParentItem	= NULL;
	int			nParentCnt		= 0;

	while (pos)
	{
		pTreeItem = pTreeItems->GetNext(pos);

		if (pTreeItem == NULL)
			break;

		nParentCnt = GetParentCnt(pTreeItem);
		pParentItem = pTreeItem->pParentItem;
	
		if (pParentItem == NULL || IsParentExpand(pTreeItem) == TRUE)
		{
			CClientDC	dc(this);
			CFont		font, *pOldFont = NULL;

			font.CreateFont(m_nFontSize, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));    

			pOldFont = dc.SelectObject(&font);

			
			CSize sizeText = dc.GetOutputTextExtent(pTreeItem->strText);
			pOldFont = dc.SelectObject(&font);

			if (nLongTextWidth < sizeText.cx + (nParentCnt * DEF_CHILD_GAP + DEF_LEFT_GAP*5 + DEF_ICON_SIZE) + DEF_RIGHT_GAP)
			{
				nLongTextWidth = sizeText.cx + (nParentCnt * DEF_CHILD_GAP + DEF_LEFT_GAP*5 + DEF_ICON_SIZE) + DEF_RIGHT_GAP;
			}
		}

		//재귀 함수
		GetLongTextWidth(&pTreeItem->listChilds, nLongTextWidth);

	}
}


/**********************************************************
@brief 스크롤바 Show hide 설정
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::SettingScrollBar()
{
	CRect		rect;

	GetClientRect(&rect);

	int nLongTextWidth = 0;
	int nHorGap = 0;

	GetLongTextWidth(&m_treeItems, nLongTextWidth);


	if (m_pHorizontalScroll != NULL)
	{
		if (m_pHorizontalScroll->GetSafeHwnd())
		{
			if (m_pHorizontalScroll->IsWindowVisible())
				nHorGap = DEF_TREE_SCROLL_SIZE;
		}
	}


	int			nWidth	= max(nLongTextWidth, rect.right);
	//int			nWidth	= max(rect.right, rect.right);
	int			nHeight	= max(GetTotalHeight() + nHorGap, rect.bottom);
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
				m_pVerticalScroll->ShowScrollBar(FALSE);
		}
	}

	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
		{
			if (m_pVerticalScroll->IsWindowVisible())
				nVerGap = DEF_TREE_SCROLL_SIZE;
		}
	}

//	m_pHorizontalScroll->ShowScrollBar(FALSE);
	//ween8
	if (m_pHorizontalScroll != NULL)
	{
		if (m_pHorizontalScroll->GetSafeHwnd())
		{
			if(rect.Width() < nWidth)
			{
				si.nMin		= 0;
				si.nMax		= nWidth;
				si.nPage	= rect.Width();
				si.nPos		= GetScrollPos(m_pHorizontalScroll);

				m_pHorizontalScroll->SetScrollInfo(&si, TRUE);
				m_pHorizontalScroll->ShowScrollBar(TRUE);
			}
			else
				m_pHorizontalScroll->ShowScrollBar(FALSE);
		}
	}

}

/**********************************************************
@brief 스크롤 영역을 제외한 부분 invalidate
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::InvalidateNoScroll()
{
	if (GetSafeHwnd() == NULL)
		return;

	CRect	rect;
	int		nVerGap = 0;
	int		nHorGap = 0;

	GetClientRect(&rect);

	if (m_pVerticalScroll != NULL && m_pVerticalScroll->GetSafeHwnd())
	{
		if (m_pVerticalScroll->IsWindowVisible())
			nVerGap = DEF_TREE_SCROLL_SIZE;
	}

	if (m_pHorizontalScroll != NULL)
	{
		if (m_pHorizontalScroll->IsWindowVisible() && m_pHorizontalScroll->GetSafeHwnd())
			nHorGap = DEF_TREE_SCROLL_SIZE;
	}
	InvalidateRect(CRect(rect.left, rect.top, rect.right - nVerGap, rect.bottom - nHorGap));
}

/**********************************************************
@brief 스크롤 영역을 제외한 부분 redrawwindow
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::RedrawNoScroll()
{
	CRect	rect;
	int		nVerGap = 0;
	int		nHorGap = 0;

	GetClientRect(&rect);

	if (m_pVerticalScroll != NULL && m_pVerticalScroll->GetSafeHwnd())
	{
		if (m_pVerticalScroll->IsWindowVisible())
			nVerGap = DEF_TREE_SCROLL_SIZE;
	}

	if (m_pHorizontalScroll != NULL && m_pHorizontalScroll->GetSafeHwnd())
	{
		if (m_pHorizontalScroll->IsWindowVisible())
			nHorGap = DEF_TREE_SCROLL_SIZE;
	}

	RedrawWindow(CRect(rect.left, rect.top, rect.right - nVerGap, rect.bottom - nHorGap), NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
}

/**********************************************************
@brief 스크롤 위치 리턴
@details 
@param pScrollBar 스크롤 포인터 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
int CSkinTreeCtrl::GetScrollPos(CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL)
		return 0;

	SCROLLINFO si = {0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;

	::GetScrollInfo(pScrollBar->GetSafeHwnd(), SB_CTL, &si);

	return si.nPos;
}

/**********************************************************
@brief 스크롤바 사이즈 리턴
@details 
@param pScrollBar 스크롤바 포인터 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
int	CSkinTreeCtrl::GetScrollbarSize(CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL)
		return 0;

	if (pScrollBar->GetSafeHwnd() == NULL)
		return 0;

	return pScrollBar->IsWindowVisible() == TRUE ? DEF_TREE_SCROLL_SIZE : 0;
}

/**********************************************************
@brief 스크롤 트래팅 위치 리턴
@details 
@param pScrollBar 스크롤 포인터 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
int CSkinTreeCtrl::GetScrollTrackPos(CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL)
		return 0;

	SCROLLINFO si = {0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;

	::GetScrollInfo(pScrollBar->GetSafeHwnd(), SB_CTL, &si);

	return si.nTrackPos;
}

/**********************************************************
@brief 윈도우 사이즈 변경시 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_pVerticalScroll != NULL)
	{
		if (m_pVerticalScroll->GetSafeHwnd())
			m_pVerticalScroll->MoveWindow(cx - DEF_TREE_SCROLL_SIZE, 0, DEF_TREE_SCROLL_SIZE, cy);
	}

	if (m_pHorizontalScroll != NULL)
	{
		if (m_pHorizontalScroll->GetSafeHwnd())
			m_pHorizontalScroll->MoveWindow(0, cy - DEF_TREE_SCROLL_SIZE, cx - DEF_TREE_SCROLL_SIZE, DEF_TREE_SCROLL_SIZE);
	}

	SettingScrollBar();
}

/**********************************************************
@brief vertical 스크롤 위치 변경시 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nScrollPos = GetScrollPos(m_pVerticalScroll);

	switch (nSBCode)
	{
	case SB_PAGEDOWN:   
	case SB_LINEDOWN:
		nScrollPos += DEF_TREE_ITEM_HEIGHT;
		break;

	case SB_LINEUP:
	case SB_PAGEUP:   
		nScrollPos -= DEF_TREE_ITEM_HEIGHT;
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


	InvalidateNoScroll();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


int CSkinTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	return 0;
}

/**********************************************************
@brief horizontal 스크롤 위치 변경시 타는 메시지
@details 
@param p1 
@param p2 
@param p3 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nScrollPos = GetScrollPos(m_pHorizontalScroll);

	switch (nSBCode)
	{

	case SB_LINELEFT:
	case SB_PAGELEFT:   
		nScrollPos -= 10;
		break;
	case SB_PAGERIGHT:   
	case SB_LINERIGHT:
		nScrollPos += 10;
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


	InvalidateNoScroll();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

/**********************************************************
@brief 아이템의 텍스트 설정
@details 
@param pos 아이템 포지션값 
@param strText 아이템의 텍스트 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::SetItemText(POSITION pos, CString strText)
{
	if (pos == NULL)
		return;

	stTreeItem *pTreeItem = GetItem(&m_treeItems, pos);

	if (pTreeItem != NULL)
	{
		pTreeItem->strText = strText;
		InvalidateNoScroll();
	}
}

/**********************************************************
@brief 아이템의 텍스트 리턴
@details 
@param pos 아이템의 포지션값 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
CString	CSkinTreeCtrl::GetItemText(POSITION pos)
{
	if (pos == NULL)
		return _T("");

	stTreeItem *pTreeItem = GetItem(&m_treeItems, pos);

	return pTreeItem != NULL ? pTreeItem->strText : _T("");
}

/**********************************************************
@brief 아이템에 유저 데이터 설정
@details 
@param pos 아이템의 포지션값
@param pData 셋팅할 유저데이터
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::SetItemData(POSITION pos, void *pData)
{
	if (pos == NULL)
		return;

	stTreeItem *pTreeItem = GetItem(&m_treeItems, pos);

	if (pTreeItem != NULL)
	{
		pTreeItem->pData = pData;
		InvalidateNoScroll();
	}
}

/**********************************************************
@brief 설정된 유저데이터 리턴
@details 
@param pos 아이템의 포지션값
@return 유저데이터 리턴
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void* CSkinTreeCtrl::GetItemData(POSITION pos)
{
	if (pos == NULL)
		return NULL;

	stTreeItem *pTreeItem = GetItem(&m_treeItems, pos);

	return pTreeItem != NULL ? pTreeItem->pData : NULL;
}


/**********************************************************
@brief 마우스 오른쪽 버튼 클릭시 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	stTreeItem *pTreeItem =NULL;

	pTreeItem = PtInItem(point);

	if (pTreeItem != NULL)
	{
		AllDeselectItems(&m_treeItems);

		pTreeItem->bSelect = TRUE;
		InvalidateNoScroll();

		if (GetParent() != NULL)
			GetParent()->PostMessage(WM_TREE_RBTN_CLICK, (WPARAM)pTreeItem);
	}

	CWnd::OnRButtonDown(nFlags, point);
}

BOOL CSkinTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_MOUSEWHEEL)
		SetMouseWheel((short)HIWORD(pMsg->wParam));
	
	return CWnd::PreTranslateMessage(pMsg);
}
