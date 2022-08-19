#pragma once


// CSkinDialog dialog

class CSkinDialog : public CDialog
{
public:
	CSkinDialog(UINT nResourceID, CWnd* pParent = NULL);   // standard constructor

	enum	{	CLICKED_BOX_NONE=0,
				CLICKED_BOX_MINIMIZE,
				CLICKED_BOX_MAXIMIZE,
				CLICKED_BOX_RESTORE,
				CLICKED_BOX_EXIT
			};

// Attributes
public:
	CRgn		m_rgnWnd;			// ������ ����

	CBrush		m_CtlColorBrush;

	int			m_nClickedBox;		// ���� Ŭ���� �ڽ� ����

	BOOL		m_bTracking;		// ���� ���콺 �ڽ����� �ִ��� �ƴ��� ���� �÷���

	BOOL		m_bDraging;			// �����찡 ���콺 �巡�׷� �̵� ������ �ƴ����� ���� �÷���
	BOOL		m_bSizeCursor;		// Ŀ������� ���������� ������� �ƴ����� ���� �÷���

	BOOL		m_bActive;

	COLORREF	m_cfText;			// �ؽ�Ʈ ��
	COLORREF	m_cfMask;			// ����ũ ��

	// ��Ʈ�� ������
	SIZE		m_sizeCapL;			// ������ ĸ�� ����
	SIZE		m_sizeCapM;			// ������ ĸ�� �߾�
	SIZE		m_sizeCapR;			// ������ ĸ�� ������
	SIZE		m_sizeFrmL;			// ������ ������ ����
	SIZE		m_sizeFrmLB;		// ������ ������ ���ʾƷ�
	SIZE		m_sizeFrmB;			// ������ ������ �Ʒ�
	SIZE		m_sizeFrmR;			// ������ ������ ������
	SIZE		m_sizeFrmRB;		// ������ ������ �����ʾƷ�
	SIZE		m_sizeBox;			// ������ �ִ�/�ּ� �ڽ� ������

	// ��Ʈ�� �ڵ�
	HBITMAP		m_hBmpCapL;			// ������ ĸ�� ����
	HBITMAP		m_hBmpCapM;			// ������ ĸ�� �߾�
	HBITMAP		m_hBmpCapR;			// ������ ĸ�� ������
	HBITMAP		m_hBmpFrmL;			// ������ ������ ����
	HBITMAP		m_hBmpFrmLB;		// ������ ������ ���ʾƷ�
	HBITMAP		m_hBmpFrmB;			// ������ ������ �Ʒ�
	HBITMAP		m_hBmpFrmR;			// ������ ������ ������
	HBITMAP		m_hBmpFrmRB;		// ������ ������ ������ �Ʒ�

	HBITMAP		m_hBmpMinimize;		// ������ �ּ� �ڽ�
	HBITMAP		m_hBmpMaximize;		// ������ �ִ� �ڽ�
	HBITMAP		m_hBmpRestore;		// ������ ���� �ڽ�
	HBITMAP		m_hBmpExit;			// ������ ���� �ڽ�

	// ������ �ڵ�
	HICON		m_hIcon;

// Operations
public:
	void		DrawFrame();
	void		SetCtlColor(COLORREF crCtlColor);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	afx_msg LRESULT OnEnterSizeMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
