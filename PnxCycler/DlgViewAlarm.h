#pragma once


// CDlgViewAlarm 대화 상자입니다.

#include "Singleton.h"
#include "TabCtrlEx.h"
#include "GroupListCtrl.h"
#include "GroupListCtrlView.h"
#include "ColorStaticST.h"
#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"
#include "resource.h"

class CDlgViewAlarm : public CDialogEx, public CSingleton< CDlgViewAlarm >
{
	DECLARE_DYNAMIC(CDlgViewAlarm)

public:
	CDlgViewAlarm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgViewAlarm();

	void	UpdateStatus(bool bInitialize);	
	void	ClearSysAlarmHisotry(int nStageNo); //220726 YYG 시스템 알람 히스토리 초기화

private:
	int					m_nCurSel = 0; //lyw4_1 : 현재 선택중인 채널
	CTabCtrlEx			m_tabCtrl;

	CGroupListCtrlView		m_listCtrlAlarm;
	CGroupListCtrlView		m_listCtrlSysAlarm; //lyw4_1 : 시스템 알람전용
	CGroupListCtrlView		m_listCtrlSysAlarmHistory_Stage1; //220726YGY
	CGroupListCtrlView		m_listCtrlSysAlarmHistory_Stage2; //220726YGY
	CGroupListCtrl		m_listCtrlChannel;

	CGroupListCtrl		m_listCtrlStation;
	CGroupListCtrlView		m_listCtrlInverterAlarm; //lyw4_1 : 시스템 알람전용
	CGroupListCtrlView		m_listCtrlEqAlarm;//220529 YGY
	CGroupListCtrlView		m_listCtrlAlarmComment;//220529 YGY
	CColorStaticST		m_ctrlDesc;

	CCriticalSection	m_csLock;
	CCriticalSection	m_csLockSysAlarm; //lyw4_1
	CCriticalSection	m_csLockInverterAlarm; //lyw4_1
	CCriticalSection	m_csLockChannel;
	CCriticalSection	m_csLockEQAlarm; //lyw4_1
	CCriticalSection	m_csLockAlarmComment; //lyw4_1

	CxStatic m_Title_SystemAlarm;
	CxStatic m_SubTitle_Stage1_AlarmHistory;
	CxStatic m_SubTitle_Stage2_AlarmHistory;
	CxStatic m_SubTitle_InvertAlarm;
	CxStatic m_SubTitle_EQAlarm;
	CxShadeButton m_Btn_AllAlarmClear;
	CxShadeButton m_Btn_SelectAlarmClear;

private:


	void	InitTabCtrl();
	void	InitListCtrl();

	void	LoadAlarm(int nChannelNo = 0);
	void	LoadInverterAlarm(int nStationNo = 0);
	void	LoadSysAlarm(int nChannelNo = 0); //lyw4_1 : 알람리스트 Load
	void	LoadEqAlarm();

	void	LoadAlarmComment();
	void	CheckAlarmExist();


	void	LoadAllChannelAlarm();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_VIEW_ALARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeTabChannel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnReceiveListCtrlSelect(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonAlarmClearAll();
	afx_msg void OnBnClickedButtonAlarmClear();



	void Destory_UI();
	CxStatic m_SubTitle_AlarmComment;
};

/*
class CDlgViewAlarm : public CDialogEx, public CSingleton< CDlgViewAlarm >
{
	DECLARE_DYNAMIC(CDlgViewAlarm)

public:
	CDlgViewAlarm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgViewAlarm();
private:
	CGroupListCtrl	m_listCtrlAlarm;

	CTabCtrlEx	m_tabCtrl;

	CCriticalSection	m_csLock;
private:
	void	InitTabCtrl();
	void	InitListCtrl();

	void	LoadAlarm(int nChannelNo = 0);

	void	CheckAlarmExist();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_VIEW_ALARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeTabChannel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
*/