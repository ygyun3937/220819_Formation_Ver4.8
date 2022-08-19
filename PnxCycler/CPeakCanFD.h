#pragma once

class CPeakCanFD : public CPeakNet
{
	enum
	{
		eByteMul = 1000,
	};


public:
	CPeakCanFD();
	virtual ~CPeakCanFD();

public:
	virtual bool InitPeakCan(void* pSetting);
	virtual void Release();
	virtual void Recv(int nIdx = 0);
	virtual void Send(void* p);
	virtual void GetCanData(SBMSPacket& data);
	virtual int GetPeakCanPortCount() { return ePeakCH_MAX; }
private:
	SBMSLOAD_PEAKCANFD	m_sSetting;
	SBMSPacket			m_sBMSPacket;			// CanFD, Can Message 받은 후에 저장한다.
	DWORD				m_dwTIMEOUT;				// Tick, 0보다 커야 동작
	bool					m_bExtendType;			// CAN_TYPE_STANDARD or PCAN_MESSAGE_EXTENDED
	CString				m_strCanFDSetting;
private:
// 	XLhandle         m_hMsgEvent;			// 이벤트 변수
// 	bool				m_bInitDone;			// 세팅 완료
// 	bool				m_bActivateThread;	// 스레드 활성화만 관여
// 	bool				m_bActivateChannel;	// 채널 활성화만 관여
	DWORD			m_dwCheckTimeOut;		// 내부 변수
};

