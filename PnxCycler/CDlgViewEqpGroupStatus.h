#pragma once


// CDlgViewEqpGroupStatus 대화 상자

class CDlgViewEqpGroupStatus : public CDialogEx, public CSingleton< CDlgViewEqpGroupStatus >
{
	DECLARE_DYNAMIC(CDlgViewEqpGroupStatus)

public:
	CDlgViewEqpGroupStatus(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewEqpGroupStatus();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_EQP_GROUP_STATUS };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	
private:
	void Initialize();
	void InitializeControls();
	void InitializeColors();

	void OnDoubleClickEquipmentName(UINT nID);



	void SetDisplayFont(UINT nID, CString sFontName, int nFontSize);

	void SetDisplayTime();
	CString GetDateTime(CString sGetType);

private:

public:
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	void UpdataEQStatus();
	COLORREF GetDisplayGroupStatusColor(int nEQStatus);

};
