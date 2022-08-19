#pragma once
// 
// #define SEND_CAN_ID_CH1							0x100		//BTU->DSP CAN ID CH1
// #define SEND_CAN_ID_CH2							0x200		//BTU->DSP CAN ID CH2
//#define SEND_CAN_ID_PARALLEL						0x300		//BTU->DSP CAN ID PARALLEL

#define SEND_CAN_ID_CH_EXTERNAL1_OFFSET					0x5		//BTU->DSP CAN ID CH1 EXTERNAL
#define SEND_CAN_ID_CH_EXTERNAL2_OFFSET					0x6		//BTU->DSP CAN ID CH1 EXTERNAL - CP/TIME
#define SEND_CAN_ID_CH_EXTERNAL3_OFFSET					0x7		//BTU->DSP CAN ID CH1 EXTERNAL - CC High/Low
#define SEND_CAN_ID_CH_EXTERNAL4_OFFSET					0x8		//BTU->DSP CAN ID CH1 EXTERNAL - CC High/Low
//
//#define SEND_CAN_ID_CH1_EXTERNAL1					0x105		//BTU->DSP CAN ID CH1 EXTERNAL
//#define SEND_CAN_ID_CH1_EXTERNAL2					0x106		//BTU->DSP CAN ID CH1 EXTERNAL - CP/TIME
//#define SEND_CAN_ID_CH1_EXTERNAL3					0x107		//BTU->DSP CAN ID CH1 EXTERNAL - CC High/Low
//#define SEND_CAN_ID_CH2_EXTERNAL1					0x205		//BTU->DSP CAN ID CH2 EXTERNAL
//#define SEND_CAN_ID_CH2_EXTERNAL2					0x206		//BTU->DSP CAN ID CH2 EXTERNAL - CP/TIME
//#define SEND_CAN_ID_CH2_EXTERNAL3					0x207		//BTU->DSP CAN ID CH2 EXTERNAL - CC High/Low
//#define SEND_CAN_ID_PARALLEL_EXTERNAL1				0x305		//BTU->DSP CAN ID PARALLEL EXTERNAL
//#define SEND_CAN_ID_PARALLEL_EXTERNAL2				0x306		//BTU->DSP CAN ID PARALLEL EXTERNAL - CP/TIME
//#define SEND_CAN_ID_PARALLEL_EXTERNAL3				0x307		//BTU->DSP CAN ID PARALLEL EXTERNAL - CC High/Low
////Dsp ch 2
//#define SEND_CAN_ID_PARALLEL_EXTERNAL3				0x307		//BTU->DSP CAN ID PARALLEL EXTERNAL
//#define SEND_CAN_ID_PARALLEL_EXTERNAL4				0x308		//BTU->DSP CAN ID PARALLEL EXTERNAL - CP/TIME

#define REV_CAN_ID_DEFAULT_CH						0x100		
#define REV_CAN_ID_DATA_OFFSET						0x10		
#define REV_CAN_ID_ERRCODE_OFFSET					0x20	
#define REV_CAN_ID_PATCOUNT_OFFSET					0x30	

//#define REV_CAN_ID_DATA_CH1							0x110		//DSP->BTU CAN VOLT,CURR DATA
//#define REV_CAN_ID_ERRCODE_CH1						0x120		//DSP->BTU CAN ERROR CODE
//#define REV_CAN_ID_PATCOUNT_CH1						0x130		//DSP->BTU CAN PATTERN COUNT
//
//#define REV_CAN_ID_DATA_CH2							0x210		//DSP->BTU CAN VOLT,CURR DATA
//#define REV_CAN_ID_ERRCODE_CH2						0x220		//DSP->BTU CAN ERROR CODE
//#define REV_CAN_ID_PATCOUNT_CH2						0x230		//DSP->BTU CAN PATTERN COUNT

//	BTU -> DSP
//  CMD - DSP Operation Command
enum { 
		DSP_SEND_CMD_MINUS_CHG						 = 0x60,		//(-)전압 충전   충전전압, 충전전류 Reference값 전송
		DSP_SEND_CMD_MINUS_DISCHG					 = 0x61,		//(-)전압 방전    방전종지전압, 방전전류 Reference값 전송
		DSP_SEND_CMD_VOL_CONTROL					 = 0x79,		//외부패턴 전압제어 모드 command
		DSP_SEND_CMD_INPUT_MC_ON					 = 0x80,		//입력 MC ON 요청
		DSP_SEND_CMD_OUTPUT_MC_ON					 = 0x81,		//출력 MC ON 요청
		DSP_SEND_CMD_CHG							 = 0x82,		//충전시작, 충전전압, 충전전류 Reference값 전송
		DSP_SEND_CMD_DISCHG							 = 0x83,		//방전시작, 방전종지전압, 방전전류 Reference값 전송
		DSP_SEND_CMD_OUTPUT_MC_OFF					 = 0x84,		//출력 MC OFF 요청
		DSP_SEND_CMD_INPUT_MC_OFF					 = 0x85,		//계통 연결 해제 요청
		DSP_SEND_CMD_CHG_DISCHG_STOP				 = 0x86,		//충방전 정지 요청
		DSP_SEND_CMD_EXT_DATA_SEND					 = 0x87,		//외부 패턴 Data 전송
		DSP_SEND_CMD_EXT_START						 = 0x88,		//외부 패턴 운전 시작 요청
		DSP_SEND_CMD_ERR_CLEAR						 = 0x89,		//Error Clear 요청
};

//  OP Mode - Operation Mode Setting
enum{ 
		DSP_SEND_OP_STANDBY							 = 0x0,		//대기 모드
		DSP_SEND_OP_SPARE1							 = 0x1,		//Spare
		DSP_SEND_OP_SPARE2						     = 0x2,		//Spare
		DSP_SEND_OP_CHG_CC							 = 0x3,		//충전 CC 요청
		DSP_SEND_OP_CHG_CV							 = 0x4,		//충전 CV 모드 진입 시 확인 송신
		DSP_SEND_OP_DISCHG_CC						 = 0x5,		//방전 CC 요청
		DSP_SEND_OP_DISCHG_CV						 = 0x6,		//방전 CV 모드 진입 시 확인 송신
		DSP_SEND_OP_CHG_CP							 = 0x7,		//충전 CP 요청
		DSP_SEND_OP_DISCHG_CP						 = 0x8,		//방전 CP 요청
};													   
//  Parallel Mode - Parallel Mode Setting
enum{ 
		DSP_SEND_PARALLEL_OFF						 = 0x0,		//개별 운전 모드 요청
		DSP_SEND_PARALLEL_ON						 = 0x1,		//병렬 운전 모드 요청
};

//	DSP -> BTU
//  CMD - DSP Operation Command
enum{ 
		DSP_REV_CMD_MINUS_CHG						 = 0x70,		//(-)충전 시작, 현재 충전전압 및 충전전류값 리턴
		DSP_REV_CMD_MINUS_DISCHG					 = 0x71,		//(-)방전 시작, 현재 충전전압 및 충전전류값 리턴
		DSP_REV_CMD_MINUS_INPUT_MC_ON				 = 0x72,		//(-)충/방전기 입력 MC ON
		DSP_REV_CMD_MINUS_OUTPUT_MC_ON				 = 0x73,		//(-)충/방전기 출력 MC ON (충방전 대기 모드 진입 및 Error Clear)
		DSP_REV_CMD_MINUS_OUTPUT_MC_OFF				 = 0x74,		//출력MC OFF 명령 일때(PC 입장) - 전압
		DSP_REV_CMD_MINUS_STOP						 = 0x76,		//장비 정지 명령 일때(PC 입장) - 전압

		DSP_REV_CMD_INPUT_MC_ON						 = 0x90,		//충/방전기 입력 MC ON
		DSP_REV_CMD_OUTPUT_MC_ON					 = 0x91,		//충/방전기 출력 MC ON (충방전 대기 모드 진입 및 Error Clear)
		DSP_REV_CMD_CHG								 = 0x92,		//충전 시작, 현재 충전전압 및 충전전류값 리턴
		DSP_REV_CMD_DISCHG							 = 0x93,		//방전 시작, 현재 충전전압 및 충전전류값 리턴
		DSP_REV_CMD_OUTPUT_MC_OFF					 = 0x94,		//충/방전기 출력MC OFF
		DSP_REV_CMD_INPUT_MC_OFF					 = 0x95,		//충/방전기 입력/출력MC OFF
		DSP_REV_CMD_CHG_DISCHG_STOP					 = 0x96,		//충방전 정지 
		DSP_REV_CMD_EXT_DATA_SEND					 = 0x97,		//외부 패턴 data 수집 
		DSP_REV_CMD_EXT_START						 = 0x98,		//외부 패턴 운전 시작 
		DSP_REV_CMD_ERR_CLEAR						 = 0x99,		//Error Code Clear 응답
};

//  OP Mode - Operation Mode Setting
enum{ 
		DSP_REV_OP_STANDBY							 = 0x0,		//대기 상태
		DSP_REV_OP_INPUT_READY						 = 0x1,		//입력단 준비 완료(입력단 초기 충전 완료)
		DSP_REV_OP_TEST_READY						 = 0x2,		//충/방전 시험 준비 완료
		DSP_REV_OP_CHG_CC							 = 0x3,		//충전 CC 진행 중
		DSP_REV_OP_CHG_CV							 = 0x4,		//충전 CV 진행 중
		DSP_REV_OP_DISCHG_CC						 = 0x5,		//방전 CC 진행 중
		DSP_REV_OP_DISCHG_CV						 = 0x4,		//방전전 CV 진행 중
		DSP_REV_OP_CHG_CP							 = 0x6,		//충전 CP 진행 중
		DSP_REV_OP_DISCHG_CP						 = 0x7,		//방전 CP 진행 중
		DSP_REV_OP_TEST_END							 = 0x8,		//시험 완료(0x8 -> 0x2)
};															
//  Parallel Mode - Parallel Mode Setting
enum{ 
		DSP_REV_PARALLEL_OFF						 = 0x0,		//개별 운전 모드 상태 완료 응답
		DSP_REV_PARALLEL_READY						 = 0x1,		//병렬 운전 모드 준비 중
		DSP_REV_PARALLEL_ON							 = 0x2,		//병렬 운전 모드 준비 완료 응답
};

//ERROR CODE
enum eWARNING{
	WARNING_ERROR_PACK_REVERSE_FAULT				 = 0x1,	    //밧데리 역결선 
	WARNING_ERROR_IBAT1_OVER_OPERATING				 = 0x2,		//채널1 시험 전류 초과
	WARNING_ERROR_PACK1_OVER_OPERATING				 = 0x4,		//채널1 시험 전압 초과 	
// 	WARNING_ERROR_IBAT2_OVER_OPERATING				 = 0x8,		//채널2 시험 전류 초과
// 	WARNING_ERROR_PACK2_OVER_OPERATING				 = 0x10,		//채널2 시험 전압 초과 
// 	WARNING_ERROR_IBAT3_OVER_OPERATING				 = 0x20,		//채널3 시험 전류 초과
// 	WARNING_ERROR_PACK3_OVER_OPERATING				 = 0x40,		//채널3 시험 전압 초과 
	WARNING_ERROR_PACK1_CONTACT_FAULT				 = 0x80,		//배터리#1 시료 없음
// 	WARNING_ERROR_PACK2_CONTACT_FAULT				 = 0x100,		//밧데리#2 시료 없음	
// 	WARNING_ERROR_PACK3_CONTACT_FAULT				 = 0x200,		//밧데리#3 시료 없음
// 	WARNING_ERROR_PARALLEL_MODE_FAULT				 = 0x400,		//병렬 운전 모드 불합리
// 	WARNING_ERROR_VPACK_CONTACT_FAULT				 = 0x800,		//밧데리#1 센싱 없음			//SMA,IBC,UPB 
// 	WARNING_ERROR_PC_DSP_ERROR						 = 0x2000		//PC DSP 통신끊김
};

enum eGRID{
	GRID_ERROR_PLL_ERROR	      					 = 1,       //Pll Error			// 입력 케이블 상순 불량
	GRID_ERROR_INVERTER_A_OVERCURRENT				 = 2,       //Grid A OC			// R상 과전류
	GRID_ERROR_INVERTER_B_OVERCURRENT				 = 4,       //Grid B OC			// S상 과전류
	GRID_ERROR_INVERTER_C_OVERCURRENT				 = 10,      //Grid C OC			// T상 과전류
	GRID_ERROR_VOUT_OVERVOLTAGE						 = 20,      //Grid OV			// 입력 과전압
	GRID_ERROR_VOUT_UNDERVOLTAGE					 = 40,      //Grid UV			// 입력 저전압
	GRID_ERROR_INITIAL_CHARGE_ERROR					 = 100,     //Init Charge Fault	// 초기 충전 이상
	GRID_ERROR_VQE_UNBALANCE						 = 200,     //Vqe_Unbalance		// 입력 전압 불평형
//	GRID_ERROR_VDE_UNBALANCE						 = 400	    //Vde_Unbalance		// 입력 Vd전압 불평형		//SMA,IBC,UPB 
};

enum eDC{
	DC_ERROR_VDC_OVERVOLTAGE						 = 0x1,	    // 정류단 과충전 발생
	DC_ERROR_VDC_UNBALANCE							 = 0x2,	    // 정류단 전압 불평형
	DC_ERROR_VBAT1_OVERVOLTAGE						 = 0x4,      // 콘덴서#1 과충전 발생
	DC_ERROR_VBAT1_UNBALANCE						 = 0x8,      // 콘덴서#1 전압 불평형
	DC_ERROR_Vpack1_OVERVOLTAGE						 = 0x10,     // 밧데리#1 과충전 발생
	DC_ERROR_IBAT1_OVERCURRENT						 = 0x20,     // 밧데리#1 과전류 발생
// 	DC_ERROR_VBAT2_OVERVOLTAGE						 = 0x40,     // 콘덴서#2 과충전 발생
// 	DC_ERROR_Vpack2_OVERVOLTAGE						 = 0x80,     // 밧데리#2 과충전 발생
// 	DC_ERROR_IBAT2_OVERCURRENT						 = 0x100,    // 밧데리#2 과전류 발생
// 	DC_ERROR_VBAT3_OVERVOLTAGE						 = 0x200,    // 콘덴서#3 과충전 발생
// 	DC_ERROR_Vpack3_OVERVOLTAGE						 = 0x400,    // 밧데리#3 과충전 발생
// 	DC_ERROR_IBAT3_OVERCURRENT						 = 0x800     // 밧데리#3 과전류 발생
};

enum eFAULT{
	FAULT_ERROR_HEAT_FAULT							 = 0x1,	    //Heat Over			// 방열판 과온도 발생
	FAULT_ERROR_AC_FUSE_FAULT						 = 0x2,       //Fuse Broken 		// 입력 퓨즈 이상
	FAULT_ERROR_DC_FUSE_FAULT						 = 0x4,       //Fuse Broken 		// 출력 퓨즈 이상
	FAULT_ERROR_FAN_FAULT							 = 0x8,       //Fan Broken		// 방열 FAN 불량
	FAULT_ERROR_EMERGENCY_STOP						 = 0x10,	    //Emergency Stop	// 비상정지
	FAULT_ERROR_DRIVE_1_FAULT						 = 0x20,		//Drive 1 Fault		// 입력 R상 IGBT 문제 발생
	FAULT_ERROR_DRIVE_2_FAULT						 = 0x40,		//Drive 2 Fault		// 입력 S상 IGBT 문제 발생
	FAULT_ERROR_DRIVE_3_FAULT						 = 0x80,		//Drive 3 Fault		// 입력 T상 IGBT 문제 발생
	FAULT_ERROR_DRIVE_4_FAULT						 = 0x100,		//채널1 IGBT 문제 발생
// 	FAULT_ERROR_DRIVE_5_FAULT						 = 0x200,		//채널2 IGBT 문제 발생
// 	FAULT_ERROR_DRIVE_6_FAULT						 = 0x400,		//채널3 IGBT 문제 발생
	FAULT_ERROR_COMM_CHK_FAULT						 = 0x800,		//PC-DSP 통신 문제 발생
};
