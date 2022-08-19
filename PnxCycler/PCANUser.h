#pragma once
#include "PCANBasicClass.h"
#include <Math.h>
using namespace std;

#define MSG_TYPE		0
#define MSG_ID			1
#define MSG_LENGTH		2
#define MSG_DATA		3
#define MSG_COUNT		4
#define MSG_TIME		5

// #define TAB_X 10	//Width
// #define TAB_Y 50	//Height

#define CAN_TYPE_MASTER 0
#define CAN_TYPE_SLAVE	1
#define LIN_TYPE_MASTER 0
#define LIN_TYPE_SLAVE	1

#define CAN_TYPE_STANDARD 0
#define CAN_TYPE_EXTENDED 1

#define REV_DATA_VOLT 0
#define REV_DATA_CURR 1

#define REV_DATA_MOTOROLA	  0
#define REV_DATA_INTEL		  1

#define REV_DATA_UNSIGNED	  0
#define REV_DATA_SIGNED		  1
//////////////////////////////////////////////////////////////////////////2015.04.09 bit �ɰ��� start
struct PACK_DATA_BIT            // Pack Voltage/Current Frame for Command/Report Frame
{
	unsigned char bit8 :1;
	unsigned char bit7 :1;
	unsigned char bit6 :1;
	unsigned char bit5 :1;
	unsigned char bit4 :1;
	unsigned char bit3 :1;
	unsigned char bit2 :1;
	unsigned char bit1 :1;

	unsigned char bit16:1;
	unsigned char bit15 :1;
	unsigned char bit14 :1;
	unsigned char bit13 :1;
	unsigned char bit12 :1;
	unsigned char bit11 :1;
	unsigned char bit10 :1;
	unsigned char bit9 :1;

	unsigned char bit24 :1;
	unsigned char bit23 :1;
	unsigned char bit22 :1;
	unsigned char bit21 :1;
	unsigned char bit20 :1;
	unsigned char bit19 :1;
	unsigned char bit18 :1;
	unsigned char bit17 :1;

	unsigned char bit32 :1;
	unsigned char bit31 :1;
	unsigned char bit30 :1;
	unsigned char bit29 :1;
	unsigned char bit28 :1;
	unsigned char bit27 :1;
	unsigned char bit26 :1;
	unsigned char bit25 :1;

	unsigned char bit40 :1;
	unsigned char bit39 :1;
	unsigned char bit38 :1;
	unsigned char bit37 :1;
	unsigned char bit36 :1;
	unsigned char bit35 :1;
	unsigned char bit34 :1;
	unsigned char bit33 :1;

	unsigned char bit48 :1;
	unsigned char bit47 :1;
	unsigned char bit46 :1;
	unsigned char bit45 :1;
	unsigned char bit44 :1;
	unsigned char bit43 :1;
	unsigned char bit42 :1;
	unsigned char bit41 :1;

	unsigned char bit56 :1;
	unsigned char bit55 :1;
	unsigned char bit54 :1;
	unsigned char bit53 :1;
	unsigned char bit52 :1;
	unsigned char bit51 :1;
	unsigned char bit50 :1;
	unsigned char bit49 :1;

	unsigned char bit64 :1;
	unsigned char bit63 :1;
	unsigned char bit62 :1;
	unsigned char bit61 :1;
	unsigned char bit60 :1;
	unsigned char bit59 :1;
	unsigned char bit58 :1;
	unsigned char bit57 :1;
};
struct DSP_DATA_BIT            // Pack Voltage/Current Frame for Command/Report Frame
{
	unsigned char CMD;
	unsigned char OPCODE_PARACODE;	//20150427 Parallel add
	unsigned char pack_vol_H;
	unsigned char pack_vol_M;
	unsigned char pack_vol_L;
	unsigned char pack_cur_H;
	unsigned char pack_cur_M;
	unsigned char pack_cur_L;
};
union pack_data_
{
	unsigned char frame[8] ;
	struct PACK_DATA_BIT bit;
	struct DSP_DATA_BIT bitDsp;
};


struct stErrCode	//LSB���� ���� ������ �����ش�.
{
	unsigned char szCmd:8;
	unsigned char szOp:8;

	unsigned char szErr_W_1:4;	//���͸�#1 �÷����
	unsigned char szErr_W_2:4;

	unsigned char szErr_G_2:4;
	unsigned char szErr_W_0:4;

	unsigned char szErr_G_0:4;
	unsigned char szErr_G_1:4;

	unsigned char szErr_D_1:4;
	unsigned char szErr_D_2:4;

	unsigned char szErr_F_2:4;
	unsigned char szErr_D_0:4;	

	unsigned char szErr_F_0:4;
	unsigned char szErr_F_1:4;
};




//////////////////////////////////////////////////////////////////////////bit �ɰ��� end
#pragma region Critical Section Class
class clsCritical	//CH1
{
private:
	CRITICAL_SECTION *m_objpCS_CH1;
	LONG volatile m_dwOwnerThread;
	LONG volatile m_dwLocked;
	bool volatile m_bDoRecursive;

public:
	explicit clsCritical(CRITICAL_SECTION *cs, bool createUnlocked = false, bool lockRecursively = false);
	~clsCritical();
	int GetRecursionCount();
	bool IsLocked();
	int Enter();
	int Leave();
};

class clsCritical_CH2	//CH2
{
private:
	CRITICAL_SECTION *m_objpCS_CH2;
	LONG volatile m_dwOwnerThread_CH2;
	LONG volatile m_dwLocked_CH2;
	bool volatile m_bDoRecursive_CH2;

public:
	explicit clsCritical_CH2(CRITICAL_SECTION *cs_CH2, bool createUnlocked_CH2 = false, bool lockRecursively_CH2 = false);
	~clsCritical_CH2();
	int GetRecursionCount_CH2();
	bool IsLocked_CH2();
	int Enter_CH2();
	int Leave_CH2();
};
class clsCritical_CH3	//CH2
{
private:
	CRITICAL_SECTION *m_objpCS_CH3;
	LONG volatile m_dwOwnerThread_CH3;
	LONG volatile m_dwLocked_CH3;
	bool volatile m_bDoRecursive_CH3;

public:
	explicit clsCritical_CH3(CRITICAL_SECTION *cs_CH3, bool createUnlocked_CH3 = false, bool lockRecursively_CH3 = false);
	~clsCritical_CH3();
	int GetRecursionCount_CH3();
	bool IsLocked_CH3();
	int Enter_CH3();
	int Leave_CH3();
};
//////////////////////////////////////////////////////////////////////////SEND CH1
class clsCritical_Send_CH1	//SEND CH1 
{
private:
	CRITICAL_SECTION *m_objpCS_SEND_CH1;
	LONG volatile m_dwOwnerThread_SEND_CH1;
	LONG volatile m_dwLocked_SEND_CH1;
	bool volatile m_bDoRecursive_SEND_CH1;

public:
	explicit clsCritical_Send_CH1(CRITICAL_SECTION *cs_SEND_CH1, bool createUnlocked_SEND_CH1 = false, bool lockRecursively_SEND_CH1 = false);
	~clsCritical_Send_CH1();
	int GetRecursionCount_SEND_CH1();
	bool IsLocked_SEND_CH1();
	int Enter_SEND_CH1();
	int Leave_SEND_CH1();
};
//////////////////////////////////////////////////////////////////////////SEND CH2
class clsCritical_Send_CH2	//SEND CH2 
{
private:
	CRITICAL_SECTION *m_objpCS_SEND_CH2;
	LONG volatile m_dwOwnerThread_SEND_CH2;
	LONG volatile m_dwLocked_SEND_CH2;
	bool volatile m_bDoRecursive_SEND_CH2;

public:
	explicit clsCritical_Send_CH2(CRITICAL_SECTION *cs_SEND_CH2, bool createUnlocked_SEND_CH2 = false, bool lockRecursively_SEND_CH2 = false);
	~clsCritical_Send_CH2();
	int GetRecursionCount_SEND_CH2();
	bool IsLocked_SEND_CH2();
	int Enter_SEND_CH2();
	int Leave_SEND_CH2();
};
/////////////3
class clsCritical_Send_CH3	//SEND CH2 
{
private:
	CRITICAL_SECTION *m_objpCS_SEND_CH3;
	LONG volatile m_dwOwnerThread_SEND_CH3;
	LONG volatile m_dwLocked_SEND_CH3;
	bool volatile m_bDoRecursive_SEND_CH3;

public:
	explicit clsCritical_Send_CH3(CRITICAL_SECTION *cs_SEND_CH3, bool createUnlocked_SEND_CH3 = false, bool lockRecursively_SEND_CH3 = false);
	~clsCritical_Send_CH3();
	int GetRecursionCount_SEND_CH3();
	bool IsLocked_SEND_CH3();
	int Enter_SEND_CH3();
	int Leave_SEND_CH3();
};

//DSP SEND REV CH1/CH2
class clsCritical_DSP1	//CH1
{
private:
	CRITICAL_SECTION *m_objpCS_DSP1;
	LONG volatile m_dwOwnerThread_DSP1;
	LONG volatile m_dwLocked_DSP1;
	bool volatile m_bDoRecursive_DSP1;

public:
	explicit clsCritical_DSP1(CRITICAL_SECTION *cs_DSP1, bool createUnlocked_DSP1 = false, bool lockRecursively_DSP1 = false);
	~clsCritical_DSP1();
	int GetRecursionCount_DSP1();
	bool IsLocked_DSP1();
	int Enter_DSP1();
	int Leave_DSP1();
};

class clsCritical_DSP2	//CH2
{
private:
	CRITICAL_SECTION *m_objpCS_DSP2;
	LONG volatile m_dwOwnerThread_DSP2;
	LONG volatile m_dwLocked_DSP2;
	bool volatile m_bDoRecursive_DSP2;

public:
	explicit clsCritical_DSP2(CRITICAL_SECTION *cs_DSP2, bool createUnlocked_DSP2 = false, bool lockRecursively_DSP2 = false);
	~clsCritical_DSP2();
	int GetRecursionCount_DSP2();
	bool IsLocked_DSP2();
	int Enter_DSP2();
	int Leave_DSP2();
};
//////////////////////////////////////////////////////////////////////////SEND DSP1
class clsCritical_Send_DSP1	//SEND DSP1 
{
private:
	CRITICAL_SECTION *m_objpCS_SEND_DSP1;
	LONG volatile m_dwOwnerThread_SEND_DSP1;
	LONG volatile m_dwLocked_SEND_DSP1;
	bool volatile m_bDoRecursive_SEND_DSP1;

public:
	explicit clsCritical_Send_DSP1(CRITICAL_SECTION *cs_SEND_DSP1, bool createUnlocked_SEND_DSP1 = false, bool lockRecursively_SEND_DSP1 = false);
	~clsCritical_Send_DSP1();
	int GetRecursionCount_SEND_DSP1();
	bool IsLocked_SEND_DSP1();
	int Enter_SEND_DSP1();
	int Leave_SEND_DSP1();
};
//////////////////////////////////////////////////////////////////////////SEND DSP2
class clsCritical_Send_DSP2	//SEND DSP2 
{
private:
	CRITICAL_SECTION *m_objpCS_SEND_DSP2;
	LONG volatile m_dwOwnerThread_SEND_DSP2;
	LONG volatile m_dwLocked_SEND_DSP2;
	bool volatile m_bDoRecursive_SEND_DSP2;

public:
	explicit clsCritical_Send_DSP2(CRITICAL_SECTION *cs_SEND_DSP2, bool createUnlocked_SEND_DSP2 = false, bool lockRecursively_SEND_DSP2 = false);
	~clsCritical_Send_DSP2();
	int GetRecursionCount_SEND_DSP2();
	bool IsLocked_SEND_DSP2();
	int Enter_SEND_DSP2();
	int Leave_SEND_DSP2();
};

#pragma endregion

#pragma region Message Status class
class MessageStatus
{
private:
	TPCANMsg m_Msg;
	TPCANTimestamp m_TimeStamp;		
	TPCANTimestamp m_oldTimeStamp;		
	int m_iIndex;
	int m_Count;
	bool m_bShowPeriod;
	bool m_bWasChanged;

public:
	MessageStatus(TPCANMsg canMsg, TPCANTimestamp canTimestamp, int listIndex);
	void Update(TPCANMsg canMsg, TPCANTimestamp canTimestamp);

	TPCANMsg GetCANMsg();
	TPCANTimestamp GetTimestamp();
	int GetPosition();
	CString GetTypeString();
	CString GetIdString();
	CString GetDataString();
	CString GetTimeString();
	int GetCount();
	bool GetShowingPeriod();
	bool GetMarkedAsUpdated();

	void SetShowingPeriod(bool value);
	void SetMarkedAsUpdated(bool value);

	__declspec(property (get = GetCANMsg)) TPCANMsg CANMsg;
	__declspec(property (get = GetTimestamp)) TPCANTimestamp Timestamp;
	__declspec(property (get = GetPosition)) int Position;
	__declspec(property (get = GetTypeString)) CString TypeString;
	__declspec(property (get = GetIdString)) CString IdString;
	__declspec(property (get = GetDataString)) CString DataString;
	__declspec(property (get = GetTimeString)) CString TimeString;
	__declspec(property (get = GetCount)) int Count;
	__declspec(property (get = GetShowingPeriod, put = SetShowingPeriod)) bool ShowingPeriod;
	__declspec(property (get = GetMarkedAsUpdated, put = SetMarkedAsUpdated)) bool MarkedAsUpdated;		
};
#pragma endregion
class PCANUser
{
public:
	PCANUser(void);
	~PCANUser(void);
	bool m_Terminated;
	PCANBasicClass *m_objPCANBasic;
	// Saves the handle of a PCAN hardware
	//
	TPCANHandle m_PcanHandle;

	// Saves the baudrate register for a conenction
	//
	TPCANBaudrate m_Baudrate;

	// Saves the type of a non-plug-and-play hardware
	//
	TPCANType m_HwType;

	// Variables to store the current reading mode
	// 0 : Timer Mode
	// 1 : Event Mode
	// 2 : Manual Mode
	//
	int m_ActiveReadingMode;

	// Read Timer identifier
	//
	UINT_PTR m_tmrRead;

	//Display Timer identifier
	//
	UINT_PTR m_tmrDisplay;

	// CAN messages array. Store the message status for its display
	//
	CPtrList *m_LastMsgsList;

	// Handle to set Received-Event
	//
	HANDLE m_hEvent;

	// Handle to the thread to read using Received-Event method
	//
	HANDLE m_hThread;

	// Handles of the current available PCAN-Hardware
	//
	TPCANHandle m_HandlesArray[27];
	// Handle for a Critical Section 
	//
	CRITICAL_SECTION *m_objpCS;

	// Convert a int value to a CString
	//
	CString IntToStr(int iValue);
	// Convert a int value to a CString formated in Hexadecimal
	//
	CString IntToHex(int iValue, short iDigits);
	// Convert hexadecimal Cstring into int value (Zero if error)
	//
	DWORD HexTextToInt(CString ToConvert);
	// Check txtData in an hexadecimal value

	unsigned int m_nCanBit[64];

	union pack_data_ UnPack_dATA;

	void RevCanBitSet(int nType = 0);					//Can ���ŵ����͸� 1BIT ������ �ɰ��� ������ ����(nTyte = 0 ����ζ�, = 1 ����
	long BitDataVal_Motorola(int nStart,int nEnd,int nSignedType);	//StartBit , Countbit ,nMode long�� ���� ������ ����
	long BitDataVal_Intel(int nStart,int nEnd,int nSignedType);	//StartBit , Countbit ,nMode long�� ���� ������ ����

	

	//////////////////////////////////////////////////////////////////////////

	struct Can_Send_Data	//CAN SEND DATA
	{
		DWORD dWID;			//ID 
		int nType;			//�Ϲ�/Ȯ�� Ÿ�� ����
		int nDLC;			//������ ����
		BYTE DATA[8];		// Data of the message (DATA[0]..DATA[7])
		int nPeriod;		//�۽� �ֱ�
		int nClsFlag;		// 0 -> �۽��ֱ⸶�� ������ ���� ��Ų��. //������ step �з����� ���� ��� �۽�, 0 �� ���� ������ �۽�, �׿� ���� ��� �ش� ���ܿ����� �۽� �� ���� 
		int nDataBufNum;	// Can ID�� �ߺ��� ��� ������ ���� 
		BYTE byDataBuf[20][8];//2015.08.13 kys  Can ID�� �ߺ��� ��� ���� ��ŭ �����͸� ����
		int nMsterSlave;	//������ : 6, //�����̺� 7
		int	nMode;			//period ==0 event grb
		char	m_strName[125];
	};
	Can_Send_Data stCanSendData[20];	//�ִ� 20������ ����

	Can_Send_Data stCanSendBuf[20];		//�ִ� 20������ ����

	void ReadMessage();
	void DataProcess(TPCANMsg theMsg, TPCANTimestamp itsTimeStamp);	

	void Data_Err_Set(BYTE szdata[12]);
	void Data_Err_Init();
	unsigned char m_szErr_Warn2;
	unsigned char m_szErr_Warn1;
	unsigned char m_szErr_Warn0;
	unsigned char m_szErr_Grid2;
	unsigned char m_szErr_Grid1;
	unsigned char m_szErr_Grid0;
	unsigned char m_szErr_Dc2;
	unsigned char m_szErr_Dc1;
	unsigned char m_szErr_Dc0;
	unsigned char m_szErr_Fault2;
	unsigned char m_szErr_Fault1;
	unsigned char m_szErr_Fault0;

	struct stErrCode stErr;

	TPCANMsg m_RevTheMsg;

	BYTE *GetByteToStr(int nNum,CString strData);	//HEX�� CSTRING TYPE -> INT�� BYTE[8] TYPE ��ȯ //nNum = �Է� ����, cstring can data
	DWORD GetDwordToStr(CString strID);		//HEX�� CSTRING TYPE -> 10���� DWORD TYPE ��ȯ

	__int64 m_n64Bit;

	long m_TestStartDate;			//��������Ͻ�
	long m_TestTotalTime;			//���� �ѽð� 1ms �� 1����
	long m_CycleTotalTime;			//Cycle �� ����ð� 1ms�� 1�� ��
	long m_CycleStartDate;			//Cycle �����Ͻ�
	long m_CycleTime;				//Cycle ����ð� 1ms�� 1�� ��
	long m_StepTime;					//Step ����ð� 1ms�� 1�� ��

	double m_dTestVersion;				//�������
	double m_dVoltage;					//����
	double m_dCurrent;					//����
	double m_dCap;						//�뷮					//���� (Ah) : ���� / 3600 + �������� / 3600
	double m_dTotalCap;					//�����뷮				//���� (Ah) : (�����)���� / 3600 + (�����)�������� / 3600
	double m_dChargeCap;					//�����뷮				//���� (Ah) : �������� / 3600 + ������������ / 3600
	double m_dDischargeCap;				//�����뷮				//���� (Ah) : �������� / 3600 + ������������ / 3600
	double m_dWatt;						//����					//���� (W) :  ���� x ����
	double m_dWattHour;					//���·�				//���� (Wh) : ���� x ���� / 3600 + �������� x �������� / 3600
	double m_dTotalWattHour;				//�������·�			//���� (Wh) : ���� x ���� + �������� x ��������
	double m_dChargeWattHour;				//��������·�			//���� (Wh) : �������� / 3600 + ������������ / 3600
	double m_dCycleCap;					//����Ŭ�����뷮		//���� (Ah) : ����Ŭ(�����)���� / 3600 + ����Ŭ(�����)�������� / 3600

	double m_dCap_Ah_Base;				//��Ī�뷮			//����(Ah)

	double m_dDcir;						//DCIR �߰�
	CString m_strType;					//CC/CV���
	CString m_strEnd;					//�������

	void ReqData(double fltVolt,double fltCurr, long lTimeMs);

	void DataInit();
};

