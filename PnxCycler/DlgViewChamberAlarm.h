#pragma once


// CDlgViewChamberAlarm 대화 상자입니다.

#include "Singleton.h"
#include "ColorStaticST.h"

class CDlgViewChamberAlarm : public CDialogEx, public CSingleton< CDlgViewChamberAlarm >
{
	DECLARE_DYNAMIC(CDlgViewChamberAlarm)
public:
	CDlgViewChamberAlarm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgViewChamberAlarm();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VIEW_CHAMBER_ALARM };
private:
	CColorStaticST	m_ctrlText;

	BOOL	m_bBlink;
private:
	void	InitStatic();
	void	Initbutton();

	void	SetBlink();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
