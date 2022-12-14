#include "stdafx.h"
#include "MgrLog.h"

#include "MgrChannel.h"
#include "MgrStatus.h"
#include "MgrConfig.h"
#include "MgrSerial.h"

#include "PnxUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMgrLog::CMgrLog()
{
	if (LoadHeaderLogString() == FALSE)
	{
		TRACE("[DEBUG] CHECK..CMgrLog::CMgrLog()");
	}
	
	m_pCurrentLogPath = new CString[CMgrChannel::GetMgr()->GetSize()];

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pSrc = listChannel.GetNext(Pos);

		if (NULL == pSrc)
			continue;

		CLogThread* pThread = new CLogThread;

		pThread->SetChannelNumber(pSrc->GetChannelNumber());

		m_listLogQueue.AddTail(pThread);

		pThread->StartThread();
	}
}


CMgrLog::~CMgrLog()
{
	m_listLogHeader.RemoveAll();

	delete[] m_pCurrentLogPath;

	auto Pos = m_listLogQueue.GetHeadPosition();

	while (Pos)
	{
		CLogThread* pThread = m_listLogQueue.GetNext(Pos);

		if (NULL == pThread)
			continue;

		pThread->StopThread();
	}

	while (!m_listLogQueue.IsEmpty())
		delete m_listLogQueue.RemoveHead();
}

void	CMgrLog::AddQueue(int nChannelNumber, CCommData* pQueue)
{
	auto Pos = m_listLogQueue.GetHeadPosition();

	while (Pos)
	{
		CLogThread* pThread = m_listLogQueue.GetNext(Pos);

		if (NULL == pThread)
			continue;

		if (pThread->GetChannelNumber() == nChannelNumber)
		{
			pThread->AddQueue(pQueue);

			break;
		}			
	}
}

void	CMgrLog::WriteLogRun(const int nChannelNumber, CCommData* pComm)
{
	CCommData* pDst = new CCommData;

	pDst->CopyData(pComm);
	pDst->SetLogType(LOG_TYPE_RUN);
	pDst->SetLogChannelNumber(nChannelNumber);
	
	AddQueue(nChannelNumber, pDst);	
}

void	CMgrLog::WriteLogEnd(const int nChannelNumber, CCommData* pComm)
{
	CCommData* pDst = new CCommData;

	pDst->CopyData(pComm);
	pDst->SetLogChannelNumber(nChannelNumber);
	pDst->SetLogType(LOG_TYPE_END);

	AddQueue(nChannelNumber, pDst);
}

void CMgrLog::WriteLogClaibrationVolt()
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (NULL == pChannel)
			continue;

		//?????? ????????
		if (pChannel->GetSelected() == true)
		{
			CCommData* pDst = new CCommData;
			pDst->SetChannelNumber(pChannel->GetChannelNumber());

			pDst->SetLogType(LOG_TYPE_CALIBRATION_VOLT);
			pDst->SetLogChannelNumber(pChannel->GetChannelNumber() );
			AddQueue(pChannel->GetChannelNumber(), pDst);

		}
	}


}

BOOL	CMgrLog::LoadHeaderLogString()
{
	m_listLogHeader.RemoveAll();

	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\MgrLogHeader.txt"), GetExePath());

	FILE *fp = NULL;

	fopen_s(&fp, strFilePath, "r");

	if (NULL == fp)
	{
		return FALSE;
	}

	char szContent[1024] = { 0, };

	memset(szContent, NULL, 1024);

	while (fgets(szContent, 1024, fp))
	{
		CString strContent;

		strContent.Format(_T("%s"), szContent);

		strContent.Remove('\r');
		strContent.Remove('\n');

		if (FALSE == strContent.IsEmpty())
			m_listLogHeader.AddTail(strContent);
	}

	fclose(fp);

	return TRUE;
}

CString	CMgrLog::GetHeaderLogString()
{
	CString strProcHeader;

	auto Pos = m_listLogHeader.GetHeadPosition();

	if (Pos == NULL)
	{
		strProcHeader.Format(_T("NULL,"));
		return strProcHeader.GetString();
	}

	while (Pos)
	{
		strProcHeader += m_listLogHeader.GetNext(Pos);
		strProcHeader += _T(",");
	}

	return strProcHeader.GetString();
}

/**
 * .
 * 
 * METHOD_NAME : GetHeaderLogStringForAutoCalibration
 * METHOD_TYPE : -
 * DESCRIPTION : AUTO_CALIBRATION
 * \return 
 */
CString CMgrLog::GetHeaderLogStringForAutoCalibration()
{
	CString strProcHeader;
	strProcHeader = _T("Cycle,Step,Mode,");

	if (CMgrConfig::GetMgr()->GetMeasureModeNo() == elFBoradStepModeMasterAutocalCurrent)		//?????????????? ???????? 
	{
		strProcHeader += _T("Target_Value(A),");
		strProcHeader += _T("AutoCal_Average_Value(A)");
	}
	else
	{
		strProcHeader += _T("Target_Value(V),");
		strProcHeader += _T("AutoCal_Average_Value(V)");
	}


	return strProcHeader.GetString();
}

bool CMgrLog::CreateLogDirectory(
	int nChannelNumber, 
	CString strChannelBarcode,
	CString strSrcFilePath, 
	SYSTEMTIME& tmCur, 
	CString strUserName, 
	CString strScheduleName, 
	CString& strLogFilePath, 
	CString strUserLogFilePath, 
	CString strWorkName,
	BOOL bScheduleName,
	BOOL bTime)
{
	if (strUserLogFilePath.IsEmpty())
		return false;

	strUserLogFilePath.TrimRight("\\");

	// By JSK - 2020.07.02 ???? ???? ???? ?????? ???????? ?????
	//MakeSureDirectoryPathExists();

	CString strResuleFilePath;
	strResuleFilePath.Format("%s\\", (LPCSTR)strUserLogFilePath);

	CreateDirectory(strResuleFilePath, NULL);
	
//#pragma region //???? - ???? / ????Name + ?????????? + ????????(hhmmss)
//Formation date / tray barcode / schedule / Channel
	CString strFolderName;
	strFolderName.Format(_T("%02d_%02d_%02d\\"), tmCur.wYear, tmCur.wMonth, tmCur.wDay);

	strResuleFilePath += strFolderName;
	CreateDirectory(strResuleFilePath, NULL);
	CMgrSerial* pMgrSerial = CMgrSerial::GetMgr();

	if (pMgrSerial != NULL)
	{
		CString strTrayBarcode;
		strTrayBarcode = pMgrSerial->GetAtBarcode(nChannelNumber / 24)->GetTrayBarcode();
		strTrayBarcode.Replace(_T("/"), _T(""));
		strFolderName.Format(_T("Tray Barcode(%s)\\"), strTrayBarcode);

		strResuleFilePath += strFolderName;
		CreateDirectory(strResuleFilePath, NULL);
	}	

	strFolderName.Format(_T("%s_%s_%02d%02d%02d\\"), strUserName, strScheduleName, tmCur.wHour, tmCur.wMinute, tmCur.wSecond);
	strResuleFilePath += strFolderName;
	CreateDirectory(strResuleFilePath, NULL);
//#pragma endregion

//#pragma region //???? - ?????? ???? / ??????_??????????_YYMMDDhhmm
	//CString strFolderName;
	//strFolderName.Format(_T("%s\\"), (LPCSTR)strWorkName);

	//strResuleFilePath += strFolderName;
	//CreateDirectory(strResuleFilePath, NULL);

	//strFolderName.Format("%s", (LPCSTR)strWorkName);
	//
	//if (bScheduleName) {
	//	strFolderName.AppendFormat("_%s", (LPCSTR)strScheduleName);
	//}

	//if (bTime) {
	//	strFolderName.AppendFormat("_%02d%02d%02d%02d%02d%02d", tmCur.wYear, tmCur.wMonth, tmCur.wDay, tmCur.wHour, tmCur.wMinute, tmCur.wSecond);
	//}

	//strFolderName.Append("\\");
	//	
	//strResuleFilePath += strFolderName;
	//CreateDirectory(strResuleFilePath, NULL);
//#pragma endregion

	strLogFilePath.Format("%s", (LPCSTR)strResuleFilePath);

	CMgrStatus* pMgrStatus = CMgrStatus::GetMgr();

	if (pMgrStatus)
	{
		if (pMgrStatus)
		{
			SCylerChannelDump* lpCyclerDump = pMgrStatus->GetCyclerChannelDump(nChannelNumber);

			if (lpCyclerDump)
				lpCyclerDump->strScheduleName = strScheduleName;
		}
	}

	if (FALSE == CheckFolderName(strUserName))
		return false;

	CString strDstFilePath;
	strDstFilePath.Format("%s%s.xml", (LPCSTR)strResuleFilePath.GetString(), (LPCSTR)strScheduleName);

	CopyFile(strSrcFilePath, strDstFilePath, FALSE);

	if (pMgrStatus)
	{
		if (pMgrStatus)
		{
			SCylerChannelDump* lpCyclerDump = pMgrStatus->GetCyclerChannelDump(nChannelNumber);

			if (lpCyclerDump)
				lpCyclerDump->strScheduleFilePath = strDstFilePath;
		}
	}
	CString strLogPath = strResuleFilePath;
	strFolderName.Format(_T("%dCH\\"), nChannelNumber + 1);
	strResuleFilePath += strFolderName;

	if (CreateDirectory(strResuleFilePath, NULL) == FALSE)
	{
		SetCurrentLogPath(nChannelNumber, strResuleFilePath);
		return FALSE;
	}

	SetCurrentLogPath(nChannelNumber, strResuleFilePath);

//#pragma region scheduledesc.txt ???? 2021-02-02 CNZ
	///////////////////////////////////////////////////////////////////////////
	CStdioFile logfile;
	CString path;
	char* pDesc_UTF8 = NULL;

	path.Empty();
	path.Format("%sscheduledesc.txt", strLogPath.GetString()); //data????

	//??????,20191126:11:50:00:000,??????,??????????,????????????,????5A
	if (logfile.Open(path, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText) == TRUE)
	{
		CString FileFormat;

		FileFormat.Format(_T("Test Date,%04d-%02d-%02d %02d:%02d:%02d,Test Name,%s,Schedule File Name,%s"),
			tmCur.wYear, tmCur.wMonth, tmCur.wDay,
			tmCur.wHour, tmCur.wMinute, tmCur.wSecond,
			(LPCSTR)strUserName,//pDlg->GetUserNumber(),
			(LPCSTR)strScheduleName);//CDlgViewScheduleInfo::GetMgr()->GetScheduleName());
		pDesc_UTF8 = CPnxUtil::GetMgr()->ANSItoUTF8(FileFormat.GetBuffer());
		logfile.Write(pDesc_UTF8, ::strlen(pDesc_UTF8));
	}
	logfile.Close();

	if (pDesc_UTF8 != NULL)
		delete pDesc_UTF8;

	CString strFilePath;
	strFilePath.Format("%sChannelBarcode.ini", strLogPath.GetString());

	CString strAppName;
	strAppName.Format(_T("Channel Barcode_%s"), strChannelBarcode);
	INIWriteInt(strAppName, _T("Ch"), nChannelNumber+1, strFilePath);

	///////////////////////////////////////////////////////////////////////////

//#pragma endregion

	return true;
}

bool CMgrLog::CreateVoltCalibrationDir(int nChannelNumber, SYSTEMTIME & tmCur, float fTargeVolt, float fVoltData)
{
	CString strResuleFilePath;
	strResuleFilePath.Format("%s\\", (LPCSTR)CMgrConfig::GetMgr()->GetLogFilePath());

	CreateDirectory(strResuleFilePath, NULL);

	//#pragma region //???? - ???? / ????Name + ?????????? + ????????(hhmmss)
	//Formation date / tray barcode / schedule / Channel
	CString strFolderName;
	strFolderName.Format(_T("%02d_%02d_%02d\\"), tmCur.wYear, tmCur.wMonth, tmCur.wDay);

	strResuleFilePath += strFolderName;
	CreateDirectory(strResuleFilePath, NULL);


	CString strLogPath = strResuleFilePath;
	strFolderName.Format(_T("%dCH\\"), nChannelNumber + 1);
	strResuleFilePath += strFolderName;

	CreateDirectory(strResuleFilePath, NULL);


	SetCurrentLogPath(nChannelNumber, strResuleFilePath);


	CString strFilePath;
	strFilePath.Format("%sVoltCalibration.csv", strResuleFilePath.GetString());

	CString strAppName;
	CString strValue;
	strAppName.Format(_T("Target %.6f"), fTargeVolt);
	strValue.Format(_T("%.6f"), fVoltData);
	INIWriteStr(strAppName, _T("Sensing "), strValue, strFilePath);

	///////////////////////////////////////////////////////////////////////////

//#pragma endregion

	return true;
}

CString	CMgrLog::GetCurrentLogPath(int nChannelNumber)
{
	return m_pCurrentLogPath[nChannelNumber];
}

void CMgrLog::SetCurrentLogPath(int nChannelNumber, CString strPath)
{
	m_pCurrentLogPath[nChannelNumber] = strPath;
}

BOOL CMgrLog::CheckFolderName(CString strName)
{
	if (strName.Find('\\') != -1 ||
		strName.Find('/') != -1 ||
		strName.Find('*') != -1 ||
		strName.Find('?') != -1 ||
		strName.Find('\"') != -1 ||
		strName.Find('<') != -1 ||
		strName.Find('>') != -1 ||
		strName.Find('|') != -1)
	{
		return FALSE;
	}

	return TRUE;
}