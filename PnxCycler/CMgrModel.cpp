#include "stdafx.h"
#include "CMgrModel.h"
#include "MgrConfig.h"
#include <algorithm>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMgrModel::CMgrModel()
{
	m_nModelIndex		= -1;
	m_nScheduleIndex	= -1;
	m_nSyncMode			= eScheduleSyncModeNotUse;
	memset(m_bScheduleSelect, false, sizeof(m_bScheduleSelect));
}

CMgrModel::~CMgrModel()
{
	ClearModel();
}

void	CMgrModel::ClearModel()
{
	auto iter = m_vecModel.begin();
	auto iterE = m_vecModel.end();

	while (iter != iterE)
	{
		SModel* lpModel = (*iter);

		if (lpModel)
		{
			lpModel->Clear();

			delete lpModel;
			lpModel = nullptr;
		}

		++iter;
	}

	m_vecModel.clear();
}

bool	CMgrModel::LoadModel()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (!pMgr)
	{
		AfxMessageBox("Process started, but the saved information could not be retrieved. Please run the program again");

		return false;
	}

	m_strFilePath.Format(_T("%s"), (LPCSTR)pMgr->GetSchedulePath());

	if (m_strFilePath.IsEmpty())
	{
		AfxMessageBox("The schedule navigation path is empty. Please check the [FilePath]Schedule= section in the Common.ini file under the Config folder of the execution path.");

		return false;
	}

	ClearModel();

	SearchModel();

	auto iter = m_vecModel.begin();
	auto iterE = m_vecModel.end();

	while (iter != iterE)
	{
		SModel* lpModel = (*iter);

		if (lpModel)
		{
			CString strFilePath;
			strFilePath.Format("%s\\*.xml", (LPCSTR)lpModel->strFilePath);

			SearchSchedule(lpModel, strFilePath);
		}		

		++iter;
	}

	return true;
}

void	CMgrModel::SearchModel()
{
	CString strFilePath;
	strFilePath.Format("%s\\*.*", (LPCSTR)m_strFilePath);

	WIN32_FIND_DATA kParser;

	HANDLE hSearch = FindFirstFile(strFilePath, &kParser);

	if (INVALID_HANDLE_VALUE == hSearch)
		return;

	BOOL bExplorer = TRUE;

	while (bExplorer)
	{
		if (kParser.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(kParser.cFileName, ".") && strcmp(kParser.cFileName, ".."))
			{
				SModel* lpModel = new SModel;

				lpModel->strModelName.Format("%s", kParser.cFileName);
				lpModel->strFilePath.Format("%s\\%s", (LPCSTR)m_strFilePath, kParser.cFileName);

				CString strDescriptionFilePath;
				strDescriptionFilePath.Format("%s\\%s.desc", (LPCSTR)m_strFilePath, kParser.cFileName);

				// 이거 안읽힘..
				/*
				{
					FILE* fp = nullptr;
					fopen_s(&fp, strDescriptionFilePath, "r");

					if (nullptr != fp)
					{
						char szDescription[255];
						ZeroMemory(szDescription, 255);

						fgets(szDescription, 255, fp);

						sModel.strModelDescription.Format("%s", szDescription);
						sModel.strModelDescription.Replace("\r", "");
						sModel.strModelDescription.Replace("\n", "");

						fclose(fp);
					}
				}
				*/
				
				SYSTEMTIME stTimeSystem, stCreateLocal;
				FileTimeToSystemTime(&kParser.ftLastWriteTime, &stTimeSystem);
				SystemTimeToTzSpecificLocalTime(NULL, &stTimeSystem, &stCreateLocal);

				char szLocalDate[255];
				GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &stCreateLocal, NULL, szLocalDate, 255);

				char szLocalTime[255];
				GetTimeFormat(LOCALE_USER_DEFAULT, 0, &stCreateLocal, NULL, szLocalTime, 255);

				lpModel->strWriteTime.Format("%s %s", szLocalDate, szLocalTime);
				lpModel->strSerialNumber.Format("%d", m_vecModel.size() + 1);

				m_vecModel.push_back(lpModel);
			}
		}

		bExplorer = FindNextFile(hSearch, &kParser);
	}

	FindClose(hSearch);
}

bool sortData(const SSchedule* a, const SSchedule* b)
{
	if (a->sModifyTime < b->sModifyTime)
		return true;
	else
		return false;
}

void	CMgrModel::SearchSchedule(SModel* lpModel, CString strFilePath)
{
	lpModel->Clear();

	WIN32_FIND_DATA kParser;

	HANDLE hSearch = FindFirstFile(strFilePath, &kParser);

	if (INVALID_HANDLE_VALUE == hSearch)
		return;

	vector<SSchedule*> lpScheduleList;

	BOOL bExplorer = TRUE;

	while (bExplorer)
	{
		if (kParser.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

		}
		else
		{
			SSchedule* lpSchedule = new SSchedule;

			lpSchedule->strScheduleName.Format("%s", kParser.cFileName);
			lpSchedule->strScheduleName.Replace(".xml", "");

			lpSchedule->strScheduleFilePath.Format("%s\\%s\\%s", (LPCSTR)m_strFilePath, (LPCSTR)lpModel->strModelName, kParser.cFileName);

			{
				SYSTEMTIME stTimeSystem, stCreateLocal;
				FileTimeToSystemTime(&kParser.ftCreationTime, &stTimeSystem);
				SystemTimeToTzSpecificLocalTime(NULL, &stTimeSystem, &stCreateLocal);

				char szLocalDate[255];
				GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &stCreateLocal, NULL, szLocalDate, 255);

				char szLocalTime[255];
				GetTimeFormat(LOCALE_USER_DEFAULT, 0, &stCreateLocal, NULL, szLocalTime, 255);

				lpSchedule->strWriteTime.Format("%s %s", szLocalDate, szLocalTime);
			}

			{
				SYSTEMTIME stTimeSystem, stCreateLocal;
				FileTimeToSystemTime(&kParser.ftLastWriteTime, &stTimeSystem);
				SystemTimeToTzSpecificLocalTime(NULL, &stTimeSystem, &stCreateLocal);

				char szLocalDate[255];
				GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &stCreateLocal, NULL, szLocalDate, 255);

				char szLocalTime[255];
				GetTimeFormat(LOCALE_USER_DEFAULT, 0, &stCreateLocal, NULL, szLocalTime, 255);

				lpSchedule->strModifyTime.Format("%s %s", szLocalDate, szLocalTime);
				lpSchedule->sModifyTime = stCreateLocal;
			}

			//list에 추가
			lpScheduleList.push_back(lpSchedule);
		}

		bExplorer = FindNextFile(hSearch, &kParser);
	}

	//추가된 리스트를 sort후 lpModel에 다시 추가
	sort(lpScheduleList.begin(), lpScheduleList.end(), sortData);
	for (int i = 0; i < lpScheduleList.size(); i++) 
	{
		lpScheduleList[i]->strSerialNumber.Format("%d", i + 1);
		lpModel->push_back(lpScheduleList[i]);
	}
	//완료 후 삭제
	lpScheduleList.clear();

	FindClose(hSearch);
}

SModel*	CMgrModel::GetModel(int nIndex)
{
	if (nIndex < 0 || (int)m_vecModel.size() <= nIndex)
		return nullptr;

	return m_vecModel[nIndex];
}

std::vector<SModel*>&	CMgrModel::GetModelVector()
{
	return m_vecModel;
}

