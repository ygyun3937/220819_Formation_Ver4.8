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
#define ecMAX_NUM_AXES_PER_NET			64  // 하나의 Network에서 지원하는 최대 모션제어 축 수 
#define ecMAX_NUM_IX_AXES				32  // 하나의 보간맵에 맵핑할 수 있는 최대 축 수 
#define ecMAX_NUM_SMMU					8
#define ecMAX_NUM_FMMU					4
#define ecMAX_NUM_PDO_UNIT				4 // OutPDO / InPDO 각각으로 할당되는 SyncManager의 최대갯수 (In / Out 각각에 할당될 수 있는 최대 갯 수임)
#define ecMAX_NUM_PDO_ASSIGN			8
#define ecMAX_NUM_InQUE_DATA_DESC		32 ///< 하나의 InQUE채널에 할당할 수 있는 데이터 종류의 최대 갯수
#define ecmMAX_NUM_MOT_IX_MAP			32 ///< 최대 정의 가능한 보간 맵 갯수
#define ecmMAX_NUM_LIST_MOT_MAP			8
#define ecmMAX_NUM_PT_MOT_MAP			8
#define ecmMAX_NUM_AXES_PER_NET			ecMAX_NUM_AXES_PER_NET
#define ecmMAX_NUM_IX_AXES				ecMAX_NUM_IX_AXES // 하나의 보간맵에 맵핑할 수 있는 최대 축 수 
#define ecmMAX_NUM_PTM_AXES				8	// 하나의 PT-Motion 맵에 맵핑할 수 있는 최대 축 수
#define ecmMAX_NUM_SPLINE_AXES			8 
#define ecmMAX_NUM_SPLINE_OBJ			100
#define ecmMAX_NUM_SPLINE_INPUT_POINTS	10000 
#define ecmMAX_NUM_ATRQ_MULTIVALS		1000  // 최대 설정 가능한 AutoTorq 모드의 Multi-Value Items 갯수 
#define ecmMAX_POSCORR_TABLE_SIZE		10000 ///< 각 축별로 할당할 수 있는 Position Correction Table의 최대 크기 
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
	ecVER_MISMATCH_HIGHER	= -2,	// 해당 파일의 버전이 기준이 되는 파일과 호환되지 않는 상위 버전임을 나타냄
	ecVER_MISMATCH_LOWER	= -1,	// 해당 파일의 버전이 기준이 되는 파일과 호환되지 않는 하위 버전임을 나타냄
	ecVER_NOT_FOUND			= 0,	// 해당 파일의 버전을 찾을 수 없음(파일이 없거나, 버전 정보를 얻을 수 없는 경우)
	ecVER_MATCH				= 1,	// 해당 파일의 버전이 기준이 되는 파일과 호환이 되는 버전임을 나타냄 
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

// OutPDO 데이터의 초기값 셋팅 모드 (OP모드로 전환될 때 셋팅되는 초기값) //
typedef enum{	
	ecOPDO_INIT_KEEPLAST,	// 마지막의 출력값을 그대로 유지한다.
	ecOPDO_INIT_ZERO,		// 모든 값을 0으로 셋팅한다.
	ecOPDO_INIT_USERDEF,	// 사용자가 미리 정의한 초기값을 적용한다.
	ecOPDO_INIT_INVALID
}EEcOPDOInitMode;

// Debug Logging Type //
typedef enum{
	ecDLOG_TYPE_FILE		= 0, // Text file에 로그 결과를 기록하는 모드 
	ecDLOG_TYPE_TRACE		= 1, // Debug 스트링 뷰어를 통해서 로그 결과를 보여 주는 모드 
	ecDLOG_TYPE_COMITOOL	= 2, // COMIZOA에서 제작한 디버깅 툴을 사용하여 로그 결과를 보여주는 모드 (현재 지원되지 않으며 항후 지원 예정)
	ecDLOG_TYPE_INVALID
}EEcDlogType;

// Debug Logging Level //
typedef enum{
	ecDLOG_LEVEL_DISALBE= 0, // No debug logging
	ecDLOG_LEVEL_ERR	= 1, // 에러 발생했을 때만 로깅 
	ecDLOG_LEVEL_CMD	= 2, // ecDLOG_LEVEL_ERR레벨을 포함하며 더해서 Set 함수 및 각종 커맨드 함수 로깅 
	ecDLOG_LEVEL_GET	= 3, // ecDLOG_LEVEL_CMD레벨을 포함하며 더해서 Get 함수도 로깅 (단 일부 상태 체크 함수들은 제외)
	ecDLOG_LEVEL_ALL	= 4, // 모든 함수에 대해서 로깅 수행 
	ecDLOG_LEVEL_INVALID
}EEcDlogLevel;

// Debug Logging Level //
typedef enum{
	ecDLOG_ERR_LOG_NONE	= 0, // 에러가 발생하더라도 로깅을 남기지 않는다. 
	ecDLOG_ERR_LOG_CMD	= 1, // Command 함수에서만 에러가 발생했을 때 로깅을 남긴다.
	ecDLOG_ERR_LOG_GET	= 2	 // 모든 함수에 대해서 에러가 발생했을 때 로깅을 남긴다.
}EEcDlogErrLogType;


// InPDOQue 기능에서 Queueing start/stop trigger 모드 종류에 대한 아이디값 //
typedef enum{
	ecTRG_MODE_NONE, // 트리거모드 사용 안함.
	ecTRG_MODE_COMMON, // 모든 채널이 공통 트리거 소스를 사용함. 이 때에는 TInPDOQueChannel::Cfg.StaTrg.pCommonTrgSig 포인터가 가리키는 Signal 변수의 값이 1이 되면 
	ecTRG_MODE_COUNT, // InputPDO 데이터 전송 이벤트 카운트를 이용해서 트리거함. 이 때에 Count 조건 값은 TInPDOQueChannel::Cfg.StaTrg.TrgRefVal 멤버 변수를 통해서 설정한다. 
	ecTRG_MODE_PDODATA, // 특정 PDO 데이터의 값을 체크하여 트리거 조건을 결정하는 모드. 
	ecTRG_MODE_INVALID
}EEcQueTrgMode;


typedef enum{
	ecCOMTRG_MODE_NONE, // 트리거모드 사용 안함.
	ecCOMTRG_MODE_MANUAL, // Manual trigger 모드 
	ecCOMTRG_MODE_COUNT, // InputPDO 데이터 전송 이벤트 카운트를 이용해서 트리거함. 이 때에 Count 조건 값은 TInPDOQueChannel::Cfg.StaTrg.TrgRefVal 멤버 변수를 통해서 설정한다. 
	ecCOMTRG_MODE_PDODATA, // 특정 PDO 데이터의 값을 체크하여 트리거 조건을 결정하는 모드. 
	ecCOMTRG_MODE_INVALID
}EEcQueCommTrgMode;

// InPDOQue 기능에서 Start/Trigger 사용할 때 데이터를 비교하는 방법의 종류에 대한 아이디값 //
typedef enum{
	ecTRG_LEV_TYPE_EQ, // 비교데이터가 Ref 데이터와 같으면(Equal) 트리거 셋
	ecTRG_LEV_TYPE_GT, // 비교데이터가 Ref 데이터보다 크면(Greater than) 트리거 셋
	ecTRG_LEV_TYPE_GE, // 비교데이터가 Ref 데이터보다 크거나 같으면 트리거 셋
	ecTRG_LEV_TYPE_LT, // 비교데이터가 Ref 데이터보다 작으면(Less than) 트리거 셋
	ecTRG_LEV_TYPE_LE, // 비교데이터가 Ref 데이터보다 작거나 같으면 트리거 셋
	ecTRG_LEV_TYPE_RE, // 비교데이터가 Ref 데이터를 기준으로 상승에지(Rising edge, 비교데이터의 한 샘플 전/후 값이 Ref값 보다 작은 상태에서 큰 상태로 변화한 경우)가 발생하면 트리거 셋
	ecTRG_LEV_TYPE_FE, // 비교데이터가 Ref 데이터를 기준으로 하강에지(Rising edge, 비교데이터의 한 샘플 전/후 값이 Ref값 보다 큰 상태에서 작은 상태로 변화한 경우)가 발생하면 트리거 셋
	ecTRG_LEV_TYPE_INVALID
}EEcTrgLevelType;

// Axis Connection Status Detailed ID //
typedef enum{
	ecmCONN_STS_NOCONFIG	= -1, // 해당축 번호로 맵핑된 Slave가 Network Configuration에서 정의되지 않음.
	ecmCONN_STS_DISCONN		= 0, // 해당축의 통신이 끊어진 상태
	ecmCONN_STS_INITIAL		= 1, // 해당축의 통신이 연결되어 있으며, AL State가 INITIAL 단계임.
	ecmCONN_STS_PREOP		= 2, // 해당축의 통신이 연결되어 있으며, AL State가 PreOP 단계임.
	ecmCONN_STS_BOOTSTRAP	= 3, // 해당축의 통신이 연결되어 있으며, AL State가 BOOTSTRAP 단계임.
	ecmCONN_STS_SAFEOP		= 4, // 해당축의 통신이 연결되어 있으며, AL State가 SafeOP 단계임.
	ecmCONN_STS_OP			= 8  // 해당축의 통신이 연결되어 있으며, AL State가 OP 단계임.
}EEcmConnStsDetail;

typedef enum{
	ecmOPMODE_CP = 8, // cyclic synchronous position mode
	ecmOPMODE_CV = 9, // cyclic synchronous velocity mode
	ecmOPMODE_CT = 10 // cyclic synchronous torque mode
}EEcmOperMode;

// Motion Property ID //
typedef enum _EEcmPropID{
	ecmMPID_EL_STOP_MODE		= 0, ///< External Limit 센서 감지 시의 정지 모드 [ 0:즉시정지(Default), 1:감속정지]
	ecmMPID_SWL_STOP_MODE		= 10, ///< Software Limit 감지 시의 정지 모드: [ 0:즉시정지(Default), 1:감속정지]
	ecmMPID_ALM_STOP_MODE		= 20, ///< Servo Alarm 감지 시의 정지 모드: [ 0:즉시정지(Default), 1:감속정지]
	ecmMPID_INP_ENABLE			= 30, ///< 서보드라이버의 INP 신호의 모션의 완료 상태 체크에 반영할 것인지 설정. [ 0:Disable(Default), 1:Enable]
	ecmMPID_SVOFF_MOVE_MODE		= 40, ///< 서보 OFF 상태에서의 이송 허용 모드(0:허용 안함(또한 이송 중에 Servo-off 불가), 1:허용함)
	ecmMPID_CLEAR_INITIAL_ALARM	= 50, ///< ECAT AL State가 OP모드로 전환될 때 1회에 한해서 서보앰프의 알람을 클리어할 것인지에 대한 설정. [0:Disable, 1:Enable(Default)]
	ecmMPID_INPUTPDO_TYPE		= 60, ///< 각 축의 InputPDO데이터를 마스터장치에서 PC측으로 전달할 때의 데이터 구조를 결정.
	ecmMPID_IGNORE_ELN			= 70, ///< (-)Limit 신호를 무시하라는 옵션에 대한 설정. [ 0:Disable(Default), 1:Enable]
	ecmMPID_IGNORE_ELP			= 71, ///< (+)Limit 신호를 무시하라는 옵션에 대한 설정. [ 0:Disable(Default), 1:Enable]
	ecmMPID_ELN_INPUT_SEL		= 72, ///< (-)Limit 신호로 사용할 신호 입력 핀을 설정하는 옵션 [ 0:NOT신호(Default), 1:SIMON1/EXT1신호, 2:SIMON2/EXT2신호, 3:SIMON3, 4:SIMON4, 5:SIMON5] <= 파나소닉서보드라이버 기준
	ecmMPID_ELP_INPUT_SEL		= 73, ///< (+)Limit 신호로 사용할 신호 입력 핀을 설정하는 옵션 [ 0:POT신호(Default), 1:SIMON1/EXT1신호, 2:SIMON2/EXT2신호, 3:SIMON3, 4:SIMON4, 5:SIMON5] <= 파나소닉서보드라이버 기준
	ecmMPID_NULL
}EEcmMioPropId;

typedef enum _EEcmHomeOptID{
	ecmHOID_TPROB_EDGE_SEL	= 0, ///< Homming시에 사용되는 Touch Probe의 Trigger Edge 설정.
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

// 'SyncOther' 모드에서 Sync. Type 에 대한 ID //
typedef enum{
	ecmSYNC_OTHER_START,
	ecmSYNC_OTHER_ACC_INI,
	ecmSYNC_OTHER_ACC_END,
	ecmSYNC_OTHER_DEC_INI,
	ecmSYNC_OTHER_DEC_END,
	ecmSYNC_OTHER_POSITION,
	ecmSYNC_OTHER_INVALID
}EEcmSyncOtherType;

// 'SyncOther' 모드에서 Sync. Type 이 ecmSYNC_OTHER_POSITION인 경우에 Position을 비교하는 방법 종류에 대한 ID //
typedef enum{
	ecmPOS_SYNC_GT, // sync축의 position이 ref. position보다 큰(Greater Than) 경우.
	ecmPOS_SYNC_GE, // sync축의 position이 ref. position보다 크거나 같은(Greater or Equal) 경우.
	ecmPOS_SYNC_LT, // sync축의 position이 ref. position보다 작은(Less Than) 경우.
	ecmPOS_SYNC_LE, // sync축의 position이 ref. position보다 크거나 같은(Less or Equal) 경우.
	ecmPOS_SYNC_POS_CROSS, // sync축의 position이 ref. position보다 작은 값에서 큰 값으로 변하는 경우.
	ecmPOS_SYNC_NEG_CROSS, // sync축의 position이 ref. position보다 큰 값에서 작은 값으로 변하는 경우.
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
	ecmMST_IN_SLAVE_MODE		= 13,  ///<해당 축이 Master/Slave 기능의 Slave축으로 동작하는 상태
	ecmMST_IN_WAIT				= 14,
	ecmMST_IN_AUTO_TORQ_MDOE	= 15,
	ecmMST_IN_PTMOTION			= 16
}EEcmMotStateId;

#define ecmMST_IN_HOME_IDLE	ecmMST_IN_HOMMING // ecmMST_IN_HOME_IDLE은 이전 버전의 라이브러리를 사용하는 사용자들을 위해서 선언된 것임.

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
	ecmIXSPT_VECTOR,	// 벡터 속도를 설정하는 방식 
	ecmIXSPT_MASTSPD,	// 마스터 축의 속도를 설정하는 방식
	ecmIXSPT_MASTRATIO,	// 마스터 축의 속도비율을 설정하는 방식.
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
	ecmJSPROP_DEC_TIME_ms,	// 저속으로 감속할때의 감속 시간을 msec 단위로 설정한다.
	ecmJSPROP_LOWVEL_RATIO, // 저속구간의 속도를 고속구간 속도에 대한 비율값으로 설정한다.
	ecmJSPROP_LOWVEL_DUR_ms, // 저속구간의 유지시간을 msec단위로 설정한다. 
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

// 하나의 리스트모션맵의 현재 동작 상태 아이디 //
/*
typedef enum{
	ecmLM_STS_DISABLED,	// List Motion 기능이 비활성화된 상태 (LmCtl_Begin() 실행하기 전)
	ecmLM_STS_PAUSED,	// List Motion 기능이 활성화(LmCtl_Begin()) 되었지만 아직 Run (LmCtl_Run()) 되지 않은 상태
	ecmLM_STS_RUN_IDLE,	// List Motion 기능이 활성화(LmCtl_Begin()) 되고, 또한  Run (LmCtl_Run()) 된 상태이지만 아직 등록된 커맨드가 하나도 없는 경우.
	ecmLM_STS_RUN_BUSY,	// List Motion 기능이 활성화(LmCtl_Begin()) 되고, 또한  Run (LmCtl_Run()) 된 상태에서 등록된 커맨드가 실행중에 있는 경우.
	ecmLM_STS_RUN_COMPT,// List Motion 기능이 활성화(LmCtl_Begin()) 되고, 또한  Run (LmCtl_Run()) 된 상태에서 등록된 커맨드가 모두 실행된 경우(참고로 ecmLM_STS_RUN_COMPT은 1개 이상의 등록된 커맨드가 있었던 경우에 해당하며, 등록된 커맨드가 아예 없는 경우는 ecmLM_STS_RUN_IDLE 상태로 반환된다)
	ecmLM_STS_INVALID
}EEcmLmSts;
*/

// 하나의 리스트모션맵의 현재 동작 상태 아이디 //
typedef enum{
	ecmLM_STS_DISABLED,	// List Motion 기능이 비활성화된 상태 (LmCtl_Begin() 실행하기 전)
	ecmLM_STS_PAUSED,	// List Motion 기능이 활성화(LmCtl_Begin()) 되었지만 아직 Run (LmCtl_Run()) 되지 않은 상태
	ecmLM_STS_RUN,		// List Motion 기능이 활성화(LmCtl_Begin()) 되고, 또한  Run (LmCtl_Run()) 된 상태.
	ecmLM_STS_STOPPING,
	ecmLM_STS_INVALID
}EEcmLmSts;

// 리스트모션 각 커맨드 아이템의 실행 상태 //
typedef enum{
	ecmLM_CMDITEM_STS_READY, 		// 아직 실행되지 않음
	ecmLM_CMDITEM_STS_BUSY,			// 현재 실행되고 있음.
	ecmLM_CMDITEM_STS_PAUSED,		// 실행 중에(완료되기 전에) 중지됨.
	ecmLM_CMDITEM_STS_COMPLETED,	// 해당 커맨드의  실행이 완료되었음.
	ecmLM_CMDITEM_STS_INVALID
}EEcmLmCmdItemSts;

// 하나의 리스트모션맵의 현재 동작 상태 아이디 //
typedef enum{
	ecmPTM_STS_DISABLED,	// PT-Motion 기능이 비활성화된 상태 (ecmPtmCtl_Begin() 실행하기 전)
	ecmPTM_STS_PAUSED,		// PT-Motion 기능이 활성화(ecmPtmCtl_Begin()) 되었지만 Hold된 상태
	ecmPTM_STS_RUN_IDLE,	// PT-Motion 기능이 활성화(ecmPtmCtl_Begin()) 되고 Hold상태도 아니지만 아직 등록된 커맨드가 하나도 없는 경우.
	ecmPTM_STS_RUN_BUSY,	// PT-Motion 기능이 활성화(ecmPtmCtl_Begin()) 되고 Hold상태도 아닌 상태에서 등록된 커맨드가 실행중에 있는 경우.
	ecmPTM_STS_RUN_COMPT,	// PT-Motion 기능이 활성화(ecmPtmCtl_Begin()) 되고 Hold상태도 아닌 상태에서 등록된 커맨드가 모두 실행된 경우(참고로 ecmPTM_STS_RUN_COMPT은 1개 이상의 등록된 커맨드가 있었던 경우에 해당하며, 등록된 커맨드가 아예 없는 경우는 ecmPTM_STS_RUN_IDLE 상태로 반환된다)
	ecmPTM_STS_INVALID
}EEcmPtmSts;

typedef enum{
	ecmPTM_CMDITEM_STS_READY, 		// 아직 실행되지 않음
	ecmPTM_CMDITEM_STS_BUSY,			// 현재 실행되고 있음.
	ecmPTM_CMDITEM_STS_PAUSED,		// 실행 중에(완료되기 전에) 중지됨.
	ecmPTM_CMDITEM_STS_COMPLETED,	// 해당 커맨드의  실행이 완료되었음.
	ecmPTM_CMDITEM_STS_INVALID
}EEcmPtmCmdItemSts;

// EEcmRingCntrDir: RingCounter 모드가 활성화된 축에 대해서 절대 좌표 이송 명령이 내려졌을 때 이송방향을 결정하는 Direction Mode의 아이디값 //
// 참고: 링카운터 모드에서는 지정한 위치를 (-)방향으로 이송해서 갈수도 있고, (+)방향으로 이송해서 갈 수도 있다.
typedef enum{
	ecmRING_DIR_NEG,	// (-)방향으로 이송하여 지정한 절대 위치로 이송
	ecmRING_DIR_POS,	// (-)방향으로 이송하여 지정한 절대 위치로 이송
	ecmRING_DIR_NEAR,	// (-)방향과 (+)방향 중에서 현재 위치로부터 지정한 절대 위치까지 거리가 더 가까운 방향으로 이송.
	ecmRING_DIR_FAR,	// (-)방향과 (+)방향 중에서 현재 위치로부터 지정한 절대 위치까지 거리가 더 먼 방향으로 이송.
	ecmRING_DIR_INVALID
}EEcmRingCntrDir;

// Position Compare 기능의 Counter Type //
typedef enum{
	ecmCMP_CNTR_COMM,
	ecmCMP_CNTR_FEED,
	ecmCMP_CNTR_INVALID
}EEcmCmpCntrType;

// EEcmCmpMethod: Compare Method for 'Position Compare Output' function //
typedef enum{
	ecmCMP_METH_EQ_NDIR,	// (-)방향 이송 중에 CP == RP 일때 Compare Output Active 되는 모드. 여기서 CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_EQ_PDIR,	// (+)방향 이송 중에 CP == RP 일때 Compare Output Active 되는 모드. 여기서 CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_EQ_BIDIR,	// 방향에 상관없이 CP == RP 일때 Compare Output Active 되는 모드. 여기서 CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_LT,			// CP < RP 일때  Compare Output Active 되는 모드. 여기서 CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_GT,			// CP > RP 일때  Compare Output Active 되는 모드. 여기서 CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_LE,			// CP <= RP 일때  Compare Output Active 되는 모드. 여기서 CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_GE,			// CP >= RP 일때  Compare Output Active 되는 모드. 여기서 CP:Current Position,  RP:Reference Position.
	ecmCMP_METH_INVALID
}EEcmCmpMethod;


// COMIZOA EtherCAT 마스터 디바이스 정보 //
typedef struct{
	t_ui16 ProdId; // Product ID: 0xA550, 0xA551, 0xA552
	t_ui16 PhysId; // Board에 장착되어 있는 스위치에 의해서 결정되는 ID
	t_ui16 NumNets; // 해당 디바이스 하나에서 제공하는 네트워크 갯수 (1 또는 2)
	t_ui16 NetIdx; // 해당 디바이스의 첫번째 네트워크가 전체 네트워크 리스트상에서 차지하는 순서. 
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
	t_i32 nWdmCompResult;		// WDM driver compatibity result, 'EEcVerCompatResult' enum 선언 참조
	t_i32 nFwCompResult;		// Firmware compatibity result, 'EEcVerCompatResult' enum 선언 참조
}TEcFileVerInfo_SDK;

// WDM(.sys) File Version & Compatibility Info //
typedef struct{
	TEcFileVersion CurVer;			// Current WDM driver File(.sys) Version
	TEcFileVersion CompSdkVer;	// SDK(DLL) File Version which is Compatible with WDM driver
	TEcFileVersion CompFwVer;	// Firmware File Version which is Compatible with WDM driver
	t_i32 nSdkCompResult;		// SDK compatibity result, 'EEcVerCompatResult' enum 선언 참조
	t_i32 nFwCompResult;		// Firmware compatibity result, 'EEcVerCompatResult' enum 선언 참조
}TEcFileVerInfo_WDM;

// Firmware File Version & Compatibility Info //
typedef struct{
	TEcFileVersion CurVer;			// Current WDM driver File(.sys) Version
	TEcFileVersion CompSdkVer;	// SDK(DLL) File Version which is Compatible with Firmware
	TEcFileVersion CompWdmVer;	// WDM driver File(.sys) Version which is Compatible with Firmware
	t_i32 nSdkCompResult;		// SDK(DLL) compatibity result, 'EEcVerCompatResult' enum 선언 참조
	t_i32 nWdmCompResult;		// WDM driver compatibity result, 'EEcVerCompatResult' enum 선언 참조
}TEcFileVerInfo_FW;


typedef struct{
	t_byte	TrgMode; // 트리거 모드, EEcQueCommTrgMode enum 데이터 참고
	t_byte	TrgLevelType; // 트리거 레벨 타입, ETrgLevelType enum 데이터 참고 
	t_word	wReseved; // reserved field (사용 안함)
	t_i16	TrgDataPdoPos; // (TrgMode==TRG_MODE_PDODATA)인 경우에 트리거 소스로 사용되는 데이터의 PDO 메모리 상의 offset 위치  
	t_i16	TrgDataSize;
	t_ui32	TrgDataMask; // (TrgMode==TRG_MODE_PDODATA)인 경우에 사용하는 마스킹 값으로서, 트리거 데이터에 이 값을 마스킹하여 트리거 데이터로 사용한다. 트리거 데이터가 특정비트 값을 사용하는 경우에 사용한다. 단, 이 값이 0이면 무시됨.
	t_i32	TrgRefVal; // Trigger의 비교대상 기준값. (TrgMode==TRG_MODE_COUNT)인 경우에는 Count 리미트값으로 사용된다. 
	t_i32	Delay; // 트리거조건이 만족한 이후에 실제 Trg 행위를 하기 전까지 지연이 필요한 경우에 이 값을 사용한다. 이때 Delay의 단위는 PDO전송 횟수이다.
}TEcInQueCommonTrg;

typedef struct{
	t_byte	TrgMode; // 트리거 모드, EEcQueTrgMode enum 데이터 참고
	t_byte	TrgLevelType; // 트리거 레벨 타입, EEcTrgLevelType enum 데이터 참고. (TrgMode==TRG_MODE_PDODATA)인 경우에만 사용함 
	t_word	wReseved; // reserved field (사용 안함)
	t_i16	TrgDataPdoPos; // 트리거 소스로 사용되는 데이터의 PDO 메모리 상의 offset 위치. (TrgMode==TRG_MODE_PDODATA)인 경우에만 사용함
	t_i16	TrgDataSize; // 트리거 소스로 사용되는 데이터의 데이터형 크기를 바이트 단위로 나타낸다. (TrgMode==TRG_MODE_PDODATA)인 경우에만 사용함
	t_ui32	TrgDataMask; // 트리거 데이터에 이 값을 마스킹하여 트리거 데이터로 사용한다. 트리거 데이터가 특정비트 값을 사용하는 경우에 사용한다. 단, 이 값이 0이면 무시됨. (TrgMode==TRG_MODE_PDODATA)인 경우에만 사용함.
	t_i32	TrgRefVal; // (TrgMode==TRG_MODE_PDODATA)인 경우 => Trigger의 비교대상 기준값. (TrgMode==TRG_MODE_COUNT)인 경우 => 이 값을 Count 리미트값으로 사용한다. 
	t_i32	Delay; // 트리거조건이 만족한 이후에 실제 Trg 행위를 하기 전까지 지연이 필요한 경우에 이 값을 사용한다. 이때 Delay의 단위는 PDO전송 횟수이다.
	HANDLE	CommonTrgHandle; // (TrgMode == TRG_MODE_COMMON(공통트리거)) 인 경우에만 사용하는 멤버로서, 공통 트리거 신호 관리하는 오브젝트의 핸들을 나타낸다.
}TEcInQueTrgCfg;

typedef struct{
	t_ui16 PdoOfsPos; // 해당 데이터가 위치하는 Input PDO 논리 메모리상의 OFFSET 주소값 
	t_ui16 DataSize; // 해당 데이터의 크기(바이트 단위);
}TEcInQueDataDesc;

// TEcSlvProdInfo: 하나의 슬레이브에 대한 Product ID 정보 // 
typedef struct{
	t_ui32 VendId;
	t_ui32 ProdCode;
	t_ui32 RevNo;
	t_ui32 SerNo;
}TEcSlvProdInfo;

// TEcSlvProdDesc: 하나의 슬레이브에 대한 Product 정보를 스트링으로 나타내는 구조체 // 
typedef struct{
	// buffer declaration //
	char szVendName[128];
	char szDevName[128];
	char szDevPhysics[16];
	char szGroupType[128];
	char szURL[256];
}TEcSlvProdDesc;

// TEcSlvTypeInfo: Slave 모듈의 장치 형식에 대한 정보를 제공하는 구조체.
// 여기서 제공하는 장치 형식은 DI, DO, AI, AO, Servo-Axis 의 채널에 대한 정보를 말한다.
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


// Homing 관련 상태 플래그들 //
typedef union{
	struct{
		t_ui16 Moving:			1; // 1 - 모터가 구동중임을 나타냄. 단, 이 플래그는 Homing이 아닌 다른 일반 이송중일때도 1로 된다.
		t_ui16 HomeBusy:		1; // 1 - Homing 동작이 진행중임을 나타냄. 
		t_ui16 HomeAttained:	1; // 1 - Homing 동작이 성공적으로 완료되었음을 나타냄.
		t_ui16 HomingError:		1; // 1 - Homing 동작 중에 에러가 발생함.
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

// Motion InputPDO Group의 헤더 정보: 이 정보가 Motion InputPDO 영역의 맨처음에 자리하게 된다. //
typedef struct{
	t_byte byReserved[4];
}TEcmInPDO_Header;

/////////////////////////////////////////////////////////////////////////////////////
// Flag: Bit 별로 Axis의 상태를 알리는 플래그들의 집합
typedef union{
	struct{
		t_ui16 RdyToSwOn: 		1; // Ready To Switch On/Off
		t_ui16 SwOn: 			1; // Switch On
		t_ui16 OperEnabled:		1; // Operation Enabled (Servo-on/off 에 해당함)
		t_ui16 ServoFault:		1; // Servo driver의 Fault 발생 상태
		t_ui16 VoltEnabled:		1; // Voltage Enabled
		t_ui16 QuickStop: 		1; // Quick Stop
		t_ui16 SwOnDisabled:	1; // Switch On Disabled
		t_ui16 ServoWarn:		1; // Servo driver의 Warning 발생 상태
		t_ui16 CtlrFault:		1; // Controller의 Fault 발생 상태
		t_ui16 HomeError:		1; // Homing중에 에러가 발생했는지를 알리는 플래그. 참고: 이 비트는 원래 "Remote Mode" 비트 영역이다. 그리고 	HomeError는 OMS2를 참조하여야 한다. 하지만 OMS2는 OPMODE가 바뀌면 참조할 수 없으므로...
		t_ui16 OMS1:			1; // Target reached
		t_ui16 IntLimit:		1; // Internal Limit Active (POT/NOT, Software Limit, Torque Limit, ...
		t_ui16 OMS2:			2; // Operation Mode Specific 2
		t_ui16 HomeBusy:		1; // Homing operation이 진행중인지
		t_ui16 HomeAttained:	1; // Homing operation이 정상적으로 완료되었는지...
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


// Motion InputPDO에서 하나의 Axis의 Status 정보 구조체 형식 0 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion의 상태를 알리는 코드값. 이 값은 Mio의 일부 비트 상태에 따라서 그 의미가 달라진다.
	// 1) Mio.ServoFault=1인 경우: Servo driver의 알람코드를 나타낸다.
	// 2) Mio.CtlrFault=1인경우: Controller의 알람코드를 나타낸다.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 인 경우: Motion Control 동작 상태 코드
	// 참고) Mio.ServoFault 와 Mio.CtlrFault 가 둘다 1인 경우에는 Servo driver의 알람코드를 나타낸다.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position 정보 //
	t_i32 CmdCnt, FeedCnt;
	// 속도 정보 //
	t_i32 CmdVel, FeedVel;	
}TEcmInPDO_AxisType0;

// Motion InputPDO에서 하나의 Axis의 Status 정보 구조체 형식 1 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion의 상태를 알리는 코드값. 이 값은 Mio의 일부 비트 상태에 따라서 그 의미가 달라진다.
	// 1) Mio.ServoFault=1인 경우: Servo driver의 알람코드를 나타낸다.
	// 2) Mio.CtlrFault=1인경우: Controller의 알람코드를 나타낸다.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 인 경우: Motion Control 동작 상태 코드
	// 참고) Mio.ServoFault 와 Mio.CtlrFault 가 둘다 1인 경우에는 Servo driver의 알람코드를 나타낸다.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position 정보 //
	t_i32 CmdCnt, FeedCnt;
	// 속도 정보 //
	t_i32 CmdVel, FeedVel;	
	t_i16 Torque;
}TEcmInPDO_AxisType1;

// Motion InputPDO에서 하나의 Axis의 Status 정보 구조체 형식 2 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion의 상태를 알리는 코드값. 이 값은 Mio의 일부 비트 상태에 따라서 그 의미가 달라진다.
	// 1) Mio.ServoFault=1인 경우: Servo driver의 알람코드를 나타낸다.
	// 2) Mio.CtlrFault=1인경우: Controller의 알람코드를 나타낸다.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 인 경우: Motion Control 동작 상태 코드
	// 참고) Mio.ServoFault 와 Mio.CtlrFault 가 둘다 1인 경우에는 Servo driver의 알람코드를 나타낸다.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position 정보 //
	t_i32 CmdCnt, FeedCnt;
	// 속도 정보 //
	t_i32 CmdVel, FeedVel;	
	t_i16 Torque;
	t_i32 TouchProbPos1;
}TEcmInPDO_AxisType2;

// Motion InputPDO에서 하나의 Axis의 Status 정보 구조체 형식 3 //
typedef struct{
	t_ui16	PDOTypeID:		8;
	t_ui16	TouchProbSts1:	3;
	t_ui16	TouchProbSts2:	3;
	t_ui16	Resv1:			2;
	////////////////////////////////////////////////////////////////////////////////////
	// Mst: Motion의 상태를 알리는 코드값. 이 값은 Mio의 일부 비트 상태에 따라서 그 의미가 달라진다.
	// 1) Mio.ServoFault=1인 경우: Servo driver의 알람코드를 나타낸다.
	// 2) Mio.CtlrFault=1인경우: Controller의 알람코드를 나타낸다.
	// 3) Mio.ServoFault=0, Mio.CtlrFault=0 인 경우: Motion Control 동작 상태 코드
	// 참고) Mio.ServoFault 와 Mio.CtlrFault 가 둘다 1인 경우에는 Servo driver의 알람코드를 나타낸다.	
	t_i16	Mst;
	// Motion Status Flags //
	TEcmSxSt_Flags Flags;
	// Digital Input //
	TEcmSxSt_DI DI;
	// Position 정보 //
	t_i32 CmdCnt, FeedCnt;
	// 속도 정보 //
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
	t_ui32	AxisType;				// 0-Axis가 아님, 1-servo driver, 2-step driver, 3-pusle motion, 4-other
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
// Flag: Bit 별로 Engine 내부 플래그들의 집합(SW Master)
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