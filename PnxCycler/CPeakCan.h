#pragma once

class CPeakCan : public CPeakNet
{
	enum
	{
		eByteMul = 1000,
	};


public:
	CPeakCan();
	virtual ~CPeakCan();

public:
	virtual bool InitPeakCan(void* pSetting);
	virtual void Release();
	virtual void Recv(int nIdx = 0);
	virtual void Send(void* p);
	virtual void GetCanData(SBMSPacket& data);
	virtual int GetPeakCanPortCount() { return ePeakCH_MAX; }
private:
	SBMSLOAD_PEAKCAN	m_sSetting;
	SBMSPacket		m_sBMSPacket;			// CanFD, Can Message ���� �Ŀ� �����Ѵ�.
	DWORD			m_dwTIMEOUT;				// Tick, 0���� Ŀ�� ����
	bool				m_bExtendType;			// CAN_TYPE_STANDARD or PCAN_MESSAGE_EXTENDED

private:
	TPCANBaudrate		m_PCanBaudrate;
// 	XLhandle         m_hMsgEvent;			// �̺�Ʈ ����
// 	bool				m_bInitDone;			// ���� �Ϸ�
// 	bool				m_bActivateThread;	// ������ Ȱ��ȭ�� ����
// 	bool				m_bActivateChannel;	// ä�� Ȱ��ȭ�� ����
	DWORD			m_dwCheckTimeOut;		// ���� ����
};
