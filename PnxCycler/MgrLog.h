#pragma once

#include "Singleton.h"
#include "CommData.h"
#include "LogThread.h"

class CMgrLog : public CSingleton< CMgrLog >
{
public:
	CMgrLog();
	virtual ~CMgrLog();

	void	WriteLogRun(const int nChannelNumber, CCommData* pComm);
	void	WriteLogEnd(const int nChannelNumber, CCommData* pComm);
	void	WriteLogClaibrationVolt();

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
		BOOL bTime);
	bool CMgrLog::CreateVoltCalibrationDir(
		int nChannelNumber,
		SYSTEMTIME& tmCur,
		float fTargeVolt,
		float fVoltData
	);
	
	CString	GetCurrentLogPath(int nChannelNumber);
	CString	GetHeaderLogString();

	CString GetHeaderLogStringForAutoCalibration();
	
	void	SetCurrentLogPath(int nChannelNumber, CString strPath);

private:
	CList< CString, CString >			m_listLogHeader;
	CList< CLogThread*, CLogThread* >	m_listLogQueue;

	CString*	m_pCurrentLogPath;	//현재 스케줄 진행중인 LOG PATH를 가진다
	CCriticalSection	m_csLock;

	CString	m_strLogTime;
	
private:
	BOOL	LoadHeaderLogString();
		
	void	AddQueue(int nChannelNumber, CCommData* pQueue);

	BOOL	CheckFolderName(CString strName);
};

