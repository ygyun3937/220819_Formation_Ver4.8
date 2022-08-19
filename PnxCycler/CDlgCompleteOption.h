#pragma once

#include "Singleton.h"
#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"
// CDlgCompleteOption 대화 상자

class CDlgCompleteOption : public CDialogEx, public CSingleton< CDlgCompleteOption >
{
	DECLARE_DYNAMIC(CDlgCompleteOption)

public:
	CDlgCompleteOption(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgCompleteOption();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_COMPLETE_OPTION };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

	afx_msg void OnBnClickedBtnImmediatelyComplete();
	afx_msg void OnBnClickedBtnStepcompleteComplete();
	afx_msg void OnBnClickedBtnCyclecompleteComplete();
	afx_msg void OnBnClickedBtnNextComplete();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CxShadeButton m_btn_Complete_Immdiately;
	CxShadeButton m_btn_Complete_AfterStep;
	CxShadeButton m_btn_Complete_Cycle;
	CxShadeButton m_btn_Complete_Reservation;
	void SetStageNo(int nStageNo) { m_nStageNo = nStageNo; }
	int GetStageNo() { return m_nStageNo; }

	void SetImmediatelyComplete(int nStageNo);

private:
	int m_nStageNo;
};
