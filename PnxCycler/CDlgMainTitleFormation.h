#pragma once

#include "Singleton.h"
#include "SkinButton.h"
#include "ColorStaticST.h"
#include "CMgrAccount.h"
#include "CxStatic.h"

// CDlgMainTitleFormation 대화 상자

class CDlgMainTitleFormation : public CDialogEx, public CSingleton< CDlgMainTitleFormation >
{
	DECLARE_DYNAMIC(CDlgMainTitleFormation)

public:
	CDlgMainTitleFormation(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMainTitleFormation();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MAIN_TITLE_FORMATION };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	//virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL Create(CWnd* pParentWnd = NULL);

public:
	void SetLoginStatus(sAccount* lpAccount);
	virtual BOOL OnInitDialog();
	void	UI_Alarm(bool bShow);

private:
	enum eViewTitleObject
	{
		eViewTitleObjectQuit = 0,
		eViewTitlePogoPinTest,
		eViewTitleObjectCalibration,
		eViewTitleObjectIoManual,
		eViewTitleObjectAnalyzeTool,
		eViewTitleObjectAlarm,
		eViewTitleObjectPtnEditor,
		eViewTitleObjectEqpSystemStatus,
		eViewTitleObjectEqpChannelStatus,
		eViewTitleObjectUser,
		eViewTitleObjectMaxCount
	};

private:
	void	InitStatic();
	void	InitButton();

	void	OnClickTitleButton(UINT nID);
	bool	ExecProgramQuit();
	bool	ExecProgramSetting();
	bool	ExecCalibration();
	bool	EcecIoManual();
	bool	ExecIoMap();
	bool	ExecChamber();
	bool	ExecAnalyzeTool();
	bool	ExecAlarm();
	bool	ExecPatternEditor();
	bool	ExecEqpSystem();
	bool	ExecEqpChannel();
	bool	ExecEqpGroup();
	bool	ExecUserLogIn();
	bool	ExecEqLog();
	bool    ExecDaq();

private:
	CSkinButton	m_btnLogo;
	CSkinButton m_btnTitle[eViewTitleObjectMaxCount];

	CSkinButton	m_btnAlarm;

	CxStatic m_ctrlText[eViewTitleObjectMaxCount];
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonAlarmWarning();
};