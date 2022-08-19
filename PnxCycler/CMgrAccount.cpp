#include "stdafx.h"
#include "CMgrAccount.h"
#include "MgrConfig.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMgrAccount::CMgrAccount()
{
	m_strLoginID.Empty();
	m_strLoginPW.Empty();

	m_bModified = false;

	m_loginAccount = nullptr;

	LoadAccount();

	//LoadCustomChannelView();
}

CMgrAccount::~CMgrAccount()
{
	{
		auto iter = m_vecAccount.begin();
		auto iterE = m_vecAccount.end();

		while (iter != iterE)
		{
			delete (*iter);

			++iter;
		}

		m_vecAccount.clear();
	}

	{
		auto iter = m_mapCustom.begin();
		auto iterE = m_mapCustom.end();

		while (iter != iterE)
		{
			sCustomView* lpView = iter->second;

			if (lpView)
			{
				auto it = lpView->vecViewItem.begin();
				auto itE = lpView->vecViewItem.end();

				while (it != itE)
				{
					delete (*it);

					++it;
				}

				lpView->vecViewItem.clear();

				delete lpView;

				lpView = nullptr;
			}

			++iter;
		}

		m_mapCustom.clear();
	}
}

void	CMgrAccount::SaveAccount()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\Users.ini"), (LPCSTR)GetExePath());

	DeleteFile(strFilePath);

	INIWriteInt("UserCount", "Count", (int)m_vecAccount.size(), strFilePath);

	int nIndex = 0;

	auto iter	= m_vecAccount.begin();
	auto iterE	= m_vecAccount.end();

	while (iter != iterE)
	{
		sAccount* lpAccount = (*iter);

		CString strAppName;
		strAppName.Format("USERS%d", nIndex++);

		INIWriteStr(strAppName, "Name"	, lpAccount->szUserID		, strFilePath);
		INIWriteStr(strAppName, "Pwd"	, lpAccount->szUserPW		, strFilePath);
		INIWriteStr(strAppName, "UserLevel"	, lpAccount->szUserLevel		, strFilePath);
		INIWriteStr(strAppName, "Path"	, lpAccount->szSaveFilePath	, strFilePath);

		if (true == lpAccount->bUseDAQView)
			INIWriteStr(strAppName, "DaQUse", "Use", strFilePath);
		else
			INIWriteStr(strAppName, "DaQUse", "NotUse", strFilePath);

		++iter;
	}
}

void	CMgrAccount::LoadAccount()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\Users.ini"), (LPCSTR)GetExePath());

	bool bExistDefaultID = false;

	int nAccountSize = INIReadInt("UserCount", "Count", strFilePath);

	for (int i = 0; i < nAccountSize; ++i)
	{
		CString strAppName;
		strAppName.Format("USERS%d", i);

		sAccount* lpAccount = new sAccount;

		_tcscpy_s(lpAccount->szUserID		, INIReadStr(strAppName, "Name", strFilePath));
		_tcscpy_s(lpAccount->szUserPW		, INIReadStr(strAppName, "Pwd", strFilePath));
		_tcscpy_s(lpAccount->szUserLevel		, INIReadStr(strAppName, "UserLevel", strFilePath));
		_tcscpy_s(lpAccount->szSaveFilePath	, INIReadStr(strAppName, "Path", strFilePath));

		CString strDAQView = INIReadStr(strAppName, "DaQUse", strFilePath);

		//if (strDAQView.Compare("Use") == 0)
			lpAccount->bUseDAQView = true;

		m_vecAccount.push_back(lpAccount);

		AddAccount(lpAccount->szUserID, false);

		if (_tcsicmp(lpAccount->szUserID, "Admin") == 0)
			bExistDefaultID = true;
	}

	if (false == bExistDefaultID)
	{
		// Default 계정
		{
			sAccount* lpAccount = new sAccount;

			_tcscpy_s(lpAccount->szUserID, "Admin");
			_tcscpy_s(lpAccount->szUserPW, "1");

			CMgrConfig* pMgr = CMgrConfig::GetMgr();

			if (pMgr)
			{
				_tcscpy_s(lpAccount->szSaveFilePath, pMgr->GetLogFilePath());
			}			

			lpAccount->bUseDAQView = false;

			m_vecAccount.push_back(lpAccount);

			AddAccount(lpAccount->szUserID, false);
		}

		SaveAccount();
	}
}

void	CMgrAccount::LoadCustomChannelView()
{
	auto iter = m_mapCustom.begin();
	auto iterE = m_mapCustom.end();

	while (iter != iterE)
	{
		CString strFilePath;

		strFilePath.Format("%s\\CustomView\\%s.ini", (LPCSTR)GetExePath(), (LPCSTR)iter->first);

		sCustomView* pView = iter->second;

		_tcscpy_s(pView->szUserID, iter->first);

		for (int i = 0; i < eCyclerChannelItemNameMaxCount; ++i)
		{
			CString strAppName;
			strAppName.Format("%s", g_lpszCyclerChannelItemName[i]);

			sChannelView* lpCyclerChannelView = new sChannelView;

			lpCyclerChannelView->nWidth		= INIReadInt(strAppName, "Width", strFilePath);
			lpCyclerChannelView->nSerialNo	= INIReadInt(strAppName, "Serial", strFilePath);

			_tcscpy_s(lpCyclerChannelView->szName	, INIReadStr(strAppName, "Name", strFilePath));
			_tcscpy_s(lpCyclerChannelView->szHeader	, strAppName);

			pView->vecViewItem.push_back(lpCyclerChannelView);
		}

		// 작업자별 등록을 마친 상태에서 Serial 별로 미리 정렬 해두면.. 나중에 또 작업할 필요는 없다!
		sort(pView->vecViewItem.begin(), pView->vecViewItem.end(), sChannelView::sCompareGreater());

		++iter;
	}
}

void	CMgrAccount::SaveCustomChannelView()
{
	auto it = m_mapCustom.find(m_strLoginID);

	if (it == m_mapCustom.end())
		return;

	CString strFilePath;
	strFilePath.Format("%s\\CustomView\\%s.ini", (LPCSTR)GetExePath(), (LPCSTR)it->first);

	sCustomView* lpCustomView = it->second;

	if (nullptr == lpCustomView)
		return;

	auto iter = lpCustomView->vecViewItem.begin();
	auto iterE = lpCustomView->vecViewItem.end();

	while (iter != iterE)
	{
		sChannelView* lpView = (*iter);

		CString strAppName;
		strAppName.Format("%s", lpView->szHeader);

		INIWriteInt(strAppName, "Serial", lpView->nSerialNo, strFilePath);

		++iter;
	}
}

void	CMgrAccount::SaveCustomChannelView(CString strID)
{
	auto it = m_mapCustom.find(strID);

	if (it == m_mapCustom.end())
		return;

	CString strFilePath;
	strFilePath.Format("%s\\CustomView\\%s.ini", (LPCSTR)GetExePath(), (LPCSTR)it->first);

	sCustomView* lpCustomView = it->second;

	if (nullptr == lpCustomView)
		return;

	auto iter = lpCustomView->vecViewItem.begin();
	auto iterE = lpCustomView->vecViewItem.end();

	while (iter != iterE)
	{
		sChannelView* lpView = (*iter);

		CString strAppName;
		strAppName.Format("%s", lpView->szHeader);

		INIWriteInt(strAppName, "Width"	, lpView->nWidth	, strFilePath);
		INIWriteInt(strAppName, "Serial", lpView->nSerialNo	, strFilePath);
		INIWriteStr(strAppName, "Name"	, lpView->szName	, strFilePath);

		++iter;
	}
}

void	CMgrAccount::SetDefaultCustomView(sCustomView* lpCustomView)
{
	CString strFilePath;
	strFilePath.Format("%s\\CustomView\\Default.ini", (LPCSTR)GetExePath());

	for (int i = 0; i < eCyclerChannelItemNameMaxCount; ++i)
	{

		CString strAppName;
		strAppName.Format("%s", g_lpszCyclerChannelItemName[i]);

		sChannelView* lpCyclerChannelView = new sChannelView;

		lpCyclerChannelView->nWidth		= INIReadInt(strAppName, "Width", strFilePath);
		lpCyclerChannelView->nSerialNo	= INIReadInt(strAppName, "Serial", strFilePath);

		_tcscpy_s(lpCyclerChannelView->szName	, INIReadStr(strAppName, "Name", strFilePath));
		_tcscpy_s(lpCyclerChannelView->szHeader	, strAppName);

		lpCustomView->vecViewItem.push_back(lpCyclerChannelView);
	}

	// 작업자별 등록을 마친 상태에서 Serial 별로 미리 정렬 해두면.. 나중에 또 작업할 필요는 없다!
	sort(lpCustomView->vecViewItem.begin(), lpCustomView->vecViewItem.end(), sChannelView::sCompareGreater());
}

void	CMgrAccount::AddAccount(CString strID, bool bDefault)
{
	auto iter = m_mapCustom.find(strID);

	if (iter == m_mapCustom.end())
	{
		// 데이터는 채우지 않고 일단 추가한다.
		sCustomView* pView = new sCustomView;

		if (true == bDefault)
		{
			SetDefaultCustomView(pView);
		}

		m_mapCustom.insert(std::make_pair(strID, pView));
	}
}

bool	CMgrAccount::IsLogin()
{
	//if (!m_strLoginID.IsEmpty() && !m_strLoginPW.IsEmpty())
		//return true;

	//lyw - 검증 완료
	if (m_loginAccount != nullptr)
		return true;
	else
		return false;
}

//lyw - 검증완료
void	CMgrAccount::Logout()
{
	m_loginAccount = nullptr;
}



bool	CMgrAccount::IsExistUserID(CString strID)
{
	auto iter = m_vecAccount.begin();
	auto iterE = m_vecAccount.end();

	while (iter != iterE)
	{
		sAccount* lpAccount = (*iter);

		if (lpAccount)
		{
			if (strcmp(lpAccount->szUserID, strID) == 0)
			{
				return true;
			}
		}

		++iter;
	}

	return false;
}


int		CMgrAccount::GetCustomViewSize()
{
	if (false == IsLogin())
		return -1;

	auto iter = m_mapCustom.find(m_strLoginID);

	if (iter == m_mapCustom.end())
		return -1;

	return (int)iter->second->vecViewItem.size();
}

sAccount*	CMgrAccount::LogInAccount(CString strID, CString strPW)
{
	auto iter = m_vecAccount.begin();
	auto iterE = m_vecAccount.end();

	while (iter != iterE)
	{
		sAccount* lpAccount = (*iter);

		if (lpAccount)
		{
			if (strcmp(lpAccount->szUserID, strID) == 0 && strcmp(lpAccount->szUserPW, strPW) == 0)
			{
				m_strLoginID.Format("%s", (LPCSTR)strID);
				m_strLoginPW.Format("%s", (LPCSTR)strPW);

				//lyw - 검증 완료
				this->m_loginAccount = lpAccount;

				return lpAccount;
			}

		}

		++iter;
	}

	return nullptr;
}

sAccount*	CMgrAccount::GetLoginedAccount()
{
	if (false == IsLogin())
		return nullptr;

	auto iter = m_vecAccount.begin();
	auto iterE = m_vecAccount.end();

	while (iter != iterE)
	{
		sAccount* lpAccount = (*iter);

		if (lpAccount)
		{
			if (strcmp(lpAccount->szUserID, m_strLoginID) == 0)
			{
				return lpAccount;
			}
		}

		++iter;
	}

	return nullptr;
}

sCustomView*	CMgrAccount::GetCustomView()
{
	if (false == IsLogin())
		return nullptr;

	auto iter = m_mapCustom.find(m_strLoginID);

	if (iter != m_mapCustom.end())
		return iter->second;

	return nullptr;
}

void	CMgrAccount::SortCustomView(int nCurSel, eCustomViewSortType eType)
{
	// 이미 GUI Event 예외 조건은 해두고 온것이기 때문에 다시 예외처리 할 필요는 없다.

	if (false == IsLogin())
		return;

	auto iter = m_mapCustom.find(m_strLoginID);

	if (iter == m_mapCustom.end())
		return;

	sCustomView* lpCustomView = iter->second;

	if (nullptr == lpCustomView)
		return;

	if (eCustomViewSortTypeUp == eType)
	{
		// 선택된 항목과 그 위에 존재하는 항목의 Serial 값을 바꾸고 재 정렬한다.
		lpCustomView->vecViewItem[nCurSel]->nSerialNo -= 1;
		lpCustomView->vecViewItem[nCurSel - 1]->nSerialNo += 1;
	}
	else
	{
		// 선택된 항목과 그 위에 존재하는 항목의 Serial 값을 바꾸고 재 정렬한다.
		lpCustomView->vecViewItem[nCurSel]->nSerialNo += 1;
		lpCustomView->vecViewItem[nCurSel + 1]->nSerialNo -= 1;
	}

	sort(lpCustomView->vecViewItem.begin(), lpCustomView->vecViewItem.end(), sChannelView::sCompareGreater());

	// 작업자별 정보 File 에 Update
	SaveCustomChannelView();

	// 해당 작업을 true 로 해두면.. 다시 그리는 쪽에서 재 생성한다
	m_bModified = true;
}

void	CMgrAccount::AddNewAccount(sAccount* pNewElement)
{
	// 새로 생성되면.. 음 기본값으로 데이터를 채울까??
	m_vecAccount.push_back(pNewElement);

	AddAccount(pNewElement->szUserID, true);

	SaveAccount();

	// 여기서 Save???
	SaveCustomChannelView(pNewElement->szUserID);
}