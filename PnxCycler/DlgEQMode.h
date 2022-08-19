#pragma once


// CDlgEQMode ��ȭ �����Դϴ�.
#include "SkinButton.h"
#include "ColorStaticST.h"
#include "afxwin.h"

enum EQModeStatic
{
	EQModeStaticSave = 0,
	EQModeStaticExit,
	EQModeStaticMaxCount
};

enum EQModeButton
{
	EQModeButtonSerial = 0,
	EQModeButtonParallel,
	EQModeButtonCV,
	EQModeButtonSave,
	EQModeButtonExit,
	EQModeButtonMaxCount
};

class CDlgEQMode : public CDialogEx, public CSingleton< CDlgEQMode >
{
	DECLARE_DYNAMIC(CDlgEQMode)

public:
	CDlgEQMode(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgEQMode();

	void	SetCVModeCheckButton(int nCh, BOOL bOn);
	void	SetPowerMapCheckButton(int nCh, BOOL bOn);
private:
	CSkinButton	m_btnEQMode[EQModeButtonMaxCount];

	CColorStaticST	m_ctrlEQMode[EQModeStaticMaxCount];

	int m_nEQMode;
	BOOL	m_bCVMode;

	BOOL	m_bPowerMap;


private:
	void	InitStatic();
	void	InitButton();

	void	LoadEQMode();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_EQUIPMENT_MODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnEqModeSave();
	afx_msg void OnBnClickedBtnEqModeExit();
	afx_msg void OnBnClickedRdoEqModeSerial();
	afx_msg void OnBnClickedRdoEqModeParallel();
	afx_msg void OnBnClickedCVMode();

	afx_msg void OnPaint();
	CButton m_check_CVMode;			//kw--- check �ڽ� ������� �߰�
	CButton m_check_CVMode2;
	afx_msg void OnBnClickedCheckCvmode2();
};
