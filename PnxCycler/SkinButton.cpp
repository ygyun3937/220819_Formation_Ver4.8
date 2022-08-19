// SkinButton.cpp : ���� �����Դϴ�.
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
@brief ��ư Ÿ�� ����
@details 
@param eBtnType ��ư Ÿ��  
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
@brief ��ư �����
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
@brief ��ư �̹��� ����
@details 
@param eBtnImageHalf �̹����� ����ҷ� �Ǿ��ִ���...
@param strNormalPath	normal�����϶� ���� �̹���
@param strOverPath      ���콺 over �����϶� ���� �̹���
@param strClickPath     ���콺 click �����϶� ���� �̹���
@param strDisablePath   ���콺 disable �����϶� ���� �̹���
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
@brief �����
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
@brief ���콺 ����� Ÿ�� �޽���
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
@brief ���콺�� Ŭ���̾�Ʈ ������ �������� Ÿ�� �޽���
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
	case E_TYPE_NORMAL:		//�Ϲݹ�ư
		{
			if (m_bLBtnClicked == TRUE)
				m_eBtnStatus = E_STATUS_CLICK;
			else
				m_eBtnStatus = E_STATUS_OVER;
		}
		break;

	case E_TYPE_CHECK_BOX:	//üũ�ڽ�
		break;

	case E_TYPE_RADIO:		//���̿���ư
		break;

	case E_TYPE_STATIC:		//Static ��Ʈ��
		break;
	}

	Invalidate();

	CWnd::OnMouseHover(nFlags, point);
}

/**********************************************************
@brief ���콺�� Ŭ���̾�Ʈ �������� ������� Ÿ�� �޽���
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
	case E_TYPE_NORMAL:		//�Ϲݹ�ư
		m_eBtnStatus = E_STATUS_NORMAL;
		break;

	case E_TYPE_CHECK_BOX:	//üũ�ڽ�
		break;

	case E_TYPE_RADIO:		//���̿���ư
		break;

	case E_TYPE_STATIC:		//Static ��Ʈ��
		break;
	}
	Invalidate();

	CWnd::OnMouseLeave();
}


/**********************************************************
@brief ���콺 ���� ��ư�� Ŭ���Ǿ����� Ÿ�� �޽���
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
	case E_TYPE_NORMAL:		//�Ϲݹ�ư
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

	case E_TYPE_CHECK_BOX:	//üũ�ڽ�
		break;

	case E_TYPE_RADIO:		//���̿���ư
		break;

	case E_TYPE_STATIC:		//Static ��Ʈ��
		break;
	}

	Invalidate();
	CWnd::OnLButtonDown(nFlags, point);
}

/**********************************************************
@brief ���콺 ���� ��ư�� �ö����� Ÿ�� �޽���
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
	case E_TYPE_NORMAL:		//�Ϲݹ�ư
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

	case E_TYPE_CHECK_BOX:	//üũ�ڽ�
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

	case E_TYPE_RADIO:		//������ư
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

	case E_TYPE_STATIC:		//Static ��Ʈ��
		break;
	}

	Invalidate();

	CWnd::OnLButtonUp(nFlags, point);
}


/**********************************************************
@brief ��ư�� �̹��� ���� ó���� ���Ͽ� �θ��� onpaint���� DC�� �������ش�.
@details 
@param pDC �θ��� DC 
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
@brief Ÿ���� üũ�ڽ��Ǵ� ���� ��ư�϶� ������ üũ
@details 
@param bCheck üũ ���� 
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
	else if (m_eBtnType == E_TYPE_RADIO) //������ üũ ���ϰ� ����
	{
		//������ hide �׷��忡�� �˸�
		m_eBtnStatus = (bCheck == TRUE ? E_STATUS_CLICK : E_STATUS_NORMAL);
	
		OnRadioClick(this);
		
		Invalidate();
	}
}

/**********************************************************
@brief Ÿ���� üũ�ڽ��Ǵ� ���� ��ư�϶� ������ üũ
@details 
@param pCheckBtn ��ư ������
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
@brief Ÿ���� ���� ��ư�϶� �׷� �߰�.
@details 
@param pSkinButton �߰��� ��ư ������ 
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
@brief ���� ��ư Ŭ���� Ÿ�� �޽���
@details 
@param pClickBtn Ŭ���� ��ư ������
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

	//���� ������ �����ΰ��
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
@brief ������ư�϶� �׷��� owner����
@details 
@param pSkinButton owner ������ 
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
