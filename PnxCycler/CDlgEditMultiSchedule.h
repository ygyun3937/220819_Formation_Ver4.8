#pragma once


// CDlgEditMultiSchedule 대화 상자
#include "Singleton.h"

class CDlgEditMultiSchedule : public CDialogEx, public CSingleton<CDlgEditMultiSchedule>
{
	DECLARE_DYNAMIC(CDlgEditMultiSchedule)

public:
	CDlgEditMultiSchedule(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgEditMultiSchedule();

	enum { IDD = IDD_DLG_EDIT_MULTE_SCHEDULE };
private:
	enum eEditMultiScheduleObject
	{
		eEditMultiScheduleObjectSerial = 1,
		eEditMultiScheduleObjectName,
		eEditMultiScheduleObjectLogFilePath,
		eEditMultiScheduleObjectUserID,
		eEditMultiScheduleObjectWorkName,
		eEditMultiScheduleObjectSyncType,
		eEditMultiScheduleObjectCellID,
		eEditMultiScheduleObjectCycle,
		eEditMultiScheduleObjectStep,
		eEditMultiScheduleObjectMaxCount
	};
	
	int	m_nCyclerChannel;
	int m_nSchedule;

	CListCtrl	m_listCyclerChannel;
	CListCtrl	m_listSchedule;
private:
	void	InitListCtrl();
	void	LoadCyclerChannel();
	void	LoadSchedule();

	int		GetColumnWidth(int nIndex);

	void ExecProcScheduleView();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnClickListEditMultischeduleChannel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickListEditMultischeduleSchedule(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnEditMultischeduleDelete();
	afx_msg void OnBnClickedBtnEditMultischeduleClose();
	afx_msg void OnBnClickedBtnEditMultischeduleUp();
	afx_msg void OnBnClickedBtnEditMultischeduleDown();
};
