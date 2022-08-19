#ifndef _COMI_ECAT_SDK_SYS_DEF_H_
#define _COMI_ECAT_SDK_SYS_DEF_H_

#include "ComiEcatSdk_Def.h"

//------------------------------------------- Defines ------------------------------------------------------------------------//
#define ecNET_CFG_FILE_VER			1	// TEcNetConfig ����ü �� TEcSlaveCfg ����ü�� ����
#define ecOPT_PDO_UNIT_TYPE_NEW			// PdoAssign �����͸� Input/Output ���о��� �����ϴ� ������� �ٲ� ���� 

#define ecMAX_NUM_LMEM_SECTIONS		16	// Maximum number of logical memory sections
#define ecMAX_LMEM_SECT_NAME_LEN	63	// Maximum string length of a logical memory section

// ���⼭ �����ϴ� ���޸��� Base�ּҴ� ���������θ� ���Ǵ� ���̸� ����ڰ� ����� ���� 
// InputPDO ������ OutputPDO ���� ��� �ּҰ� 0���� �����Ѵ�.
#define ecINPDO_BASE_LOGIC_ADDR		10000
#define ecOUTPDO_BASE_LOGIC_ADDR	0

#define ecSLAVE_ID_MASTER	0xffff   // Master �����ΰ� ó���ؾ� �ϴ� Command�� �� SlaveIdx �� ��ſ� �����ؾ� �ϴ� ID�� 

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
	ecAXIS_TYPE_NONE, // Slave�� ������� ����� ���� ���
	ecAXIS_TYPE_SERVO, // Slave�� servo driver�� ���
	ecAXIS_TYPE_STEP, // Slave�� step driver�� ���
	ecAXIS_TYPE_PULSEMOTION // Slave�� Pulse Motion Controller�� ���.
}EEcAxisType;

typedef enum{
	ecPDO_SYNC_FREERUN, // Slave�� ������� ����� ���� ���
	ecPDO_SYNC_SM, // Slave�� servo driver�� ���
	ecPDO_SYNC_DC, // Slave�� step driver�� ���
	ecPDO_SYNC_NONE = 0xff, // PDO Sync Mode ������ �������� �ʴ� ���.
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
	ecAI_ST_MINMAX, // Scale�����Ͱ� Min/Max ���� ��Ÿ���� ���.
	ecAI_ST_MULTIPLY, // Scale �����Ͱ� ���ϱⰪ���� ���Ǵ� ���. �� ���� ScaleMin ���� ��������� ���ȴ�.  
	ecAI_ST_INVALID
}EEcAiScaleType;

typedef enum{
	ecAO_DT_INT, // signed integer
	ecAO_DT_UINT, // unsigned integer
	ecAO_DT_FLT, // floating point data
	ecAO_DT_INVALID
}EEcAoDataType;

typedef enum{
	ecAO_ST_MINMAX, // Scale�����Ͱ� Min/Max ���� ��Ÿ���� ���.
	ecAO_ST_MULTIPLY, // Scale �����Ͱ� ���ϱⰪ���� ���Ǵ� ���. �� ���� ScaleMin ���� ��������� ���ȴ�.  
	ecAO_ST_INVALID
}EEcAoScaleType;

// EEcEcatCmd: COE SDOInfo Service �߿��� 'OD List Get' ���� �̿��� �� Get�ϴ� Obect Dictionary List�� ������ ��Ÿ���� ID������ ������ �� //
typedef enum{
	ecODLT_LEN_LIST		= 0x0, // �� OD List Type(1~5)�� ���� List�� ũ�⿡ ���� ����Ʈ(�̶����� ����Ʈ �������� 2����Ʈ ���� 5���� �����ȴ�)
	ecODLT_ALL_OBJ		= 0x1, // Object Dictionary�� �ִ� ��� Object�� Index ����Ʈ 
	ecODLT_RXPDO_OBJ	= 0x2, // RxPDO�� ���� ������ Object���� ����Ʈ 
	ecODLT_TXPDO_OBJ	= 0x3, // TxPDO�� ���� ������ Object���� ����Ʈ 
	ecODLT_STORE_OBJ	= 0x4, // ����̽��� ��ü�� �� ����Ǿ�߸� �ϴ� Object���� ����Ʈ 
	ecODLT_STARTUP_OBJ	= 0x5, // startup parameter�� ���Ǵ� Object���� ����Ʈ 
	ecODLT_INVALID
}EEcSdoiODListType;


// EEcFwuDnldCmd: FW Download �Լ��� ������ �� �Ű������� ���޵Ǵ� ��ɵ��� ID //
typedef enum{
	ecFWU_DNLD_START, // download �����϶�� Ŀ�ǵ�
	ecFWU_DNLD_CONTINUE, // �Ϲ����� download ������ ���� Ŀ�ǵ�
	ecFWU_DNLD_LAST, // ������ �������� download ���� Ŀ�ǵ�
	ecFWU_DNLD_END // download ���� Ŀ�ǵ�
}EEcFwuDnldCmd;

typedef enum{
	ecFWU_UPLD_FILEINFO, // FileInfo ������ ���ε��ϴ� Ŀ�ǵ�
	ecFWU_UPLD_FILEDATA // File data�� ���ε��ϴ� Ŀ�ǵ�
}EEcFwuUpldCmd;

//------------------------------------------------------- Structures -----------------------------------------------------------------------------//
typedef struct{
	t_ui16 wYear;
	t_ui16 wMonth;
	t_ui16 wDayOfWeek; // �� �ʵ�� ��� ���� �� ����
	t_ui16 wDay;
	t_ui16 wHour;
	t_ui16 wMinute;
	t_ui16 wSecond;
	t_ui16 wMilliseconds; // �� �ʵ�� ��� ���� �� ����
}TEcDateTime;

typedef struct{
	#define ecFWU_SIGNATURE	0x56981532
	t_ui32 Signature; // ������ ��ȿ���� ��Ÿ���� ���� ����ϴ� signature 0x56981532
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
	t_ui32 iProgress:	16; // ������: 0.01% ������ ���� ���������� ǥ��
	t_ui32 TargSize;
	t_ui32 DoneSize;	
}TEcFwuStatus;

// TEcSdoiODList: COE SDO Object Dictionary List ������ //
typedef struct{
	t_ui16 SturctType; // ����ü Ÿ��: TEcSdoiODList, TEcSdoiObjDesc, TEcSdoiEntryDesc �� ������ ����ü�� ���� �޸� ������ �� �� ecGn_ReleaseSdoInfoData() �Լ� �ϳ��� ���ؼ� �����ϹǷ� ���޵� ����ü�� Ÿ���� �ľ��ϱ� ���ؼ� �ʿ��ϴ�.
	t_ui16 ListType;
	t_ui16 NumItems;
	t_ui16 *Items;
}TEcSdoiODList;

// TEcSdoiODList: COE SDO Object Description ������ //
typedef struct{
	t_ui16 SturctType; // ����ü Ÿ��: TEcSdoiODList, TEcSdoiObjDesc, TEcSdoiEntryDesc �� ������ ����ü�� ���� �޸� ������ �� �� ecGn_ReleaseSdoInfoData() �Լ� �ϳ��� ���ؼ� �����ϹǷ� ���޵� ����ü�� Ÿ���� �ľ��ϱ� ���ؼ� �ʿ��ϴ�.
	t_ui16 ObjIndex; // Object Index
	t_ui16 DataType; // Data Type of the object
	t_ui8 MaxSubIdx; // Maximum number of sub indices
	t_ui8 ObjCode;	 // Object Code: 7-Variable, 8-Array, 9-Record
	t_char *pObjName; // Name of the object
}TEcSdoiObjDesc;

// TEcSdoiODList: COE SDO Object Entry Description ������ //
typedef struct{
	t_ui16 SturctType; // ����ü Ÿ��: TEcSdoiODList, TEcSdoiObjDesc, TEcSdoiEntryDesc �� ������ ����ü�� ���� �޸� ������ �� �� ecGn_ReleaseSdoInfoData() �Լ� �ϳ��� ���ؼ� �����ϹǷ� ���޵� ����ü�� Ÿ���� �ľ��ϱ� ���ؼ� �ʿ��ϴ�.
	t_ui16 ObjIndex; // Object Index
	t_ui8 SubIndex;
	t_ui8 ValueInfo; // ��Ʈ���� pEntryData ���� ���� �������� ������ ��Ÿ���µ�, �� ���� 0�̸� pEntryData���� SubIndex�� Object �̸��� �������.
	t_ui16 DataType; // �ش� Object�� data type index
	t_ui16 BitLen; // bit length of the object
	t_ui16 ObjAccess; // � AL State���� �ش� object�� access�� �� �ִ����� ��Ÿ���� ��.
	t_byte *pEntryData; // Entry Description Data
}TEcSdoiEntryDesc;

// �� �����̺��� Device Type ���� : ecSlv_GetDevTypeInfo() �Լ��� Argument�� ����. //
typedef struct{
	t_ui16 DeviceType; // CoE SDO 1000h �� ����.
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
	t_ui16 Resv; // Dummy ���� 4����Ʈ ������ ���߱� ���ؼ� �ִ� ����.
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


// ESI�� "DeviceType:Mailbox:CoE" �׸� //
typedef struct{
	t_ui32 IsSupport:		1;
	t_ui32 IsPdoAssign:		1; // ESI "Mailbox:CoE"�� PdoAssign �Ӽ� ����
	t_ui32 IsPdoConfig:		1; // ESI "Mailbox:CoE"�� PdoConfig �Ӽ� ����
	t_ui32 IsPdoUpload:		1; // ESI "Mailbox:CoE"�� PdoUpload �Ӽ� ����
	t_ui32 IsComptAccess:	1; // ESI "Mailbox:CoE"�� CompleteAccess �Ӽ� ����
	t_ui32 IsSegmSdoSupp:	1; // ESI "Mailbox:CoE"�� SegmendedSdo �Ӽ� ����
	t_ui32 Resv:			26;
}TEcMbxCoE;


#define ecPDO_ASSGN_SECT_SIZE	136

typedef struct {
	t_ui8 SmIdx;
	t_ui8 NumAssVals;
	t_ui16 AssVal[255]; // �̸���� �迭 ũ��� 255�� �����Ǿ� ������ �̰��� ����ü�� Ʋ�� ��� ���� ���̰� ���� ���Ǵ� AssVal �迭�� ũ��� NumAssVals �� �����Ѵ�.
}TEcPdoAssObj;

typedef struct{
	t_ui32 VendId;
	t_ui32 ProdCode;
	t_i16 AutoAddr; // �ڵ��ּ�. ���������� �Ű����� �ּҷμ� ù��° �����̺�� 0 �ι�°�� -1, ����°�� -2,....�� ������ ���� �����ȴ�.
	t_ui16 PhysAddr; // �����ּ�
	t_ui32 SmIdx_OutMbx:	4; // OutMailbox������ ���Ǵ� SyncManager Index, OutMbox�� �������� ������ �� ���� 0xff�� �����Ѵ�.
	t_ui32 SmIdx_InMbx:		4; // InMailbox������ ���Ǵ� SyncManager Index, InMbox�� �������� ������ �� ���� 0xff�� �����Ѵ�.
	t_ui32 AxisType:		4; // 0-Axis�� �ƴ�, 1-servo driver, 2-step driver, 3-pusle motion, 4-other
	t_ui32 NumSM:			4; // ���� ����ϴ� SyncManager ����
	t_ui32 SmSyncMode:		4; // 0-Free Run Mode,  1-SM Event Mdoe,   2 - DC Mode
	t_ui32 AliasILMode:		2; // Alias�� PhysAddr�� �� ���Ͷ��ϴ� �ɼ�: 0-'Not use', 1-'RegisterMode', 2-'Reg134 Mode'
	t_ui32 AliasILSize:		2; // Alias�������� ����Ʈ ũ��: 0-'Not use', 1-'1����Ʈ', 2-'2����Ʈ'
	t_ui32 Resv1:			8;
	// SyncManager ���� //
	TEcSmCfg SM[ecMAX_NUM_SMMU]; // �� SM�� ������
	TEcFmmuCfg FMMU[ecMAX_NUM_FMMU];
	TEcMbxCoE MbxCoE;
	t_byte PdoAssData[ecPDO_ASSGN_SECT_SIZE];
	/* PdoAssInf ������ �Ʒ��� ���� ������ �����͸� �����Ѵ�
		typedef struct{
			t_ui8 NumPdoAssObjs ;
			t_ui8 Resv0;
			struct _TEcPdoAssObjInfo{
				t_ui8 SmIdx;
				t_ui8 NumAssVals;
				t_ui16 AssVal[NumAssVals];
			}PdoAssObj[NumPdoAssObjs];
		}TEcPdoAssInfo;
		���⼭ NumPdoUnits�� _TEcPdoUnit
	*/
	t_ui16 AliasILReg; // AliasILMode��  1-'RegisterMode'�� ��쿡 Alias�� �д� �������� �ּ�
	t_ui16 AliasILValue; // Alias Register Value (for AliasInterlcok)
}TEcSlaveCfg; // ���� �̴�Ĺ ����� ���ؼ� �� �ʿ���(Essential) �����̺� ���� ������

// a 'Logical Memory Section' configuration //
typedef struct{
	//char szName[ecMAX_LMEM_SECT_NAME_LEN];
	t_byte Type; // 0-DI, 1-DO, 2-AI, 3-AO, 4-SERVO, 5-ETC
	t_byte Resv[3];
	t_ui16 StartAddr; // Section�� �����ּ� (Logical Memory �������� �ּ�), 2048���� �۾ƾ� �Ѵ�.
	t_ui16 Length; // Section ����Ʈ ũ��. 2048���� �۾ƾ� �Ѵ�. 
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

// Digital Input Global Channel ���� ���� //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaGlobChan;
	t_ui16	StaLogAddr; // start logical address
	t_ui8	StaLogBit;	// start bit of logical data
	t_ui8	NumChans;	// number of channels in this channel group
}TEcDiChanMap;

// Digital Output Global Channel ���� ���� //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaGlobChan;
	t_ui16	StaLogAddr; // start logical address
	t_ui8	StaLogBit;	// start bit of logical data
	t_ui8	NumChans;	// number of channels in this channel group
}TEcDoChanMap;

// Analog Input Global Channel ���� ���� //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaLogAddr;
	t_ui32	StaLogBit:	4;
	t_ui32	BitLength:	6;
	t_ui32	DataType:	3; // 0-signed int, 1-unsigned int, 2-float; EEcAiDataType Ÿ�� ���� 
	t_ui32	ScaleType:	3; // 0-MinMax, 1-Multiply; EEcAiScaleType Ÿ�� ���� 
	t_ui32	Resv:		16;
	t_f32	ScaleMin;	// Scale Min. Value, �� ScaleType=ecAI_ST_MULTIPLY�� ��쿡�� �� ���� Multiplication ���� �����Ѵ�.  
	t_f32	ScaleMax;	// Scale Max. Value, �� ScaleType=ecAI_ST_MULTIPLY�� ��쿡�� �� ���� ���õȴ�.
}TEcAiChanMap;


// Analog Output Global Channel ���� ���� //
typedef struct{
	t_ui16	SlvIdx;
	t_ui16	StaLogAddr;
	t_ui32	StaLogBit:	4;
	t_ui32	BitLength:	6;
	t_ui32	DataType:	3; // 0-signed int, 1-unsigned int, 2-float; EEcAoDataType Ÿ�� ���� 
	t_ui32	ScaleType:	3; // 0-MinMax, 1-Multiply; EEcAoScaleType Ÿ�� ���� 
	t_ui32	Resv:		16;
	t_f32	ScaleMin;	// Scale Min. Value, �� ScaleType=ecAI_ST_MULTIPLY�� ��쿡�� �� ���� Multiplication ���� �����Ѵ�.  
	t_f32	ScaleMax;	// Scale Max. Value, �� ScaleType=ecAI_ST_MULTIPLY�� ��쿡�� �� ���� ���õȴ�.
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