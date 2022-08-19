// SkinButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SkinButton.h"

// CSkinButton

#pragma warning(disable:4100)

IMPLEMENT_DYNAMIC(CSkinButton, CWnd)

CSkinButton::CSkinButton()
{
	m_eBtnStatus	= E_STATUS_NORMAL;
	m_eBtnType		= E_TYPE_NORMAL;
	m_bLBtnClicked	= FALSE;
	m_nFontSize		= 15;
	m_colorText		= RGB(40, 40, 40);
	m_bUseEffect	= TRUE;
	m_pGroupOwner	= NULL;
	m_nTextAlign	= DT_LEFT;
	m_eBtnImageHalf = E_HALF_1;



}

CSkinButton::~CSkinButton()
{
	m_arrRadioGorup.RemoveAll();

}


BEGIN_MESSAGE_MAP(CSkinButton, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


BOOL CSkinButton::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;//CWnd::OnEraseBkgnd(pDC);
}

/**********************************************************
@brief 버튼 타입 설정
@details 
@param eBtnType 버튼 타입  
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::SetBtnType(E_BTN_TYPE eBtnType)
{ 
	m_eBtnType = eBtnType;

	if (m_eBtnType == E_TYPE_STATIC)
	{
		m_nTextAlign = DT_CENTER;
		EnableWindow(FALSE);
	}
}

/**********************************************************
@brief 버튼 드로잉
@details 
@param pDC 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::DrawBtn(CDC *pDC)
{
	Image		*pImage = NULL;
	Graphics	grs(pDC->GetSafeHdc());
	CRect		rcClient;
	CString		strText;
	CFont		font, *pOldFont	= NULL;
	int			nTextLeftGap = 0;

	font.CreateFont(m_nFontSize, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));    

	pOldFont = pDC->SelectObject(&font);

	GetClientRect(&rcClient);

	if (IsWindowEnabled() == FALSE && m_eBtnType == E_TYPE_NORMAL)
		m_eBtnStatus = E_STATUS_DISABLE;

	switch (m_eBtnStatus)
	{
	case E_STATUS_NORMAL:
		pImage = new Image((CStringW)m_strNormalImagePath);
		break;

	case E_STATUS_OVER:
		pImage = new Image((CStringW)m_strOverImagePath);
		break;

	case E_STATUS_CLICK:
		pImage = new Image((CStringW)m_strClickImagePath);
		break;

	case E_STATUS_DISABLE:
		pImage = new Image((CStringW)m_strDisableImagePath);
		break;
	}

	if (pImage != NULL)
	{
		int nDrawEffect = 0;
		int nImageSize	= 0;
		int nImageStep	= 0;

		if (m_bLBtnClicked == TRUE && m_bUseEffect == TRUE)
			nDrawEffect = 1;
		

		if (m_eBtnImageHalf == E_HALF_1)
		{
			nImageStep = 0;
			nImageSize = pImage->GetWidth();
		}
		else if (m_eBtnImageHalf == E_HALF_2)
		{
			nImageSize = pImage->GetWidth() / 2;
			
			if (m_eBtnStatus == E_STATUS_NORMAL || m_eBtnStatus == E_STATUS_OVER || m_eBtnStatus == E_STATUS_DISABLE)
				nImageStep = 0;
			else
				nImageStep = 1;
		}
		else if (m_eBtnImageHalf == E_HALF_4)
		{
			nImageSize = pImage->GetWidth() / 4;
			nImageStep = m_eBtnStatus;
		}

		grs.DrawImage(pImage, Rect(nDrawEffect, nDrawEffect, nImageSize-nDrawEffect, pImage->GetHeight()-nDrawEffect), 
			nImageStep * nImageSize, 0, nImageSize, pImage->GetHeight(), UnitPixel);

		nTextLeftGap = pImage->GetWidth() + 3;

		delete pImage;
		pImage = NULL;
	}

	GetWindowText(strText);

	pDC->SetTextColor(m_colorText);

	if (m_eBtnType == E_TYPE_RADIO || m_eBtnType == E_TYPE_CHECK_BOX)
		pDC->DrawText(strText, CRect(rcClient.left + nTextLeftGap, rcClient.top, rcClient.right, rcClient.bottom), m_nTextAlign | DT_SINGLELINE | DT_VCENTER);
	else
		pDC->DrawText(strText, rcClient, m_nTextAlign | DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject(pOldFont);
	font.DeleteObject();
}


/**********************************************************
@brief 버튼 이미지 설정
@details 
@param eBtnImageHalf 이미지가 몇분할로 되어있는지...
@param strNormalPath	normal상태일때 보일 이미지
@param strOverPath      마우스 over 상태일때 보일 이미지
@param strClickPath     마우스 click 상태일때 보일 이미지
@param strDisablePath   마우스 disable 상태일때 보일 이미지
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
BOOL CSkinButton::SetBtnImagePath(E_BTN_IMAGE_HALF eBtnImageHalf, CString strNormalPath, CString strOverPath, CString strClickPath, CString strDisablePath)
{
	if (strNormalPath.IsEmpty() == TRUE || strOverPath.IsEmpty() == TRUE || strClickPath.IsEmpty() == TRUE || strDisablePath.IsEmpty() == TRUE)
		return FALSE;
	
	m_eBtnImageHalf			= eBtnImageHalf;
	m_strNormalImagePath	= strNormalPath; 
	m_strOverImagePath		= strOverPath;
	m_strClickImagePath		= strClickPath;
	m_strDisableImagePath	= strDisablePath;

	Image img((CStringW)m_strNormalImagePath);

	if (GetParent() != NULL)
	{
		CRect rect;
		GetWindowRect(&rect);
		GetParent()->ScreenToClient(&rect);

		int nImageWidth = 0;

		if (eBtnImageHalf == E_HALF_1)
			nImageWidth = img.GetWidth();
		else if (eBtnImageHalf == E_HALF_2)
			nImageWidth = img.GetWidth() / 2;
		else if (eBtnImageHalf == E_HALF_4)
			nImageWidth = img.GetWidth() / 4;

		if (m_eBtnType == E_TYPE_RADIO || m_eBtnType == E_TYPE_CHECK_BOX)
		{
			CDC		*pDC = GetDC();
			CSize	sizeText;

			CString strText;
			GetWindowText(strText);

			if (pDC != NULL && strText.IsEmpty() == FALSE)
			{
				CFont		font, *pOldFont	= NULL;

				font.CreateFont(m_nFontSize, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));    

				pOldFont = pDC->SelectObject(&font);

				sizeText = pDC->GetOutputTextExtent(strText);

				pDC->SelectObject(pOldFont);

				font.DeleteObject();
				ReleaseDC(pDC);
			}

			MoveWindow(rect.left, rect.top, nImageWidth + sizeText.cx + 3, img.GetHeight());

		}
		else
		{
			MoveWindow(rect.left, rect.top, nImageWidth, img.GetHeight());
		}
	}
	
	return TRUE;
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
void CSkinButton::OnPaint()
{
	CPaintDC	dc(this); 


	CDC			memDC;
	CBitmap		bitmap, *pOldBitmap = NULL;
	CRect		rcClient;

	GetClientRect(&rcClient);

	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.right, rcClient.bottom);

	pOldBitmap = memDC.SelectObject(&bitmap);
	int nBKMode = memDC.SetBkMode(TRANSPARENT);


	if (m_cdcBK.GetSafeHdc())
		memDC.BitBlt(0, 0, rcClient.right, rcClient.bottom, &m_cdcBK, 0, 0, SRCCOPY);
	
	DrawBtn(&memDC);
	
	dc.BitBlt(0, 0, rcClient.right, rcClient.bottom, &memDC, 0, 0, SRCCOPY);

	memDC.SetBkMode(nBKMode);

	memDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	memDC.DeleteDC();
}

/**********************************************************
@brief 마우스 무브시 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.hwndTrack = this->GetSafeHwnd();
	tme.dwHoverTime = 25;

	_TrackMouseEvent(&tme);


	CWnd::OnMouseMove(nFlags, point);
}

/**********************************************************
@brief 마우스가 클라이언트 영역에 들어왔을때 타는 메시지
@details  
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::OnMouseHover(UINT nFlags, CPoint point)
{
	switch (m_eBtnType)
	{
	case E_TYPE_NORMAL:		//일반버튼
		{
			if (m_bLBtnClicked == TRUE)
				m_eBtnStatus = E_STATUS_CLICK;
			else
				m_eBtnStatus = E_STATUS_OVER;
		}
		break;

	case E_TYPE_CHECK_BOX:	//체크박스
		break;

	case E_TYPE_RADIO:		//라이오버튼
		break;

	case E_TYPE_STATIC:		//Static 컨트롤
		break;
	}

	Invalidate();

	CWnd::OnMouseHover(nFlags, point);
}

/**********************************************************
@brief 마우스가 클라이언트 영역에서 벗어났을때 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::OnMouseLeave()
{
	switch (m_eBtnType)
	{
	case E_TYPE_NORMAL:		//일반버튼
		m_eBtnStatus = E_STATUS_NORMAL;
		break;

	case E_TYPE_CHECK_BOX:	//체크박스
		break;

	case E_TYPE_RADIO:		//라이오버튼
		break;

	case E_TYPE_STATIC:		//Static 컨트롤
		break;
	}
	Invalidate();

	CWnd::OnMouseLeave();
}


/**********************************************************
@brief 마우스 왼쪽 버튼이 클릭되었을때 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	if (IsWindowEnabled() == TRUE)
		m_bLBtnClicked = TRUE;

	switch (m_eBtnType)
	{
	case E_TYPE_NORMAL:		//일반버튼
		{
			if (IsWindowEnabled() == FALSE)
			{
				m_eBtnStatus = E_STATUS_DISABLE;
				Invalidate();
				return;
			}

			m_eBtnStatus = E_STATUS_CLICK;
		}
		break;

	case E_TYPE_CHECK_BOX:	//체크박스
		break;

	case E_TYPE_RADIO:		//라이오버튼
		break;

	case E_TYPE_STATIC:		//Static 컨트롤
		break;
	}

	Invalidate();
	CWnd::OnLButtonDown(nFlags, point);
}

/**********************************************************
@brief 마우스 왼쪽 버튼이 올라갔을때 타는 메시지
@details 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (m_bLBtnClicked == FALSE)
	{
		CWnd::OnLButtonUp(nFlags, point);
		return;
	}

	m_bLBtnClicked = FALSE;


	CRect rcClient;
	GetClientRect(&rcClient);

	switch (m_eBtnType)
	{
	case E_TYPE_NORMAL:		//일반버튼
		{
			if (IsWindowEnabled() == FALSE)
			{
				m_eBtnStatus = E_STATUS_DISABLE;
				Invalidate();
				return;
			}

			if (rcClient.PtInRect(point) == TRUE)
				m_eBtnStatus = E_STATUS_OVER;
			else
				m_eBtnStatus = E_STATUS_NORMAL;

			if (GetParent() != NULL)
			{
				UINT nID = GetDlgCtrlID();

				GetParent()->SendMessage(WM_COMMAND, nID, 0);
			}
		}
		break;

	case E_TYPE_CHECK_BOX:	//체크박스
		{
			if (IsWindowEnabled() == FALSE)
			{
				Invalidate();
				return;
			}

			m_eBtnStatus = (m_eBtnStatus == E_STATUS_NORMAL ? E_STATUS_CLICK : E_STATUS_NORMAL);
			
			if (GetParent() != NULL)
			{
				UINT nID = GetDlgCtrlID();

				GetParent()->SendMessage(WM_COMMAND, nID, 0);
			}
		}
		
		break;

	case E_TYPE_RADIO:		//라디오버튼
		{
			if (IsWindowEnabled() == FALSE)
			{
				Invalidate();
				return;
			}

			m_eBtnStatus = E_STATUS_CLICK;

			OnRadioClick(this);

			if (GetParent() != NULL)
			{
				UINT nID = GetDlgCtrlID();

				GetParent()->SendMessage(WM_COMMAND, nID, 0);
			}
		}
		break;

	case E_TYPE_STATIC:		//Static 컨트롤
		break;
	}

	Invalidate();

	CWnd::OnLButtonUp(nFlags, point);
}


/**********************************************************
@brief 버튼의 이미지 투명 처리를 위하여 부모의 onpaint에서 DC를 셋팅해준다.
@details 
@param pDC 부모의 DC 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::SetBkDC(CDC *pDC)
{
	CRect rect, rectS;
	CBitmap bmp, *pOldBitmap;

	GetClientRect(rect);
	GetWindowRect(rectS);
	GetParent()->ScreenToClient(rectS);

	m_cdcBK.DeleteDC();

	m_cdcBK.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	pOldBitmap = m_cdcBK.SelectObject(&bmp);


	m_cdcBK.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rectS.left, rectS.top, SRCCOPY);
	bmp.DeleteObject();


}

/**********************************************************
@brief 타입이 체크박스또는 라디오 버튼일때 강제로 체크
@details 
@param bCheck 체크 유무 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::SetCheck(BOOL bCheck)
{
	if (m_eBtnType == E_TYPE_CHECK_BOX)
	{
		m_eBtnStatus = (bCheck == TRUE ? E_STATUS_CLICK : E_STATUS_NORMAL);
		Invalidate();
	}
	else if (m_eBtnType == E_TYPE_RADIO) //강제로 체크 못하게 못함
	{
		//나머지 hide 그룹장에게 알림
		m_eBtnStatus = (bCheck == TRUE ? E_STATUS_CLICK : E_STATUS_NORMAL);
	
		OnRadioClick(this);
		
		Invalidate();
	}
}

/**********************************************************
@brief 타입이 체크박스또는 라디오 버튼일때 강제로 체크
@details 
@param pCheckBtn 버튼 포인터
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::SetCheck(CSkinButton *pCheckBtn)
{
	for (int i=0; i<m_arrRadioGorup.GetCount(); i++)
	{
		CSkinButton *pSkinButton = m_arrRadioGorup.GetAt(i);

		if (pSkinButton != NULL && pSkinButton != pCheckBtn)
			pSkinButton->SetBtnStatus(E_STATUS_NORMAL);
	}
}

/**********************************************************
@brief 타입이 라디오 버튼일때 그룹 추가.
@details 
@param pSkinButton 추가될 버튼 포인터 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::AddRadioGroup(CSkinButton *pSkinButton)
{
	if (pSkinButton == NULL || pSkinButton == this)
		return;

	pSkinButton->SetGroupOwner(this);

	m_arrRadioGorup.Add(pSkinButton);
}

/**********************************************************
@brief 라디오 버튼 클릭시 타는 메시지
@details 
@param pClickBtn 클릭된 버튼 포인터
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::OnRadioClick(CSkinButton *pClickBtn)
{
	if (pClickBtn == NULL)
		return;

	CSkinButton *pGroupOwner = pClickBtn->GetGroupOwner();

	//현재 본인이 오너인경우
	if (pGroupOwner == NULL)
	{
		for (int i=0; i<m_arrRadioGorup.GetCount(); i++)
		{
			CSkinButton *pSkinButton = m_arrRadioGorup.GetAt(i);

			if (pSkinButton != NULL)
			{
				pSkinButton->SetBtnStatus(E_STATUS_NORMAL);
			}
		}

		SetBtnStatus(E_STATUS_CLICK);
	}
	else
	{
		pGroupOwner->SetBtnStatus(E_STATUS_NORMAL);
		pGroupOwner->SetCheck(pClickBtn);

		
	}
}


/**********************************************************
@brief 라디오버튼일때 그룹의 owner설정
@details 
@param pSkinButton owner 포인터 
@return 
@exception 
@bug 
@todo 
@see winforsys
*********************************************************/
void CSkinButton::SetGroupOwner(CSkinButton *pSkinButton)
{
	m_pGroupOwner = pSkinButton;
}
