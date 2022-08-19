#pragma once


// CDlgViewDAQ 대화 상자입니다.
#include "GroupListCtrl.h"
#include "SkinButton.h"
#include "ColorStaticST.h"
#include "GridCtrl_src/GridCtrl.h"

enum ListCtrlDAQ
{
	ListCtrlDAQUniqueCode = 0,
	ListCtrlDAQIndex,
	ListCtrlDAQVoltage,
	ListCtrlDAQTemperature,
	ListCtrlDAQVoltageMin,
	ListCtrlDAQVoltageMax,
	ListCtrlDAQMaxCount,
};

class CDlgViewDAQ : public CDialogEx, public CSingleton< CDlgViewDAQ >
{
	DECLARE_DYNAMIC(CDlgViewDAQ)
private:
	//CGroupListCtrl	m_listDAQ;

	CSkinButton	m_btnDAQ;

	CColorStaticST	m_ctrlDAQText;
	CColorStaticST	m_ctrlDAQName;	

	CRect	m_rect;

	int		m_nChannelNumer;
private:
	void	InitCtrl();
	void	InitListCtrl();
	void	LoadDrawINI();
public:
	CDlgViewDAQ(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgViewDAQ();

	void	LoadDAQ();

	void	SetChannelNumber(int nChannelNumber);

	void	TimerStart();
	void	TimerEnd();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_VIEW_DAQ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	//pnx_hr 2021.02.16
	CGridCtrl m_DaqGrid;
	CStringArray m_strArrTitle;
	int m_nGridRowSize = 0;
	CArray<int, int> m_ArrTitleSize;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
