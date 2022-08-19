#ifndef _COMI_ECAT_SDK_SYS_DEF_H_
#define _COMI_ECAT_SDK_SYS_DEF_H_

#include "ComiEcatSdk_Def.h"

//------------------------------------------- Defines ------------------------------------------------------------------------//
#define ecNET_CFG_FILE_VER			1	// TEcNetConfig 구조체 및 TEcSlaveCfg 구조체의 버전
#define ecOPT_PDO_UNIT_TYPE_NEW			// PdoAssign 데이터를 Input/Output 구분없이 설정하는 방식으로 바꾼 버전 

#define ecMAX_NUM_LMEM_SECTIONS		16	// Maximum number of logical memory sections
#define ecMAX_LMEM_SECT_NAME_LEN	63	// Maximum string length of a logical memory section

// 여기서 정의하는 논리메모리의 Base주소는 내부적으로만 사용되는 것이며 사용자가 사용할 때는 
// InputPDO 영역과 OutputPDO 영역 모두 주소가 0부터 시작한다.
#define ecINPDO_BASE_LOGIC_ADDR		10000
#define ecOUTPDO_BASE_LOGIC_ADDR	0

#define ecSLAVE_ID_MASTER	0xffff   // Master 스스로가 처리해야 하는 Command일 때 SlaveIdx 값 대신에 셋팅해야 하는 ID값 

//--------------------------------------- Enumeration ----------------------------------------------------------------------//

typedef enum{
	ecNETSEL_NET0	= 0,
	ecNETSEL_NET1	= 1,
	ecNETSEL_ALL	= 2 
}EEcLocNetSel;

typedef enum{
	ecLOGSECT_DI,
	ecLOGSECT_DO,
	ecLOGSECT_AI,
	ecLOGSECT_AO,
	ecLOGSECT_SERVO,
	ecLOGSECT_ETC=15,
	ecLOGSECT_INVALID=16
}EEcLogSectType;

typedef enum{
	ecAXIS_TYPE_NONE, // Slave가 모션제어 기능이 없는 경우
	ecAXIS_TYPE_SERVO, // Slave가 servo driver인 경우
	ecAXIS_TYPE_STEP, // Slave가 step driver인 경우
	ecAXIS_TYPE_PULSEMOTION // Slave가 Pulse Motion Controller인 경우.
}EEcAxisType;

typedef enum{
	ecPDO_SYNC_FREERUN, // Slave가 모션제어 기능이 없는 경우
	ecPDO_SYNC_SM, // Slave가 servo driver인 경우
	ecPDO_SYNC_DC, // Slave가 step driver인 경우
	ecPDO_SYNC_NONE = 0xff, // PDO Sync Mode 셋팅을 지원하지 않는 경우.
}EEcPdoSyncMode;

typedef enum{
	ecALIASIL_MODE_NONE,
	ecALIASIL_MODE_REG,
	ecALIASIL_MODE_REG134,
	ecALIASIL_MODE_INVALID
}EEcAliasILMode;

typedef enum{
	ecCH_MAP_TYPE_DI, // Digital Input Channel Mapping
	ecCH_MAP_TYPE_DO, // Digital Output Channel Mapping
	ecCH_MAP_TYPE_AI, // Analog Input Channel Mapping
	ecCH_MAP_TYPE_AO, // Analog Output Channel Mapping
	ecCH_MAP_TYPE_INVALID
}EEcChanMapType;

typedef enum{
	ecAI_DT_INT, // signed integer
	ecAI_DT_UINT, // unsigned integer
	ecAI_DT_FLT, // floating point data
	ecAI_DT_INVALID
}EEcAiDataType;

typedef enum{
	ecAI_ST_MINMAX, // Scale데이터가 Min/Max 값을 나타내는 경우.
	ecAI_ST_MULTIPLY, // Scale 데이터가 곱하기값으로 사용되는 경우. 이 때는 ScaleMin 값이 계수값으로 사용된다.  
	ecAI_ST_INVALID
}EEcAiScaleType;

typedef enum{
	ecAO_DT_INT, // signed integer
	ecAO_DT_UINT, // unsigned integer
	ecAO_DT_FLT, // floating point data
	ecAO_DT_INVALID
}EEcAoDataType;

typedef enum{
	ecAO_ST_MINMAX, // Scale데이터가 Min/Max 값을 나타내는 경우.
	ecAO_ST_MULTIPLY, // Scale 데이터가 곱하기값으로 사용되는 경우. 이 때는 ScaleMin 값이 계수값으로 사용된다.  
	ecAO_ST_INVALID
}EEcAoScaleType;

// EEcEcatCmd: COE SDOInfo Service 중에서 'OD List Get' 서비를 이용할 때 Get하는 Obect Dictionary List의 종류를 나타내는 ID값들을 정의한 것 //
typedef enum{
	ecODLT_LEN_LIST		= 0x0, // 각 OD List Type(1~5)에 대한 List의 크기에 대한 리스트(이때에는 리스트 아이템이 2바이트 정수 5개로 구성된다)
	ecODLT_ALL_OBJ		= 0x1, // Object Dictionary에 있는 모든 Object의 Index 리스트 
	ecODLT_RXPDO_OBJ	= 0x2, // RxPDO에 맵핑 가능한 Object들의 리스트 
	ecODLT_TXPDO_OBJ	= 0x3, // TxPDO에 맵핑 가능한 Object들의 리스트 
	ecODLT_STORE_OBJ	= 0x4, // 디바이스를 교체할 때 저장되어야만 하는 Object들의 리스트 
	ecODLT_STARTUP_OBJ	= 0x5, // startup parameter로 사용되는 Object들의 리스트 
	ecODLT_INVALID
}EEcSdoiODListType;


// EEcFwuDnldCmd: FW Download 함수를 수행할 때 매개변수로 전달되는 명령들의 ID //
typedef enum{
	ecFWU_DNLD_START, // download 시작하라는 커맨드
	ecFWU_DNLD_CONTINUE, // 일반적인 download 데이터 전송 커맨드
	ecFWU_DNLD_LAST, // 마지막 데이터의 download 전송 커맨드
	ecFWU_DNLD_END // download 종료 커맨드
}EEcFwuDnldCmd;

typedef enum{
	ecFWU_UPLD_FILEINFO, // FileInfo 정보만 업로드하는 커맨드
	ecFWU_UPLD_FILEDATA // File data를 업로드하는 커맨드
}EEcFwuUpldCmd;

//------------------------------------------------------- Structures -----------------------------------------------------------------------------//
typedef struct{
	t_ui16 wYear;
	t_ui16 wMonth;
	t_ui16 wDayOfWeek; // 이 필드는 사용 안할 수 있음
	t_ui16 wDay;
	t_ui16 wHour;
	t_ui16 wMinute;
	t_ui16 wSecond;
	t_ui16 wMilliseconds; // 이 필드는 사용 안할 수 있음
}TEcDateTime;

typedef struct{
	#define ecFWU_SIGNATURE	0x56981532
	t_ui32 Signature; // 파일의 유효성을 나타내기 위해 사용하는 signature 0x56981532
	t_ui32 FileSize;
	TEcDateTime FileTime;
	TEcDateTime DnldTime;
	t_byte Resv[128];
}TEcFwuFileInfo;

typedef struct{
	t_ui32 IsAbort:	1;
	t_ui32 Resv:	3;
	t_ui32 Phase:	4; // 0 - Idle,  1-Downloading, 2-Uploading, 3-Verifying
	t_ui32 State:	4; // 0-before job,  1-busy,  2-Normally Complete(OK),  3-abnormally complete(Error)
	t_ui32 Resv2:	4;
	t_ui32 iProgress:	16; // 진행율: 0.01% 단위의 값을 정수값으로 표현
	t_ui32 TargSize;
	t_ui32 DoneSize;	
}TEcFwuStatus;

// TEcSdoiODList: COE SDO Object Dictionary List 데이터 //
typedef struct{
	t_ui16 SturctType; // 구조체 타입: TEcSdoiODList, TEcSdoiObjDesc, TEcSdoiEntryDesc 의 세가지 구조체에 대한 메모리 해제를 할 때 ecGn_ReleaseSdoInfoData() 함수 하나를 통해서 수행하므로 전달된 구조체의 타입을 파악하기 위해서 필요하다.
	t_ui16 ListType;
	t_ui16 NumItems;
	t_ui16 *Items;
}TEcSdoiODList;

// TEcSdoiODList: COE SDO Object Description 데이터 //
typedef struct{
	t_ui16 SturctType; // 구조체 타입: TEcSdoiODList, TEcSdoiObjDesc, TEcSdoiEntryDesc 의 세가지 구조체에 대한 메모리 해제를 할 때 ecGn_ReleaseSdoInfoData() 함수 하나를 통해서 수행하므로 전달된 구조체의 타입을 파악하기 위해서 필요하다.
	t_ui16 ObjIndex; // Object Index
	t_ui16 DataType; // Data Type of the object
	t_ui8 MaxSubIdx; // Maximum number of sub indices
	t_ui8 ObjCode;	 // Object Code: 7-Variable, 8-Array, 9-Record
	t_char *pObjName; // Name of the object
}TEcSdoiObjDesc;

// TEcSdoiODList: COE SDO Object Entry Description 데이터 //
typedef struct{
	t_ui16 SturctType; // 구조체 타입: TEcSdoiODList, TEcSdoiObjDesc, TEcSdoiEntryDesc 의 세가지 구조체에 대한 메모리 해제를 할 때 ecGn_ReleaseSdoInfoData() 함수 하나를 통해서 수행하므로 전달된 구조체의 타입을 파악하기 위해서 필요하다.
	t_ui16 ObjIndex; // Object Index
	t_ui8 SubIndex;
	t_ui8 ValueInfo; // 비트별로 pEntryData 값에 담기는 데이터의 종류를 나타내는데, 이 값이 0이면 pEntryData에는 SubIndex의 Object 이름이 담겨진다.
	t_ui16 DataType; // 해당 Object의 data type index
	t_ui16 BitLen; // bit length of the object
	t_ui16 ObjAccess; // 어떤 AL State에서 해당 object를 access할 수 있는지를 나타내는 값.
	t_byte *pEntryData; // Entry Description Data
}TEcSdoiEntryDesc;

// 각 슬레이브의 Device Type 정보 : ecSlv_GetDevTypeInfo() 함수의 Argument로 사용됨. //
typedef struct{
	t_ui16 DeviceType; // CoE SDO 1000h 값 참조.
	t_ui16 NumDI;
	t_ui16 NumDO;
	t_ui16 NumAI;
	t_ui16 NumAO;
	t_ui16 NumServo;
	t_ui16 Resv[8];
}TEcDevTypeInfo;

// 'Sync Manager' item configuration //
typedef struct{
	t_ui16 StaAddr;
	t_ui16 Len;
	t_byte CtlByte;
	t_byte ActByte;
	t_ui16 Resv; // Dummy 변수 4바이트 정렬을 맞추기 위해서 있는 것임.
}TEcSmCfg;

// FMMU item configuration //
typedef struct{
	t_ui32	StaLAddr;
	t_ui16	Len;
	t_ui8	LStaBit;
	t_ui8	LStopBit;
	t_ui16	StaPAddr;
	t_ui8	PStaBit;
	t_ui8	Type; 
	t_ui8	Activate;
	t_ui8	Resv[3];
}TEcFmmuCfg;


// ESI의 "DeviceType:Mailbox:CoE" 항목 //
typedef struct{
	t_ui32 IsSupport:		1;
	t_ui32 IsPdoAssign:		1; // ESI "Mailbox:CoE"의 PdoAssign 속성 참고
	t_ui32 IsPdoConfig:		1; // ESI "Mailbox:CoE"의 PdoConfig 속성 참고
	t_ui32 IsPdoUpload:		1; // ESI "Mailbox:CoE"의 PdoUpload 속성 참고
	t_ui32 IsComptAccess:	1; // ESI "Mailbox:CoE"의 CompleteAccess 속성 참고
	t_ui32 IsSegmSdoSupp:	1; // ESI "Mailbox:CoE"의 SegmendedSdo 속성 참고
	t_ui32 Resv:			26;
}TEcMbxCoE;


#define ecPDO_ASSGN_SECT_SIZE	136

typedef struct {
	t_ui8 SmIdx;
	t_ui8 NumAssVals;
	t_ui16 AssVal[255]; // 이멤버의 배열 크기는 255로 설정되어 있으나 이것은 구조체의 틀을 잡기 위한 것이고 실제 사용되는 AssVal 배열의 크기는 NumAssVals 가 결정한다.
}TEcPdoAssObj;

typedef struct{
	t_ui32 VendId;
	t_ui32 ProdCode;
	t_i16 AutoAddr; // 자동주소. 연결순서대로 매겨지는 주소로서 첫번째 슬레이브는 0 두번째는 -1, 세번째는 -2,....의 순으로 값이 설정된다.
	t_ui16 PhysAddr; // 물리주소
	t_ui32 SmIdx_OutMbx:	4; // OutMailbox용으로 사용되는 SyncManager Index, OutMbox를 지원하지 않으면 이 값을 0xff로 설정한다.
	t_ui32 SmIdx_InMbx:		4; // InMailbox용으로 사용되는 SyncManager Index, InMbox를 지원하지 않으면 이 값을 0xff로 설정한다.
	t_ui32 AxisType:		4; // 0-Axis가 아님, 1-servo driver, 2-step driver, 3-pusle motion, 4-other
	t_ui32 NumSM:			4; // 실제 사용하는 SyncManager 갯수
	t_ui32 SmSyncMode:		4; // 0-Free Run Mode,  1-SM Event Mdoe,   2 - DC Mode
	t_ui32 AliasILMode:		2; // Alias를 PhysAddr와 비교 인터락하는 옵션: 0-'Not use', 1-'RegisterMode', 2-'Reg134 Mode'
	t_ui32 AliasILSize:		2; // Alias데이터의 바이트 크기: 0-'Not use', 1-'1바이트', 2-'2바이트'
	t_ui32 Resv1:			8;
	// SyncManager 설정 //
	TEcSmCfg SM[ecMAX_NUM_SMMU]; // 각 SM의 설정값
	TEcFmmuCfg FMMU[ecMAX_NUM_FMMU];
	TEcMbxCoE MbxCoE;
	t_byte PdoAssData[ecPDO_ASSGN_SECT_SIZE];
	/* PdoAssInf 영역은 아래와 같은 구조로 데이터를 구성한다
		typedef struct{
			t_ui8 NumPdoAssObjs ;
			t_ui8 Resv0;
			struct _TEcPdoAssObjInfo{
				t_ui8 SmIdx;
				t_ui8 NumAssVals;
				t_ui16 AssVal[NumAssVals];
			}PdoAssObj[NumPdoAssObjs];
		}TEcPdoAssInfo;
		여기서 NumPdoUnits와 _TEcPdoUnit
	*/
	t_ui16 AliasILReg; // AliasILMode가  1-'RegisterMode'인 경우에 Alias를 읽는 레지스터 주소
	t_ui16 AliasILValue; // Alias Register Value (for AliasInterlcok)
}TEcSlaveCfg; // 실제 이더캣 통신을 위해서 꼭 필요한(Essential) 슬레이브 설정 정보들

// a 'Logical Memory Section' configuration //
typedef struct{
	//char szName[ecMAX_LMEM_SECT_NAME_LEN];
	t_byte Type; // 0-DI, 1-DO, 2-AI, 3-AO, 4-SERVO, 5-ETC
	t_byte Resv[3];
	t_ui16 StartAddr; // Section의 시작주소 (Logical Memory 내에서의 주소), 2048보다 작아야 한다.
	t_ui16 Length; // Section 바이트 크기. 2048보다 작아야 한다. 
}TEcLogMemSectCfg;

// Network configuration header //
typedef struct{
	t_ui32	Version;  
	t_ui32	CycleTime;
	t_ui8	NumOutpdoLogSects; // Number of logical memory sections which has been defined
	t_ui8	NumInpdoLogSects; // Number of logical memory sections which has been defined
	t_ui16	NumSlaves; // Number of cofgured slaves
	t_ui16	NumDiChanMaps;
	t_ui16	NumDoChanMaps;
	t_ui16	NumAiChanMaps;
	t_ui16	NumAoChanMaps;
	t_byte	Resv[56];
}TEcNetConfig;

typedef struct{
	t_bool IsActive;
	t_bool IsChkTimeout;
	t_ui16 Resv;
	t_ui32 LastCfgCmdTime;
}TEcCfgModeInfo;

//=================== I/O Global Channel Mapping Info. ========================================================//

// Digital Input Global Channel 맵핑 정보 //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaGlobChan;
	t_ui16	StaLogAddr; // start logical address
	t_ui8	StaLogBit;	// start bit of logical data
	t_ui8	NumChans;	// number of channels in this channel group
}TEcDiChanMap;

// Digital Output Global Channel 맵핑 정보 //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaGlobChan;
	t_ui16	StaLogAddr; // start logical address
	t_ui8	StaLogBit;	// start bit of logical data
	t_ui8	NumChans;	// number of channels in this channel group
}TEcDoChanMap;

// Analog Input Global Channel 맵핑 정보 //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaLogAddr;
	t_ui32	StaLogBit:	4;
	t_ui32	BitLength:	6;
	t_ui32	DataType:	3; // 0-signed int, 1-unsigned int, 2-float; EEcAiDataType 타입 참조 
	t_ui32	ScaleType:	3; // 0-MinMax, 1-Multiply; EEcAiScaleType 타입 참조 
	t_ui32	Resv:		16;
	t_f32	ScaleMin;	// Scale Min. Value, 단 ScaleType=ecAI_ST_MULTIPLY인 경우에는 이 값은 Multiplication 값을 셋팅한다.  
	t_f32	ScaleMax;	// Scale Max. Value, 단 ScaleType=ecAI_ST_MULTIPLY인 경우에는 이 값은 무시된다.
}TEcAiChanMap;


// Analog Output Global Channel 맵핑 정보 //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaLogAddr;
	t_ui32	StaLogBit:	4;
	t_ui32	BitLength:	6;
	t_ui32	DataType:	3; // 0-signed int, 1-unsigned int, 2-float; EEcAoDataType 타입 참조 
	t_ui32	ScaleType:	3; // 0-MinMax, 1-Multiply; EEcAoScaleType 타입 참조 
	t_ui32	Resv:		16;
	t_f32	ScaleMin;	// Scale Min. Value, 단 ScaleType=ecAI_ST_MULTIPLY인 경우에는 이 값은 Multiplication 값을 셋팅한다.  
	t_f32	ScaleMax;	// Scale Max. Value, 단 ScaleType=ecAI_ST_MULTIPLY인 경우에는 이 값은 무시된다.
}TEcAoChanMap;

typedef struct{
	t_ui32 NumDiChanMaps;
	t_ui32 NumDoChanMaps;
	t_ui32 NumAiChanMaps;
	t_ui32 NumAoChanMaps;
	t_ui32 DiGlbChan_Trace;
	t_ui32 DoGlbChan_Trace;
} TIOChannelInfo;


#endif