#pragma once


// CDlgSystemSafety 대화 상자입니다.

#include "Singleton.h"
#include "ColorStaticST.h"


class CDlgSystemSafety : public CDialogEx, public CSingleton< CDlgSystemSafety >
{
	DECLARE_DYNAMIC(CDlgSystemSafety)

public:
	CDlgSystemSafety(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSystemSafety();

	enum { IDD = IDC_DLG_SYSTEM_SAFETY_CONDITION };
private:
	enum eSystemSafetyObject
	{
		eSystemSafetyObjectVoltage = 0,
		eSystemSafetyObjectCurrent,
		eSystemSafetyObjectCapacity,
		eSystemSafetyObjectTemperature,
		eSystemSafetyObjectDeltaV,
		eSystemSafetyObjectDeltaI,
		eSystemSafetyObjectDeltaTime,
		eSystemSafetyObjectErrorLoggingTime,
		eSystemSafetyObjectCvInterlockDeltaV,
		eSystemSafetyObjectCvInterlockDeltaY,
		eSystemSafetyObjectMaxCount
	};

	enum eSystemSafetyType
	{
		eSystemSafetyTypeMin = 0,
		eSystemSafetyTypeMax,
		eSystemSafetyTypeMaxCount
	};

	CColorStaticST m_ctrlTitle[eSystemSafetyObjectMaxCount];	
	CColorStaticST m_ctrlUnits[eSystemSafetyObjectMaxCount];
	CColorStaticST m_ctrlValue[eSystemSafetyTypeMaxCount];
private:
	void	InitStatic();

	void	LoadSystemSafety();

// 대화 상자 데이터입니다.
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnSystemSafetySave();
	afx_msg void OnBnClickedBtnSystemSafetyClose();	
};
