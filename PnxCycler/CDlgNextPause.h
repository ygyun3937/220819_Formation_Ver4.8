#pragma once


// CDlgNextPause 대화 상자
#include "Singleton.h"
#include "ColorStaticST.h"

class CDlgNextPause : public CDialogEx, public CSingleton< CDlgNextPause >
{
	DECLARE_DYNAMIC(CDlgNextPause)

public:
	CDlgNextPause(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgNextPause();

	enum { IDD = IDD_DLG_NEXT_PAUSE };
private:
	enum eNextPauseObject
	{
		eNextPauseObjectCycleNumber = 0,
		eNextPauseObjectStepNumber,
		eNextPauseObjectMaxCount
	};

	CColorStaticST	m_ctrlText[eNextPauseObjectMaxCount];
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
	afx_msg void OnBnClickedBtnPasueMoveNextExit();
	afx_msg void OnBnClickedBtnPasueMoveNextCycle();	
};
