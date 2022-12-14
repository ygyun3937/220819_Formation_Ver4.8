#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"

#define OPERATION_MAX_COUNT			13
#define CYLINDER_STEP_COUNT		8
//220612 YGY Formation DCIR ADD
#define PRECHARGE_CONTROL_INPUT_COUNT 32
#define PRECHARGE_CONTROL_OUTPUT_COUNT 16

#define DCIR_CONTROL_INPUT_COUNT 56
#define DCIR_CONTROL_OUTPUT_COUNT 24



#pragma once

// CDlgViewIoManual 대화 상자
class CDlgViewIoManual : public CDialogEx, public CSingleton< CDlgViewIoManual >
{
	DECLARE_DYNAMIC(CDlgViewIoManual)

public:
	CDlgViewIoManual(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewIoManual();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_IO_MANUAL };
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

	void SetManualOperationButton(int nEqpStageIndexNo, bool usableValue);

private:
	
public:
	CxStatic m_StaticMoxaInputIO_Board1[8];

	CxShadeButton m_StaticMoxaOutputIO_Board1[8];

	void UI_Update_IO();

	afx_msg void OnBnClickedMoxa3Out1();
	afx_msg void OnBnClickedMoxa3Out2();
	afx_msg void OnBnClickedMoxa3Out3();
	afx_msg void OnBnClickedMoxa3Out4();
	afx_msg void OnBnClickedMoxa3Out5();
	afx_msg void OnBnClickedMoxa3Out6();
	afx_msg void OnBnClickedMoxa3Out7();
	afx_msg void OnBnClickedMoxa3Out8();

	afx_msg void OnBnClickedMoxaConnect();

	CxStatic	m_Control_DCIR_Static_InputIO[DCIR_CONTROL_INPUT_COUNT];
	CxShadeButton	m_Control_DCIR_Button_OutputIO[DCIR_CONTROL_OUTPUT_COUNT];

	CxStatic	m_Control_PreCharge_Static_InputIO[DCIR_CONTROL_INPUT_COUNT];
	CxShadeButton	m_Control_PreCharge_Button_OutputIO[DCIR_CONTROL_OUTPUT_COUNT];

	void UI_SetEQID();
	afx_msg void OnBnClickedMoxaioPrechargeOutput9();
	afx_msg void OnBnClickedMoxaioPrechargeOutput10();
	afx_msg void OnBnClickedMoxaioPrechargeOutput11();
	afx_msg void OnBnClickedMoxaioPrechargeOutput12();
	afx_msg void OnBnClickedMoxaioPrechargeOutput1();
	afx_msg void OnBnClickedMoxaioPrechargeOutput2();
	afx_msg void OnBnClickedMoxaioPrechargeOutput3();
	afx_msg void OnBnClickedMoxaioPrechargeOutput4();
	afx_msg void OnBnClickedMoxaioDcirOutput1();
	afx_msg void OnBnClickedMoxaioDcirOutput2();
	afx_msg void OnBnClickedMoxaioDcirOutput3();
	afx_msg void OnBnClickedMoxaioDcirOutput4();
	afx_msg void OnBnClickedMoxaioDcirOutput5();
	afx_msg void OnBnClickedMoxaioDcirOutput6();
	afx_msg void OnBnClickedMoxaioDcirOutput7();
	afx_msg void OnBnClickedMoxaioDcirOutput8();
	afx_msg void OnBnClickedMoxaioDcirOutput9();
	afx_msg void OnBnClickedMoxaioDcirOutput10();
	afx_msg void OnBnClickedMoxaioDcirOutput17();
	afx_msg void OnBnClickedMoxaioDcirOutput18();
	afx_msg void OnBnClickedMoxaioDcirOutput19();
	afx_msg void OnBnClickedMoxaioDcirOutput20();
	afx_msg void OnBnClickedMoxaioDcirOutput21();
	afx_msg void OnBnClickedMoxaioDcirOutput22();
	afx_msg void OnBnClickedMoxaioDcirOutput23();
	afx_msg void OnBnClickedMoxaioDcirOutput24();
	afx_msg void OnStnClickedMoxaioDcirInput26();
};
