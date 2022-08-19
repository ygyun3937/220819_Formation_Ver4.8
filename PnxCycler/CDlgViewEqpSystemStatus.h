#pragma once

// CDlgViewEqpSystemStatus 대화 상자
#include "GroupListCtrl.h"
#include "MgrChannel.h"
#include "SkinButton.h"
#include "ColorStaticST.h"
#include "GridCtrl_src/GridCtrl.h"
#include "CxStatic.h"
#include "GroupListCtrl.h"

class CDlgViewEqpSystemStatus : public CDialogEx, public CSingleton< CDlgViewEqpSystemStatus >
{
	DECLARE_DYNAMIC(CDlgViewEqpSystemStatus)

public:
	CDlgViewEqpSystemStatus(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewEqpSystemStatus();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_EQP_SYSTEM_STATUS };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CArray<int, int> m_ArrTitleSize;

	CStringArray m_strArrTitle;
	int m_nGridRowSize = 0;


	void	SetChannelNumber(int nChannelNumber);


protected:

private:	

	void	InitCtrl();
	void	LoadDrawINI();
	
	CRect	m_rect;

	int		m_nChannelNumer;



public:
	void Input_Status_Check();

	void Update_Dir_list(int nStageNo);

	CString m_strTrayPath;

	CxStatic m_FanStatus[16];
	CxStatic m_EQStatus;
	CxStatic m_EMGStatus;
	CxStatic m_SmokeStatus[2];

	CxStatic		m_Data_TrayBarcode;
	CxStatic		m_Data_ChannelBarcode;

	CxShadeButton m_Btn_LogShortCut;
	CxShadeButton m_Btn_ContactVoltSetting;
	CxShadeButton m_Btn_BarcodeOption;

	CGridCtrl m_DirList_Grid;
	CGridCtrl m_GridDirList;

	CMonthCalCtrl m_Select_Date;

//챔버 정보
public:
	void	LoadChamber();

	int		GetListCtrlColumn(int nIndex);
private:
	enum eViewChamberObject_SystemStatusView
	{
		eViewChamberObject_SystemStatusView_Serial = 0,
		eViewChamberObject_SystemStatusView_Status,
		eViewChamberObject_SystemStatusView_CurrentTemp,
		eViewChamberObject_SystemStatusView_SettingAutoTemp,
		eViewChamberObject_SystemStatusView_OnOff,
		eViewChamberObject_SystemStatusView_MaxCount
	};
	CGroupListCtrl	m_listChamber;
	void	InitList_ChamberCtrl();
	void	InitList_DirList();

	int m_nBarcodeReadingFocus;

	BOOL m_bChBarcodeCheck;

	CString m_strSchedulePath;

	int m_nStageNo;
public:

	void UI_Barcode_Focus(int nID, BOOL bFocus);


	void Set_BarcodeReadingFocus(int nID);
	int Get_BarcodeReadingFocus() { return m_nBarcodeReadingFocus; }

	void Set_BarcodeStationNo(int nStageNo) { m_nStageNo = nStageNo; }
	int Get_BarcodeStagionNo() { return m_nStageNo; }
	
	void Set_ChBarcodeCheckMode(bool bChBarcodeCheckMode);
	BOOL Get_ChBarcodeCheckMode() { return m_bChBarcodeCheck; }


	afx_msg void OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);

	
	void Set_SchedulePath(CString strSchedulePath);
	CString Get_SchedulePath() { return m_strSchedulePath; }

	afx_msg void OnStnClickedStaticFindChbarcode();
	afx_msg void OnMcnSelchangeMonthcalendar1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonContactVoltApply();
	afx_msg void OnBnClickedButtonBarcodeCheckOption();
	afx_msg void OnBnClickedButtonLogFolderShotcuts();
	afx_msg void OnStnClickedStaticFindTraybarcode();

};
