#pragma once

#include "Singleton.h"

enum LogType
{
	System = 0,
	Dbg,
	Operation,
	Err,
};

struct LOGDATA
{
	LogType Type;
	SYSTEMTIME Time;
	CString strValue;
};

class CMgrSystemLog : public CSingleton <CMgrSystemLog>
{
public:
	CMgrSystemLog();
	~CMgrSystemLog();

	void LOG(LogType nType, const char* fmt, ...);

private:
	CCriticalSection m_cs;
	CWinThread*	m_pThread;
	BOOL m_bLoop;
	CStdioFile  m_stdFile;
	CList< LOGDATA*, LOGDATA* > m_Queue;
	CString m_strData;
private:
	BOOL WriteLog(LOGDATA* pData);
	BOOL WriteLog(CString parmStrLog);
	CString GetLoadData(LOGDATA* pData);
	LOGDATA* GetTxtQueue();
	int GetQueueCount() { return m_Queue.GetCount(); };
	void AddTxtQueue(LOGDATA* pQueue);
	static UINT QueuingThread(LPVOID pParam);

	BOOL WriteLog();
	BOOL PushWriteLog(LOGDATA* pData);



};

