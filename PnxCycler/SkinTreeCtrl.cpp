// SkinTreeCtrl.cpp : ���� �����Դϴ�.
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
@param zDelta ���� ���������� �Ʒ��� ���������� üũ 
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
@brief ������ ����
@details 
@param pos  ������ ������
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
@brief ��ü ������ ����
@details 
@param pTreeItems ������ ������ ����Ʈ
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
@brief ��ũ�� ����
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
@brief ������ ���� ����
@details 
@param pos ������ �����ǰ� 
@return ������ ��������
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
@brief ������ ���� ����
@details 
@param pTreeItems �� ����Ʈ�ȿ��ִ� �������� �ش� �������� ������� ���� ����
@param pos ������ ������ ������ pos
@return ������ ��������
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
@brief ������ �߰�
@details 
@param posParent �θ� ������ 
@param pTreeItem �߰��� ������ ���� 
@return  ���� �߰� �Ǹ� �����ǰ� ����
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

	//��Ʈ
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
@brief Expand �̹��� ����
@details 
@param strNormalImagePath �⺻ �̹���
@param strMoreImagePath  expand�Ǿ����� �̹���
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
@brief �������� �θ� ���� ���ϴ� �Լ�
@details 
@param pTreeItem ���� ������������
@return ���� ī��Ʈ ����
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
@brief Expand �����
@details 
@param pDC DC
@param rect �׷��� ��ġ
@param bExpand expand ����
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
@brief �θ� expand�Ǿ����� ����
@details 
@param pTreeItem �� �������� �θ�
@return expand ���� ����
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
@brief ��ü Ʈ�� �������� ���� ����
@details 
@param &nTotalHeight ������ ���� 
@param pTreeItems �ش� ����Ʈ�� ���� ���� 
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
		
		//��� �Լ�
		GetTotalHeight(nTotalHeight, &pTreeItem->listChilds);
	}
}

/**********************************************************
@brief ��ü �����۵��� ����
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
@brief Expand ������ Ŭ���Ǿ����� ����
@details 
@param point Ŭ���� ��ġ 
@return Ŭ���� ������ ��������
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
@brief  Expand ������ Ŭ���Ǿ����� ����
@details 
@param pTreeItems �˻� �� ����Ʈ
@param point Ŭ���� ��ġ
@param nHeight ��ͷ����� nHeight ���������� ����
@return Ŭ���� ������ ��������
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

		//��� �Լ�
		pTreeItem = PtInItemExpand(&pTreeItem->listChilds, point, nHeight);

		if (pTreeItem != NULL)
			return pTreeItem;
	}

	return NULL;
}

/**********************************************************
@brief ���� ù��°�� �׷��� ������ ����
@details 
@param pTreeItems �� ����Ʈ �� �˻� 
@param &nTop  ����Լ��� ���� ���� ����
@return ���� ù��° ������ ���� ����
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

		//��� �Լ�
		pTreeItem = GetFirstDrawItem(&pTreeItem->listChilds, nTop);

		if (pTreeItem != NULL)
			return pTreeItem;
	}

	return NULL;
}

/**********************************************************
@brief �ش� ����Ʈ�� �������� ���õǾ��ٸ� ������ ���� ����
@details 
@param pTreeItems �˻��� ����Ʈ 
@param point Ŭ���� ��ġ 
@param &nHeight ��͸� ���� ���� ����
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

		//��� �Լ�
		pTreeItem = PtInItem(&pTreeItem->listChilds, point, nHeight);

		if (pTreeItem != NULL)
			return pTreeItem;
	}

	return NULL;
}

/**********************************************************
@brief �ش� ����Ʈ�� �������� ���õǾ��ٸ� ������ ���� ����
@details 
@param point Ŭ���� ����Ʈ 
@return ������ ���� ����
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
@brief Expand �̹��� ��뿩�� ����
@details 
@return ��뿩��
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
@brief �ش� ����Ʈ�� ������ �����
@details 
@param pTreeItems  
@param nTop ����Ʈ�� �׷��� Top�� 
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

			//�����ۺ� ������
			pDC->FillSolidRect(0, rcItem.top+1, rcClient.right, rcItem.bottom - rcItem.top, pTreeItem->colorItem);


			//Expand ǥ�� Draw
			if (pTreeItem->listChilds.GetCount() > 0 && IsParentExpand(pTreeItem) == TRUE)
				DrawExpand(pDC, rcExpand, pTreeItem->bExpand);

			//������ Draw
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
		
			//������ ���ý� ���󺯰�
			if (pTreeItem->bSelect == TRUE)
			{
				SolidBrush		brushDeep(m_colorSelectItem);
				grs.FillRectangle(&brushDeep, 0, rcItem.top, rcClient.right, rcItem.bottom - rcItem.top);
			}

			nTop++;
		}
		
		//pTreeItem->bDraw = TRUE;

		//��� �Լ�
		DrawItems(pDC, &pTreeItem->listChilds, nTop);
	}

	pDC->SelectObject(pOldFont);

	font.DeleteObject();
}

/**********************************************************
@brief ������ ����Ʈ ����
@details 
@param pTreeItems �ش� ����Ʈ�� ������ ����Ʈ ����
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

		//��� �Լ�
		AllDeselectItems(&pTreeItem->listChilds);
	}
}


/**********************************************************
@brief �����
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
@brief �������� child�� ī��Ʈ ����
@details 
@param &nChildCnt  childī��Ʈ ��������
@param pTreeItem   ���� ������
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
@brief Draw�� Effect ����
@details 
@param pTreeItem effect������ ������
@param bDraw	�׸��� �׸��� ������ ����
@param nSleep ȿ�� �ð�
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
@brief ���콺 ���ʹ�ư Ŭ���� Ÿ�� �޽���
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
@brief ���콺 ���ʹ�ư ����Ŭ���� Ÿ�� �޽���
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
@brief ��ũ�ѹ� ����
@details 
@param nVerticalScrollID   Vertical ��ũ�� ���ҽ� ���̵�
@param nHorizontalScrollID  Horizontal ��ũ�� ���ҽ� ���̵�
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
@brief ����Ʈ���� ����� �ؽ�Ʈ�� ���� ���ϴ� �Լ�
@details 
@param pTreeItems ���� ����Ʈ 
@param &nLongTextWidth ���� ������ ����
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

		//��� �Լ�
		GetLongTextWidth(&pTreeItem->listChilds, nLongTextWidth);

	}
}


/**********************************************************
@brief ��ũ�ѹ� Show hide ����
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
@brief ��ũ�� ������ ������ �κ� invalidate
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
@brief ��ũ�� ������ ������ �κ� redrawwindow
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
@brief ��ũ�� ��ġ ����
@details 
@param pScrollBar ��ũ�� ������ 
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
@brief ��ũ�ѹ� ������ ����
@details 
@param pScrollBar ��ũ�ѹ� ������ 
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
@brief ��ũ�� Ʈ���� ��ġ ����
@details 
@param pScrollBar ��ũ�� ������ 
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
@brief ������ ������ ����� Ÿ�� �޽���
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
@brief vertical ��ũ�� ��ġ ����� Ÿ�� �޽���
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
@brief horizontal ��ũ�� ��ġ ����� Ÿ�� �޽���
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
@brief �������� �ؽ�Ʈ ����
@details 
@param pos ������ �����ǰ� 
@param strText �������� �ؽ�Ʈ 
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
@brief �������� �ؽ�Ʈ ����
@details 
@param pos �������� �����ǰ� 
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
@brief �����ۿ� ���� ������ ����
@details 
@param pos �������� �����ǰ�
@param pData ������ ����������
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
@brief ������ ���������� ����
@details 
@param pos �������� �����ǰ�
@return ���������� ����
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
@brief ���콺 ������ ��ư Ŭ���� Ÿ�� �޽���
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_MOUSEWHEEL)
		SetMouseWheel((short)HIWORD(pMsg->wParam));
	
	return CWnd::PreTranslateMessage(pMsg);
}
