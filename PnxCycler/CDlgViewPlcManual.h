﻿#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"

#include "CDlgViewPlc1.h"
#include "CDlgViewPlc2.h"

#define OPERATION_MAX_COUNT			13
#define CYLINDER_STEP_COUNT		8

#pragma once

// CDlgViewPlcManual 대화 상자
class CDlgViewPlcManual : public CDialogEx, public CSingleton< CDlgViewPlcManual >
{
	DECLARE_DYNAMIC(CDlgViewPlcManual)

public:
	CDlgViewPlcManual(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewPlcManual();




// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_PLC_MANUAL };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();

	afx_msg void OnBnClickedBtnViewIoManualS1RunmodeAuto();
	afx_msg void OnBnClickedBtnViewIoManualS1RunmodeManual();
	afx_msg void OnBnClickedBtnViewIoManualS1SideCenteringFwd();
	afx_msg void OnBnClickedBtnViewIoManualS1FrontbackCenteringFwd();
	afx_msg void OnBnClickedBtnViewIoManualS1TopCylinderDescend();
	afx_msg void OnBnClickedBtnViewIoManualS1BottomCylinderAscend();
	afx_msg void OnBnClickedBtnViewIoManualS1BottomCylinderDescend();
	afx_msg void OnBnClickedBtnViewIoManualS1TopCylinderAcend();
	afx_msg void OnBnClickedBtnViewIoManualS1FrontbackCenteringBwd();
	afx_msg void OnBnClickedBtnViewIoManualS1SideCenteringBwd();

	afx_msg void OnBnClickedBtnViewIoManualS2RunmodeAuto();
	afx_msg void OnBnClickedBtnViewIoManualS2RunmodeManual();
	afx_msg void OnBnClickedBtnViewIoManualS2SideCenteringFwd();
	afx_msg void OnBnClickedBtnViewIoManualS2FrontbackCenteringFwd();
	afx_msg void OnBnClickedBtnViewIoManualS2TopCylinderDescend();
	afx_msg void OnBnClickedBtnViewIoManualS2BottomCylinderAscend();
	afx_msg void OnBnClickedBtnViewIoManualS2BottomCylinderDescend();
	afx_msg void OnBnClickedBtnViewIoManualS2TopCylinderAcend();
	afx_msg void OnBnClickedBtnViewIoManualS2FrontbackCenteringBwd();
	afx_msg void OnBnClickedBtnViewIoManualS2SideCenteringBwd();

public:
	//virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);



private:
	void Initialize();
	void InitializeVariables();
	void InitializeControls();
	void InitializeTimers();

//	void SetManualOperationButton(int nEqpStageIndexNo, bool usableValue);

//	void UpdateManualOperationStep();
	//void SetOperationStepColorS1(int nSubEqpNo, enumProcessStepManualS1 nOperationStepNo);
	//void SetOperationStepColorS2(int nSubEqpNo, enumProcessStepManualS2 nOperationStepNo);

private:
	int		m_nSelectedSubEqpNo;			//현재 사용 하지 않음
	CString m_strSelectedSubEqpId;

	CxStatic m_staticOperationControlS1[OPERATION_MAX_COUNT];
	CxStatic m_staticOperationControlS2[OPERATION_MAX_COUNT];

	CxShadeButton m_staticButton_Stage1_Step[CYLINDER_STEP_COUNT];
	CxShadeButton m_staticButton_Stage2_Step[CYLINDER_STEP_COUNT];
public:

	void UI_Step_Stage1_Blink(int nCurrentStep, bool bBlinkFlag);
	void UI_Step_Stage2_Blink(int nCurrentStep, bool bBlinkFlag);
	void UI_Step_Stage1_Status();
	void UI_Step_Stage2_Status();

	CxStatic m_StaticMoxaInputIO_Board1[8];

	CxShadeButton m_StaticMoxaOutputIO_Board1[8];
	void SetPlcConnectingStatus(CString str);	//[JMW:220321]
	void SetPlcConnectingStatus2(CString str);

	//void UI_Update_IO();

	void SetEditSendData();
	void GetPlcReceive();
	void GetPlcReceiveBlock();
	void SetPlcSend();

	void SetEditSendData2();
	void GetPlcReceive2();
	void SetPlcSend2();
	void CommPlcAndPc(); //[JMW:220314]
	void CommPlcAndPc2();

	void SetBcrData(CString strBcrData);
	CString m_strBcrBackData;
	afx_msg void OnBnClickedMoxaLoad();
	afx_msg void OnBnClickedMoxaStop();
	afx_msg void OnBnClickedPlcOpen();
	afx_msg void OnBnClickedPlcClose();
	afx_msg void OnBnClickedPlc1BitReset();
	afx_msg void OnBnClickedPlcOpen2();
	afx_msg void OnBnClickedPlcClose2();
	afx_msg void OnBnClickedPlcSendReaddata();
	afx_msg void OnBnClickedPlcTestStart();
	afx_msg void OnBnClickedPlcTestStop();
	//[JMW:220314]
	CMFCButton m_bPlc1Bit[TYPE_PLC_MAX];
	CMFCButton m_bPlc2Bit[TYPE_PLC_MAX];
	void BitButtonColorAllReset();
	void ButtonBackgroundValue(int* Rgb, CString bit);
	afx_msg void OnBnClickedPlc1ReadData5();
	afx_msg void OnBnClickedPlc1ReadData2();
	afx_msg void OnBnClickedPlc1ReadData3();
	afx_msg void OnBnClickedPlc1ReadData4();
	afx_msg void OnBnClickedPlc1ReadData6();
	afx_msg void OnBnClickedPlc1SendData2();
	afx_msg void OnBnClickedPlc1SendData3();
	afx_msg void OnBnClickedPlc1SendData4();
	afx_msg void OnBnClickedPlc1SendData5();
	afx_msg void OnBnClickedPlc1SendData6();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedPlcTestStart2();
	CMFCButton m_bRead2Bit1;
	CMFCButton m_bRead2Bit2;
	CMFCButton m_bRead2Bit3;
	CMFCButton m_bRead2Bit4;
	CMFCButton m_bRead2Bit5;
	CMFCButton m_bRead2Bit6;
	CMFCButton m_bSend2Bit1;
	CMFCButton m_bSend2Bit2;
	CMFCButton m_bSend2Bit3;
	CMFCButton m_bSend2Bit4;
	CMFCButton m_bSend2Bit5;
	CMFCButton m_bSend2Bit6;
	afx_msg void OnBnClickedPlc2ReadData2();
	afx_msg void OnBnClickedPlc2ReadData3();
	afx_msg void OnBnClickedPlc2ReadData4();
	afx_msg void OnBnClickedPlc2ReadData5();
	afx_msg void OnBnClickedPlc2ReadData6();
	afx_msg void OnBnClickedPlc2SendData2();
	afx_msg void OnBnClickedPlc2SendData3();
	afx_msg void OnBnClickedPlc2SendData4();
	afx_msg void OnBnClickedPlc2SendData5();
	afx_msg void OnBnClickedPlc2SendData6();
	afx_msg void OnBnClickedPlc2BitReset();
	afx_msg void OnBnClickedPlcTestStop2();
	afx_msg void OnBnClickedPlcSendReaddata2();
	afx_msg void OnBnClickedPlc1SendData7();
};
