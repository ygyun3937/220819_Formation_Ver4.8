#pragma once


#include "Singleton.h"
#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"
// CDlgPauseOption 대화 상자

class CDlgPauseOption : public CDialogEx, public CSingleton< CDlgPauseOption >
{
	DECLARE_DYNAMIC(CDlgPauseOption)

public:
	CDlgPauseOption(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgPauseOption();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_PAUSE_OPTION };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	afx_msg void OnBnClickedBtnImmediatelyPause();
	afx_msg void OnBnClickedBtnStepcompletePause();
	afx_msg void OnBnClickedBtnCyclecompletePause();
	afx_msg void OnBnClickedBtnNextPause();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	CxShadeButton m_BtnPauseOption[4];
};
