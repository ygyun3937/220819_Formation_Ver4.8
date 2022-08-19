#pragma once

/********************************************************
@brief 버튼 클래스 
@details 일반버튼, 체크박스, 라디오버튼, 스태틱 버튼 설정가능
@author ween8
@date 2016-06-13 12:49:59
@version 
********************************************************/
class AFX_EXT_CLASS CSkinButton : public CWnd
{
	DECLARE_DYNAMIC(CSkinButton)

public:
	CSkinButton();
	virtual ~CSkinButton();

protected:
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void	OnMouseLeave();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);


public:
	enum E_BTN_TYPE
	{
		E_TYPE_NORMAL = 0,	//일반버튼
		E_TYPE_CHECK_BOX,	//체크박스
		E_TYPE_RADIO,		//라디오버튼
		E_TYPE_STATIC		//Static 컨트롤
	};

	enum E_BTN_STATUS
	{
		E_STATUS_NORMAL = 0,
		E_STATUS_OVER,
		E_STATUS_CLICK,
		E_STATUS_DISABLE
	};

	enum E_BTN_IMAGE_HALF
	{
		E_HALF_1,
		E_HALF_2,
		E_HALF_4,
	};

public:
	//버튼 타입 설정
	void				SetBtnType(E_BTN_TYPE eBtnType);

	//이미지 경로
	BOOL				SetBtnImagePath(E_BTN_IMAGE_HALF eBtnImageHalf, CString strNormalPath, CString strOverPath, CString strClickPath, CString strDisablePath);
	
	//PNG 이미지 배경을 투명하게 하기위해 Parent의 DC를 가지고 백그라운트를 그림
	void				SetBkDC(CDC *pDC);
	void				SetFontSize(int nFontSize) { m_nFontSize = nFontSize; Invalidate(); }
	void				SetTextColor(COLORREF colorText) { m_colorText = colorText; }

	//현재 체크된 상태인지 확인 (체크박스 & 라디오버튼)
	void				SetCheck(BOOL bCheck);
	BOOL				IsChecked() { return m_eBtnStatus == E_STATUS_CLICK ? TRUE : FALSE; }

	//버튼 클릭 시 클릭된것 같은 효과 사용여부
	void				SetBtnEffect(BOOL bUseEffect) { m_bUseEffect = bUseEffect; }

	//라디오 버튼인경우 그룹지정.
	void				AddRadioGroup(CSkinButton *pSkinButton);

	//텍스트 align
	void				SetTextAlign(UINT nTextAlign) { m_nTextAlign = nTextAlign; }

private:
	void				SetGroupOwner(CSkinButton *pSkinButton);
	CSkinButton*		GetGroupOwner() { return m_pGroupOwner; }
	void				OnRadioClick(CSkinButton *pClickBtn);
	void				SetCheck(CSkinButton *pCheckBtn);
	void				SetBtnStatus(E_BTN_STATUS eBtnStatus) { m_eBtnStatus = eBtnStatus; Invalidate(); }

	void				DrawBtn(CDC *pDC);

private:
	E_BTN_IMAGE_HALF	m_eBtnImageHalf;
	UINT				m_nTextAlign;
	BOOL				m_bUseEffect;
	int					m_nFontSize;
	BOOL				m_bLBtnClicked;
	CString				m_strNormalImagePath; 
	CString				m_strOverImagePath;
	CString				m_strClickImagePath; 
	CString				m_strDisableImagePath;

	E_BTN_STATUS		m_eBtnStatus;
	E_BTN_TYPE			m_eBtnType;

	COLORREF			m_colorText;
	CDC					m_cdcBK;

	CSkinButton			*m_pGroupOwner;

	CArray<CSkinButton*, CSkinButton*> m_arrRadioGorup;
};


