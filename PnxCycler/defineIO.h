#pragma once
#if !defined(DEFINE_IO_INCLUDED_)
#define DEFINE_IO_INCLUDED_


//I/O Map 화면 표시용 파일 경로(.ini)
//#define IOMAP_PATH				_T("C:\\SystemData\\")


//hsg Add 
#define MODULE_COUNT			32
#define MODULE_OUTPORT_START	2
#define MODULE_INPORT_START		0

#define		IO_PARA_COUNT		2
#define		IO_ADDRESS			0
#define		IO_NAME				1
//#define		IO_SLOT			2
//#define		IO_INDEX		3

#define		IO_IN_SLOT_MAX		2
#define		IO_OUTPUT_SLOT_MAX	2
#define		IO_SLOT_MAX			4

#define		IO_VALUE_COUNT		32
#define		IO_SIZE_MAX			512

#define		IO_ON				1
#define		IO_OFF				0

#define		IO_MAP_PARA			5
#define		IO_MAP_NAME			0
#define		IO_MAP_SLAVE		1
#define		IO_MAP_BIT			2
#define		IO_MAP_ENABLE		3


#define		IO_INPUT_MAX		64
#define		IO_OUTPUT_MAX		64

#define		GEAR_RATIO			1
enum InputList_DFS_AJIN
{
	//INPUT_1
	INPUT_DFS_STAGE1_CP1_TRIP
	, INPUT_DFS_STAGE1_CP2_TRIP
	, INPUT_DFS_STAGE1_CP3_TRIP
	, INPUT_DFS_STAGE1_CP4_TRIP
	, INPUT_DFS_STAGE1_CP5_TRIP
	, INPUT_DFS_STAGE1_CP6_TRIP
	, INPUT_DFS_STAGE1_CP7_TRIP
	, INPUT_DFS_STAGE1_CP8_TRIP
	, INPUT_DFS_STAGE1_CP9_TRIP
	, INPUT_DFS_STAGE1_CP10_TRIP
	, INPUT_DFS_STAGE1_SPARE1
	, INPUT_DFS_STAGE1_SPARE2
	, INPUT_DFS_STAGE1_SPARE3
	, INPUT_DFS_STAGE1_SPARE4
	, INPUT_DFS_STAGE1_SPARE5
	, INPUT_DFS_STAGE1_SPARE6
	, INPUT_DFS_STAGE1_EMO_SW
	, INPUT_DFS_STAGE1_POWER_ON_SW
	, INPUT_DFS_STAGE1_POWER_OFF_SW
	, INPUT_DFS_STAGE1_RESET_SW
	, INPUT_DFS_STAGE1_MAIN_POWER_ON_SW
	, INPUT_DFS_STAGE1_MAIN_POWER_OFF_SW
	, INPUT_DFS_STAGE1_AUTO_MODE
	, INPUT_DFS_STAGE1_TEMP_OVER_SIGNAL
	, INPUT_DFS_STAGE1_SPARE7
	, INPUT_DFS_STAGE1_SPARE8
	, INPUT_DFS_STAGE1_SPARE9
	, INPUT_DFS_STAGE1_SPARE10
	, INPUT_DFS_STAGE1_SPARE11
	, INPUT_DFS_STAGE1_SPARE12
	, INPUT_DFS_STAGE1_SPARE13
	, INPUT_DFS_STAGE1_SPARE14



	//INPUT_2
	, INPUT_DFS_STAGE1_SPARE16
	, INPUT_DFS_STAGE1_SPARE17
	, INPUT_DFS_STAGE1_SPARE18
	, INPUT_DFS_STAGE1_SPARE19
	, INPUT_DFS_STAGE1_SPARE20
	, INPUT_DFS_STAGE1_SPARE21
	, INPUT_DFS_STAGE1_SPARE22
	, INPUT_DFS_STAGE1_SPARE23
	, INPUT_DFS_STAGE1_SPARE24
	, INPUT_DFS_STAGE1_SPARE25
	, INPUT_DFS_STAGE1_SPARE26
	, INPUT_DFS_STAGE1_SPARE27
	, INPUT_DFS_STAGE1_SPARE28
	, INPUT_DFS_STAGE1_SPARE29
	, INPUT_DFS_STAGE1_SPARE30
	, INPUT_DFS_STAGE1_SPARE31
	, INPUT_DFS_STAGE1_SPARE32
	, INPUT_DFS_STAGE1_SPARE33
	, INPUT_DFS_STAGE1_SPARE34
	, INPUT_DFS_STAGE1_SPARE35
	, INPUT_DFS_STAGE1_SPARE36
	, INPUT_DFS_STAGE1_SPARE37
	, INPUT_DFS_STAGE1_SPARE38
	, INPUT_DFS_STAGE1_SPARE39
	, INPUT_DFS_STAGE1_SPARE40
	, INPUT_DFS_STAGE1_SPARE41
	, INPUT_DFS_STAGE1_SPARE42
	, INPUT_DFS_STAGE1_SPARE43
	, INPUT_DFS_STAGE1_SPARE44
	, INPUT_DFS_STAGE1_SPARE45
	, INPUT_DFS_STAGE1_SPARE46
	, INPUT_DFS_STAGE1_SPARE47

	//INPUT_1
	, INPUT_DFS_STAGE2_SPARE1 = 96
	, INPUT_DFS_STAGE2_SPARE2
	, INPUT_DFS_STAGE2_SPARE3
	, INPUT_DFS_STAGE2_SPARE4
	, INPUT_DFS_STAGE2_SPARE5
	, INPUT_DFS_STAGE2_SPARE6
	, INPUT_DFS_STAGE2_SPARE7
	, INPUT_DFS_STAGE2_SPARE8
	, INPUT_DFS_STAGE2_SPARE9
	, INPUT_DFS_STAGE2_SPARE10
	, INPUT_DFS_STAGE2_SPARE11
	, INPUT_DFS_STAGE2_SPARE12
	, INPUT_DFS_STAGE2_SPARE13
	, INPUT_DFS_STAGE2_SPARE14
	, INPUT_DFS_STAGE2_SPARE15
	, INPUT_DFS_STAGE2_SPARE16
	, INPUT_DFS_STAGE2_EMO_SW
	, INPUT_DFS_STAGE2_POWER_ON_SW
	, INPUT_DFS_STAGE2_POWER_OFF_SW
	, INPUT_DFS_STAGE2_RESET_SW
	, INPUT_DFS_STAGE2_SPARE17
	, INPUT_DFS_STAGE2_SPARE18
	, INPUT_DFS_STAGE2_SPARE19
	, INPUT_DFS_STAGE2_SPARE20
	, INPUT_DFS_STAGE2_SPARE21
	, INPUT_DFS_STAGE2_SPARE22
	, INPUT_DFS_STAGE2_SPARE23
	, INPUT_DFS_STAGE2_SPARE24
	, INPUT_DFS_STAGE2_SPARE25
	, INPUT_DFS_STAGE2_SPARE26
	, INPUT_DFS_STAGE2_SPARE27
	, INPUT_DFS_STAGE2_SPARE28

	//INPUT_2
	, INPUT_DFS_STAGE2_SPARE29
	, INPUT_DFS_STAGE2_SPARE30
	, INPUT_DFS_STAGE2_SPARE31
	, INPUT_DFS_STAGE2_SPARE32
	, INPUT_DFS_STAGE2_SPARE33
	, INPUT_DFS_STAGE2_SPARE34
	, INPUT_DFS_STAGE2_SPARE35
	, INPUT_DFS_STAGE2_SPARE36
	, INPUT_DFS_STAGE2_SPARE37
	, INPUT_DFS_STAGE2_SPARE38
	, INPUT_DFS_STAGE2_SPARE39
	, INPUT_DFS_STAGE2_SPARE40
	, INPUT_DFS_STAGE2_SPARE41
	, INPUT_DFS_STAGE2_SPARE42
	, INPUT_DFS_STAGE2_SPARE43
	, INPUT_DFS_STAGE2_SPARE44
	, INPUT_DFS_STAGE2_SPARE45
	, INPUT_DFS_STAGE2_SPARE46
	, INPUT_DFS_STAGE2_SPARE47
	, INPUT_DFS_STAGE2_SPARE48
	, INPUT_DFS_STAGE2_SPARE49
	, INPUT_DFS_STAGE2_SPARE50
	, INPUT_DFS_STAGE2_SPARE51
	, INPUT_DFS_STAGE2_SPARE52
	, INPUT_DFS_STAGE2_SPARE53
	, INPUT_DFS_STAGE2_SPARE54
	, INPUT_DFS_STAGE2_SPARE55
	, INPUT_DFS_STAGE2_SPARE56
	, INPUT_DFS_STAGE2_SPARE57
	, INPUT_DFS_STAGE2_SPARE58
	, INPUT_DFS_STAGE2_SPARE59
	, INPUT_DFS_STAGE2_SPARE60

	, INPUT_DFS_TOTAL
};

enum OutputList_DFS_AJIN
{
	// OUTPUT_1
	OUTPUT_DFS_STAGE1_SPARE1
	, OUTPUT_DFS_STAGE1_SPARE2
	, OUTPUT_DFS_STAGE1_SPARE3
	, OUTPUT_DFS_STAGE1_SPARE4
	, OUTPUT_DFS_STAGE1_SPARE5
	, OUTPUT_DFS_STAGE1_SPARE6
	, OUTPUT_DFS_STAGE1_SPARE7
	, OUTPUT_DFS_STAGE1_SPARE8
	, OUTPUT_DFS_STAGE1_SPARE9
	, OUTPUT_DFS_STAGE1_SPARE10
	, OUTPUT_DFS_STAGE1_SPARE11
	, OUTPUT_DFS_STAGE1_SPARE12
	, OUTPUT_DFS_STAGE1_SPARE13
	, OUTPUT_DFS_STAGE1_POWER_ON_SW_LAMP
	, OUTPUT_DFS_STAGE1_POWER_OFF_SW_LAMP
	, OUTPUT_DFS_STAGE1_Reset_SW_LAMP
	, OUTPUT_DFS_STAGE1_SPARE14
	, OUTPUT_DFS_STAGE1_SPARE15
	, OUTPUT_DFS_STAGE1_SPARE16
	, OUTPUT_DFS_STAGE1_SPARE17
	, OUTPUT_DFS_STAGE1_SPARE18
	, OUTPUT_DFS_STAGE1_SPARE19
	, OUTPUT_DFS_STAGE1_SPARE20
	, OUTPUT_DFS_STAGE1_TowerLamp_RED_Lamp
	, OUTPUT_DFS_STAGE1_TowerLamp_Yellow_Lamp
	, OUTPUT_DFS_STAGE1_TowerLamp_Green_Lamp
	, OUTPUT_DFS_STAGE1_TowerLamp_Blue_Lamp
	, OUTPUT_DFS_STAGE1_TowerLamp_White_Lamp
	, OUTPUT_DFS_STAGE1_TowerLamp_Buzzer	
	, OUTPUT_DFS_STAGE1_MAIN_POWER_ON_SW_Lamp
	, OUTPUT_DFS_STAGE1_MAIN_POWER_OFF_SW_Lamp
	, OUTPUT_DFS_STAGE1_Auto_Mode_Start


	// OUTPUT_2
	, OUTPUT_DFS_STAGE2_SPARE1 =96
	, OUTPUT_DFS_STAGE2_SPARE2
	, OUTPUT_DFS_STAGE2_SPARE3
	, OUTPUT_DFS_STAGE2_SPARE4
	, OUTPUT_DFS_STAGE2_SPARE5
	, OUTPUT_DFS_STAGE2_SPARE6
	, OUTPUT_DFS_STAGE2_SPARE7
	, OUTPUT_DFS_STAGE2_SPARE8
	, OUTPUT_DFS_STAGE2_SPARE9
	, OUTPUT_DFS_STAGE2_SPARE10
	, OUTPUT_DFS_STAGE2_SPARE11
	, OUTPUT_DFS_STAGE2_SPARE12
	, OUTPUT_DFS_STAGE2_SPARE13
	, OUTPUT_DFS_STAGE2_SPARE14
	, OUTPUT_DFS_STAGE2_SPARE15
	, OUTPUT_DFS_STAGE2_SPARE16
	, OUTPUT_DFS_STAGE2_SPARE17
	, OUTPUT_DFS_STAGE2_SPARE18
	, OUTPUT_DFS_STAGE2_SPARE19
	, OUTPUT_DFS_STAGE2_SPARE20
	, OUTPUT_DFS_STAGE2_SPARE21
	, OUTPUT_DFS_STAGE2_SPARE22
	, OUTPUT_DFS_STAGE2_SPARE23
	, OUTPUT_DFS_STAGE2_SPARE24
	, OUTPUT_DFS_STAGE2_POWER_ON_SW_LAMP
	, OUTPUT_DFS_STAGE2_POWER_OFF_SW_LAMP
	, OUTPUT_DFS_STAGE2_Reset_SW_LAMP
	, OUTPUT_DFS_STAGE2_SPARE25
	, OUTPUT_DFS_STAGE2_SPARE26
	, OUTPUT_DFS_STAGE2_SPARE27
	, OUTPUT_DFS_STAGE2_SPARE28
	, OUTPUT_DFS_STAGE2_SPARE29


	, OUTPUT_DFS_TOTAL
};


enum InputList_Hanwha_AJIN
{
	//INPUT_1
	INPUT_Hanwha_STAGE1_SPARE1
	, INPUT_Hanwha_STAGE1_SPARE2
	, INPUT_Hanwha_STAGE1_SPARE3
	, INPUT_Hanwha_STAGE1_SPARE4
	, INPUT_Hanwha_STAGE1_SPARE5
	, INPUT_Hanwha_STAGE1_SPARE6
	, INPUT_Hanwha_STAGE1_SPARE7
	, INPUT_Hanwha_STAGE1_SPARE8
	, INPUT_Hanwha_STAGE1_SPARE9
	, INPUT_Hanwha_STAGE1_SPARE10
	, INPUT_Hanwha_STAGE1_SPARE11
	, INPUT_Hanwha_STAGE1_SPARE12
	, INPUT_Hanwha_STAGE1_SPARE13
	, INPUT_Hanwha_STAGE1_SPARE14
	, INPUT_Hanwha_STAGE1_SPARE15
	, INPUT_Hanwha_STAGE1_SPARE16
	, INPUT_Hanwha_STAGE1_EMO_SW
	, INPUT_Hanwha_STAGE1_POWER_ON_SW
	, INPUT_Hanwha_STAGE1_POWER_OFF_SW
	, INPUT_Hanwha_STAGE1_RESET_SW
	, INPUT_Hanwha_STAGE1_SPARE17
	, INPUT_Hanwha_STAGE1_SPARE18
	, INPUT_Hanwha_STAGE1_SPARE19
	, INPUT_Hanwha_STAGE1_TRAY_INTERLOCK_CHECK_SENSOR
	, INPUT_Hanwha_STAGE1_FAN1_STATUS
	, INPUT_Hanwha_STAGE1_FAN2_STATUS
	, INPUT_Hanwha_STAGE1_FAN3_STATUS
	, INPUT_Hanwha_STAGE1_FAN4_STATUS
	, INPUT_Hanwha_STAGE1_FAN5_STATUS
	, INPUT_Hanwha_STAGE1_FAN6_STATUS
	, INPUT_Hanwha_STAGE1_FAN7_STATUS
	, INPUT_Hanwha_STAGE1_FAN8_STATUS


	//INPUT_2
	, INPUT_Hanwha_STAGE1_TRAY_CHECK_SENSOR
	, INPUT_Hanwha_STAGE1_TRAY_CENTERING_CYLINDER_1_FORWARD
	, INPUT_Hanwha_STAGE1_TRAY_CENTERING_CYLINDER_1_BACKWARD
	, INPUT_Hanwha_STAGE1_TRAY_CENTERING_CYLINDER_2_FORWARD
	, INPUT_Hanwha_STAGE1_TRAY_CENTERING_CYLINDER_2_BACKWARD
	, INPUT_Hanwha_STAGE1_JOB_CHANGE_CYLINDER_1_APPLY
	, INPUT_Hanwha_STAGE1_JOB_CHANGE_CYLINDER_1_NOT_APPLY
	, INPUT_Hanwha_STAGE1_JOB_CHANGE_CYLINDER_2_APPLY
	, INPUT_Hanwha_STAGE1_JOB_CHANGE_CYLINDER_2_NOT_APPLY
	, INPUT_Hanwha_STAGE1_TRAY_CLAMP_CYLINDER_1_UNCLAMP
	, INPUT_Hanwha_STAGE1_TRAY_CLAMP_CYLINDER_1_CLAMP
	, INPUT_Hanwha_STAGE1_TRAY_CLAMP_CYLINDER_2_UNCLAMP
	, INPUT_Hanwha_STAGE1_TRAY_CLAMP_CYLINDER_2_CLAMP
	, INPUT_Hanwha_STAGE1_PUSHER_UP
	, INPUT_Hanwha_STAGE1_PUSHER_DOWN
	, INPUT_Hanwha_STAGE1_SPARE21
	, INPUT_Hanwha_STAGE1_PROBE_UP
	, INPUT_Hanwha_STAGE1_PROBE_DOWN
	, INPUT_Hanwha_STAGE1_SMOKE_CHECK_SENSOR_1
	, INPUT_Hanwha_STAGE1_SMOKE_CHECK_SENSOR_2
	, INPUT_Hanwha_STAGE1_FIRE_CHECK_SENSOR
	, INPUT_Hanwha_STAGE1_SPARE22
	, INPUT_Hanwha_STAGE1_SPARE23
	, INPUT_Hanwha_STAGE1_SPARE24
	, INPUT_Hanwha_STAGE1_SPARE25
	, INPUT_Hanwha_STAGE1_SPARE26
	, INPUT_Hanwha_STAGE1_SPARE27
	, INPUT_Hanwha_STAGE1_SPARE28
	, INPUT_Hanwha_STAGE1_SPARE29
	, INPUT_Hanwha_STAGE1_SPARE30
	, INPUT_Hanwha_STAGE1_SPARE31
	, INPUT_Hanwha_STAGE1_SPARE32

	//INPUT_3
	, INPUT_Hanwha_STAGE2_SPARE1 = 96
	, INPUT_Hanwha_STAGE2_SPARE2
	, INPUT_Hanwha_STAGE2_SPARE3
	, INPUT_Hanwha_STAGE2_SPARE4
	, INPUT_Hanwha_STAGE2_SPARE5
	, INPUT_Hanwha_STAGE2_SPARE6
	, INPUT_Hanwha_STAGE2_SPARE7
	, INPUT_Hanwha_STAGE2_SPARE8
	, INPUT_Hanwha_STAGE2_SPARE9
	, INPUT_Hanwha_STAGE2_SPARE10
	, INPUT_Hanwha_STAGE2_SPARE11
	, INPUT_Hanwha_STAGE2_SPARE12
	, INPUT_Hanwha_STAGE2_SPARE13
	, INPUT_Hanwha_STAGE2_SPARE14
	, INPUT_Hanwha_STAGE2_SPARE15
	, INPUT_Hanwha_STAGE2_SPARE16
	, INPUT_Hanwha_STAGE2_EMO_SW
	, INPUT_Hanwha_STAGE2_POWER_ON_SW
	, INPUT_Hanwha_STAGE2_POWER_OFF_SW
	, INPUT_Hanwha_STAGE2_RESET_SW
	, INPUT_Hanwha_STAGE2_SPARE17
	, INPUT_Hanwha_STAGE2_SPARE18
	, INPUT_Hanwha_STAGE2_SPARE19
	, INPUT_Hanwha_STAGE2_TRAY_INTERLOCK_CHECK_SENSOR
	, INPUT_Hanwha_STAGE2_FAN1_STATUS
	, INPUT_Hanwha_STAGE2_FAN2_STATUS
	, INPUT_Hanwha_STAGE2_FAN3_STATUS
	, INPUT_Hanwha_STAGE2_FAN4_STATUS
	, INPUT_Hanwha_STAGE2_FAN5_STATUS
	, INPUT_Hanwha_STAGE2_FAN6_STATUS
	, INPUT_Hanwha_STAGE2_FAN7_STATUS
	, INPUT_Hanwha_STAGE2_FAN8_STATUS

	//INPUT_4
	, INPUT_Hanwha_STAGE2_TRAY_CHECK_SENSOR
	, INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_1_FORWARD
	, INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_1_BACKWARD
	, INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_2_FORWARD
	, INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_2_BACKWARD
	, INPUT_Hanwha_STAGE2_JOB_CHANGE_CYLINDER_1_APPLY
	, INPUT_Hanwha_STAGE2_JOB_CHANGE_CYLINDER_1_NOT_APPLY
	, INPUT_Hanwha_STAGE2_JOB_CHANGE_CYLINDER_2_APPLY
	, INPUT_Hanwha_STAGE2_JOB_CHANGE_CYLINDER_2_NOT_APPLY
	, INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_1_UNCLAMP
	, INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_1_CLAMP
	, INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_2_UNCLAMP
	, INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_2_CLAMP
	, INPUT_Hanwha_STAGE2_PUSHER_UP
	, INPUT_Hanwha_STAGE2_PUSHER_DOWN
	, INPUT_Hanwha_STAGE2_SPARE21
	, INPUT_Hanwha_STAGE2_PROBE_UP
	, INPUT_Hanwha_STAGE2_PROBE_DOWN
	, INPUT_Hanwha_STAGE2_SMOKE_CHECK_SENSOR_1
	, INPUT_Hanwha_STAGE2_SMOKE_CHECK_SENSOR_2
	, INPUT_Hanwha_STAGE2_FIRE_CHECK_SENSOR
	, INPUT_Hanwha_STAGE2_SPARE22
	, INPUT_Hanwha_STAGE2_SPARE23
	, INPUT_Hanwha_STAGE2_SPARE24
	, INPUT_Hanwha_STAGE2_SPARE25
	, INPUT_Hanwha_STAGE2_SPARE26
	, INPUT_Hanwha_STAGE2_SPARE27
	, INPUT_Hanwha_STAGE2_SPARE28
	, INPUT_Hanwha_STAGE2_SPARE29
	, INPUT_Hanwha_STAGE2_SPARE30
	, INPUT_Hanwha_STAGE2_SPARE31
	, INPUT_Hanwha_STAGE2_SPARE32

	, INPUT_Hanwha_TOTAL
};

enum OutputList_Hanwha_AJIN
{
	// OUTPUT_1
	OUTPUT_Hanwha_STAGE1_PUSHER_UNIT_DOWN
	, OUTPUT_Hanwha_STAGE1_PUSHER_UNIT_UP
	, OUTPUT_Hanwha_STAGE1_PROBE_UNIT_DOWN
	, OUTPUT_Hanwha_STAGE1_PROBE_UNIT_UP
	, OUTPUT_Hanwha_STAGE1_JOB_CHANGER_NOT_APPLY
	, OUTPUT_Hanwha_STAGE1_JOB_CHANGER_APPLY
	, OUTPUT_Hanwha_STAGE1_CENTERING_BACKWARD
	, OUTPUT_Hanwha_STAGE1_CENTERING_FORWARD
	, OUTPUT_Hanwha_STAGE1_CLAMP_UNCLAMP
	, OUTPUT_Hanwha_STAGE1_CLAMP_CLAMP
	, OUTPUT_Hanwha_STAGE1_SPARE1
	, OUTPUT_Hanwha_STAGE1_SPARE2
	, OUTPUT_Hanwha_STAGE1_SPARE3
	, OUTPUT_Hanwha_STAGE1_SPARE4
	, OUTPUT_Hanwha_STAGE1_SPARE5
	, OUTPUT_Hanwha_STAGE1_SPARE6
	, OUTPUT_Hanwha_STAGE1_POWER_ON_SW_LAMP
	, OUTPUT_Hanwha_STAGE1_POWER_OFF_SW_LAMP
	, OUTPUT_Hanwha_STAGE1_RESET_SW_LAMP
	, OUTPUT_Hanwha_STAGE1_TOWER_RED_LAMP
	, OUTPUT_Hanwha_STAGE1_TOWER_YELLOW_LAMP
	, OUTPUT_Hanwha_STAGE1_TOWER_GREEN_LAMP
	, OUTPUT_Hanwha_STAGE1_TOWER_BUZZER
	, OUTPUT_Hanwha_STAGE1_SPARE8
	, OUTPUT_Hanwha_STAGE1_SPARE9
	, OUTPUT_Hanwha_STAGE1_SPARE10
	, OUTPUT_Hanwha_STAGE1_SPARE11
	, OUTPUT_Hanwha_STAGE1_SPARE12
	, OUTPUT_Hanwha_STAGE1_SPARE13
	, OUTPUT_Hanwha_STAGE1_SPARE14
	, OUTPUT_Hanwha_STAGE1_SPARE15
	, OUTPUT_Hanwha_STAGE1_SPARE16

	// OUTPUT_2
	, OUTPUT_Hanwha_STAGE2_PUSHER_UNIT_DOWN = 96
	, OUTPUT_Hanwha_STAGE2_PUSHER_UNIT_UP
	, OUTPUT_Hanwha_STAGE2_PROBE_UNIT_DOWN
	, OUTPUT_Hanwha_STAGE2_PROBE_UNIT_UP
	, OUTPUT_Hanwha_STAGE2_JOB_CHANGER_NOT_APPLY
	, OUTPUT_Hanwha_STAGE2_JOB_CHANGER_APPLY
	, OUTPUT_Hanwha_STAGE2_CENTERING_BACKWARD
	, OUTPUT_Hanwha_STAGE2_CENTERING_FORWARD
	, OUTPUT_Hanwha_STAGE2_CLAMP_UNCLAMP
	, OUTPUT_Hanwha_STAGE2_CLAMP_CLAMP
	, OUTPUT_Hanwha_STAGE2_SPARE1
	, OUTPUT_Hanwha_STAGE2_SPARE2
	, OUTPUT_Hanwha_STAGE2_SPARE3
	, OUTPUT_Hanwha_STAGE2_SPARE4
	, OUTPUT_Hanwha_STAGE2_SPARE5
	, OUTPUT_Hanwha_STAGE2_SPARE6
	, OUTPUT_Hanwha_STAGE2_POWER_ON_SW_LAMP
	, OUTPUT_Hanwha_STAGE2_POWER_OFF_SW_LAMP
	, OUTPUT_Hanwha_STAGE2_RESET_SW_LAMP
	, OUTPUT_Hanwha_STAGE2_SPARE7
	, OUTPUT_Hanwha_STAGE2_SPARE8
	, OUTPUT_Hanwha_STAGE2_SPARE9
	, OUTPUT_Hanwha_STAGE2_SPARE10
	, OUTPUT_Hanwha_STAGE2_SPARE11
	, OUTPUT_Hanwha_STAGE2_SPARE12
	, OUTPUT_Hanwha_STAGE2_SPARE13
	, OUTPUT_Hanwha_STAGE2_SPARE14
	, OUTPUT_Hanwha_STAGE2_SPARE15
	, OUTPUT_Hanwha_STAGE2_SPARE16
	, OUTPUT_Hanwha_STAGE2_SPARE17
	, OUTPUT_Hanwha_STAGE2_SPARE18
	, OUTPUT_Hanwha_STAGE2_SPARE19
	, OUTPUT_Hanwha_TOTAL
};



#endif 



