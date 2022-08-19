// TabCtrlEx.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "TabCtrlEx.h"


// CTabCtrlEx

IMPLEMENT_DYNAMIC(CTabCtrlEx, CTabCtrl)

CTabCtrlEx::CTabCtrlEx()
{
	m_Font.CreateFont(14,
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Chicago"));	
}

CTabCtrlEx::~CTabCtrlEx()
{
	m_Font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CTabCtrlEx 메시지 처리기입니다.

void	CTabCtrlEx::SetBkColor(int nCurSel, BOOL bChange)
{
	auto Pos = m_listChange.FindIndex(nCurSel);

	if (Pos)
	{
		m_listChange.SetAt(Pos, bChange);
	}
}

void	CTabCtrlEx::SetTabCtrlEnd()
{
	for (int i = 0; i < GetItemCount() ; ++i)
		m_listChange.AddTail(FALSE);
}

void CTabCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nCurSel = lpDrawItemStruct->itemID;

	if (nCurSel<0)
		return;

	char szText[40] = { 0,};

	TC_ITEM item;

	//item.mask = TCIF_TEXT | TCIF_IMAGE;
	item.mask = TCIF_TEXT;
	item.pszText = szText;
	item.cchTextMax = 40;

	if (!GetItem(nCurSel, &item))
		return;

	SetFont(&m_Font);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rt = lpDrawItemStruct->rcItem;

	rt.top += ::GetSystemMetrics(SM_CYEDGE);

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rt, ColorWhite);

	
	
	/*
	CImageList* pImageList = GetImageList();

	if (pImageList != NULL && item.iImage >= 0)
	{
		rt.left += pDC->GetTextExtent(" ").cx;

		IMAGEINFO img;

		pImageList->GetImageInfo(item.iImage, &img);

		CRect rtImg(img.rcImage);

		pImageList->Draw(pDC, item.iImage, CPoint(rt.left, rt.top), ILD_TRANSPARENT);

		rt.left += rtImg.Width();
	}
	*/

	//pDC->FillSolidRect(rt, m_clrBkColor);

	rt.top -= ::GetSystemMetrics(SM_CYEDGE);

	auto Pos = m_listChange.FindIndex(nCurSel);

	if (Pos)
	{
		BOOL bChange = m_listChange.GetAt(Pos);

		if (TRUE == bChange)
			pDC->SetTextColor(ColorLightRed);
		else
			pDC->SetTextColor(ColorLightBlack);
	}

	pDC->DrawText(szText, rt, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}


BOOL CTabCtrlEx::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	GetClientRect(&rt);

	rt.top += 20;

	pDC->FillSolidRect(rt, ColorWhite);

	//return CTabCtrl::OnEraseBkgnd(pDC);

	return TRUE;
}
