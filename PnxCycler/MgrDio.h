
#include "Singleton.h"
#include "Dask.h"
#include "usbdask.h"
#include "bdaqctrl.h"
#include "TcpManager.h"

#include "CAjinIoControl.h"			// AJIN
#include "Alarm.h"
#include "AlarmMap.h"


// FORMATION
#define		IO_SLOT_MAX		3*10		// (INPUT 2 + OUTPUT 1) * 10
#define		IO_PARA_COUNT	2
#define		IO_VALUE_COUNT	32
#define		IO_ADDRESS		0
#define		IO_NAME			1

#define EQP_SUBEQP_MAX_COUNT	10
#define EQP_STAGE_DIO_MAXCOUNT	10

//Moxa 220206
#define		IO_SLOT_COUNT 8
#define		IO_SLOT_INPUT1 0
#define		IO_SLOT_INPUT2 1
#define		IO_SLOT_INOUT1 2
#define		IO_SLOT_INOUT2 3
#define		IO_SLOT_INOUT3 4

#define		IO_CHANNLE_COUNT 16



#pragma once

using namespace Automation::BDaq;

enum ChamberNum
{
	Chamber_1 = 0,
	Chamber_2,
	Chamber_3,
	Chamber_4,
};

enum StatusLightDioNum
{
	Red = 16,
	Orange,
	Green,
	Blue,
};

enum enumStepMeasurementRun
{
	//PLC 상태 확인
	PLC_READY_CHECK,

	//BCR CHECK
	PC_BCR_CHECK,

	//스케줄 상태 확인
	PC_SCHEDULE_SELECT_CHECK,

	//LOAD REQ
	PC_TO_PLC_LOAD_REQ,

	//LOAD CHECK
	PLC_TO_PC_LOAD_FLAG,

	//SCHEDULE START
	PC_BUSY_ON,

	PC_SCHEDULE_START_CHECK,

	PC_SCHEDULE_END_CHECK,

	//UNLOAD REQ
	PC_TO_PLC_UNLOAD_REQ,

	//UNLOAD CHECK,
	PLC_TO_PC_UNLOAD_FLAG,

	//END
	STEP_END,
};


enum enumEQStatus_Hanwha
{
	enumEQHanwha_Idle, 
	enumEQHanwha_Ready,
	enumEQHanwha_Stop,
	enumEQHanwha_Run,
	enumEQHanwha_Complete,
	enumEQHanwha_Alarm,
	enumEQHanwha_Manual,
	enumEQHanwha_Status_Count,
};

enum enumTowerLampUnit_Hanwha
{
	enumTowerHanwha_Red,
	enumTowerHanwha_Yellow,
	enumTowerHanwha_Green,
	enumTowerHanwha_Buzzer,
	enumTowerHanwha_TowerLampUnit_Count,
};

enum enumEQStatus_DFS
{
	enumEQDFS_Idle,
	enumEQDFS_Ready,
	enumEQDFS_Stop,
	enumEQDFS_Run,
	enumEQDFS_Complete,
	enumEQDFS_Alarm,
	enumEQDFS_Manual,
	enumEQDFS_Status_Count,
};

enum enumTowerLampUnit_DFS
{
	enumTowerDFS_Red,
	enumTowerDFS_Yellow,
	enumTowerDFS_Green,
	enumTowerDFS_Blue,
	enumTowerDFS_White,
	enumTowerDFS_Buzzer,
	enumTowerDFS_TowerLampUnit_Count,
};

enum enumEQType_Formation
{
	enumEQType_Precharger,	//PLC 제어권, MOXA IO 통신타입 모듈 사용, Precharger
	enumEQType_DCIR,		//PC 제어권, MOXA IO 통신타입 모듈 사용, DCIR
	enumEQType_Formation,	//PC 제어권, AJIN IO 펄스타입 모듈 사용, 2단랙
	enumEQType_DFS,
};

typedef struct tagDevConfParam
{
	int      deviceNumber;
	WCHAR    profilePath[256];
}DevConfParam, *pDevConfParam;

typedef struct _EquipmentStageDioInfo
{
	int		m_nInputDioCount;
	int		m_nOutputDioCount;

	int		m_nFirstInputDioNo;
	int		m_nLastInputDioNo;
	int		m_nFirstOutputDioNo;
	int		m_nLastOutputDioNo;

	int		m_nInputPageMap[EQP_STAGE_DIO_MAXCOUNT];
	int		GetInputPageDioNo(int nPageNo) { return m_nInputPageMap[nPageNo]; }
	int		GetInputPageNo(int nDioIndexNo)
	{
		for (int nPageMapCount = 0; nPageMapCount < EQP_STAGE_DIO_MAXCOUNT; nPageMapCount++)
		{
			if (nDioIndexNo == m_nInputPageMap[nPageMapCount])
				return nPageMapCount;
		}
		return -1;
	}

	int		m_nOutputPageMap[EQP_STAGE_DIO_MAXCOUNT];
	int		GetOutputPageDioNo(int nPageNo) { return m_nOutputPageMap[nPageNo]; }
	int		GetOutputPageNo(int nDioIndexNo)
	{
		for (int nPageMapCount = 0; nPageMapCount < EQP_STAGE_DIO_MAXCOUNT; nPageMapCount++)
		{
			if (nDioIndexNo == m_nOutputPageMap[nPageMapCount])
				return nPageMapCount;
		}
		return -1;
	}
} EQP_STAGE_DIO_INFO, *pEqpStageDioInfo;

typedef struct _SubEquipmentDioInfo
{
	int		m_nSubEqpNo;
	CString m_strSubEqpId;

	int		m_nTotalDioCount;
	int		m_nInputDioTotalCount;
	int		m_nOutputDioTotalCount;

	EQP_STAGE_DIO_INFO m_EqpStageDioInfo[EQP_SUBEQP_MAX_COUNT];
	EQP_STAGE_DIO_INFO GetEqpStageDioInfo(int nIndexNo) { return m_EqpStageDioInfo[nIndexNo]; }

} SUBEQP_DIO_INFO, *pSubEqpDioInfo;

// -------------------------------------------------------------------------------------------------

class CMgrDio : public CSingleton<CMgrDio>
{
public:
	CMgrDio();
	~CMgrDio();


public:
	bool	Initialize();											// 초기화

	bool	InitializeVariables();
	bool    LoadDioIni();		
	bool	InitializeThread();
	
	void	Init_OutputIO();

	// ----------------------------------------------------------------------------

	int		GetCardType(CString strType);
	int		GetCardTypeint();

	// ----------------------------------------------------------------------------

	bool    m_fnDioOpen(CString strType);							// Dio Port Open

	bool	m_fnDioOpenAdLink(int nCardNum);							
	bool	m_fnDioOpen4750();
	bool    m_fnDioOpenEtherCat();
	BOOL    m_fnDioOpenAjin();										// AJIN

	bool	m_fnDioADLINKControl(int nCardNum, int nCh, bool bOn);	// ADLink Dio Out Control
	bool	m_fnDioADVANTECHControl(int nCh, bool bOn);				// Advantech Dio Out control
	bool    m_fnDioEtherCatControl(int nCh, BOOL bOn);				// EtherCat Dio Out control
	bool    m_fnGetDioOutput(int nCh, bool* bOn);

	bool	m_fnDioStatus(int nCardNum, BYTE byStatus[50]);		//Dio Status Read

	void	StartThread();
	void	StopThread();

	void	ConfDevice(int nType);
	void	ConfigureDevice(int nType);
	void	SetStatus(int nStatus, int nChannel);
	void	SetDIO(int nCh, BOOL bOn);

	void	DIOAllOff(int nChannel);
	int*	GetDO();
	void	GetDI();

	int		GetDIStatus(int nPos) { return m_nDIStatus[nPos]; }

	void	CheckError(ErrorCode errorCode);

	void	SetBuzzer(BOOL bFlag) { m_bBuzzer = bFlag; }
	BOOL	GetBuzzer() { return m_bBuzzer; }

	byte	drawButton(int i, int j, ErrorCode ret, int  portNumber, byte * portValue);
	byte	drawButtonIn(int i, int j, ErrorCode ret, int  portNumber, byte * portValue);

	int		m_nCardType;

	int m_iStatusTowerLamp_Hanwha[enumEQStatus_Hanwha::enumEQHanwha_Status_Count][enumTowerLampUnit_Hanwha::enumTowerHanwha_TowerLampUnit_Count];
	int m_iStatusTowerLamp_DFS[enumEQStatus_DFS::enumEQDFS_Status_Count][enumTowerLampUnit_DFS::enumTowerDFS_TowerLampUnit_Count];

	void SetEQType(CString strType);
	int GetEQType() { return m_nEQType; }

	
private:
	int		m_nCardNum;
	U32		m_uOutPut;
	U32		m_uInPut;
	I16		m_iPciCard[2];		//확장성....
	bool	m_bDioOpen;

	//CWinThread*	m_pDioThread;
	static	UINT	DioThread(LPVOID pParam);
	BOOL			m_bLoop;

	InstantDoCtrl *      m_instantDoCtrl;
	InstantDiCtrl *		 m_instantDiCtrl;
	DevConfParam         m_confParam; // the device's configure information from config form.
	//byte* m_portData;
	int m_portCount;
	int m_nDOStatus[64];
	int m_nDIStatus[64];

	BOOL m_bBuzzer;
	CString m_strDeviceName;

	CWinThread*		m_pStatusLightThread;
	static	UINT	m_fnStatusLightThread(LPVOID pParam);
	
	BOOL			m_bStatusLightLoop;

	ULONGLONG		m_ullastPLCHeartBitReceiveTime;
	void			PLC_Heart_Bit_Check(ULONGLONG ulTime);
	
// -------------------------------------------------------------------------------------------------
// FORMATION
private:
	CAjinIoControl* m_AjinIoControl;

	SUBEQP_DIO_INFO m_SubEqpDioInfo[10];

	int		m_nSubEqpIndexNo;

	int		m_nTotalDioCount;
	int		m_nInputDioCount;
	int		m_nOutputDioCount;

	int		m_nFirstInputDioNo;
	int		m_nFirstOutputDioNo;
	int		m_nChCount;

	int		m_nEQType;

	BOOL	m_bFanSatus[2][8];

public:
	CString	m_strIoName[IO_SLOT_MAX][IO_PARA_COUNT][IO_VALUE_COUNT];
	SUBEQP_DIO_INFO GetSubEqpDioInfo(int nIndexNo) { return m_SubEqpDioInfo[nIndexNo]; }
	SUBEQP_DIO_INFO GetSubEqpDioInfo(CString subEqpId);

	// MAIN EQUIPMENT
	int		GetInputDioCount() { return m_nInputDioCount; }
	int		GetOutputDioCount() { return m_nOutputDioCount; }
	int		GetFirstInputDioNo() { return m_nFirstInputDioNo; }
	int		GetFirstOutputDioNo() { return m_nFirstOutputDioNo; }
	int		GetChCount() { return m_nChCount; }

	// ----------------------------------------------------------------------------

	CAjinIoControl* GetAjinIoControl() { return m_AjinIoControl; }
	
	bool	GetAjinInputData(int address);
	bool	GetAjinOutputData(int address);
	bool	SetAjinOutputData(int address, bool value);
	bool	IsGetAjinOutputData(int address, bool value);

	//Manual 동작
	bool GetFanStatus(int nIndex);
	
	///////MOXA IO
	//Client 
	//TcpInfo	MoxaIOBoard[IO_SLOT_COUNT];



	int iRet;			//stored return code
	int	iHandle;		//stored handle for ioLogik series device
	BYTE bytStatus;	 	//stored connection status
	DWORD dwTimeOut;
	char bytInputValue[32];
	char Password[8];
	BYTE bytStartChannel;
	BYTE bytCount;
	WORD wValue[16];
	DWORD dwValue[16];
	DWORD dwHiValue[16];
	DWORD dwLoValue[16];
	WORD wHiValue[16];
	WORD wLoValue[16];

	bool m_bConnect;
	void SetConnect(bool bConnect);
	bool GetConnect();

	void MoxaIOConnect();
	void MoxaIOCheckConnection(int nIONumber);

	void MoxaIOClose();
//----Moxa 통합---//
	void SetMoxa_Init();			//PC -> PLC Signal Reset, Switch Lamp Reset

	void SetMoxa_PowerOnLamp(bool bLampOn);
	void SetMoxa_PowerOffLamp(bool bLampOn);
	void SetMoxa_PowerResetLamp(bool bLampOn);

	bool GetMoxa_PowerOnSignal();		//PLC -> PC Start Bit Set ::: Test IO Address 02
	bool GetMoxa_PowerOffSignal();		//PLC -> PC Start Bit Set ::: Test IO Address 02
	bool GetMoxa_PowerResetSignal();		//PLC -> PC Start Bit Set ::: Test IO Address 02

	void SetMoxa_TowerLamp_Red(bool bLampOn);
	void SetMoxa_TowerLamp_YELLOW(bool bLampOn);
	void SetMoxa_TowerLamp_GREEN(bool bLampOn);
	void SetMoxa_TowerLamp_BUZZER(bool bBuzzer);

//------------------PreCharge Start ------------------------

	//Moxa PLC Communication ()
	//PLC -> PC
	bool GetMoxa_PLC_HeartBeat();	//PLC -> PC Heart Bit Set 몇초이상 상태 변화 없는 경우::: Test IO Address 00
	bool GetMoxa_PLC_Ready();		//PLC -> PC Ready Bit Set ::: Test IO Address 01
	bool GetMoxa_PLC_Start();		//PLC -> PC Start Bit Set ::: Test IO Address 02
	bool GetMoxa_ChamberDoorOpen();		// false : Close true : Open

	//PC -> PLC
	void SetMoxa_PC_HeartBit(bool bHeartBit);
	void SetMoxa_PC_Ready(bool bReadyBit);			//PC -> PLC Schedule Ready Bit Set ::: Test IO Address 9
	void SetMoxa_PC_Busy(bool bBusyBit);			//PC -> PLC Busy Bit Set Schedule Processing ::: Test IO Address 10 
	void SetMoxa_PC_WorkComplete(bool bCompleteBit);			//PC -> PLC Complete Bit Set Schedule Processing ::: Test IO Address 10 

	bool GetMoxa_PC_Busy();		//PLC -> PC Ready Bit Set ::: Test IO Address 01
		
//-------------------PreCharge End---------------------------------------
//------------------DCIR Start ------------------------

	void SetMoxa_DCIR_Pusher_UpDown(bool bUpDown);
	void SetMoxa_DCIR_Probe_UpDown(bool bUpDown);
	void SetMoxa_DCIR_Tray_Centering_ForBack(bool bForBack);
	void SetMoxa_DCIR_Tray_Clamp_ForBack(bool bClampUnclamp);
	
	void Set_SwitchLamp();
	void Set_TowerLamp(int nStatus);

	bool GetMoxa_DCIR_TrayCheckSensor();
	bool GetMoxa_DCIR_TrayInterlockCheckSensor();

	bool GetMoxa_DCIR_PowerOnSignal();		//PLC -> PC Start Bit Set ::: Test IO Address 02
	bool GetMoxa_DCIR_PowerOffSignal();		//PLC -> PC Start Bit Set ::: Test IO Address 02
	bool GetMoxa_DCIR_PowerResetSignal();		//PLC -> PC Start Bit Set ::: Test IO Address 02

//-------------------DCIR End---------------------------------------



	BOOL MoxaIoRead(int nIONumber);
	void MoxaIoWrite(int nIONumber);

	void CheckErr(int iRet, char * szFunctionName);


	bool m_bIO[IO_SLOT_COUNT][IO_CHANNLE_COUNT];

	bool GetIOStatus(int nBoardNumber, int nChNumber) 
	{		
		return m_bIO[nBoardNumber][nChNumber];
	}
	void SetIO(int nBoardNumber, int nChNumber, bool bOnOff);

	void SetOutPutIO(int nBoardNumber, int nChNumber, bool bOnOff);

	bool GetOutIoStatus(int nBoardNumber,int nChNumber);
	
	int GetIOTotalCount() { return m_nTotalDioCount; }

	int nTimeOutCnt;
	int nTimeOutMax;


	//BCR Read 
	void SetBcrRead(BOOL nFlag);
	bool GetBcrRead();
	BOOL m_bSetBcrReadFlag;



	void SetMeasurementStep(int estepPress);

	int		m_ePressRunstepMeasurementCurrent;

	BOOL m_AutoCompleteFlag;
	BOOL m_ManualUnloadStartFlag;
	BOOL m_ManualStartStop;
	BOOL m_RepeatRunStart;



	void SetCenteringCylinder(int nStageNo, bool bForBack);
	void SetClampCylinder(int nStageNo, bool bForBack);
	void SetProbeCylinder(int nStageNo, bool bUpDown);
	void SetPusherCylinder(int nStageNo,bool bUpDown);
	void SetJobChangeCylinder(int nStageNo,bool bForBack);


	bool GetFireCheckSensor(int nStageNo);
	bool GetSmokeCheckSensor1(int nStageNo);
	bool GetSmokeCheckSensor2(int nStageNo);
	bool GetEMOCheckSensor(int nStageNo);

	ULONGLONG		m_ullastFanStatusOnLastTime[16];
	BOOL Fan_Status_Check(int nFanIndex, int nCheckTime);	//220704 YGY Formation Fan Status Check


	void Set_FanStatus(int nFanIndex, bool bFanSatus);
	BOOL Get_FanStatus(int nStageNo, int nFanIndex);

	BOOL Get_Alarm_FanStatus(int nStageNo,int nAlarmCount);


	void Update_TowerLamp();
};

