#ifndef _COMI_ECAT_SDK_DEF_H_
#define _COMI_ECAT_SDK_DEF_H_

#ifndef _in
 #define _in
#endif
#ifndef _out
 #define _out
#endif
#ifndef _inout
 #define _inout
#endif

#pragma pack(push, 1)

#define ecMAX_NUM_SLAVES				200
#define ecMAX_NUM_AXES_PER_NET			64  // �ϳ��� Network���� �����ϴ� �ִ� ������� �� �� 
#define ecMAX_NUM_IX_AXES				32  // �ϳ��� �����ʿ� ������ �� �ִ� �ִ� �� �� 
#define ecMAX_NUM_SMMU					8
#define ecMAX_NUM_FMMU					4
#define ecMAX_NUM_PDO_UNIT				4 // OutPDO / InPDO �������� �Ҵ�Ǵ� SyncManager�� �ִ밹�� (In / Out ������ �Ҵ�� �� �ִ� �ִ� �� ����)
#define ecMAX_NUM_PDO_ASSIGN			8
#define ecMAX_NUM_InQUE_DATA_DESC		32 ///< �ϳ��� InQUEä�ο� �Ҵ��� �� �ִ� ������ ������ �ִ� ����
#define ecmMAX_NUM_MOT_IX_MAP			32 ///< �ִ� ���� ������ ���� �� ����
#define ecmMAX_NUM_LIST_MOT_MAP			8
#define ecmMAX_NUM_PT_MOT_MAP			8
#define ecmMAX_NUM_AXES_PER_NET			ecMAX_NUM_AXES_PER_NET
#define ecmMAX_NUM_IX_AXES				ecMAX_NUM_IX_AXES // �ϳ��� �����ʿ� ������ �� �ִ� �ִ� �� �� 
#define ecmMAX_NUM_PTM_AXES				8	// �ϳ��� PT-Motion �ʿ� ������ �� �ִ� �ִ� �� ��
#define ecmMAX_NUM_SPLINE_AXES			8 
#define ecmMAX_NUM_SPLINE_OBJ			100
#define ecmMAX_NUM_SPLINE_INPUT_POINTS	10000 
#define ecmMAX_NUM_ATRQ_MULTIVALS		1000  // �ִ� ���� ������ AutoTorq ����� Multi-Value Items ���� 
#define ecmMAX_POSCORR_TABLE_SIZE		10000 ///< �� �ະ�� �Ҵ��� �� �ִ� Position Correction Table�� �ִ� ũ�� 
#define ecmMAX_CMPCONT_TABLE_SIZE		32768
#define ecmDEFAULT_LM_QUE_DEPTH			50000 // default list motion queue depth
#define EC_MAXNAME						62	// max product name length

typedef char			t_char;
typedef unsigned char	t_byte;
typedef unsigned char	t_uchar;
typedef t_byte			t_bool;
typedef t_char			t_i8;
typedef t_uchar			t_ui8;
typedef unsigned short	t_ui16;
typedef short			t_i16;
typedef t_ui16			t_word;
typedef unsigned int	t_ui32;
typedef int				t_i32;
typedef unsigned __int64     t_ui64;
typedef t_ui32			t_dword;
typedef float			t_f32;
typedef double			t_f64;
typedef t_bool			t_success;
typedef t_ui32			t_cmdidx;
typedef t_ui32			t_pdoidx;
typedef t_ui32			t_handle32;
typedef __int64			t_i64;
typedef unsigned __int64	t_ui64;
typedef void*			t_handle;

// File Version Compatibility Result ID //
typedef enum{
	ecVER_MISMATCH_HIGHER	= -2,	// �ش� ������ ������ ������ �Ǵ� ���ϰ� ȣȯ���� �ʴ� ���� �������� ��Ÿ��
	ecVER_MISMATCH_LOWER	= -1,	// �ش� ������ ������ ������ �Ǵ� ���ϰ� ȣȯ���� �ʴ� ���� �������� ��Ÿ��
	ecVER_NOT_FOUND			= 0,	// �ش� ������ ������ ã�� �� ����(������ ���ų�, ���� ������ ���� �� ���� ���)
	ecVER_MATCH				= 1,	// �ش� ������ ������ ������ �Ǵ� ���ϰ� ȣȯ�� �Ǵ� �������� ��Ÿ�� 
	ecVER_INVALID
}EEcVerCompatResult;


typedef enum{
	ecFF_TYPE1 = 1,
	ecFF_TYPE2 = 2
}EEcFastFuncType;

typedef enum{
	ecCMD_APRD = 0x01,
	ecCMD_APWR = 0x02,
	ecCMD_FPRD = 0x04,
	ecCMD_FPWR = 0x05,
	ecCMD_BRD  = 0x07,
	ecCMD_BWR  = 0X08,
	ecCMD_LRD  = 0x0a,
	ecCMD_LWR  = 0x0b,
	ecCMD_LRW  = 0x0c,
	ecCMD_ARMW = 0x0d,
	ecCMD_FRMW = 0x0e
}EEcEcatCmd;

typedef enum{
	ecSLV_INFO_VENDOR_ID = 0,
	ecSLV_INFO_PROD_CODE = 1,
	ecSLV_INFO_REV_NO	 = 2,
	ecSLV_INFO_SER_NO	 = 3
}EEcSlvInfoType;

typedef enum{
	ecAL_STATE_DISCON	 = 0,
	ecAL_STATE_INITIAL	 = 1,
	ecAL_STATE_PREOP	 = 2,
	ecAL_STATE_BOOTSTRAP = 3,
	ecAL_STATE_SAFEOP	 = 4,
	ecAL_STATE_OP		 = 8
}EEcAlState;

// OutPDO �������� �ʱⰪ ���� ��� (OP���� ��ȯ�� �� ���õǴ� �ʱⰪ) //
typedef enum{	
	ecOPDO_INIT_KEEPLAST,	// �������� ��°��� �״�� �����Ѵ�.
	ecOPDO_INIT_ZERO,		// ��� ���� 0���� �����Ѵ�.
	ecOPDO_INIT_USERDEF,	// ����ڰ� �̸� ������ �ʱⰪ�� �����Ѵ�.
	ecOPDO_INIT_INVALID
}EEcOPDOInitMode;

// Debug Logging Type //
typedef enum{
	ecDLOG_TYPE_FILE		= 0, // Text file�� �α� ����� ����ϴ� ��� 
	ecDLOG_TYPE_TRACE		= 1, // Debug ��Ʈ�� �� ���ؼ� �α� ����� ���� �ִ� ��� 
	ecDLOG_TYPE_COMITOOL	= 2, // COMIZOA���� ������ ����� ���� ����Ͽ� �α� ����� �����ִ� ��� (���� �������� ������ ���� ���� ����)
	ecDLOG_TYPE_INVALID
}EEcDlogType;

// Debug Logging Level //
typedef enum{
	ecDLOG_LEVEL_DISALBE= 0, // No debug logging
	ecDLOG_LEVEL_ERR	= 1, // ���� �߻����� ���� �α� 
	ecDLOG_LEVEL_CMD	= 2, // ecDLOG_LEVEL_ERR������ �����ϸ� ���ؼ� Set �Լ� �� ���� Ŀ�ǵ� �Լ� �α� 
	ecDLOG_LEVEL_GET	= 3, // ecDLOG_LEVEL_CMD������ �����ϸ� ���ؼ� Get �Լ��� �α� (�� �Ϻ� ���� üũ �Լ����� ����)
	ecDLOG_LEVEL_ALL	= 4, // ��� �Լ��� ���ؼ� �α� ���� 
	ecDLOG_LEVEL_INVALID
}EEcDlogLevel;

// Debug Logging Level //
typedef enum{
	ecDLOG_ERR_LOG_NONE	= 0, // ������ �߻��ϴ��� �α��� ������ �ʴ´�. 
	ecDLOG_ERR_LOG_CMD	= 1, // Command �Լ������� ������ �߻����� �� �α��� �����.
	ecDLOG_ERR_LOG_GET	= 2	 // ��� �Լ��� ���ؼ� ������ �߻����� �� �α��� �����.
}EEcDlogErrLogType;


// InPDOQue ��ɿ��� Queueing start/stop trigger ��� ������ ���� ���̵� //
typedef enum{
	ecTRG_MODE_NONE, // Ʈ���Ÿ�� ��� ����.
	ecTRG_MODE_COMMON, // ��� ä���� ���� Ʈ���� �ҽ��� �����. �� ������ TInPDOQueChannel::Cfg.StaTrg.pCommonTrgSig �����Ͱ� ����Ű�� Signal ������ ���� 1�� �Ǹ� 
	ecTRG_MODE_COUNT, // InputPDO ������ ���� �̺�Ʈ ī��Ʈ�� �̿��ؼ� Ʈ������. �� ���� Count ���� ���� TInPDOQueChannel::Cfg.StaTrg.TrgRefVal ��� ������ ���ؼ� �����Ѵ�. 
	ecTRG_MODE_PDODATA, // Ư�� PDO �������� ���� üũ�Ͽ� Ʈ���� ������ �����ϴ� ���. 
	ecTRG_MODE_INVALID
}EEcQueTrgMode;


typedef enum{
	ecCOMTRG_MODE_NONE, // Ʈ���Ÿ�� ��� ����.
	ecCOMTRG_MODE_MANUAL, // Manual trigger ��� 
	ecCOMTRG_MODE_COUNT, // InputPDO ������ ���� �̺�Ʈ ī��Ʈ�� �̿��ؼ� Ʈ������. �� ���� Count ���� ���� TInPDOQueChannel::Cfg.StaTrg.TrgRefVal ��� ������ ���ؼ� �����Ѵ�. 
	ecCOMTRG_MODE_PDODATA, // Ư�� PDO �������� ���� üũ�Ͽ� Ʈ���� ������ �����ϴ� ���. 
	ecCOMTRG_MODE_INVALID
}EEcQueCommTrgMode;

// InPDOQue ��ɿ��� Start/Trigger ����� �� �����͸� ���ϴ� ����� ������ ���� ���̵� //
typedef enum{
	ecTRG_LEV_TYPE_EQ, // �񱳵����Ͱ� Ref �����Ϳ� ������(Equal) Ʈ���� ��
	ecTRG_LEV_TYPE_GT, // �񱳵����Ͱ� Ref �����ͺ��� ũ��(Greater than) Ʈ���� ��
	ecTRG_LEV_TYPE_GE, // �񱳵����Ͱ� Ref �����ͺ��� ũ�ų� ������ Ʈ���� ��
	ecTRG_LEV_TYPE_LT, // �񱳵����Ͱ� Ref �����ͺ��� ������(Less than) Ʈ���� ��
	ecTRG_LEV_TYPE_LE, // �񱳵����Ͱ� Ref �����ͺ��� �۰ų� ������ Ʈ���� ��
	ecTRG_LEV_TYPE_RE, // �񱳵����Ͱ� Ref �����͸� �������� ��¿���(Rising edge, �񱳵������� �� ���� ��/�� ���� Ref�� ���� ���� ���¿��� ū ���·� ��ȭ�� ���)�� �߻��ϸ� Ʈ���� ��
	ecTRG_LEV_TYPE_FE, // �񱳵����Ͱ� Ref �����͸� �������� �ϰ�����(Rising edge, �񱳵������� �� ���� ��/�� ���� Ref�� ���� ū ���¿��� ���� ���·� ��ȭ�� ���)�� �߻��ϸ� Ʈ���� ��
	ecTRG_LEV_TYPE_INVALID
}EEcTrgLevelType;

// Axis Connection Status Detailed ID //
typedef enum{
	ecmCONN_STS_NOCONFIG	= -1, // �ش��� ��ȣ�� ���ε� Slave�� Network Configuration���� ���ǵ��� ����.
	ecmCONN_STS_DISCONN		= 0, // �ش����� ����� ������ ����
	ecmCONN_STS_INITIAL		= 1, // �ش����� ����� ����Ǿ� ������, AL State�� INITIAL �ܰ���.
	ecmCONN_STS_PREOP		= 2, // �ش����� ����� ����Ǿ� ������, AL State�� PreOP �ܰ���.
	ecmCONN_STS_BOOTSTRAP	= 3, // �ش����� ����� ����Ǿ� ������, AL State�� BOOTSTRAP �ܰ���.
	ecmCONN_STS_SAFEOP		= 4, // �ش����� ����� ����Ǿ� ������, AL State�� SafeOP �ܰ���.
	ecmCONN_STS_OP			= 8  // �ش����� ����� ����Ǿ� ������, AL State�� OP �ܰ���.
}EEcmConnStsDetail;

typedef enum{
	ecmOPMODE_CP = 8, // cyclic synchronous position mode
	ecmOPMODE_CV = 9, // cyclic synchronous velocity mode
	ecmOPMODE_CT = 10 // cyclic synchronous torque mode
}EEcmOperMode;

// Motion Property ID //
typedef enum _EEcmPropID{
	ecmMPID_EL_STOP_MODE		= 0, ///< External Limit ���� ���� ���� ���� ��� [ 0:�������(Default), 1:��������]
	ecmMPID_SWL_STOP_MODE		= 10, ///< Software Limit ���� ���� ���� ���: [ 0:�������(Default), 1:��������]
	ecmMPID_ALM_STOP_MODE		= 20, ///< Servo Alarm ���� ���� ���� ���: [ 0:�������(Default), 1:��������]
	ecmMPID_INP_ENABLE			= 30, ///< ��������̹��� INP ��ȣ�� ����� �Ϸ� ���� üũ�� �ݿ��� ������ ����. [ 0:Disable(Default), 1:Enable]
	ecmMPID_SVOFF_MOVE_MODE		= 40, ///< ���� OFF ���¿����� �̼� ��� ���(0:��� ����(���� �̼� �߿� Servo-off �Ұ�), 1:�����)
	ecmMPID_CLEAR_INITIAL_ALARM	= 50, ///< ECAT AL State�� OP���� ��ȯ�� �� 1ȸ�� ���ؼ� ���������� �˶��� Ŭ������ �������� ���� ����. [0:Disable, 1:Enable(Default)]
	ecmMPID_INPUTPDO_TYPE		= 60, ///< �� ���� InputPDO�����͸� ��������ġ���� PC������ ������ ���� ������ ������ ����.
	ecmMPID_IGNORE_ELN			= 70, ///< (-)Limit ��ȣ�� �����϶�� �ɼǿ� ���� ����. [ 0:Disable(Default), 1:Enable]
	ecmMPID_IGNORE_ELP			= 71, ///< (+)Limit ��ȣ�� �����϶�� �ɼǿ� ���� ����. [ 0:Disable(Default), 1:Enable]
	ecmMPID_ELN_INPUT_SEL		= 72, ///< (-)Limit ��ȣ�� ����� ��ȣ �Է� ���� �����ϴ� �ɼ� [ 0:NOT��ȣ(Default), 1:SIMON1/EXT1��ȣ, 2:SIMON2/EXT2��ȣ, 3:SIMON3, 4:SIMON4, 5:SIMON5] <= �ĳ��Ҵм�������̹� ����
	ecmMPID_ELP_INPUT_SEL		= 73, ///< (+)Limit ��ȣ�� ����� ��ȣ �Է� ���� �����ϴ� �ɼ� [ 0:POT��ȣ(Default), 1:SIMON1/EXT1��ȣ, 2:SIMON2/EXT2��ȣ, 3:SIMON3, 4:SIMON4, 5:SIMON5] <= �ĳ��Ҵм�������̹� ����
	ecmMPID_NULL
}EEcmMioPropId;

typedef enum _EEcmHomeOptID{
	ecmHOID_TPROB_EDGE_SEL	= 0, ///< Homming�ÿ� ���Ǵ� Touch Probe�� Trigger Edge ����.
	ecmHOID_NULL
}EEcmHomeOptID;

// Signal logic //
typedef enum{
	ecmLOGIC_A	= 0,
	ecmLOGIC_B	= 1		
}EEcmSigLogic;

// Trigger Signal Edge //
typedef enum{
	ecmEDGE_NEG	= 0, // Positive Edge
	ecmEDGE_POS	= 1	 // Negative Edge	
}EEcmSigEdge;

typedef enum{
	ecmDIR_N	= 0,
	ecmDIR_P	= 1
}EEcmDir;

// Speed Mode //
typedef enum{
	ecmSMODE_CONST	= 0,
	ecmSMODE_TRAPE	= 1,
	ecmSMODE_SCURVE = 2, 
	ecmSMODE_INVALID
}EEcmSpeedMode;

// 'SyncOther' ��忡�� Sync. Type �� ���� ID //
typedef enum{
	ecmSYNC_OTHER_START,
	ecmSYNC_OTHER_ACC_INI,
	ecmSYNC_OTHER_ACC_END,
	ecmSYNC_OTHER_DEC_INI,
	ecmSYNC_OTHER_DEC_END,
	ecmSYNC_OTHER_POSITION,
	ecmSYNC_OTHER_INVALID
}EEcmSyncOtherType;

// 'SyncOther' ��忡�� Sync. Type �� ecmSYNC_OTHER_POSITION�� ��쿡 Position�� ���ϴ� ��� ������ ���� ID //
typedef enum{
	ecmPOS_SYNC_GT, // sync���� position�� ref. position���� ū(Greater Than) ���.
	ecmPOS_SYNC_GE, // sync���� position�� ref. position���� ũ�ų� ����(Greater or Equal) ���.
	ecmPOS_SYNC_LT, // sync���� position�� ref. position���� ����(Less Than) ���.
	ecmPOS_SYNC_LE, // sync���� position�� ref. position���� ũ�ų� ����(Less or Equal) ���.
	ecmPOS_SYNC_POS_CROSS, // sync���� position�� ref. position���� ���� ������ ū ������ ���ϴ� ���.
	ecmPOS_SYNC_NEG_CROSS, // sync���� position�� ref. position���� ū ������ ���� ������ ���ϴ� ���.
	ecmPOS_SYNC_INVALID
}EEcmPosSyncMethod;

typedef enum _EEcmAutoTorqValMode{
	ecmATRQ_VAL_SINGLE, // single torque value output mode
	ecmATRQ_VAL_MULTI,  // Multiple level torque value output mode
	ecmATRQ_VAL_INVALID
}EEcmAutoTorqValMode;


typedef enum _EEcmAtrqLimMask {
	ecmATRQ_LMBIT_HIGHSPD,
	ecmATRQ_LMBIT_LOWSPD,
	ecmATRQ_LMBIT_TIME
}EEcmAtrqLimMask;

// Counter name //
typedef enum _EEcmCntr { 
	ecmCNT_COMM, /*Command*/
	ecmCNT_FEED, /*Feedback*/
	ecmCNT_COMM_RAW,
	ecmCNT_FEED_RAW
}EEcmCntr;

// Motion State //
typedef enum _EEcmMotStateId{
	ecmMST_STOP			= 0,
	ecmMST_IN_ACCEL		= 1,
	ecmMST_IN_WORKSPD	= 2,
	ecmMST_IN_DECEL		= 3,
	ecmMST_IN_INISPD	= 4,
	ecmMST_IN_WAIT_INP	= 5,
	ecmMST_IN_HOMMING	= 10,
	ecmMST_IN_SLAVE_MODE		= 13,  ///<�ش� ���� Master/Slave ����� Slave������ �����ϴ� ����
	ecmMST_IN_WAIT				= 14,
	ecmMST_IN_AUTO_TORQ_MDOE	= 15,
	ecmMST_IN_PTMOTION			= 16
}EEcmMotStateId;

#define ecmMST_IN_HOME_IDLE	ecmMST_IN_HOMMING // ecmMST_IN_HOME_IDLE�� ���� ������ ���̺귯���� ����ϴ� ����ڵ��� ���ؼ� ����� ����.

// Interpolation Map Index //
typedef enum
{
	ecmIX_MAP0			=		0,
	ecmIX_MAP1			=		1,
	ecmIX_MAP2			=		2,
	ecmIX_MAP3			=		3,
	ecmIX_MAP4			=		4,
	ecmIX_MAP5			=		5,
	ecmIX_MAP6			=		6,
	ecmIX_MAP7			=		7,
	ecmIX_MAP8			=		8,
	ecmIX_MAP9			=		9,
	ecmIX_MAP10			=		10,
	ecmIX_MAP11			=		11,
	ecmIX_MAP12			=		12,
	ecmIX_MAP13			=		13,
	ecmIX_MAP14			=		14,
	ecmIX_MAP15			=		15,
}EEcmIxMap;

// interpolation mode(using to IxMapAxes) //
typedef enum{
	ecmIX_MODE_NONE			= 0,
	ecmIX_MODE_LINEAR		= 1,
	ecmIX_MODE_CIRCULAR		= 2,
	ecmIX_MODE_HELICARL		= 3,
	ecmIX_MODE_SPLINE		= 4,
	ecmIX_MODE_MPRLin2X		= 5,
	ecmIX_MODE_MoveVia2X	= 6,
	ecmIX_MODE_INVALID
}EEcmIxMODE;


// Speed Pattern Type of Interpoation Motion //
typedef enum{
	ecmIXSPT_VECTOR,	// ���� �ӵ��� �����ϴ� ��� 
	ecmIXSPT_MASTSPD,	// ������ ���� �ӵ��� �����ϴ� ���
	ecmIXSPT_MASTRATIO,	// ������ ���� �ӵ������� �����ϴ� ���.
	ecmIXSPT_INVALID
}EEcmIxSpdPattType;

typedef enum{
	ecmARC_CW  = 0,
	ecmARC_CCW = 1
}EEcmArcDir;

typedef enum{
	ecmJSTYPE_DISABLED,
	ecmJSTYPE_USE_JERKTHRESH,
	ecmJSTYPE_INVALID
}EEcmSplJsType;

// Spline 'Jerk Smoothing Property' ID //
typedef enum{
	ecmJSPROP_DEC_TIME_ms,	// �������� �����Ҷ��� ���� �ð��� msec ������ �����Ѵ�.
	ecmJSPROP_LOWVEL_RATIO, // ���ӱ����� �ӵ��� ��ӱ��� �ӵ��� ���� ���������� �����Ѵ�.
	ecmJSPROP_LOWVEL_DUR_ms, // ���ӱ����� �����ð��� msec������ �����Ѵ�. 
	ecmJSPROP_INVALID
}EEcmSplJsPropId;

// Round Data Type ID //
typedef enum{
	ecmROUND_DT_NONE	= 0,
	ecmROUND_DT_RADIUS	= 1,
	ecmROUND_DT_OFFSET	= 2,
	ecmROUND_DT_INVALID
}EEcmRoundDataType;


typedef enum{
	ecmLM_QUEFULL_SKIP,
	ecmLM_QUEFULL_WAIT,
	ecmLM_QUEFULL_INVALID
}EEcmLmQueFullMode;

// �ϳ��� ����Ʈ��Ǹ��� ���� ���� ���� ���̵� //
/*
typedef enum{
	ecmLM_STS_DISABLED,	// List Motion ����� ��Ȱ��ȭ�� ���� (LmCtl_Begin() �����ϱ� ��)
	ecmLM_STS_PAUSED,	// List Motion ����� Ȱ��ȭ(LmCtl_Begin()) �Ǿ����� ���� Run (LmCtl_Run()) ���� ���� ����
	ecmLM_STS_RUN_IDLE,	// List Motion ����� Ȱ��ȭ(LmCtl_Begin()) �ǰ�, ����  Run (LmCtl_Run()) �� ���������� ���� ��ϵ� Ŀ�ǵ尡 �ϳ��� ���� ���.
	ecmLM_STS_RUN_BUSY,	// List Motion ����� Ȱ��ȭ(LmCtl_Begin()) �ǰ�, ����  Run (LmCtl_Run()) �� ���¿��� ��ϵ� Ŀ�ǵ尡 �����߿� �ִ� ���.
	ecmLM_STS_RUN_COMPT,// List Motion ����� Ȱ��ȭ(LmCtl_Begin()) �ǰ�, ����  Run (LmCtl_Run()) �� ���¿��� ��ϵ� Ŀ�ǵ尡 ��� ����� ���(����� ecmLM_STS_RUN_COMPT�� 1�� �̻��� ��ϵ� Ŀ�ǵ尡 �־��� ��쿡 �ش��ϸ�, ��ϵ� Ŀ�ǵ尡 �ƿ� ���� ���� ecmLM_STS_RUN_IDLE ���·� ��ȯ�ȴ�)
	ecmLM_STS_INVALID
}EEcmLmSts;
*/

// �ϳ��� ����Ʈ��Ǹ��� ���� ���� ���� ���̵� //
typedef enum{
	ecmLM_STS_DISABLED,	// List Motion ����� ��Ȱ��ȭ�� ���� (LmCtl_Begin() �����ϱ� ��)
	ecmLM_STS_PAUSED,	// List Motion ����� Ȱ��ȭ(LmCtl_Begin()) �Ǿ����� ���� Run (LmCtl_Run()) ���� ���� ����
	ecmLM_STS_RUN,		// List Motion ����� Ȱ��ȭ(LmCtl_Begin()) �ǰ�, ����  Run (LmCtl_Run()) �� ����.
	ecmLM_STS_STOPPING,
	ecmLM_STS_INVALID
}EEcmLmSts;

// ����Ʈ��� �� Ŀ�ǵ� �������� ���� ���� //
typedef enum{
	ecmLM_CMDITEM_STS_READY, 		// ���� ������� ����
	ecmLM_CMDITEM_STS_BUSY,			// ���� ����ǰ� ����.
	ecmLM_CMDITEM_STS_PAUSED,		// ���� �߿�(�Ϸ�Ǳ� ����) ������.
	ecmLM_CMDITEM_STS_COMPLETED,	// �ش� Ŀ�ǵ���  ������ �Ϸ�Ǿ���.
	ecmLM_CMDITEM_STS_INVALID
}EEcmLmCmdItemSts;

// �ϳ��� ����Ʈ��Ǹ��� ���� ���� ���� ���̵� //
typedef enum{
	ecmPTM_STS_DISABLED,	// PT-Motion ����� ��Ȱ��ȭ�� ���� (ecmPtmCtl_Begin() �����ϱ� ��)
	ecmPTM_STS_PAUSED,		// PT-Motion ����� Ȱ��ȭ(ecmPtmCtl_Begin()) �Ǿ����� Hold�� ����
	ecmPTM_STS_RUN_IDLE,	// PT-Motion ����� Ȱ��ȭ(ecmPtmCtl_Begin()) �ǰ� Hold���µ� �ƴ����� ���� ��ϵ� Ŀ�ǵ尡 �ϳ��� ���� ���.
	ecmPTM_STS_RUN_BUSY,	// PT-Motion ����� Ȱ��ȭ(ecmPtmCtl_Begin()) �ǰ� Hold���µ� �ƴ� ���¿��� ��ϵ� Ŀ�ǵ尡 �����߿� �ִ� ���.
	ecmPTM_STS_RUN_COMPT,	// PT-Motion ����� Ȱ��ȭ(ecmPtmCtl_Begin()) �ǰ� Hold���µ� �ƴ� ���¿��� ��ϵ� Ŀ�ǵ尡 ��� ����� ���(����� ecmPTM_STS_RUN_COMPT�� 1�� �̻��� ��ϵ� Ŀ�ǵ尡 �־��� ��쿡 �ش��ϸ�, ��ϵ� Ŀ�ǵ尡 �ƿ� ���� ���� ecmPTM_STS_RUN_IDLE ���·� ��ȯ�ȴ�)
	ecmPTM_STS_INVALID
}EEcmPtmSts;

typedef enum{
	ecmPTM_CMDITEM_STS_READY, 		// ���� ������� ����
	ecmPTM_CMDITEM_STS_BUSY,			// ���� ����ǰ� ����.
	ecmPTM_CMDITEM_STS_PAUSED,		// ���� �߿�(�Ϸ�Ǳ� ����) ������.
	ecmPTM_CMDITEM_STS_COMPLETED,	// �ش� Ŀ�ǵ���  ������ �Ϸ�Ǿ���.
	ecmPTM_CMDITEM_STS_INVALID
}EEcmPtmCmdItemSts;

// EEcmRingCntrDir: RingCounter ��尡 Ȱ��ȭ�� �࿡ ���ؼ� ���� ��ǥ �̼� ����� �������� �� �̼۹����� �����ϴ� Direction Mode�� ���̵� //
// ����: ��ī���� ��忡���� ������ ��ġ�� (-)�������� �̼��ؼ� ������ �ְ�, (+)�������� �̼��ؼ� �� ���� �ִ�.
typedef enum{
	ecmRING_DIR_NEG,	// (-)�������� �̼��Ͽ� ������ ���� ��ġ�� �̼�
	ecmRING_DIR_POS,	// (-)�������� �̼��Ͽ� ������ ���� ��ġ�� �̼�
	ecmRING_DIR_NEAR,	// (-)����� (+)���� �߿��� ���� ��ġ�κ��� ������ ���� ��ġ���� �Ÿ��� �� ����� �������� �̼�.
	ecmRING_DIR_FAR,	// (-)����� (+)���� �߿��� ���� ��ġ�κ��� ������ ���� ��ġ���� �Ÿ��� �� �� �������� �̼�.
	ecmRING_DIR_INVALID
}EEcmRingCntrDir;

// Position Compare ����� Counter Type //
typedef enum{
	ecmCMP_CNTR_COMM,
	ecmCMP_CNTR_FEED,
	ecmCMP_CNTR_INVALID
}EEcmCmpCntrType;

// EEcmCmpMethod: Compare Method for 'Position Compare Output' function //
typedef enum{
	ecmCMP_METH_EQ_NDIR,	// (-)���� �̼� �߿� CP == RP �϶� Compare Output Active �Ǵ� ���. ���⼭ CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_EQ_PDIR,	// (+)���� �̼� �߿� CP == RP �϶� Compare Output Active �Ǵ� ���. ���⼭ CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_EQ_BIDIR,	// ���⿡ ������� CP == RP �϶� Compare Output Active �Ǵ� ���. ���⼭ CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_LT,			// CP < RP �϶�  Compare Output Active �Ǵ� ���. ���⼭ CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_GT,			// CP > RP �϶�  Compare Output Active �Ǵ� ���. ���⼭ CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_LE,			// CP <= RP �϶�  Compare Output Active �Ǵ� ���. ���⼭ CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_GE,			// CP >= RP �϶�  Compare Output Active �Ǵ� ���. ���⼭ CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_INVALID
}EEcmCmpMethod;


// COMIZOA EtherCAT ������ ����̽� ���� //
typedef struct{
	t_ui16 ProdId; // Product ID: 0xA550, 0xA551, 0xA552
	t_ui16 PhysId; // Board�� �����Ǿ� �ִ� ����ġ�� ���ؼ� �����Ǵ� ID
	t_ui16 NumNets; // �ش� ����̽� �ϳ����� �����ϴ� ��Ʈ��ũ ���� (1 �Ǵ� 2)
	t_ui16 NetIdx; // �ش� ����̽��� ù��° ��Ʈ��ũ�� ��ü ��Ʈ��ũ ����Ʈ�󿡼� �����ϴ� ����. 
	t_ui16 FwVerMinor; // Firmware version(minor)
	t_ui16 FwVerMajor; // Firmware version(major)
}TEcDevInfo;

// File Version Info //
typedef struct{
	t_ui16 MajorVer;
	t_ui16 MinorVer;
	t_ui16 BuildNo;
	t_ui16 RevNo;
}TEcFileVersion;

// SDK(DLL) File Version & Compatibility Info //
typedef struct{
	TEcFileVersion CurVer;			// Current SDK File Version
	TEcFileVersion CompWdmVer;	// WDM driver File(.sys) Version which is Compatible with SDK DLL
	TEcFileVersion CompFwVer;	// Firmware File Version which is Compatible with SDK DLL
	t_i32 nWdmCompResult;		// WDM driver compatibity result, 'EEcVerCompatResult' enum ���� ����
	t_i32 nFwCompResult;		// Firmware compatibity result, 'EEcVerCompatResult' enum ���� ����
}TEcFileVerInfo_SDK;

// WDM(.sys) File Version & Compatibility Info //
typedef struct{
	TEcFileVersion CurVer;			// Current WDM driver File(.sys) Version
	TEcFileVersion CompSdkVer;	// SDK(DLL) File Version which is Compatible with WDM driver
	TEcFileVersion CompFwVer;	// Firmware File Version which is Compatible with WDM driver
	t_i32 nSdkCompResult;		// SDK compatibity result, 'EEcVerCompatResult' enum ���� ����
	t_i32 nFwCompResult;		// Firmware compatibity result, 'EEcVerCompatResult' enum ���� ����
}TEcFileVerInfo_WDM;

// Firmware File Version & Compatibility Info //
typedef struct{
	TEcFileVersion CurVer;			// Current WDM driver File(.sys) Version
	TEcFileVersion CompSdkVer;	// SDK(DLL) File Version which is Compatible with Firmware
	TEcFileVersion CompWdmVer;	// WDM driver File(.sys) Version which is Compatible with Firmware
	t_i32 nSdkCompResult;		// SDK(DLL) compatibity result, 'EEcVerCompatResult' enum ���� ����
	t_i32 nWdmCompResult;		// WDM driver compatibity result, 'EEcVerCompatResult' enum ���� ����
}TEcFileVerInfo_FW;


typedef struct{
	t_byte	TrgMode; // Ʈ���� ���, EEcQueCommTrgMode enum ������ ����
	t_byte	TrgLevelType; // Ʈ���� ���� Ÿ��, ETrgLevelType enum ������ ���� 
	t_word	wReseved; // reserved field (��� ����)
	t_i16	TrgDataPdoPos; // (TrgMode==TRG_MODE_PDODATA)�� ��쿡 Ʈ���� �ҽ��� ���Ǵ� �������� PDO �޸� ���� offset ��ġ  
	t_i16	TrgDataSize;
	t_ui32	TrgDataMask; // (TrgMode==TRG_MODE_PDODATA)�� ��쿡 ����ϴ� ����ŷ �����μ�, Ʈ���� �����Ϳ� �� ���� ����ŷ�Ͽ� Ʈ���� �����ͷ� ����Ѵ�. Ʈ���� �����Ͱ� Ư����Ʈ ���� ����ϴ� ��쿡 ����Ѵ�. ��, �� ���� 0�̸� ���õ�.
	t_i32	TrgRefVal; // Trigger�� �񱳴�� ���ذ�. (TrgMode==TRG_MODE_COUNT)�� ��쿡�� Count ����Ʈ������ ���ȴ�. 
	t_i32	Delay; // Ʈ���������� ������ ���Ŀ� ���� Trg ������ �ϱ� ������ ������ �ʿ��� ��쿡 �� ���� ����Ѵ�. �̶� Delay�� ������ PDO���� Ƚ���̴�.
}TEcInQueCommonTrg;

typedef struct{
	t_byte	TrgMode; // Ʈ���� ���, EEcQueTrgMode enum ������ ����
	t_byte	TrgLevelType; // Ʈ���� ���� Ÿ��, EEcTrgLevelType enum ������ ����. (TrgMode==TRG_MODE_PDODATA)�� ��쿡�� ����� 
	t_word	wReseved; // reserved field (��� ����)
	t_i16	TrgDataPdoPos; // Ʈ���� �ҽ��� ���Ǵ� �������� PDO �޸� ���� offset ��ġ. (TrgMode==TRG_MODE_PDODATA)�� ��쿡�� �����
	t_i16	TrgDataSize; // Ʈ���� �ҽ��� ���Ǵ� �������� �������� ũ�⸦ ����Ʈ ������ ��Ÿ����. (TrgMode==TRG_MODE_PDODATA)�� ��쿡�� �����
	t_ui32	TrgDataMask; // Ʈ���� �����Ϳ� �� ���� ����ŷ�Ͽ� Ʈ���� �����ͷ� ����Ѵ�. Ʈ���� �����Ͱ� Ư����Ʈ ���� ����ϴ� ��쿡 ����Ѵ�. ��, �� ���� 0�̸� ���õ�. (TrgMode==TRG_MODE_PDODATA)�� ��쿡�� �����.
	t_i32	TrgRefVal; // (TrgMode==TRG_MODE_PDODATA)�� ��� => Trigger�� �񱳴�� ���ذ�. (TrgMode==TRG_MODE_COUNT)�� ��� => �� ���� Count ����Ʈ������ ����Ѵ�. 
	t_i32	Delay; // Ʈ���������� ������ ���Ŀ� ���� Trg ������ �ϱ� ������ ������ �ʿ��� ��쿡 �� ���� ����Ѵ�. �̶� Delay�� ������ PDO���� Ƚ���̴�.
	HANDLE	CommonTrgHandle; // (TrgMode == TRG_MODE_COMMON(����Ʈ����)) �� ��쿡�� ����ϴ� ����μ�, ���� Ʈ���� ��ȣ �����ϴ� ������Ʈ�� �ڵ��� ��Ÿ����.
}TEcInQueTrgCfg;

typedef struct{
	t_ui16 PdoOfsPos; // �ش� �����Ͱ� ��ġ�ϴ� Input PDO �� �޸𸮻��� OFFSET �ּҰ� 
	t_ui16 DataSize; // �ش� �������� ũ��(����Ʈ ����);
}TEcInQueDataDesc;

// TEcSlvProdInfo: �ϳ��� �����̺꿡 ���� Product ID ���� // 
typedef struct{
	t_ui32 VendId;
	t_ui32 ProdCode;
	t_ui32 RevNo;
	t_ui32 SerNo;
}TEcSlvProdInfo;

// TEcSlvProdDesc: �ϳ��� �����̺꿡 ���� Product ������ ��Ʈ������ ��Ÿ���� ����ü // 
typedef struct{
	// buffer declaration //
	char szVendName[128];
	char szDevName[128];
	char szDevPhysics[16];
	char szGroupType[128];
	char szURL[256];
}TEcSlvProdDesc;

// TEcSlvTypeInfo: Slave ����� ��ġ ���Ŀ� ���� ������ �����ϴ� ����ü.
// ���⼭ �����ϴ� ��ġ ������ DI, DO, AI, AO, Servo-Axis �� ä�ο� ���� ������ ���Ѵ�.
typedef struct{
	// Digital Input Function Info. //
	struct{
		t_i16 StaGlobChannel; // start global channel no.
		t_ui16 NumChannels; // number of channels
	}DI;
	// Digital Output Function Info. //
	struct{	
		t_i16 StaGlobChannel; // start global channel no.
		t_ui16 NumChannels; // number of channels
	}DO;
	// Analog Input Function Info. //
	struct{
		t_i16 StaGlobChannel; // start global channel no.
		t_ui16 NumChannels; // number of channels
	}AI;
	// Analog Input Function Info. //
	struct{
		t_i16 StaGlobChannel; // start global channel no.
		t_ui16 NumChannels; // number of channels
	}AO;
	// Servo Motor Function Info. //
	struct{
		t_i16 StaAxisNo; // start global channel no.
		t_ui16 NumAxes; // number of channels
	}Servo;
	// Reserved for future //
	t_byte Resv[64];
}TEcSlvTypeInfo;


// Homing ���� ���� �÷��׵� //
typedef union{
	struct{
		t_ui16 Moving:			1; // 1 - ���Ͱ� ���������� ��Ÿ��. ��, �� �÷��״� Homing�� �ƴ� �ٸ� �Ϲ� �̼����϶��� 1�� �ȴ�.
		t_ui16 HomeBusy:		1; // 1 - Homing ������ ���������� ��Ÿ��. 
		t_ui16 HomeAttained:	1; // 1 - Homing ������ ���������� �Ϸ�Ǿ����� ��Ÿ��.
		t_ui16 HomingError:		1; // 1 - Homing ���� �߿� ������ �߻���.
		t_ui16 Resv:			12;
	}bit;
	t_ui16 word;
}TEcmHomeSt_Flags;

// List-Motion Running Status //
typedef struct{
	t_i32	RunSts;
	t_ui32	RemStepCount;
	t_ui32	RunStepCount;
	t_i32	RunStepId;
	t_dword	RunStepParam1;
	t_dword	RunStepParam2;
}TEcLmRunStsAll;

#pragma pack(push, 1)

// Motion InputPDO Group�� ��� ����: �� ������ Motion InputPDO ������ ��ó���� �ڸ��ϰ� �ȴ�. //
typedef struct{
	t_byte byReserved[4];
}TEcmInPDO_Header;

/////////////////////////////////////////////////////////////////////////////////////
// Flag: Bit ���� Axis�� ���¸� �˸��� �÷��׵��� ����
typedef union{
	struct{
		t_ui16 RdyToSwOn: 		1; // Ready To Switch On/Off
		t_ui16 SwOn: 			1; // Switch On
		t_ui16 OperEnabled:		1; // Operation Enabled (Servo-on/off �� �ش���)
		t_ui16 ServoFault:		1; // Servo driver�� Fault �߻� ����
		t_ui16 VoltEnabled:		1; // Voltage Enabled
		t_ui16 QuickStop: 		1; // Quick Stop
		t_ui16 SwOnDisabled:	1; // Switch On Disabled
		t_ui16 ServoWarn:		1; // Servo driver�� Warning �߻� ����
		t_ui16 CtlrFault:		1; // Controller�� Fault �߻� ����
		t_ui16 HomeError:		1; // Homing�߿� ������ �߻��ߴ����� �˸��� �÷���. ����: �� ��Ʈ�� ���� "Remote Mode" ��Ʈ �����̴�. �׸��� 	HomeError�� OMS2�� �����Ͽ��� �Ѵ�. ������ OMS2�� OPMODE�� �ٲ�� ������ �� �����Ƿ�...
		t_ui16 OMS1:			1; // Target reached
		t_ui16 IntLimit:		1; // Internal Limit Active (POT/NOT, Software Limit, Torque Limit, ...
		t_ui16 OMS2:			2; // Operation Mode Specific 2
		t_ui16 HomeBusy:		1; // Homing operation�� ����������
		t_ui16 HomeAttained:	1; // Homing operation�� ���������� �Ϸ�Ǿ�����...
	}bit;
	t_ui16 word;
}TEcmSxSt_Flags;

// Digital Input of an Axis //
typedef union{
	struct{
		t_ui16 NOT:		1;
		t_ui16 POT: 	1;
		t_ui16 HOME:	1;
		t_ui16 INP:		1;
		t_ui16 b4:		1; // SI-MON1/EXT1
		t_ui16 b5:		1; // SI-MON2/EXT2
		t_ui16 b6:		1; // SI-MON3
		t_ui16 b7:		1; // SI-MON4
		t_ui16 b8:		1; // SI-MON5/E-STOP
		t_ui16 b9:		1; // Reserved
		t_ui16 b10:		1; // Reserved
		t_ui16 b11:		1; // Reserved
		t_ui16 b12:		1; // Reserved
		t_ui16 b13:		1; // Reserved
		t_ui16 b14:		1; // Reserved
		t_ui16 EMG:		1; // Reserved
	}bit;
	t_ui16 word;
}TEcmSxSt_DI;


// Motion InputPDO���� �ϳ��� Axis�� Status ���� ����ü ���� 0 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion�� ���¸� �˸��� �ڵ尪. �� ���� Mio�� �Ϻ� ��Ʈ ���¿� ���� �� �ǹ̰� �޶�����.
	// 1) Mio.ServoFault=1�� ���: Servo driver�� �˶��ڵ带 ��Ÿ����.
	// 2) Mio.CtlrFault=1�ΰ��: Controller�� �˶��ڵ带 ��Ÿ����.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 �� ���: Motion Control ���� ���� �ڵ�
	// ����) Mio.ServoFault �� Mio.CtlrFault �� �Ѵ� 1�� ��쿡�� Servo driver�� �˶��ڵ带 ��Ÿ����.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position ���� //
	t_i32 CmdCnt, FeedCnt;
	// �ӵ� ���� //
	t_i32 CmdVel, FeedVel;	
}TEcmInPDO_AxisType0;

// Motion InputPDO���� �ϳ��� Axis�� Status ���� ����ü ���� 1 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion�� ���¸� �˸��� �ڵ尪. �� ���� Mio�� �Ϻ� ��Ʈ ���¿� ���� �� �ǹ̰� �޶�����.
	// 1) Mio.ServoFault=1�� ���: Servo driver�� �˶��ڵ带 ��Ÿ����.
	// 2) Mio.CtlrFault=1�ΰ��: Controller�� �˶��ڵ带 ��Ÿ����.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 �� ���: Motion Control ���� ���� �ڵ�
	// ����) Mio.ServoFault �� Mio.CtlrFault �� �Ѵ� 1�� ��쿡�� Servo driver�� �˶��ڵ带 ��Ÿ����.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position ���� //
	t_i32 CmdCnt, FeedCnt;
	// �ӵ� ���� //
	t_i32 CmdVel, FeedVel;	
	t_i16 Torque;
}TEcmInPDO_AxisType1;

// Motion InputPDO���� �ϳ��� Axis�� Status ���� ����ü ���� 2 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion�� ���¸� �˸��� �ڵ尪. �� ���� Mio�� �Ϻ� ��Ʈ ���¿� ���� �� �ǹ̰� �޶�����.
	// 1) Mio.ServoFault=1�� ���: Servo driver�� �˶��ڵ带 ��Ÿ����.
	// 2) Mio.CtlrFault=1�ΰ��: Controller�� �˶��ڵ带 ��Ÿ����.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 �� ���: Motion Control ���� ���� �ڵ�
	// ����) Mio.ServoFault �� Mio.CtlrFault �� �Ѵ� 1�� ��쿡�� Servo driver�� �˶��ڵ带 ��Ÿ����.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position ���� //
	t_i32 CmdCnt, FeedCnt;
	// �ӵ� ���� //
	t_i32 CmdVel, FeedVel;	
	t_i16 Torque;
	t_i32 TouchProbPos1;
}TEcmInPDO_AxisType2;

// Motion InputPDO���� �ϳ��� Axis�� Status ���� ����ü ���� 3 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion�� ���¸� �˸��� �ڵ尪. �� ���� Mio�� �Ϻ� ��Ʈ ���¿� ���� �� �ǹ̰� �޶�����.
	// 1) Mio.ServoFault=1�� ���: Servo driver�� �˶��ڵ带 ��Ÿ����.
	// 2) Mio.CtlrFault=1�ΰ��: Controller�� �˶��ڵ带 ��Ÿ����.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 �� ���: Motion Control ���� ���� �ڵ�
	// ����) Mio.ServoFault �� Mio.CtlrFault �� �Ѵ� 1�� ��쿡�� Servo driver�� �˶��ڵ带 ��Ÿ����.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position ���� //
	t_i32 CmdCnt, FeedCnt;
	// �ӵ� ���� //
	t_i32 CmdVel, FeedVel;	
	t_i16 Torque;
	t_i32 TouchProbPos1;
}TEcmInPDO_AxisType3;

#define TEcAxisInPDO_PCI0 TEcmInPDO_AxisType0
#define TEcAxisInPDO_PCI1 TEcmInPDO_AxisType1
#define TEcAxisInPDO_PCI2 TEcmInPDO_AxisType2
#define TEcAxisInPDO_PCI3 TEcmInPDO_AxisType3

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SW EtherCAT Master Only
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum{
	ecENG_MODE_FREERUN	 = 0,
	ecENG_MODE_RUNTIME	 = 1
}EEcEngineMode;

typedef enum{
	ecENG_AUTORECOV_DISABLE	 = 0,
	ecENG_AUTORECOV_ENABLE	 = 1
}EEcEngineAutoRecovery;

typedef enum{
	ecENG_DCCONFIG_DISABLE	 = 0,
	ecENG_DCCONFIG_ENABLE	 = 1
}EEcEngineDCConfig;

// Slave Product Information Extension(SW Master)
typedef struct
{		
	t_ui32	AxisType;				// 0-Axis�� �ƴ�, 1-servo driver, 2-step driver, 3-pusle motion, 4-other
	t_ui16	state;					// state of slave
	t_ui16	ALstatuscode;			// AL status code
	t_ui16	configadr;				// Configured address
	t_ui16	aliasadr;				// Alias address
	t_ui32	eep_man;				// Manufacturer from EEprom
	t_ui32	eep_id;					// ID from EEprom
	t_ui32	eep_rev;				// revision from EEprom
	t_ui16	Itype;					// Interface type
	t_ui16	Dtype;					// Device type
	t_ui8	islost;					// TRUE is slave is not responding at all
	char	name[EC_MAXNAME + 1];	// readable name
} TEcSlvProdInfoEx;

// Slave State Information(SW Master)
typedef struct
{	
	t_ui16	state;					// state of slave
	t_ui16	ALstatuscode;			// AL status code
	t_ui8	islost;					// TRUE is slave is not responding at all
} TEcSlvState;

/////////////////////////////////////////////////////////////////////////////////////
// Flag: Bit ���� Engine ���� �÷��׵��� ����(SW Master)
typedef union{
	struct{
		t_ui16 RunMode: 		1;	// 0 : Free Run Mode , 1 : Runtime Mode
		t_ui16 AutoRecovery:	1;	// 0 : Auto Recovery Disable, 1 : Auto Recovery Enable 
		t_ui16 DcConfig:		1;	// 0 : Dc Config Disable, 1 : DC Config Enable 
		t_ui16 Reserved:		13;	// Reserved
	}bit;
	t_ui16 word;
} TEcSwEngine_Flags;

#pragma pack(pop)

#endif