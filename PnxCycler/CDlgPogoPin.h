#pragma once


// CDlgPogoPin 대화 상자
#include "Singleton.h"
#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"


class CDlgPogoPin : public CDialogEx, public CSingleton< CDlgPogoPin >
{
	DECLARE_DYNAMIC(CDlgPogoPin)

public:
	CDlgPogoPin(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgPogoPin();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_POGOPIIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	afx_msg void OnBnClickedButton1();

	CArray<int, int> m_ArrTitleSize;
	CStringArray m_strArrTitle;
	int m_nGridRowSize = 0;


	void	Init_Control();
	void	LoadDrawINI();
	bool	LoadPogoPin();

	CxShadeButton m_Btn_Pogo;
	CGridCtrl m_PogoPinGrid1;
	CGridCtrl m_PogoPinGrid2;
	CxStatic m_Title_Stage1;
	CxStatic m_Title_Stage2;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonStage1Apply();
	afx_msg void OnBnClickedButtonStage2Apply();
	CxShadeButton m_btn_Stage1_Apply;
	CxShadeButton m_btn_Stage2_Apply;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
