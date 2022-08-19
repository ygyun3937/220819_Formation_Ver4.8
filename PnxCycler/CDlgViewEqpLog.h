#pragma once


// CDlgViewEqpLog 대화 상자

class CDlgViewEqpLog : public CDialogEx, public CSingleton< CDlgViewEqpLog >
{
	DECLARE_DYNAMIC(CDlgViewEqpLog)

public:
	CDlgViewEqpLog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewEqpLog();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_EQP_LOG };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedBtnLogLoad();
	CFont m_ListBoxFont;


	CListCtrl m_ListControl_Log;
	CString m_strLog[MAX_LOG_DATA];
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnLoglistup();
};
