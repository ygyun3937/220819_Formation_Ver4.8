#pragma once


// CDlgSystemSafety ��ȭ �����Դϴ�.

#include "Singleton.h"
#include "ColorStaticST.h"


class CDlgSystemSafety : public CDialogEx, public CSingleton< CDlgSystemSafety >
{
	DECLARE_DYNAMIC(CDlgSystemSafety)

public:
	CDlgSystemSafety(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
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

// ��ȭ ���� �������Դϴ�.
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
