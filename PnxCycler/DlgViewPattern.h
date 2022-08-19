#pragma once


// CDlgViewPattern ��ȭ �����Դϴ�.

#include "GroupListCtrl.h"
#include "MgrChannel.h"
#include "ColorStaticST.h"
#include "SkinButton.h"
#include "GridCtrl_src/GridCtrl.h"
#include "Cycle.h"
class CDlgViewPattern : public CDialogEx, public CSingleton< CDlgViewPattern >
{
	DECLARE_DYNAMIC(CDlgViewPattern)

public:
	CDlgViewPattern(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgViewPattern();

	void	ShowViewPattern(int nChannelNumber);
	void	LoadPattern(int nChannelNumber);


	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_VIEW_PATTERN };
private:
	CColorStaticST	m_ctrlSchText;

	CSkinButton		m_imgSchedule;
	CChannel*		m_pCurChannel;

	// enum �� private �� �����ϸ�, �ش� class ������ ��� �����ؼ� �̸��� ���ĵ� �ȴ�
	enum StepListCtrlType
	{
		//kw------------- ����
		/*StepListCtrlTypeCycleNo = 0,
		StepListCtrlTypeStepNo,
		StepListCtrlTypeStepType,
		StepListCtrlTypeStepMode*/

		StepListCtrlTypeCycleNo,
		StepListCtrlTypeStepNo,
		StepListCtrlTypeStepMode,
		StepListCtrlTypeStepType,
		StepListCtrlTypeSetVoltage,
		StepListCtrlTypeSetAmpere,
		StepListCtrlTypeSetWatt,
		StepListCtrlTypeCurrVolt,
		StepListCtrlTypeCurrAmpere,
		StepListCtrlTypeExecute
	};

	enum PTNViewTimerEvent
	{
		PTNViewTimerID = 100,
		PTNViewRefreshTime = 500
	};

private:
	void	DrawPattern(int nRow, CStringArray& arr);
	void	DrawCondition(int nIndex, CStringArray& arr);
	void	DrawCycleCondition(CStep* pStep);

	void	SetFont(CGridCtrl* pGrid, CString strFontName, int nSize, bool bBold);
	void	InitListCtrl(CGridCtrl* pGrid, CString columnNames);//, CString cloumnSize
	void	DrawConditionData(int nRow);
	void	ShowDrawPattern();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	//pnx_hr 2021.02.16
	CGridCtrl m_GridPattern;
	CGridCtrl m_GridCondition;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool m_InitWidth = false;

	void LoadDrawINI();
	CStringArray m_strArrTitle;
	CArray<int, int> m_ArrTitleSize;

	CString m_strArrPattern;
	CString m_strArrCondition;
	CString m_StrArrCondSize;
	int m_nGridRowSize = 0;
	int m_iCurChannelStepInfo = 0;
	CString m_strStatus;
	int				m_nPrevRow;
	CStep*			m_pCurStep;
};

