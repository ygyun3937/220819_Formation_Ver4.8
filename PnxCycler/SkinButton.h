#pragma once

/********************************************************
@brief ��ư Ŭ���� 
@details �Ϲݹ�ư, üũ�ڽ�, ������ư, ����ƽ ��ư ��������
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
		E_TYPE_NORMAL = 0,	//�Ϲݹ�ư
		E_TYPE_CHECK_BOX,	//üũ�ڽ�
		E_TYPE_RADIO,		//������ư
		E_TYPE_STATIC		//Static ��Ʈ��
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
	//��ư Ÿ�� ����
	void				SetBtnType(E_BTN_TYPE eBtnType);

	//�̹��� ���
	BOOL				SetBtnImagePath(E_BTN_IMAGE_HALF eBtnImageHalf, CString strNormalPath, CString strOverPath, CString strClickPath, CString strDisablePath);
	
	//PNG �̹��� ����� �����ϰ� �ϱ����� Parent�� DC�� ������ ��׶��Ʈ�� �׸�
	void				SetBkDC(CDC *pDC);
	void				SetFontSize(int nFontSize) { m_nFontSize = nFontSize; Invalidate(); }
	void				SetTextColor(COLORREF colorText) { m_colorText = colorText; }

	//���� üũ�� �������� Ȯ�� (üũ�ڽ� & ������ư)
	void				SetCheck(BOOL bCheck);
	BOOL				IsChecked() { return m_eBtnStatus == E_STATUS_CLICK ? TRUE : FALSE; }

	//��ư Ŭ�� �� Ŭ���Ȱ� ���� ȿ�� ��뿩��
	void				SetBtnEffect(BOOL bUseEffect) { m_bUseEffect = bUseEffect; }

	//���� ��ư�ΰ�� �׷�����.
	void				AddRadioGroup(CSkinButton *pSkinButton);

	//�ؽ�Ʈ align
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


