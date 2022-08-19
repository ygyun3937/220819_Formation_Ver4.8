#pragma once


// CDlgViewScheduleInfo 대화 상자
#include "Singleton.h"

using namespace std;

class CChannel;
class CMgrSchedule;

#include "CMgrModel.h"

class CDlgViewScheduleInfo : public CDialogEx, public CSingleton< CDlgViewScheduleInfo>
{
	DECLARE_DYNAMIC(CDlgViewScheduleInfo)

public:
	CDlgViewScheduleInfo(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewScheduleInfo();

	void	ShowViewScheduleInfo(SModel* lpModel, SSchedule* lpSchedule);

	enum { IDD = IDD_DLG_VIEW_SCHEDULE_INFO };
private:
	enum eViewScheduleInfoObject
	{
		eViewScheduleInfoObjectSerial = 0,
		eViewScheduleInfoObjectStepType,
		eViewScheduleInfoObjectChargeVoltage,
		eViewScheduleInfoObjectDischargeVoltage,
		eViewScheduleInfoObjectChargeCurrent,
		eViewScheduleInfoObjectDischargeCurrent,
		eViewScheduleInfoObjectPower,
		eViewScheduleInfoObjectImpedance,
		eViewScheduleInfoObjectTemperature,
		eViewScheduleInfoObjectMaxCount
	};

	enum eViewScheduleInfoModel
	{
		eViewScheduleInfoModelName = 0,
		eViewScheduleInfoModelDesc,
		eViewScheduleInfoModelMaxCount
	};

	enum eViewScheduleInfoSchedule
	{
		eViewScheduleInfoScheduleName = 0,
		eViewScheduleInfoScheduleUser,
		eViewScheduleInfoScheduleDesc,
		eViewScheduleInfoScheduleTime,
		eViewScheduleInfoScheduleMaxCount
	};

	enum eViewScheduleInfoSafety
	{
		eViewScheduleInfoSafetyVoltageMax = 0,
		eViewScheduleInfoSafetyVoltageMin,
		eViewScheduleInfoSafetyCurrentMax,
		eViewScheduleInfoSafetyCurrentMin,
		eViewScheduleInfoSafetyCapacityMax,
		eViewScheduleInfoSafetyTemperatureMax,
		eViewScheduleInfoSafetyMaxCount
	};

	enum eViewScheduleComplete
	{
		eViewScheduleCompleteVoltage = 0,
		eViewScheduleCompleteCurrent,
		eViewScheduleCompleteCapacity,
		eViewScheduleCompleteTime,
		eViewScheduleCompleteWatt,
		eViewScheduleCompleteWattHour,
		eViewScheduleCompleteMaxCount
	};

	enum eViewScheduleSystemSafety
	{
		eViewScheduleSystemSafetyVoltage = 0,
		eViewScheduleSystemSafetyCurrent,
		eViewScheduleSystemSafetyCapacity,
		eViewScheduleSystemSafetyTemperature,
		eViewScheduleSystemSafetyMaxCount
	};

	enum eViewScheduleLogType
	{
		eViewScheduleLogTypeVoltage = 0,
		eViewScheduleLogTypeTime,
		eViewScheduleLogTypeCurrent,
		eViewScheduleLogTypeTemperature,
		eViewScheduleLogTypeMaxCount
	};

	CListCtrl m_listSchedule;
	CListCtrl m_listGrade;

	SModel*		m_lpModel;
	SSchedule*	m_lpSchedule;

	CChannel*		m_lpCyclerChannel;
	CMgrSchedule*	m_lpScheduleInfo;

private:
	void	InitListCtrl();

	void	LoadModel(bool bInitialize);
	void	LoadStepInfo(int nCurSel);
	void	LoadSchedule();	

	int		GetListCtrlColumnWidth(int nColumnIndex);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnViewScheduleInfoClose();
	afx_msg void OnClickListViewScheduleInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickListViewScheduleInfoGrade(NMHDR *pNMHDR, LRESULT *pResult);
};
