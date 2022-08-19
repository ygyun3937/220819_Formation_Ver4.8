// DlgAuth.cpp : implementation file
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgAuth.h"
#include "afxdialogex.h"

#include <wincrypt.h>


// CDlgAuth dialog

IMPLEMENT_DYNAMIC(CDlgAuth, CDialogEx)

CDlgAuth::CDlgAuth(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAuth::IDD, pParent)
{

}

CDlgAuth::~CDlgAuth()
{
}

void CDlgAuth::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAuth, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgAuth::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgAuth::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON_AUTH_KEY, &CDlgAuth::OnBnClickedButtonAuthKey)
	ON_BN_CLICKED(IDC_BUTTON_AUTH_ID, &CDlgAuth::OnBnClickedButtonAuthId)
	ON_BN_CLICKED(IDCANCEL, &CDlgAuth::OnBnClickedCancel)
END_MESSAGE_MAP()




BOOL CDlgAuth::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CheckDlgButton(IDC_RADIO1, TRUE);
	OnBnClickedRadio1();

	GetSystemTime(&sys_time);

	CString strDate, strTime;
	strDate.Format("%04d / %02d / %02d", sys_time.wYear, sys_time.wMonth, sys_time.wDay);
	strTime.Format("%02d : %02d", sys_time.wHour, sys_time.wMinute);
	
	SetDlgItemText(IDC_STATIC_DATE, strDate);
	SetDlgItemText(IDC_STATIC_TIME, strTime);

	return TRUE;
}

void CDlgAuth::OnBnClickedRadio1()
{
	GetDlgItem(IDC_EDIT_KEY1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_KEY2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_KEY3)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_KEY4)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_AUTH_KEY)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_AUTH_ID)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_KEY1)->SetFocus();
}


void CDlgAuth::OnBnClickedRadio2()
{
	GetDlgItem(IDC_EDIT_KEY1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_KEY2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_KEY3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_KEY4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_AUTH_KEY)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_AUTH_ID)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_ID)->SetFocus();
}


void CDlgAuth::OnBnClickedButtonAuthKey()
{
	char szBuffer[16];
	BYTE hash[128];
	DWORD hashLength = 128;

	memset(szBuffer, 0x00, sizeof(szBuffer));
	sprintf(szBuffer, "%04d%02d%02d%02d%02d", sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute);
	if (!GetMD5Hash(szBuffer, strlen(szBuffer), hash, &hashLength)) {
		AfxMessageBox("키 생성 실패. 다시 시도해 주세요.");
		return;
	}
	CString hashKey;
	hashKey.Format("%02X%02X", hash[0], hash[1]);

	CString inputKey(""), temp;
	GetDlgItemText(IDC_EDIT_KEY1, temp);
	inputKey.Append(temp);
	GetDlgItemText(IDC_EDIT_KEY2, temp);
	inputKey.Append(temp);
	GetDlgItemText(IDC_EDIT_KEY3, temp);
	inputKey.Append(temp);
	GetDlgItemText(IDC_EDIT_KEY4, temp);
	inputKey.Append(temp);

	inputKey.MakeUpper();
	if (StrCmp(hashKey, inputKey) != 0) {
		AfxMessageBox("키 매칭 실패. 다시 시도해 주세요.");
	}
	else {
		AfxMessageBox("성공");
		CDialogEx::OnOK();
	}
}


void CDlgAuth::OnBnClickedButtonAuthId()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\SDI_Account.ini"), (LPCSTR)GetExePath());

	CString id = INIReadStr("Account", _T("id"), strFilePath);
	CString password = INIReadStr("Account", _T("password"), strFilePath);

	CString inputId, inputPw;
	GetDlgItemText(IDC_EDIT_ID, inputId);
	GetDlgItemText(IDC_EDIT_PASS, inputPw);

	if (id.Compare(inputId) != 0 || password.Compare(inputPw) != 0) {
		AfxMessageBox("아이디 혹은 비밀번호가 다릅니다.");
	}
	else {
		CDialogEx::OnOK();
	}
}


void CDlgAuth::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CDlgAuth::OnOK()
{
}

bool CDlgAuth::GetMD5Hash(char *buffer, DWORD dwBufferSize, BYTE *byteFinalHash, DWORD* dwFinalHashSize)
{
	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	DWORD cbHashSize = 0;

	try {
		if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == 0) {
			throw "CryptAcquireContext";
		}

		if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == 0) {
			throw "CryptCreateHash";
		}

		if (CryptHashData(hHash, (const BYTE*)buffer, dwBufferSize, 0) == 0) {
			throw "CryptHashData";
		}

		DWORD dwCount = sizeof(DWORD);
		if (CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&cbHashSize, &dwCount, 0) == 0) {
			throw "CryptGetHashParam";
		}

		if (*dwFinalHashSize < cbHashSize) {
			printf("\n Output buffer %d, isnot sufficient, Required size = %d", *dwFinalHashSize, cbHashSize);
			bResult = TRUE;
			throw "Memory problem";
		}

		if (CryptGetHashParam(hHash, HP_HASHVAL, byteFinalHash, dwFinalHashSize, 0)) {
			bResult = TRUE;
		}
		else {
			throw "CryptGetHashParam";
		}
	}
	catch (char* errMsg) {
		printf("%s failed, Error = 0x%.8x\n", errMsg, GetLastError());
	}

	if (hHash) {
		CryptDestroyHash(hHash);
	}

	if (hProv) {
		CryptReleaseContext(hProv, 0);
	}

	return bResult;
}