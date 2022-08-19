#pragma once


// CDlgViewAlarm ��ȭ �����Դϴ�.

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
	CDlgViewAlarm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgViewAlarm();

	void	UpdateStatus(bool bInitialize);	
	void	ClearSysAlarmHisotry(int nStageNo); //220726 YYG �ý��� �˶� �����丮 �ʱ�ȭ

private:
	int					m_nCurSel = 0; //lyw4_1 : ���� �������� ä��
	CTabCtrlEx			m_tabCtrl;

	CGroupListCtrlView		m_listCtrlAlarm;
	CGroupListCtrlView		m_listCtrlSysAlarm; //lyw4_1 : �ý��� �˶�����
	CGroupListCtrlView		m_listCtrlSysAlarmHistory_Stage1; //220726YGY
	CGroupListCtrlView		m_listCtrlSysAlarmHistory_Stage2; //220726YGY
	CGroupListCtrl		m_listCtrlChannel;

	CGroupListCtrl		m_listCtrlStation;
	CGroupListCtrlView		m_listCtrlInverterAlarm; //lyw4_1 : �ý��� �˶�����
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
	void	LoadSysAlarm(int nChannelNo = 0); //lyw4_1 : �˶�����Ʈ Load
	void	LoadEqAlarm();

	void	LoadAlarmComment();
	void	CheckAlarmExist();


	void	LoadAllChannelAlarm();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_VIEW_ALARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
	CDlgViewAlarm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_VIEW_ALARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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