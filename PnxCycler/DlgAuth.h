#pragma once


// CDlgAuth dialog

class CDlgAuth : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAuth)

public:
	CDlgAuth(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAuth();

// Dialog Data
	enum { IDD = IDD_DLGAUTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	SYSTEMTIME sys_time;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedButtonAuthKey();
	afx_msg void OnBnClickedButtonAuthId();
	afx_msg void OnBnClickedCancel();
	virtual void OnOK();
	bool GetMD5Hash(char *buffer, DWORD dwBufferSize, BYTE *byteFinalHash, DWORD* dwFinalHashSize);
};
