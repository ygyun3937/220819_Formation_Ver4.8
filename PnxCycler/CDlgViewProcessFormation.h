#pragma once

//#include "Singleton.h"
#include <vector>

// CDlgViewProcessFormation 대화 상자

class CDlgViewProcessFormation : public CDialogEx, public CSingleton< CDlgViewProcessFormation >
{
	DECLARE_DYNAMIC(CDlgViewProcessFormation)

public:
	CDlgViewProcessFormation(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewProcessFormation();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_PROCESS_FORMATION };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();

public:
	afx_msg void OnBnClickedButtonViewProcessFormationSelectSchedule();
	afx_msg void OnBnClickedButtonViewProcessFormationStart();

public:
	//virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
public:
	void	SetScheduleLoadingType(int nType) { m_nScheduleLoadingType = nType; }

private:
	void	SelectSchedule();
	void	SelectMasterSlave();

	void	SaveHistory();

	void	ExecProcScheduleStart();
	void	GetCellID(std::vector<CString>& vecValue);
	void	GetCyclerChannelNumber(std::vector<int>& vecValue);

private:
	bool	IsStepSyncValidate();
	bool	IsTemperatureSyncValidate();

	int		GetStartCycle();
	int		GetStartStep();
	int		GetSyncMode();

	CString GetDescription();
	CString GetEQPID();
	CString GetLogFilePath();
	CString GetScheduleName();
	CString GetScheduleFilePath();
	CString GetUserName();
	CString	GetWorkName();

	bool	IsSystemError();

	void	SetSelectChannelAll();

private:
	int		m_nScheduleLoadingType;
};
