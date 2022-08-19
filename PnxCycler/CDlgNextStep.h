#pragma once


// CDlgNextStep 대화 상자
#include "Singleton.h"
#include "ColorStaticST.h"

class CDlgNextStep : public CDialogEx, public CSingleton< CDlgNextStep >
{
	DECLARE_DYNAMIC(CDlgNextStep)

public:
	CDlgNextStep(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgNextStep();

	enum { IDD = IDD_DLG_NEXT_STEP };
private:
	enum eNextStepObject
	{
		eNextStepObjectCycleNumber = 0,
		eNextStepObjectStepNumber,
		eNextStepObjectMaxCount
	};

	CColorStaticST	m_ctrlText[eNextStepObjectMaxCount];

private:
	void	InitStatic();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnStepMoveNextCycle();
	afx_msg void OnBnClickedBtnStepMoveNextExit();	
};
