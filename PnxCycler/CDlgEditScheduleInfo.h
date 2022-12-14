#pragma once


// CDlgEditScheduleInfo 대화 상자

#include "Singleton.h"
#include <vector>
#include "resource.h"

#define	DEFINE_ENTER_START_CYCLE	0
#define	DEFINE_ENTER_START_STEP		1
#define	DEFINE_ENTER_START_SCHEDULE	2

class CDlgEditScheduleInfo : public CDialogEx, public CSingleton<CDlgEditScheduleInfo>
{
	DECLARE_DYNAMIC(CDlgEditScheduleInfo)

public:
	CDlgEditScheduleInfo(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgEditScheduleInfo();

	enum { IDD = IDD_DLG_EDIT_SCHEDULE_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnEditScheduleView();
	afx_msg void OnBnClickedBtnEditScheduleFilePath();
	afx_msg void OnBnClickedBtnEditSchedulePrev();
	afx_msg void OnBnClickedBtnEditScheduleStart();
	afx_msg void OnBnClickedBtnEditScheduleCancle();
	afx_msg void OnClickListEditScheduleInfoWorkName(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListEditScheduleInfoMaterial(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnCbnEditchangeCmbEditScheduleInfoCycle();
	afx_msg void OnCbnEditchangeCmbEditScheduleInfoStep();
	afx_msg void OnCbnSelchangeCmbEditScheduleInfoCycle();
	afx_msg void OnCbnSelchangeCmbEditScheduleInfoStep();
	afx_msg void OnEnChangeEdtCycleLoopCount();

public:
	void	SetScheduleLoadingType(int nType) { m_nScheduleLoadingType = nType; }

	BOOL	LoadScheduleCombo();
	void	ExecProcScheduleStart();		// 2021-01-24 : Private -> Public

private:
	enum eMaterialObject
	{
		eMaterialObjectNone = 0,
		eMaterialObjectChannel,
		eMaterialObjectCellID
	};

private:
	CListCtrl	m_listWorkName;
	CListCtrl	m_listGroup;
	CListCtrl	m_listMaterial;

	int		m_nMaterialItem;
	int		m_nMaterialSubItem;
	int		m_nScheduleLoadingType;

	int		m_nEnterKeyEvent;

private:
	void	InitListCtrl();
	void	InitRadioButton();
	void	LoadScheduleInfo();
	void	LoadWorkNameHistory();
	void	LoadGroupCombo();
	void	LoadMaterial();
	void	SaveHistory();

	void	ExecProcScheduleView();
	//void	ExecProcScheduleStart();
	void	GetCellID(std::vector<CString>& vecValue);
	void	GetCyclerChannelNumber(std::vector<int>& vecValue);

	bool	IsStepSyncValidate();
	bool	IsTemperatureSyncValidate();
	bool	IsEnableScheduleStart();

	int		GetStartCycle();
	int		GetStartStep();
	int		GetSyncMode();

	CString	GetSelectedCyclerChannelCount();
	CString	GetSelectedCyclerChannel();

	CString GetUserName();
	CString	GetWorkName();
	CString GetEQPID();
	CString GetScheduleName();
	CString GetScheduleFilePath();
	CString GetLogFilePath();
	CString GetDescription();
};
