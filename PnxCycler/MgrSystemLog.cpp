#include "stdafx.h"
#include "MgrSystemLog.h"
#include <sys/stat.h>
//#include <unistd.h>
#include <string>
#include <fstream>

CMgrSystemLog::CMgrSystemLog()
{
	m_pThread = NULL;
	m_bLoop = TRUE;

	if (NULL == m_pThread)
		m_pThread = AfxBeginThread(QueuingThread, this);
}


CMgrSystemLog::~CMgrSystemLog()
{	
	if (m_pThread != NULL)
	{
		m_bLoop = FALSE;

		::WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		m_pThread = NULL;
	}

	//if (!m_Queue.IsEmpty())
	//{
	//	CString strDebug;
	//	strDebug.Format(_T("delete syslog count %d"), m_Queue.GetSize());

		while (!m_Queue.IsEmpty())
			delete m_Queue.RemoveHead();
	//}

// 	while (!m_Queue.IsEmpty())
// 	{
// 		LOGDATA* pLog = m_Queue.RemoveHead();
// 
// 		if (NULL != pLog)
// 		{
// 			WriteLog(pLog);
// 
// 			delete pLog;
// 		}
// 
// 		Sleep(1);
// 	}

// 	while (!m_TxtQueue.IsEmpty())
// 		delete m_TxtQueue.RemoveHead();
}

void CMgrSystemLog::LOG(LogType nType, const char* fmt, ...)
{
	if (fmt != NULL)
	{
		SYSTEMTIME	cur_time;
		va_list		args;

		GetLocalTime(&cur_time);

		_TCHAR szLog[1024];

		ZeroMemory(szLog, sizeof(szLog));
		va_start(args, fmt);
		wvsprintf(szLog, fmt, args);
		va_end(args);

		if (m_bLoop)
		{
			LOGDATA* pData = new LOGDATA();
			if (pData != nullptr)
			{
				pData->Type = nType;
				pData->Time = cur_time;
				pData->strValue.Format(_T("%s"), szLog);

				AddTxtQueue(pData);
			}
			
		}
	}
}

inline bool file_exists(CString name) {
	struct stat buffer;
	return (stat(name.GetBuffer(), &buffer) == 0);

}
BOOL CMgrSystemLog::WriteLog(LOGDATA* pData)
{
//	FILE* fp = NULL;
//
//	SYSTEMTIME LogTime = pData->Time;
//	CString	strPath = _T("");
//
//	strPath.Format("%s\\MainLog\\%04d_%02d_%02d_%02d.log",
//		GetExePath(),
//		LogTime.wYear,
//		LogTime.wMonth,
//		LogTime.wDay,
//		LogTime.wHour);
//
//	fopen_s(&fp, (LPSTR)(LPCSTR)strPath, "a+");
//
//	if (fp != NULL)
//	{
//		CString strLog;
//		strLog.Format("%04d-%02d-%02d %02d:%02d:%02d:%03ld : ",
//			LogTime.wYear,
//			LogTime.wMonth,
//			LogTime.wDay,
//			LogTime.wHour,
//			LogTime.wMinute,
//			LogTime.wSecond,
//			LogTime.wMilliseconds);
//
////		BOOL bWriteWindow = FALSE;
//
//// 		switch (pData->Type)
//// 		{
//// 		case System:	strLog += _T("[SYSTEM    ] : "); bWriteWindow = TRUE; break;
//// 		case Dbg:		strLog += _T("[DEBUG     ] : "); bWriteWindow = FALSE; break;
//// 		case Operation: strLog += _T("[OPERATION ] : "); bWriteWindow = TRUE; break;
//// 		case Err:		strLog += _T("[ERROR     ] : "); bWriteWindow = TRUE; break;
//// 		}
//
//		strLog += pData->strValue;
//
//		fprintf(fp, "%s\r\n", (LPSTR)(LPCSTR)strLog);
//		fflush(fp);
//		fclose(fp);
//
//		//CDlgMainTitle의 리치에딧 컨트롤에 단순히 View만 해주도록 메세지 전달함
//		//SendMessgae를 어디서 적절히 처리 할지는 구현 후 정리 필요
//		//if (CDlgMainTitle::GetMgr() != NULL)
//		//{
//		//	//::PostMessage
//		//	if (CDlgMainTitle::GetMgr()->IsSystemLogViewOption() == TRUE)
//		//	{
//		//		::SendMessage(CDlgMainTitle::GetMgr()->GetSafeHwnd(), WM_UPDATE_VIEW_SYSLOG, (WPARAM)(&strLog), 0);
//		//	}
//		//}
//	}
//
//	return TRUE;


	SYSTEMTIME LogTime = pData->Time;
	CString	strPath = _T("");

	strPath.Format("%s\\MainLog\\%04d_%02d_%02d_%02d.log",
		GetExePath(),
		LogTime.wYear,
		LogTime.wMonth,
		LogTime.wDay,
		LogTime.wHour);

	bool bExistFile = false;
//#ifdef _UNICODE
//	bExistFile = (_waccess_s(strPath, 0) != -1);
//#else 
//	bExistFile = (_access_s(strPath, 0) != -1);
//#endif // _UNICODE

	bExistFile = file_exists(strPath);


//	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strPath) && GetLastError() == ERROR_FILE_NOT_FOUND)

	if (bExistFile == false)
	{
		if (m_stdFile.m_pStream)
		{
			m_stdFile.Close();
		}

		if (FALSE == m_stdFile.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
			return false;

		m_stdFile.WriteString(GetLoadData(pData));
		m_stdFile.Flush();
	}
	else
	{
		if (!m_stdFile.m_pStream)
		{
			if (FALSE == m_stdFile.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
				return false;
			
			m_stdFile.SeekToEnd();
		}

		m_stdFile.WriteString(GetLoadData(pData));
		m_stdFile.Flush();
	}

	// 스케줄 끝난거 체크 되었으면

	if (this->m_bLoop == false)
	{
		if (m_stdFile.m_pStream)
			m_stdFile.Close();
	}

	return true;
}

BOOL CMgrSystemLog::PushWriteLog(LOGDATA* pData)
{
	m_strData += GetLoadData(pData);

	return true;
}

BOOL CMgrSystemLog::WriteLog()
{
	if (m_strData.IsEmpty() == true)
		return false;

	SYSTEMTIME LogTime;
	GetLocalTime(&LogTime);

	CString	strPath = _T("");

	strPath.Format("%s\\MainLog\\%04d_%02d_%02d_%02d.log",
		GetExePath(),
		LogTime.wYear,
		LogTime.wMonth,
		LogTime.wDay,
		LogTime.wHour);

	/*CString strQCount = "";
	strQCount.Format("[Queue Count] - %d\n", GetQueueCount());
	m_strData += strQCount;*/
	//CFileFind kParser;

	//if (FALSE == kParser.FindFile(strPath))
	
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strPath) && GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		if (m_stdFile.m_pStream)
		{
			m_stdFile.Close();
		}

		if (FALSE == m_stdFile.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
			return false;

		m_stdFile.WriteString(m_strData);
		m_stdFile.Flush();
	}
	else
	{
		//kParser.Close();

		if (!m_stdFile.m_pStream)
		{
			if (FALSE == m_stdFile.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
				return false;
			m_stdFile.SeekToEnd();
		}

		m_stdFile.WriteString(m_strData);
		m_stdFile.Flush();
	}
	m_strData.Empty();
	// 스케줄 끝난거 체크 되었으면

	if (this->m_bLoop == false)
	{
		if (m_stdFile.m_pStream)
			m_stdFile.Close();
	}

	return true;
}


CString CMgrSystemLog::GetLoadData(LOGDATA* pData)
{
	SYSTEMTIME LogTime = pData->Time;

	CString strLog;
	strLog.Format("%04d-%02d-%02d %02d:%02d:%02d:%03ld : ",
		LogTime.wYear,
		LogTime.wMonth,
		LogTime.wDay,
		LogTime.wHour,
		LogTime.wMinute,
		LogTime.wSecond,
		LogTime.wMilliseconds);

	BOOL bWriteWindow = FALSE;

	switch (pData->Type)
	{
	case System:	strLog += _T("[SYSTEM    ] : "); bWriteWindow = TRUE; break;
	case Dbg:		strLog += _T("[DEBUG     ] : "); bWriteWindow = FALSE; break;
	case Operation: strLog += _T("[OPERATION ] : "); bWriteWindow = TRUE; break;
	case Err:		strLog += _T("[ERROR     ] : "); bWriteWindow = TRUE; break;
	}

	strLog += pData->strValue + "\n";

	return strLog;
}

//BOOL CMgrSystemLog::WriteLog(LOGDATA* pData)
//{
//	CFileFind kParser;
//
//	if (FALSE == kParser.FindFile(strFilePath))
//	{
//		if (m_file[nFileType].m_pStream)
//		{
//			m_file[nFileType].Close();
//		}
//
//		if (FALSE == m_file[nFileType].Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
//			return false;
//
//		CMgrLog* pMgr = CMgrLog::GetMgr();
//
//		if (pMgr)
//		{
//			m_file[nFileType].WriteString(pMgr->GetHeaderLogString() + CMgrDAQ::GetMgr()->GetDaQLogHeader() + "\n");
//			m_file[nFileType].WriteString(strCyclerChannelInfo);
//			m_file[nFileType].Flush();
//		}
//	}
//	else
//	{
//		kParser.Close();
//
//		if (!m_file[nFileType].m_pStream)
//			return false;
//
//		m_file[nFileType].SeekToEnd();
//		m_file[nFileType].WriteString(strCyclerChannelInfo);
//		m_file[nFileType].Flush();
//
//		if (eResultFileTypeStepTotal == nFileType)
//		{
//			if (m_file[nFileType].GetLength() >= 50 * 1024 * 1024)
//			{
//				m_nFileCount[nFileType] += 1;
//			}
//		}
//	}
//
//	// 스케줄 끝난거 체크 되었으면
//
//	if (this->m_bLoop == false)
//	{
//		if (m_file[nFileType].m_pStream)
//			m_file[nFileType].Close();
//
//		m_nFileCount[nFileType] = 0;
//	}
//
//	return true;
//}
/*
BOOL CMgrSystemLog::WriteLog(CString parmStrLog)
{
	FILE* fp = NULL;

	SYSTEMTIME LogTime;

	GetLocalTime(&LogTime);

	CString	strPath = _T("");

	strPath.Format("%s\\MainLog\\%04d_%02d_%02d_%02d.log",
		GetExePath(),
		LogTime.wYear,
		LogTime.wMonth,
		LogTime.wDay,
		LogTime.wHour);

	fopen_s(&fp, (LPSTR)(LPCSTR)strPath, "a+");

	if (fp != NULL)
	{
		CString strLog;
		strLog.Format("%04d-%02d-%02d %02d:%02d:%02d:%03ld : ",
			LogTime.wYear,
			LogTime.wMonth,
			LogTime.wDay,
			LogTime.wHour,
			LogTime.wMinute,
			LogTime.wSecond,
			LogTime.wMilliseconds);

		strLog += parmStrLog;

		fprintf(fp, "%s\r\n", (LPSTR)(LPCSTR)strLog);
		fflush(fp);
		fclose(fp);
	}

	return TRUE;
}
*/
LOGDATA* CMgrSystemLog::GetTxtQueue()
{
	m_cs.Lock();

	LOGDATA* pQueue = NULL;

	if (FALSE == m_Queue.IsEmpty())
		pQueue = m_Queue.RemoveHead();

	m_cs.Unlock();

	return pQueue;
}

void CMgrSystemLog::AddTxtQueue(LOGDATA* pQueue)
{
	m_cs.Lock();

	m_Queue.AddTail(pQueue);

	m_cs.Unlock();
}

UINT CMgrSystemLog::QueuingThread(LPVOID pParam)
{
	CMgrSystemLog* pMgr = (CMgrSystemLog*)pParam;

	if (NULL == pMgr)
	{
		return 0;
	}

	int i = 0;
	while (pMgr->m_bLoop)
	{
		for (i = 0; i < 10; i++)
		{
			LOGDATA* pLog = pMgr->GetTxtQueue();

			if (NULL != pLog)
			{
				pMgr->PushWriteLog(pLog);

				delete pLog;

			}
			else
			{
				break;
			}
		}
		pMgr->WriteLog();
		Sleep(1);
	}

	return 0;
}

/*
UINT CMgrSystemLog::QueuingThread(LPVOID pParam)
{
	CMgrSystemLog* pMgr = (CMgrSystemLog*)pParam;

	if (NULL == pMgr)
	{
		return 0;
	}

	LOGDATA logData;
	CString strData = "";
	while (pMgr->m_bLoop)
	{
		LOGDATA* pLog = pMgr->GetTxtQueue();

		

		if (NULL != pLog)
		{
			logData.strValue = pLog->strValue;
			logData.Time = pLog->Time;
			logData.Type = pLog->Type;

			DELETE_POINTER(pLog);

			//if (pMgr->m_Queue.GetCount() != 0 && pMgr->m_Queue.GetCount() % 10 == 0)
			{
				strData.Format("CMgrSystemLog::Queue Count - %d", pMgr->m_Queue.GetCount());
				logData.strValue = strData;
			}
				//logData.strValue = "CMgrSystemLog::Queue Count" + pMgr->m_Queue.GetCount();
				//	SYSLOG(Dbg, "CCommEthernet::GetQueue - %d", pMgr->m_Queue.GetCount());

			pMgr->WriteLog(&logData);

		}

		Sleep(1);
	}

	return 0;
}
*/