//#include "Singleton.h"
#include <vector>
#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"
#include "SkinButton.h"

#pragma once

// CDlgViewEqpChannelStatus 대화 상자

class CDlgViewEqpChannelStatus : public CDialogEx, public CSingleton< CDlgViewEqpChannelStatus >
{
	DECLARE_DYNAMIC(CDlgViewEqpChannelStatus)

public:
	CDlgViewEqpChannelStatus(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewEqpChannelStatus();

	// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_EQP_CHANNEL_STATUS };
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

	afx_msg void OnBnClickedBtnViewEqpChannelStatusStage1SelectSchedule();
	afx_msg void OnBnClickedBtnViewEqpChannelStatusStage1Start();
	afx_msg void OnBnClickedBtnViewEqpChannelStatusStage1Group1();
	afx_msg void OnBnClickedBtnViewEqpChannelStatusStage2Group();

public:
	void	SetSubEqpIndexNo(int nSubEqpIndexNo) { m_nSubEqpIndexNo = nSubEqpIndexNo; }
	int		GetSubEqpIndexNo() { return m_nSubEqpIndexNo; }

	void	SetSubEqpId(CString sSubEqpId) { m_strSubEqpId = sSubEqpId;}
	CString GetSubEqpId() { return m_strSubEqpId; }



public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();

	void Initialize();
	void InitializeControls();
	void InitializeTimers();

	bool SetDisplayEqpId(CString eqpId);

	void SelectSchedule(int nStageNo);
private:

	bool LoadDrawINI();
	bool InitializeChannelInfo();

	bool SetDisplayChannelNo(int nEqpChannelIndexNo, CString sValue);
	bool SetDisplayChannelVoltage(int nEqpChannelIndexNo, CString sValue);
	bool SetDisplayChannelAmpare(int nEqpChannelIndexNo, CString sValue);
	bool SetDisplayChannelAmpareHour(int nEqpChannelIndexNo, CString sValue);
	bool SetDisplayChannelCode(int nEqpChannelIndexNo, CString sValue);
	bool SetDisplayChannelTemperature(int nEqpChannelIndexNo, CString sValue);

	bool SetDisplayChannelStepTime(int nEqpChannelIndexNo, CString sValue);
	bool SetDisplayChannelTotalTime(int nEqpChannelIndexNo, CString sValue);

	bool SetDisplayChannelCycleNumber(int nEqpChannelIndexNo, CString sValue);
	bool SetDisplayChannelStepNumber(int nEqpChannelIndexNo, CString sValue);
	
	void SetInverterStatus(int nStation);
	


	COLORREF GetDisplayChannelStatusColor(int nStatusValue);

	COLORREF GetDisplayChannelOpCodeColor(int nOpCodeValue);

	
	bool SetStaticControlColor(UINT nID, COLORREF crColor);

	bool UpdateEqpChannelStatus();


	void Init_Stage(int nStageNo);

	// -------------------------------------------------------------------------
	
	void SelectSchedule();
	void SetSelectChannelAll();
	void SelectMasterSlave();
	void SelectCalibration();	// 220818 YGY Current Calibration Mode Master Slave Channel Select
	bool IsSystemError();
	void SetSelectEQChAll(int nEqpStageIndexNo);

	// -------------------------------------------------------------------------


	int GetStartCycle();
	int	GetStartStep();
	int	GetSyncMode();

	bool IsStepSyncValidate();
	bool IsTemperatureSyncValidate();
	//220526 YGY Formation Type Added function for selecting by stage
	bool IsTemperatureSyncValidate(int nStageNo);

	void SaveHistory();

	void GetCellID(std::vector<CString>& vecValue);
	void GetCyclerChannelNumber(std::vector<int>& vecValue);
	//220526 YGY Formation Type Added function for selecting by stage
	void GetCyclerChannelNumber(int nStageNo,std::vector<int>& vecValue);

	CString GetScheduleFilePath();
	//220526 YGY Formation Type Added function for selecting by stage
	CString GetScheduleFilePath(int nStageNo);
	CString GetScheduleName();
	//220526 YGY Formation Type Added function for selecting by stage
	CString GetScheduleName(int nStageNo);
	CString GetEqpId();
	CString GetLoginUserName();
	CString	GetWorkName();
	CString GetLogFilePath();
	CString GetDescription();

private:
	// CONFIG
	CStringArray	m_strArrTitle;
	int				m_nSubEqpIndexNo;
	CString			m_strSubEqpId;

	int		m_nScheduleLoadingType;
	int		m_nSelectedStageNo;

	CxShadeButton	m_BtnSelectStageS1;
	CxShadeButton	m_BtnSelectStageS2;

	CString			m_strStage1;
	CString			m_strStage2;
	CString			m_strChStatus[EQ_CH_INDEX];
	CString			m_strChOpCode[EQ_CH_INDEX];

	CxStatic		m_staticControl[EQ_CH_INDEX];
	CxStatic		m_staticSetStatusColor[(int)eCyclerChannelStateMaxCount];
	CxShadeButton	m_BtnSelectCh[EQ_CH_INDEX];

	void UpdataChStatus();
	void UpdateDaqTemp();

	void OnClickTitleButton(UINT nID);
	void OnClickChBarcode(UINT nID);

	BOOL m_bCheckStatus;	

	int m_nBarcodeReadingFocus;


	BOOL m_bStageDataSwitching[EQ_STAGE_INDEX];

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CxShadeButton m_Btn_SelectSchedule;
	CxShadeButton m_Btn_Start;
	CxShadeButton m_Btn_Pause;
	
	afx_msg void OnBnClickedBtnViewEqpChannelStatusPause();

	bool m_bSelectStage[EQ_STAGE_INDEX];

	void SetSelectStage(int nStageNo,bool bSelect);
	bool GetSelectStage(int nStageNo);

	CxStatic m_Status_Stage1;
	CxStatic m_Status_Stage2;
	void ExecProcScheduleStart();
	//220526 YGY Formation Type Added function for selecting by stage
	void ExecProcScheduleStart(int nStageNo);

	CxStatic m_staticStepTime[EQ_CH_INDEX];
	CxStatic m_staticTotalTime[EQ_CH_INDEX];
	CxStatic m_staticChBarcode[EQ_CH_INDEX];
	CxStatic m_staticCycle[EQ_CH_INDEX];
	CxStatic m_staticStep[EQ_CH_INDEX];

	CxStatic m_staticTemp[EQ_CH_INDEX];

	CxStatic m_staticAmp[EQ_CH_INDEX];
	CxStatic m_staticVolt[EQ_CH_INDEX];

	CxStatic m_staticChCode[EQ_CH_INDEX];
	CxStatic m_staticCapacity[EQ_CH_INDEX];

	CxStatic m_Status_Inverter;
	afx_msg void OnBnClickedBtnViewEqpChannelStatusComplete();
	CxShadeButton m_btn_Complete_Option;
	CxShadeButton m_btn_AlarmClear;
	CxShadeButton m_btn_DataSwtching[EQ_STAGE_INDEX];
	afx_msg void OnBnClickedBtnViewEqpChannelStatusAlarmClear();
	CxStatic m_Status_Schedule;
	CxStatic m_Status_Schedule_Model;
	



	//Stage  UI Visible Function 

	void Set_Stage_UI_Visible();

	void UI_Update_Focus(int nID);

	CxStatic m_Title_Stage1_CH;
	CxStatic m_Title_Stage1_StepTime;
	CxStatic m_Title_Stage1_TotalTime;
	CxStatic m_Title_Stage1_Volt;
	CxStatic m_Title_Stage1_Current;
	CxStatic m_Title_Stage1_Cap;
	CxStatic m_Title_Stage1_Code;
	CxStatic m_Title_Stage1_Temp;

	CxStatic m_Title_Stage2_CH;
	CxStatic m_Title_Stage2_StepTime;
	CxStatic m_Title_Stage2_TotalTime;
	CxStatic m_Title_Stage2_Volt;
	CxStatic m_Title_Stage2_Current;
	CxStatic m_Title_Stage2_Cap;
	CxStatic m_Title_Stage2_Code;
	CxStatic m_Title_Stage2_Temp;


	void Display_Manage();


	CxStatic m_Status_Disk;
	CxStatic m_staticTrayBarcode;

	void SetPlcDataParam();
	CxStatic m_Line_1;
	CxStatic m_Status_Start_Message;
	CxStatic m_staticVacValue;


	void Set_BarcodeReadingFocus(int nID);

	int Get_BarcodeReadingFocus() { return m_nBarcodeReadingFocus; }

	void UI_Ch_Init(int nStageNo);

	void UI_Clear(int nStageNo);

	void UI_BarcodeUpdate(int nStageNo);

	afx_msg void OnBnClickedButtonBarcodeOption();
	CxStatic m_staticVacuumPercent;
	CxStatic m_Line_2;
	CxStatic m_Status_Stage2_Start_Message;
	CxStatic m_Status_Schedule_Stage2_Model;
	CxStatic m_Status_Stage2_Schedule;
	CxStatic m_staticTrayBarcode_Stage2;
	CxStatic m_InverterStatus_2;
	CxStatic m_Status_Disk2;
	CxStatic m_Data_IO_Status;

	CxShadeButton m_Btn_Stage2_Mode;
	CxShadeButton m_Btn_Stage2_Start;
	CxShadeButton m_Btn_Stage2_SelectSchedule;
	CxShadeButton m_Btn_Stage1_Mode;
	CxShadeButton m_Btn_Stage1_Init;
	CxShadeButton m_Btn_Stage2_Init;
	CxShadeButton m_btn_Stage2_Complete_Option;
	CxShadeButton m_Btn_BarcodeOption;

	void Set_UI_Data_Switching(int nStageNo,bool bSwitching);
	BOOL Get_UI_Data_Switching(int nStageNo) { return m_bStageDataSwitching[nStageNo]; }


	void UI_Update_Switching(int nStageNo);



	afx_msg void OnBnClickedBtnViewEqpChannelStatusSelectStage2Schedule();
	afx_msg void OnBnClickedBtnViewEqpChannelStatusStage2Start();
	afx_msg void OnBnClickedBtnViewEqpChannelStatusStage2Complete();
	afx_msg void OnStnClickedStaticViewEqpChannelStatusTraybarcode();
	afx_msg void OnStnClickedStaticViewEqpChannelStatusStage2Traybarcode();
	afx_msg void OnStnClickedStaticViewEqpChannelChBarcode1();
	afx_msg void OnBnClickedBtnViewEqpStage1Mode();
	afx_msg void OnBnClickedBtnViewEqpStage2Init();
	afx_msg void OnBnClickedBtnViewEqpStage1Init();
	afx_msg void OnBnClickedBtnViewEqpStage1Mode2();
	afx_msg void OnBnClickedBtnViewEqpStage1DataSwitching();
	afx_msg void OnBnClickedBtnViewEqpStage1DataSwitching2();


	CxStatic m_Data_PumpOpenPercent;
};
