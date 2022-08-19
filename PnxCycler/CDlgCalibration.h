#pragma once


// CDlgCalibration 대화 상자
#include "Singleton.h"
#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"

class CDlgCalibration : public CDialogEx, public CSingleton< CDlgCalibration >
{
	DECLARE_DYNAMIC(CDlgCalibration)

public:
	CDlgCalibration(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgCalibration();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_CALIBRATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	   	 
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void Init_Control(); //Daq Read Data Table
	void Init_Data();	//Calibration Mode 초기화


	afx_msg void OnBnClickedButtonCalmodeApply();
	int m_Radio;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonCaltestStart();
};
