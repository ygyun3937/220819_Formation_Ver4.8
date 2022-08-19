#ifndef _COMIECAT_SDK_ERRCODE_H_
#define _COMIECAT_SDK_ERRCODE_H_

#define ecERR_NONE						0
#define ecERR_DEVICE_NOT_LOADED			-5  // 지정된 디바이스가 로드되지 않은 경우.
#define ecERR_WDM_VER_READ_FAIL			-6	// WDM 드라이버에서 버전 정보를 읽는데 실패한 경우.
#define ecERR_FW_VER_READ_FAIL			-7	// Firmware 버전 정보를 읽는데 실패한 경우.
#define ecERR_DEV_BOOT_TIMEOUT			-8	// Master Device가 지정된 시간 안에 부팅이 완료되지 않은 경우.
#define ecERR_DEV_BOOT_NOT_COMPT		-9  // Master Device가 아직 부트업이 완료되지 않은 경우.
#define ecERR_INVALID_BOARDID			-10
#define ecERR_INVALID_DEVIDX			-11
#define ecERR_INVALID_VERSION			-12	// SDK(DLL), WDM 드라이버, 펌웨어(Firmware) 상호간에 버전이 호환되지 않는 경우.
#define ecERR_INVALID_NETID				-20
#define ecERR_INVALID_SLAVEID			-25 // 슬레이브 인덱스 또는 슬레이브 주소가 잘못된 경우...
#define ecERR_INVALID_CHANNEL			-30 // 축번호 또는 채널번호 등이 잘못된 경우...
#define ecERR_INVALID_IXMAP_IDX			-40 // Motion 제어 기능에서 Interpolation 보간 맵 번호가 잘못된 경우...
#define ecERR_INVALID_IXMAP_AXES		-50 // IXMAP에 포함된 축의 구성이 잘못된 경우.  
#define ecERR_INVALID_LMMAP_IDX			-55	// 리스트모션 보간 맵 번호가 잘못된 경우.
#define ecERR_INVALID_FUNC_ARG			-60 // 함수의 매개변수가 유효하지 않은 값인 경우.
#define ecERR_INVALID_HANDLE			-65 // Invalid handle value가 매개변수로 전달된 경우.
#define ecERR_INVALID_RESULT_DATA		-66 // 결과 데이터가 유효하지 않은 경우.
#define ecERR_INVALID_SIZE_INFO			-67 // 크기 관련 데이터 정보가 유효하지 않은 경우.
#define ecERR_NULL_DLLNETCTXT			-69
#define ecERR_NULL_WDMNETCTXT			-70 // WDM 드라이버에서 제공하는 공유메모리 포인터가 NULL인 경우.
#define ecERR_NULL_INPDOLMEM			-71 // WDM 드라이버에서 제공하는 InPDO용 논리메모리 포인터가 NULL인 경우.
#define ecERR_NULL_OUTPDOLMEM			-72 // WDM 드라이버에서 제공하는 InPDO용 논리메모리 포인터가 NULL인 경우.
#define ecERR_INVALID_AXIS_INPDO_TYPE	-73 // 현재 설정되어 있는 Axis InputPDO 형식이 원하는 데이터를 지원하지 않음.
#define ecERR_INVALID_SECTION_ID		-80 // PDO Section ID가 올바르지 않거나, 환경설정된 섹션 리스트에 없는 경우.
#define ecERR_INVALID_LOG_MEM_ADDR		-90 // Logical Memory의 주소값이 올바르지 않은 경우.
#define ecERR_INVALID_BUFFER			-95	// 버퍼의 주소값이 유효하지 않은 경우
#define ecERR_INVALID_IO_CHAN_MAP_DATA	-100 // I/O 채널 맵핑 데이터의 일부 멤버값이 올바르지 않은 경우.
#define ecERR_INVALID_FILE_PATH			-110 // 지정한 파일의 경로가 잘못되었거나 파일이 없는 경우.
#define ecERR_INTERNAL_PROCESS_FAULT	-111 // 내부 연산 오류 
#define ecERR_MUTEX_CREATE_FAIL			-112 // Mutex 생성 실패
#define ecERR_FILE_OPEN_FAIL			-120
#define ecERR_FILE_NOT_FOUND			-125 // 지정한 파일이 검색되지 않은 경우.
#define ecERR_FILE_READ_ERROR			-130 // 파일을 읽는 과정에서 에러가 발생한 경우.
#define ecERR_FILE_VERIF_DATA_ERROR		-140 // CFG파일 Verfication을 수행했을 때 상호간의 데이터가 일치하지 않는 경우.
#define ecERR_MEM_ALLOC_FAIL			-150 // Memory Allocation fail
#define ecERR_XML_POS_SAVE_ERR			-160 // XML 데이터 읽기할 때 현재의 TREE position을 저장하는데 오류가 발생한 경우.
#define ecERR_XML_ELEM_NOT_FOUND		-165 // XML 데이터 읽기할 때 지정한 Element가 검색되지 않은 경우.
#define ecERR_INVALID_ODLIST_LEN_INFO	-166 // 슬레이브로부터 전달된 SDO Info. OD List의 크기 정보가 유효하지 않은 경우.
#define ecERR_INVALID_FIRMWARE_SIGN		-180 // 마스터장치에 다운로드 되어 있는 펌웨어의 signature가 유효하지 않은 경우.
#define ecERR_FW_VERIF_DATA_MISMATCH	-181 // F/W Download를 한 후에 Verify 과정에서 데이터가 일치하지 않은 경우.
#define ecERR_EEPROM_WRITE_FAIL			-182 // ESC의 EEPROM에 데이터 쓰기 실패
#define ecERR_IMPROPER_AL_STATE			-183 // AL-STATE가 적절하지 않은 경우.

// Motion 제어 전용 에러 코드 //
#define ecERR_MOTION_BASE				-1000  // Motion 제어 에러코드 시작 번호 
#define ecERR_MOT_SERVO_ALARM			(ecERR_MOTION_BASE - 10) // Servo driver에 알람이 발생한 경우.
#define ecERR_MOT_SEQ_SKIPPED			(ecERR_MOTION_BASE - 20) // Motion command has been skipped because the axis is already running
#define ecERR_MOT_LM_QUE_FULL			(ecERR_MOTION_BASE - 30) //	ListMotion Queue가 꽉차서 리스트모션 커맨드를 Queueing할 수 없는 경우. 
#define ecERR_MOT_LM_INVALID_OWNERSHIP	(ecERR_MOTION_BASE - 40) //	동일한 ListMotion 맵에 대한 제어권을 다른 프로세스에서 가로챈 경우 

//---------------------------------------------------------------------------------------------------------------------------
//			WDM 드라이버 프로그램에서 발생하는 에러코드
//---------------------------------------------------------------------------------------------------------------------------
#define ERR_COMMON_WITH_APP_BASE		-5000
#define ERR_INVALID_DSP_IDX				(ERR_COMMON_WITH_APP_BASE - 1) // 잘못된 DSP Index를 매개변수로 커맨드를 요청한 경우.
#define ERR_DSPCMD_IRQ_TIMEOUT			(ERR_COMMON_WITH_APP_BASE - 5) // DSP Command 요청 인터럽트 요청이 제한된 시간내에 성공하지 못한 경우.
#define ERR_DSPCMD_ACK_CLR_TIMEOUT		(ERR_COMMON_WITH_APP_BASE - 6) // DSP 가 PCI Command에 대한 Ack 플래그를 정해진 시간 내에 클리어하지 않은 경우 
#define ERR_DSPCMD_ACK_SET_TIMEOUT		(ERR_COMMON_WITH_APP_BASE - 7) // DSP 가 PCI Command에 대한 Ack 플래그를 정해진 시간 내에 셋하지 않은 경우 
#define ERR_PENDEDIRP_ADDTOLIST_FAIL	(ERR_COMMON_WITH_APP_BASE - 10) // 특정 IRP를 PendingList에 등록하려했으나 실패함. 
#define ERR_DSPCMD_ANS_TIMEOUT			(ERR_COMMON_WITH_APP_BASE - 20) // DSP 커맨드가 제한된 시간 내에 응답을 하지 않음.


//---------------------------------------------------------------------------------------------------------------------------
//			ETHERCAT 통신 처리 관련 에러코드
//---------------------------------------------------------------------------------------------------------------------------

#define ecERR_ECATNET_BASE				-10000  // DSP0(NET)에서 사용하는 에러코드 영역의 시작값.
#define ecERR_GEN_BASE					ecERR_ECATNET_BASE
#define ecERR_GEN_INVAL_ARGUMENT		(ecERR_GEN_BASE - 10)  // 함수 호출 시에 매개변수값이 잘못된 경우.
#define ecERR_GEN_DGRAM_OBJ_ALLOC_FAIL	(ecERR_GEN_BASE - 20) // Slave에서 DataGram 메모리를 할당하는데 실패함.
#define ecERR_GEN_MCMD_ALLOC_FAIL		(ecERR_GEN_BASE - 25) // Slave에서 DataGram 메모리를 할당하는데 실패함.
#define ecERR_GEN_MCMD_TIMEOUT			(ecERR_GEN_BASE - 30) // MasterCmd 처리 타임아웃
#define ecERR_GEN_MCMD_ITEM_NOT_FOUND	(ecERR_GEN_BASE - 40) // 지정한 MCmdIdx에 대한 MasterCmd Item을 리스트에서 찾을 수 없는 경우
#define ecERR_GEN_BUF_SIZE_TOO_SMALL	(ecERR_GEN_BASE - 50)
#define ecERR_GEN_SLV_PHYSADDR_NOT_SET	(ecERR_GEN_BASE - 60) // Slave에 Physical Address가 셋팅되지 않음.
#define ecERR_GEN_INVAL_REG_RW_SIZE		(ecERR_GEN_BASE - 70) // Slave Register Read/Write 시에 허용된 크기보다 큰 데이터를 Read/Write한 경우.
#define ecERR_GEN_INVAL_SLAVE_ID		(ecERR_GEN_BASE - 80)
#define ecERR_GEN_ECSLV_OBJ_NOT_FOUND	(ecERR_GEN_BASE - 90) // TEcSlave 객체 포인터가 null인 경우.
#define ecERR_GEN_STATE_CHANGE_FAIL		(ecERR_GEN_BASE - 100) // TEcSlave 객체 포인터가 null인 경우.

// 커맨드  에러 //
#define ecERR_CMD_BASE					(ecERR_ECATNET_BASE - 200) // -10200 부터 시작
#define ecERR_CMD_VENDID_READ_FAIL		(ecERR_CMD_BASE - 10)	// Vendor ID 읽기 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_PRODCODE_READ_FAIL	(ecERR_CMD_BASE - 20)	// Product Code 읽기 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_REVNO_READ_FAIL		(ecERR_CMD_BASE - 30)	// Revision No. 읽기 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_SERNO_READ_FAIL		(ecERR_CMD_BASE - 40)	// Serial No. 읽기 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_SET_PHY_ADDR_FAIL		(ecERR_CMD_BASE - 50) 	// Physical Address 셋팅 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_SET_DL_CTL_REG_FAIL	(ecERR_CMD_BASE - 60) 	// DL Control Register 셋팅 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_SET_AL_STATE_CHANGE_FAIL (ecERR_CMD_BASE - 65) // AL State 변경 과정 중에 에러 발생
#define ecERR_CMD_PDOMAP_ASSIGN_FAIL	(ecERR_CMD_BASE - 70) 	// In/Out PDO Map Assign 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_FMMU_SETTING_FAIL		(ecERR_CMD_BASE - 80) 	// In/Out PDO Map Assign 실패. 세부 실패 원인은 서브데이터 참고할 것
#define ecERR_CMD_PDO_SYNC_MODE_SET_FAIL	(ecERR_CMD_BASE - 90) 	// In/Out PDO Map Assign 실패. 세부 실패 원인은 서브데이터 참고할 것

// EtherCAT 통신 관련 일반  에러 //
#define ecERR_ECG_BASE					(ecERR_ECATNET_BASE - 400) // -10400 부터 시작
#define ecERR_ECG_DGRM_TIMEOUT			(ecERR_MBX_BASE - 5)  // 특정 EC 커맨드에 대해서 제한 시간 이내에 유효한 응답을 수신하지 못하  경우
#define ecERR_ECG_STATE_CHANGE_TIMEOUT	(ecERR_MBX_BASE - 10) // Slave의 State Change를 요청했으나 실제 변경하는데 제한된 시간을 초과한 경우

// Mailbox 통신 관련 에러 //
#define ecERR_MBX_BASE					(ecERR_ECATNET_BASE - 600) // -10600부터 시작
#define ecERR_MBX_COE_NOT_SUPP			(ecERR_MBX_BASE - 10) // 지정한  슬레이브가 CoE 프로토콜을 지원하지 않는 경우
#define ecERR_MBX_INVAL_SDO_RESP		(ecERR_MBX_BASE - 20) // SDO Command에 대한 응답 프레임이 올바르지 않음
#define ecERR_MBX_INVAL_SDO_SIZE		(ecERR_MBX_BASE - 30) // SDO의 Size 정보가 잘못된 경우
#define ecERR_MBX_SDO_DATA_SIZE_ERR		(ecERR_MBX_BASE - 40) // SDO 데이터가 제한된 크기보다 큰 경우.
#define ecERR_MBX_INVAL_SLAVE_ID		(ecERR_MBX_BASE - 50) // Slave Id가 잘못 지정된 경우
#define ecERR_MBX_TXFER_ABORTED			(ecERR_MBX_BASE - 60) // Mailbox 통신이 abort됨, 이 때 Abort Code ID를 알려면 TEcSlave::LastErrSubData를 참조하면 된다. //

// PDO 통신 관련 에러 //
#define ecERR_PDO_BASE					(ecERR_ECATNET_BASE - 800) // -10800부터 시작
#define ecERR_INVALID_PDO_SIZE			(ecERR_PDO_BASE - 10) // PDO 크기 정보가 잘못된 경우.
#define ecERR_PDO_BUF_ALLOC_FAIL		(ecERR_PDO_BASE - 20) // PDO 버퍼 할당 실패
#define ecERR_INVALID_OUTPDO_SM_IDX		(ecERR_PDO_BASE - 30) // OutPDO에 대한 SM 인덱스가 올바르지 않다.
#define ecERR_INVALID_INPDO_SM_IDX		(ecERR_PDO_BASE - 40) // OutPDO에 대한 SM 인덱스가 올바르지 않다.

//---------------------------------------------------------------------------------------------------------------------------
//			마스터장치내에서 APPLICATION 커맨드 처리 관련 에러코드
//---------------------------------------------------------------------------------------------------------------------------

// EcatApp 프로세서와의 IPC 통신 관련 에러 //
#define ecERR_IPC_BASE					(ecERR_ECATNET_BASE - 1000) // -11000부터 시작
#define ecERR_IPC_MSGQ_HEAP_CRE_FAIL	(ecERR_IPC_BASE - 10) // IPC Message Que를 위한 Heap을 생성하는데 실패함.
#define ecERR_IPC_MSGQ_CRE_FAIL			(ecERR_IPC_BASE - 20) // IPC Message Que를 생성하느데 실패함.
#define ecERR_IPC_INVAL_ECM_DATASIZE	(ecERR_IPC_BASE - 30) // EcmCmd의 DataSize로 지정된 값이 너무 큰 경우
#define ecERR_IPC_MSGQ_PUT_FAIL			(ecERR_IPC_BASE - 40) // IPC Message Que에 메시지를 Put하는데 실패함 (MessageQ_put() 함수 실행 실패)
#define ecERR_IPC_NOTIEVT_FAIL			(ecERR_IPC_BASE - 50) // IPC Notification event를 처리하는데 에러가 발생함
#define ecERR_IPC_WAITACK_TIMEOUT		(ecERR_IPC_BASE - 60) // IPC Message 를 잘 받았다는 EcatMast측의 ACK 메시지 수신에 대한 타임아웃 발생함

#define	eaERR_NONE						0
#define eaERR_ECATAPP_BASE				-20000  // DSP1(APP)에서 사용하는 에러코드 영역의 시작값.
#define eaERR_GEN_BASE					eaERR_ECATAPP_BASE
#define eaERR_GEN_MALLOC_FAIL			(eaERR_GEN_BASE - 10) // memory allocation fail
#define eaERR_FLASH_DEV_OPEN_FAIL		(eaERR_GEN_BASE - 30) // Flash 메모리장치를 open하는데 에러가 발생한 경우.
#define eaERR_FLASH_DEV_WRITE_FAIL		(eaERR_GEN_BASE - 31) // Flash 메모리장치에 데이터를 쓰기하는 과정에서 에러가 발생한 경우.
#define eaERR_FLASH_DEV_READ_FAIL		(eaERR_GEN_BASE - 31) // Flash 메모리장치에 데이터를 읽기하는 과정에서 에러가 발생한 경우.
#define eaERR_INVALID_FWU_SIGNATURE		(eaERR_GEN_BASE - 35) // Flash 메모리장치에 저장된 FW 정보가 유효하지 않은 경우.
#define eaERR_INVALID_BUF_SIZE			(eaERR_GEN_BASE - 40) // Buffer size 정보가 잘못된 경우.
#define eaERR_INVALID_BUFFER			(eaERR_GEN_BASE - 41) // 지정된 버퍼가 NULL이거나 잘못된 버퍼가 지정된 경우.

// PCI 커맨드 처리 관련 에러 //
#define eaERR_PCICMD_BASE				(eaERR_ECATAPP_BASE - 200) // - 20200 부터 시작.
#define eaERR_PCICMD_TIMEOUT			(eaERR_PCICMD_BASE - 10)	// PciCmd를 처리하는 과정에서 타임아웃 발생함. 이때 Error SubData는 CmdId를 나타낸다.
#define eaERR_PCICMD_INVALID_CMDID		(eaERR_PCICMD_BASE - 20)	// PciCmd를 처리하는 과정에서 타임아웃 발생함. 이때 Error SubData는 CmdId를 나타낸다.
#define eaERR_PCICMD_INVALID_ARG		(eaERR_PCICMD_BASE - 30)	// PciCmd의 Argument가 유효하지 않음.
#define eaERR_PCICMD_INVALID_SLVIDX		(eaERR_PCICMD_BASE - 40)	// PciCmd의 SlaveIndex 가 잘못 전달된 경우.
#define eaERR_PCICMD_INVALID_AXIS		(eaERR_PCICMD_BASE - 50)	// Motion PciCmd의 Axis 매개변수가 잘못 전달된 경우.
#define eaERR_PCICMD_INVALID_IXMAP_IDX	(eaERR_PCICMD_BASE - 60)	// Motion PciCmd의 Axis 매개변수가 잘못 전달된 경우.
#define eaERR_PCICMD_HOME_START_TIMEOUT	(eaERR_PCICMD_BASE - 70)	// Homing을 시작시켰으나 서보드라이버가 Homing을 진행하지 않는 경우.
#define eaERR_PCICMD_INVALID_AL_STATE	(eaERR_PCICMD_BASE - 80)	// 해당 커맨드를 실행할 수 있는 AL State가 아닌 경우.

// EcatMast(Net) 프로세서와의 IPC 통신 관련 에러 //
#define eaERR_IPC_BASE					(eaERR_ECATAPP_BASE - 400) // - 20400 부터 시작.
#define eaERR_IPC_MSGQ_HEAP_CRE_FAIL	(eaERR_IPC_BASE - 10) // IPC Message Que를 위한 Heap을 생성하는데 실패함.
#define eaERR_IPC_MSGQ_CRE_FAIL			(eaERR_IPC_BASE - 20) // IPC Message Que를 생성하느데 실패함.
#define eaERR_IPC_INVAL_EAPC_DATASIZE	(eaERR_IPC_BASE - 30) // EapCmd의 DataSize로 지정된 값이 너무 큰 경우
#define eaERR_IPC_MSGQ_PUT_FAIL			(eaERR_IPC_BASE - 40) // IPC Message Que에 메시지를 Put하는데 실패함 (MessageQ_put() 함수 실행 실패)
#define eaERR_IPC_NOTIEVT_FAIL			(eaERR_IPC_BASE - 50) // IPC Notification event를 처리하는데 에러가 발생함
#define eaERR_IPC_WAITACK_TIMEOUT		(eaERR_IPC_BASE - 60) // IPC Message 를 잘 받았다는 EcatMast측의 ACK 메시지 수신에 대한 타임아웃 발생함

// 모션제어 관련 에러코드 //
#define eaERR_MOT_BASE					(eaERR_ECATAPP_BASE - 1000) // - 21000 부터 시작.
#define eaERR_AXIS_MOT_QUE_FULL			(eaERR_MOT_BASE - 2)  ///< axis-motion-que가 꽉차서 모션을 예약하지 못하였음.
#define eaERR_INVALID_PARAMETER			(eaERR_MOT_BASE - 10) ///< Some of the funcion parameters are invalid
#define eaERR_INVALID_AXIS				(eaERR_MOT_BASE - 11) ///< The axis setting parameter(s) is(are) invalid
#define eaERR_INVALID_SPEED_SET			(eaERR_MOT_BASE - 12) ///< Speed setting value is not valid
#define eaERR_INVALID_IXMAP				(eaERR_MOT_BASE - 13) ///< Invalid Interpolation Map
#define eaERR_INVALID_LMMAP				(eaERR_MOT_BASE - 14) ///< Invalid List-Motion Map
#define eaERR_INVALID_NUMAXIS			(eaERR_MOT_BASE - 15) ///< Invlaid number of axis(Mx)
#define eaERR_INVALID_MAST_AXIS			(eaERR_MOT_BASE - 16) ///< Invlaid master axis settings for Master/Slave Motion
#define eaERR_INVALID_SPLINE_SETUP		(eaERR_MOT_BASE - 17) ///< SpineSetup() 함수가 수행되지 않은 상태에서 Spline보간 관련 다른 함수를 실행한 경우.
#define eaERR_INVALID_POS_DATA			(eaERR_MOT_BASE - 38) ///< Position 데이터가 유효하지 않은 경우.
#define eaERR_IX_AXES_NOT_DEFINED		(eaERR_MOT_BASE - 30) ///< 보간 축 설정이 수행되지 않은 경우.
#define eaERR_IX_OBJ_POOL_FULL			(eaERR_MOT_BASE - 35) ///< spline/MPRLin2X 등의  object 등록 pool에 현재 등록되어 있는 object가 최대 갯수만큼 등록되어서 더 이상 추가할 수 없는 경우.
#define eaERR_IX_OBJ_NOT_FOUND			(eaERR_MOT_BASE - 36) ///< spline/MPRLin2X 등의  object pool에서 지정한 SPLINE OBJECT를 찾을 수 없음.
#define eaERR_IX_OBJ_NOT_BUILDED		(eaERR_MOT_BASE - 37) ///< spline/MPRLin2X 등의  object Build를 수행하지 않고 이송 시작을 수행한 경우.
#define eaERR_STOP_BY_SLP				(eaERR_MOT_BASE - 50) ///< Abnormally stopped by positive soft limit
#define eaERR_STOP_BY_SLN				(eaERR_MOT_BASE - 51) ///< Abnormally stopped by negative soft limit
#define eaERR_STOP_BY_ELP				(eaERR_MOT_BASE - 52)	///< Abnormally stopped by (+) external limit
#define eaERR_STOP_BY_ELN				(eaERR_MOT_BASE - 53)	///< Abnormally stopped by (+) external limit
#define eaERR_STOP_BY_ALM				(eaERR_MOT_BASE - 54)	///< Abnormally stopped by alarm input signal
#define eaERR_STOP_BY_COMM_ERROR		(eaERR_MOT_BASE - 55)	///< Abnormally Stopped by communication error
#define eaERR_STOP_BY_OTHER_AXIS		(eaERR_MOT_BASE - 56)	///< 다른 축과의 협업 이송 중에 다른 축의 에러에 의해서 정지된 경우.
#define eaERR_STOP_BY_SVOFF				(eaERR_MOT_BASE - 57)	///< SERVO-OFF된 상태에서 이송명령이 내려진 경우, 또는 이송 중에 SERVO-OFF된 경우.
#define eaERR_MOT_SEQ_SKIPPED			(eaERR_MOT_BASE - 60)	///< Motion command has been skipped because the axis is already running
#define eaERR_SKIPPED_BY_SERVO_FAULT	(eaERR_MOT_BASE - 62)	///< Motion Control Command is skipped because of servo-driver alarm.
#define eaERR_SKIPPED_BY_SERVO_OFF		(eaERR_MOT_BASE - 63)	///< Motion Control Command is skipped because of 'Operation Enable(Servo-ON)' state is off.
#define eaERR_SKIPPED_BY_SERVO_ON		(eaERR_MOT_BASE - 64)	///< 서보온이 되어 있는 상태에서 처리할 수 없는 명령이 하달되어 해당 명령의 수행이 되지 않은 경우.
#define eaERR_HOME_START_TIMEOUT		(eaERR_MOT_BASE - 70) // 서보드라이버에 Homming start 명령을 내렸으나 지정된 시간 내에  Homming이 시작되지 않는 경우.
#define eaERR_HOME_COMPT_FAIL			(eaERR_MOT_BASE - 71) // Servo driver의 Homing 작업을 완료하는 과정에서 에러가 발생함.
#define eaERR_HOME_TPROBE_NOT_TRIGGERED	(eaERR_MOT_BASE - 72) // 101번 이상의 원점복귀 모드를 수행할 때 Touch Probe가 정상적으로 Trigger되지 않은 경우.
#define eaERR_HOME_STEP_TIMEOUT0		(eaERR_MOT_BASE - 80) // Touch Probe 셋팅이 제한시간내에 완료되지 않은 경우.
// 이 중간의 에러코드는 HOME_STEP 번호에 따라서 매겨지므로 따로 정의하지 말것 //
#define eaERR_HOME_STEP_TIMEOUT19		(eaERR_MOT_BASE - 99) // Touch Probe 셋팅이 제한시간내에 완료되지 않은 경우.
#define eaERR_LM_QUE_FULL				(eaERR_MOT_BASE - 110) // 리스트모션의 커맨드 큐가 꽉차서 커맨드를 등록하지 못한 경우.
#define eaERR_PTM_QUE_FULL				(eaERR_MOT_BASE - 130) // 리스트모션의 커맨드 큐가 꽉차서 커맨드를 등록하지 못한 경우.
#define eaERR_PTM_MAP_DISABLED			(eaERR_MOT_BASE - 131) // PT모션 맵이 Begin(ecmLmCtl_Begin() 함수를 통해서 실행됨)되지 않은 상태에서 PT모션 관련 함수들을 실행한 경우.
#define eaERR_INAVLID_PDO_MAP			(eaERR_MOT_BASE - 150) // 현재의 PDO Mapping에서 지원되지 않는 기능을 사용하려고 하는 경우.


// SW EtherCAT Master Error Code ///////////////////////////////////////////////////////////////////////////////////////////////////////
#define secERR_SWECAT_BASE					-50000  // SW EtherCAT Master Error Code 시작값
#define secERR_NULL_FILENAME				(secERR_SWECAT_BASE - 1) // 파일 메모리 포인터가 NULL 인 경우.
#define secERR_NULL_ECATMAINTASK			(secERR_SWECAT_BASE - 2) // EcatCheckProc 쓰레드 생성 실패한 경우.
#define secERR_NULL_ECATCHECKPROC			(secERR_SWECAT_BASE - 3) // EcatCheckProc 쓰레드 생성 실패한 경우.
#define secERR_NONE_NETADAPTER				(secERR_SWECAT_BASE - 4) // Network Adapter가 발견되지 않은 경우.
#define secERR_NOT_OP_ALLSLAVE				(secERR_SWECAT_BASE - 5) // 일부 또는 전체 Slave가 OP 상태에 있지 않는 경우.

#define secERR_NULL_BUFFER					(secERR_SWECAT_BASE - 6) // 버퍼가 NULL인 경우
#define secERR_EEPROM_READ					(secERR_SWECAT_BASE - 7) // EEPROM Read 실패한 경우
#define secERR_EEPROM_WRITE					(secERR_SWECAT_BASE - 8) // EEPROM Write 실패한 경우
#define secERR_EEPROM_FILE					(secERR_SWECAT_BASE - 9) // EEPROM File 읽기가 실패한 경우
#define secERR_INVALID_EEPROM_RW_MODE		(secERR_SWECAT_BASE - 10) // 지원되지 않는 EEPROM Read/Write Mode 요청시

#define secERR_OVERSIZE_FIRMWARE			(secERR_SWECAT_BASE - 11) // 펌웨어 크기가 너무 큰 경우
#define secERR_FOE_WRITE					(secERR_SWECAT_BASE - 12) // FOE Write 실패한 경우
#define secERR_FIRMWARE_FILE				(secERR_SWECAT_BASE - 13) // Firmware 파일 읽기가 실패한 경우
#define secERR_TIMEOUT_BOOTMODE				(secERR_SWECAT_BASE - 14) // BOOT 모드로 요청에 대해 Timeout된 경우

#define secERR_SDO_READ						(secERR_SWECAT_BASE - 15) // SDO Read 실패시
#define secERR_SDO_READ_TIMEOUT				(secERR_SWECAT_BASE - 16) // SDO Read Timeout
#define secERR_SDO_WRITE					(secERR_SWECAT_BASE - 17) // SDO Write 실패시
#define secERR_SDO_WRITE_TIMEOUT			(secERR_SWECAT_BASE - 18) // SDO Write Timeout
#define secERR_NOT_SUPPORTED_DEVICE			(secERR_SWECAT_BASE - 19) // 지원되지 않는 Device

#define secERR_NET_CMD_SCAN					(secERR_SWECAT_BASE - 20) /// Scan Request 실패시
#define secERR_NET_CMD_SCAN_TIMEOUT			(secERR_SWECAT_BASE - 21) // Scan Response Timeout
#define secERR_SLV_CONFIG_CHANGED			(secERR_SWECAT_BASE - 22) // Scan 한 Slave와 Configuration 파일의 정보가 일치하지 않는 경우

// AL status code list type definition(SW Master) //////////////////////////////////////////////////////////////////////////////////////
#define EC_MAXERRORNAME					127	// Error 문자열 최대길이(SW Master)

// Software Master Only
typedef struct{
	t_ui16 ALstatuscode;									// AL status code
	t_byte ALstatuscodedescription[EC_MAXERRORNAME + 1];	// Readable description
} TALstatuscodelist;

/** AL status code list definition */
const TALstatuscodelist ec_ALstatuscodelist[] = {
	{0x0000 , "No error" },
	{0x0001 , "Unspecified error" },
	{0x0002 , "No memory" },
	{0x0011 , "Invalid requested state change" },
	{0x0012 , "Unknown requested state" },
	{0x0013 , "Bootstrap not supported" },
	{0x0014 , "No valid firmware" },
	{0x0015 , "Invalid mailbox configuration" },
	{0x0016 , "Invalid mailbox configuration" },
	{0x0017 , "Invalid sync manager configuration" },
	{0x0018 , "No valid inputs available" },
	{0x0019 , "No valid outputs" },
	{0x001A , "Synchronization error" },
	{0x001B , "Sync manager watchdog" },
	{0x001C , "Invalid sync Manager types" },
	{0x001D , "Invalid output configuration" },
	{0x001E , "Invalid input configuration" },
	{0x001F , "Invalid watchdog configuration" },
	{0x0020 , "Slave needs cold start" },
	{0x0021 , "Slave needs INIT" },
	{0x0022 , "Slave needs PREOP" },
	{0x0023 , "Slave needs SAFEOP" },
	{0x0024 , "Invalid input mapping" },
	{0x0025 , "Invalid output mapping" },
	{0x0026 , "Inconsistent settings" },
	{0x0027 , "Freerun not supported" },
	{0x0028 , "Synchronisation not supported" },
	{0x0029 , "Freerun needs 3buffer mode" },
	{0x002A , "Background watchdog" },
	{0x002B , "No valid Inputs and Outputs" },
	{0x002C , "Fatal sync error" },
	{0x002D , "No sync error" }, // was "Invalid Output FMMU Configuration"
	{0x002E , "Invalid input FMMU configuration" },
	{0x0030 , "Invalid DC SYNC configuration" },
	{0x0031 , "Invalid DC latch configuration" },
	{0x0032 , "PLL error" },
	{0x0033 , "DC sync IO error" },
	{0x0034 , "DC sync timeout error" },
	{0x0035 , "DC invalid sync cycle time" },
	{0x0035 , "DC invalid sync0 cycle time" },
	{0x0035 , "DC invalid sync1 cycle time" },
	{0x0042 , "MBX_EOE" },
	{0x0043 , "MBX_COE" },
	{0x0044 , "MBX_FOE" },
	{0x0045 , "MBX_SOE" },
	{0x004F , "MBX_VOE" },
	{0x0050 , "EEPROM no access" },
	{0x0051 , "EEPROM error" },
	{0x0060 , "Slave restarted locally" },
	{0x0061 , "Device identification value updated" },
	{0x00f0 , "Application controller available" },
	{0xffff , "Unknown" }
};

#endif