#pragma once


// CDlgMainTitle 대화 상자입니다.

#include "Singleton.h"
#include "SkinButton.h"
#include "ColorStaticST.h"
#include "CMgrAccount.h"

class CDlgMainTitle : public CDialogEx, public CSingleton< CDlgMainTitle >
{
	DECLARE_DYNAMIC(CDlgMainTitle)

public:
	CDlgMainTitle(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMainTitle();

	enum { IDD = IDD_DLG_MAIN_TITLE };
private:
	enum eViewTitleObject
	{
		eViewTitleObjectQuit = 0,
		eViewTitleObjectSetting,
		eViewTitleObjectAnalyzeTool,
		eViewTitleObjectPtnEditor,		
		eViewTitleObjectMaxCount		
	};

	CSkinButton	m_btnLogo;
	CSkinButton m_btnTitle[eViewTitleObjectMaxCount];

	CColorStaticST	m_ctrlText[eViewTitleObjectMaxCount];
private:
	void	InitStatic();
	void	InitButton();
	void	OnClickTitleButton(UINT nID);

	bool	ExecProgramQuit();
	bool	ExecProgramSetting();
	bool	ExecAnalyzeTool();
	bool	ExecPatternEditor();
	bool	ExecUserLogIn();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void SetLoginStatus(sAccount* lpAccount);

	//
	/*BOOL IsSystemLogViewOption() { return m_bSystemLogViewOption; }
	BOOL m_bSystemLogViewOption = FALSE;*/
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
