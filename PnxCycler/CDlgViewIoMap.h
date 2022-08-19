#include "CxStatic.h"
#include "GridCtrl_src/GridCtrl.h"

// FORMATION
#define		IO_SLOT_MAX		4
#define		IO_PARA_COUNT	2
#define		IO_VALUE_COUNT	32
#define		IO_ADDRESS		0
#define		IO_NAME			1


#pragma once

// CDlgViewIoMap 대화 상자

class CDlgViewIoMap : public CDialogEx, public CSingleton< CDlgViewIoMap >
{
	DECLARE_DYNAMIC(CDlgViewIoMap)

public:
	CDlgViewIoMap(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgViewIoMap();

	// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_VIEW_IO_MAP };
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
	
	afx_msg void OnBnClickedBtnViewIoMapClose();
	afx_msg void OnBnClickedBtnViewIoMapInputPrev();
	afx_msg void OnBnClickedBtnViewIoMapInputNext();
	afx_msg void OnBnClickedBtnViewIoMapOutputPrev();
	afx_msg void OnBnClickedBtnViewIoMapOutputNext();

	afx_msg void OnCbnSelchangeCboViewIoMapSubeqpid();
	afx_msg void OnCbnSelchangeCboViewIoMapStageid();

public:
	//virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	void	InitializeMemberVariables();
	bool	InitializeControls();				// 초기화
	void	OnOutputClickCommand(UINT nID);		// OUTPUT BUTTON 

	void	InitializeStatusControl();

private:
	enum CommandType
	{
		CommandTypeOutput01,
		CommandTypeMaxCount
	};

private:
	bool	LoadAllDioInfo(int nSubEqpIndexNo, int nSelectedEqpStageNo);

	bool	LoadInputDioInfo(int nDioIndexNo);
	bool	LoadOutputDioInfo(int nDioIndexNo);

	bool	SetInputPageButton(int nDioPageNo, CString strDirection);
	bool	SetOutputPageButton(int nDioPageNo, CString strDirection);

	bool	SetInputIo(int nIndex);
	bool	SetOutputIo(int nIndex);

private:
	int		m_nInputDioCount;
	int		m_nOutputDioCount;

	int		m_nSubEqpIndexNo;
	int		m_nSubEqpId;
	int		m_nSelectedEqpStageNo;

	CComboBox m_cboSubEqpList;
	CComboBox m_cboEqpStageList;

	int		m_nCurrentInputPageNo;
	int		m_nCurrentOutputPageNo;

	CxStatic m_InputIO_Address[IO_VALUE_COUNT];
	CxStatic m_InputIO_Status[IO_VALUE_COUNT];
	CxStatic m_OuputIO_Address[IO_VALUE_COUNT];
	CxShadeButton m_OutPutIO_Status[IO_VALUE_COUNT];
public:
	afx_msg void OnBnClickedBtnViewIoInfoActOutput00();
};
