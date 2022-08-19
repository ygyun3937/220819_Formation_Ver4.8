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
		DSP_SEND_CMD_MINUS_CHG						 = 0x60,		//(-)���� ����   ��������, �������� Reference�� ����
		DSP_SEND_CMD_MINUS_DISCHG					 = 0x61,		//(-)���� ����    ������������, �������� Reference�� ����
		DSP_SEND_CMD_VOL_CONTROL					 = 0x79,		//�ܺ����� �������� ��� command
		DSP_SEND_CMD_INPUT_MC_ON					 = 0x80,		//�Է� MC ON ��û
		DSP_SEND_CMD_OUTPUT_MC_ON					 = 0x81,		//��� MC ON ��û
		DSP_SEND_CMD_CHG							 = 0x82,		//��������, ��������, �������� Reference�� ����
		DSP_SEND_CMD_DISCHG							 = 0x83,		//��������, ������������, �������� Reference�� ����
		DSP_SEND_CMD_OUTPUT_MC_OFF					 = 0x84,		//��� MC OFF ��û
		DSP_SEND_CMD_INPUT_MC_OFF					 = 0x85,		//���� ���� ���� ��û
		DSP_SEND_CMD_CHG_DISCHG_STOP				 = 0x86,		//����� ���� ��û
		DSP_SEND_CMD_EXT_DATA_SEND					 = 0x87,		//�ܺ� ���� Data ����
		DSP_SEND_CMD_EXT_START						 = 0x88,		//�ܺ� ���� ���� ���� ��û
		DSP_SEND_CMD_ERR_CLEAR						 = 0x89,		//Error Clear ��û
};

//  OP Mode - Operation Mode Setting
enum{ 
		DSP_SEND_OP_STANDBY							 = 0x0,		//��� ���
		DSP_SEND_OP_SPARE1							 = 0x1,		//Spare
		DSP_SEND_OP_SPARE2						     = 0x2,		//Spare
		DSP_SEND_OP_CHG_CC							 = 0x3,		//���� CC ��û
		DSP_SEND_OP_CHG_CV							 = 0x4,		//���� CV ��� ���� �� Ȯ�� �۽�
		DSP_SEND_OP_DISCHG_CC						 = 0x5,		//���� CC ��û
		DSP_SEND_OP_DISCHG_CV						 = 0x6,		//���� CV ��� ���� �� Ȯ�� �۽�
		DSP_SEND_OP_CHG_CP							 = 0x7,		//���� CP ��û
		DSP_SEND_OP_DISCHG_CP						 = 0x8,		//���� CP ��û
};													   
//  Parallel Mode - Parallel Mode Setting
enum{ 
		DSP_SEND_PARALLEL_OFF						 = 0x0,		//���� ���� ��� ��û
		DSP_SEND_PARALLEL_ON						 = 0x1,		//���� ���� ��� ��û
};

//	DSP -> BTU
//  CMD - DSP Operation Command
enum{ 
		DSP_REV_CMD_MINUS_CHG						 = 0x70,		//(-)���� ����, ���� �������� �� ���������� ����
		DSP_REV_CMD_MINUS_DISCHG					 = 0x71,		//(-)���� ����, ���� �������� �� ���������� ����
		DSP_REV_CMD_MINUS_INPUT_MC_ON				 = 0x72,		//(-)��/������ �Է� MC ON
		DSP_REV_CMD_MINUS_OUTPUT_MC_ON				 = 0x73,		//(-)��/������ ��� MC ON (����� ��� ��� ���� �� Error Clear)
		DSP_REV_CMD_MINUS_OUTPUT_MC_OFF				 = 0x74,		//���MC OFF ��� �϶�(PC ����) - ����
		DSP_REV_CMD_MINUS_STOP						 = 0x76,		//��� ���� ��� �϶�(PC ����) - ����

		DSP_REV_CMD_INPUT_MC_ON						 = 0x90,		//��/������ �Է� MC ON
		DSP_REV_CMD_OUTPUT_MC_ON					 = 0x91,		//��/������ ��� MC ON (����� ��� ��� ���� �� Error Clear)
		DSP_REV_CMD_CHG								 = 0x92,		//���� ����, ���� �������� �� ���������� ����
		DSP_REV_CMD_DISCHG							 = 0x93,		//���� ����, ���� �������� �� ���������� ����
		DSP_REV_CMD_OUTPUT_MC_OFF					 = 0x94,		//��/������ ���MC OFF
		DSP_REV_CMD_INPUT_MC_OFF					 = 0x95,		//��/������ �Է�/���MC OFF
		DSP_REV_CMD_CHG_DISCHG_STOP					 = 0x96,		//����� ���� 
		DSP_REV_CMD_EXT_DATA_SEND					 = 0x97,		//�ܺ� ���� data ���� 
		DSP_REV_CMD_EXT_START						 = 0x98,		//�ܺ� ���� ���� ���� 
		DSP_REV_CMD_ERR_CLEAR						 = 0x99,		//Error Code Clear ����
};

//  OP Mode - Operation Mode Setting
enum{ 
		DSP_REV_OP_STANDBY							 = 0x0,		//��� ����
		DSP_REV_OP_INPUT_READY						 = 0x1,		//�Է´� �غ� �Ϸ�(�Է´� �ʱ� ���� �Ϸ�)
		DSP_REV_OP_TEST_READY						 = 0x2,		//��/���� ���� �غ� �Ϸ�
		DSP_REV_OP_CHG_CC							 = 0x3,		//���� CC ���� ��
		DSP_REV_OP_CHG_CV							 = 0x4,		//���� CV ���� ��
		DSP_REV_OP_DISCHG_CC						 = 0x5,		//���� CC ���� ��
		DSP_REV_OP_DISCHG_CV						 = 0x4,		//������ CV ���� ��
		DSP_REV_OP_CHG_CP							 = 0x6,		//���� CP ���� ��
		DSP_REV_OP_DISCHG_CP						 = 0x7,		//���� CP ���� ��
		DSP_REV_OP_TEST_END							 = 0x8,		//���� �Ϸ�(0x8 -> 0x2)
};															
//  Parallel Mode - Parallel Mode Setting
enum{ 
		DSP_REV_PARALLEL_OFF						 = 0x0,		//���� ���� ��� ���� �Ϸ� ����
		DSP_REV_PARALLEL_READY						 = 0x1,		//���� ���� ��� �غ� ��
		DSP_REV_PARALLEL_ON							 = 0x2,		//���� ���� ��� �غ� �Ϸ� ����
};

//ERROR CODE
enum eWARNING{
	WARNING_ERROR_PACK_REVERSE_FAULT				 = 0x1,	    //�嵥�� ���ἱ 
	WARNING_ERROR_IBAT1_OVER_OPERATING				 = 0x2,		//ä��1 ���� ���� �ʰ�
	WARNING_ERROR_PACK1_OVER_OPERATING				 = 0x4,		//ä��1 ���� ���� �ʰ� 	
// 	WARNING_ERROR_IBAT2_OVER_OPERATING				 = 0x8,		//ä��2 ���� ���� �ʰ�
// 	WARNING_ERROR_PACK2_OVER_OPERATING				 = 0x10,		//ä��2 ���� ���� �ʰ� 
// 	WARNING_ERROR_IBAT3_OVER_OPERATING				 = 0x20,		//ä��3 ���� ���� �ʰ�
// 	WARNING_ERROR_PACK3_OVER_OPERATING				 = 0x40,		//ä��3 ���� ���� �ʰ� 
	WARNING_ERROR_PACK1_CONTACT_FAULT				 = 0x80,		//���͸�#1 �÷� ����
// 	WARNING_ERROR_PACK2_CONTACT_FAULT				 = 0x100,		//�嵥��#2 �÷� ����	
// 	WARNING_ERROR_PACK3_CONTACT_FAULT				 = 0x200,		//�嵥��#3 �÷� ����
// 	WARNING_ERROR_PARALLEL_MODE_FAULT				 = 0x400,		//���� ���� ��� ���ո�
// 	WARNING_ERROR_VPACK_CONTACT_FAULT				 = 0x800,		//�嵥��#1 ���� ����			//SMA,IBC,UPB 
// 	WARNING_ERROR_PC_DSP_ERROR						 = 0x2000		//PC DSP ��Ų���
};

enum eGRID{
	GRID_ERROR_PLL_ERROR	      					 = 1,       //Pll Error			// �Է� ���̺� ��� �ҷ�
	GRID_ERROR_INVERTER_A_OVERCURRENT				 = 2,       //Grid A OC			// R�� ������
	GRID_ERROR_INVERTER_B_OVERCURRENT				 = 4,       //Grid B OC			// S�� ������
	GRID_ERROR_INVERTER_C_OVERCURRENT				 = 10,      //Grid C OC			// T�� ������
	GRID_ERROR_VOUT_OVERVOLTAGE						 = 20,      //Grid OV			// �Է� ������
	GRID_ERROR_VOUT_UNDERVOLTAGE					 = 40,      //Grid UV			// �Է� ������
	GRID_ERROR_INITIAL_CHARGE_ERROR					 = 100,     //Init Charge Fault	// �ʱ� ���� �̻�
	GRID_ERROR_VQE_UNBALANCE						 = 200,     //Vqe_Unbalance		// �Է� ���� ������
//	GRID_ERROR_VDE_UNBALANCE						 = 400	    //Vde_Unbalance		// �Է� Vd���� ������		//SMA,IBC,UPB 
};

enum eDC{
	DC_ERROR_VDC_OVERVOLTAGE						 = 0x1,	    // ������ ������ �߻�
	DC_ERROR_VDC_UNBALANCE							 = 0x2,	    // ������ ���� ������
	DC_ERROR_VBAT1_OVERVOLTAGE						 = 0x4,      // �ܵ���#1 ������ �߻�
	DC_ERROR_VBAT1_UNBALANCE						 = 0x8,      // �ܵ���#1 ���� ������
	DC_ERROR_Vpack1_OVERVOLTAGE						 = 0x10,     // �嵥��#1 ������ �߻�
	DC_ERROR_IBAT1_OVERCURRENT						 = 0x20,     // �嵥��#1 ������ �߻�
// 	DC_ERROR_VBAT2_OVERVOLTAGE						 = 0x40,     // �ܵ���#2 ������ �߻�
// 	DC_ERROR_Vpack2_OVERVOLTAGE						 = 0x80,     // �嵥��#2 ������ �߻�
// 	DC_ERROR_IBAT2_OVERCURRENT						 = 0x100,    // �嵥��#2 ������ �߻�
// 	DC_ERROR_VBAT3_OVERVOLTAGE						 = 0x200,    // �ܵ���#3 ������ �߻�
// 	DC_ERROR_Vpack3_OVERVOLTAGE						 = 0x400,    // �嵥��#3 ������ �߻�
// 	DC_ERROR_IBAT3_OVERCURRENT						 = 0x800     // �嵥��#3 ������ �߻�
};

enum eFAULT{
	FAULT_ERROR_HEAT_FAULT							 = 0x1,	    //Heat Over			// �濭�� ���µ� �߻�
	FAULT_ERROR_AC_FUSE_FAULT						 = 0x2,       //Fuse Broken 		// �Է� ǻ�� �̻�
	FAULT_ERROR_DC_FUSE_FAULT						 = 0x4,       //Fuse Broken 		// ��� ǻ�� �̻�
	FAULT_ERROR_FAN_FAULT							 = 0x8,       //Fan Broken		// �濭 FAN �ҷ�
	FAULT_ERROR_EMERGENCY_STOP						 = 0x10,	    //Emergency Stop	// �������
	FAULT_ERROR_DRIVE_1_FAULT						 = 0x20,		//Drive 1 Fault		// �Է� R�� IGBT ���� �߻�
	FAULT_ERROR_DRIVE_2_FAULT						 = 0x40,		//Drive 2 Fault		// �Է� S�� IGBT ���� �߻�
	FAULT_ERROR_DRIVE_3_FAULT						 = 0x80,		//Drive 3 Fault		// �Է� T�� IGBT ���� �߻�
	FAULT_ERROR_DRIVE_4_FAULT						 = 0x100,		//ä��1 IGBT ���� �߻�
// 	FAULT_ERROR_DRIVE_5_FAULT						 = 0x200,		//ä��2 IGBT ���� �߻�
// 	FAULT_ERROR_DRIVE_6_FAULT						 = 0x400,		//ä��3 IGBT ���� �߻�
	FAULT_ERROR_COMM_CHK_FAULT						 = 0x800,		//PC-DSP ��� ���� �߻�
};
