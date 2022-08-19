#pragma once

#include "Singleton.h"
#include "AlarmMap.h"
#include "AlarmInverterMap.h"
#include "SysAlarm.h"
#include <vector>
#include <map>

//220529 YGY ä�� �� �˶��� �ƴ� ��� ���� �������� ���� �˶��� ��� �߰�
typedef struct tagALARM_ITEM
{
	tagALARM_ITEM() : code(0), text(""), level(0) {}

	void operator = (const tagALARM_ITEM & rhs)
	{
		code = rhs.code;
		text.Format(rhs.text);
		level = rhs.level;
		time = rhs.time;
	}
	int code;
	CString text;
	CString Comment;
	int level;
	CTime time;

}ALARM_ITEM;

typedef std::vector<ALARM_ITEM> tagVEALARM;

#define  ALARM_LIGHT				1
#define  ALARM_HEAVY				2
enum enumEqAlarm {
	ALARM_PLC_HEARTBIT_RECEIVE_ERROR = 0,	//PLC
	ALARM_CHAMBER_DOOR_OPEN_ERROR = 1,	//PLC
	ALARM_CHAMBER_ALARM_ERROR = 2,		//Chamber
	ALARM_STAGE1_STATUS_ALARM = 3,
	ALARM_STAGE1_TRAY_SENSOR_CHECK_OVER_TIME = 4,
	ALARM_STAGE1_CENTERING_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME = 5,
	ALARM_STAGE1_CLAMP_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME = 6,
	ALARM_STAGE1_PUSHER_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME = 7,
	ALARM_STAGE1_PROBE_CYLINDER_UP_SENSOR_CHECK_OVER_TIME = 8,
	ALARM_STAGE1_SCHDULE_SELECT_OVER_TIME = 9,
	ALARM_STAGE1_PROBE_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME = 10,
	ALARM_STAGE1_PUSHER_CYLINDER_UP_SENSOR_CHECK_OVER_TIME = 11,
	ALARM_STAGE1_CENTERING_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME = 12,
	ALARM_STAGE1_CLAMP_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME = 13,
	ALARM_STAGE1_TRAY_INTERLOCK_SENSOR_DETECT = 14,
	ALARM_STAGE2_TRAY_INTERLOCK_SENSOR_DETECT = 15,
	ALARM_STAGE2_STATUS_ALARM = 16,
	ALARM_STAGE2_TRAY_SENSOR_CHECK_OVER_TIME = 17,
	ALARM_STAGE2_CENTERING_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME = 18,
	ALARM_STAGE2_CLAMP_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME = 19,
	ALARM_STAGE2_PUSHER_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME = 20,
	ALARM_STAGE2_PROBE_CYLINDER_UP_SENSOR_CHECK_OVER_TIME = 21,
	ALARM_STAGE2_SCHDULE_SELECT_OVER_TIME = 22,
	ALARM_STAGE2_PROBE_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME = 23,
	ALARM_STAGE2_PUSHER_CYLINDER_UP_SENSOR_CHECK_OVER_TIME = 24,
	ALARM_STAGE2_CENTERING_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME = 25,
	ALARM_STAGE2_CLAMP_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME = 26,
	ALARM_EQ_STAGE1_FIRE_SENSOR_ON = 27,
	ALARM_EQ_STAGE2_FIRE_SENSOR_ON = 28,
	ALARM_EQ_STAGE1_SMOKE_SENSOR1_ON = 29,
	ALARM_EQ_STAGE2_SMOKE_SENSOR1_ON = 30,
	ALARM_EQ_STAGE1_SMOKE_SENSOR2_ON = 31,
	ALARM_EQ_STAGE2_SMOKE_SENSOR2_ON = 32,
	ALARM_EQ_STAGE1_FAN_STATUS_ALARM = 33,
	ALARM_EQ_STAGE2_FAN_STATUS_ALARM = 34,
	ALARM_EQ_IO_BOARD_DISCONNECT = 35,
	ALARM_DFS_STAGE1_HEART_BIT_RECEIVE_ERR = 36,
	ALARM_DFS_STAGE2_HEART_BIT_RECEIVE_ERR = 37,
	ALARM_DFS_STAGE1_READY_BIT_RECEIVE_ERR = 38,
	ALARM_DFS_STAGE2_READY_BIT_RECEIVE_ERR = 39,
	ALARM_DFS_STAGE1_PLC_LOAD_COMPLETE_BIT_RECEIVE_ERR = 40,
	ALARM_DFS_STAGE2_PLC_LOAD_COMPLETE_BIT_RECEIVE_ERR = 41,
	ALARM_DFS_STAGE1_PLC_UNLOAD_COMPLETE_BIT_RECEIVE_ERR = 42,
	ALARM_DFS_STAGE2_PLC_UNLOAD_COMPLETE_BIT_RECEIVE_ERR = 43,

};

class CMgrAlarm : public CSingleton< CMgrAlarm >
{
public:
	CMgrAlarm();
	virtual ~CMgrAlarm();

	void	StartThread();
	void	StopThread();

	void	GetChannelAlarm(int nChannel, int nIndex, CStringArray& arr); //Formation ���� ���
	void	GetChannelAlarmComment(int nChannel, int nIndex, CStringArray& arr); //Formation ���� ���
	void	GetStationInverterAlarm(int nStationNo,int nIndex, CStringArray& arr); //Formation ���� ���
	void	GetStationInverterAlarmComment(int nStationNo,int nIndex, CStringArray& arr); //Formation ���� ���

	void	GetChannelAlarm(int nChannel, int nIndex, __int16* pBuff);
	BOOL	ExistChannelAlarm(int nChannel);
	BOOL	ExistInverterAlarm(int nStagionNo);

	void	AddAlarm(int nChannelNumber, int nType, BYTE byAlarm);
	CString	AddAlarm(int nChannelNumber, int nType, BYTE byAlarm1, BYTE byAlarm2);
	//CString	AddAlarmCan(int nChannelNumber, int nWr, int nGrd, int nDc, int nFault);				//1013 hsm
	CString	AddInverterAlarmCan(int nStationNumber,int nAlarmType, BYTE byteAlarm2, BYTE byteAlarm1);			// PCAN

	CAlarmMap*	GetAlarmMap(int nChannelNo);
	CAlarmInverterMap* GetInverterAlarmMap(int nStationNo);
	//lyw4_1 
	void	AddSysAlarm(int nChannelNumber, CString strTitle, CString strContents, int nAlarmlevel = ALARM_LIGHT);	//�˶��߰� nAlarmlevel : Heavy �� ��� ��� �ʱ�ȭ �� ���� Light�� ��� �׳� ���� 
	void	ClearSysAlarm(int nChannelNumber); //�˶� �ʱ�ȭ



		//220529 YGY ä�� �� �˶��� �ƴ� ��� ���� �������� ���� �˶��� ��� �߰�
	BOOL	AddEqAlarm(int nCode, CString text, int level,CString comment, ALARM_ITEM* item);	//�˶��߰�
	tagVEALARM	GetEqAlarm();
	void	ClearEqAlarm();

	CList<CSysAlarm>* GetSysAlarmList(int nChannelNumber); //ä���� �˶� ��������
	
	//lyw4_1 ---------------------------------------

private:
	CWinThread*	m_pThread;

	BOOL	m_bLoop;

	// KEY = Channel, DATA = Alarm Text
	std::map< int, CAlarmMap* > m_mapAlarm;
	//lyw4_1 : KEY = Channel, �ý��� �˶� ���� map
	std::map< int, CList<CSysAlarm>* > m_mapSysAlarm;
	std::map< int, CAlarmInverterMap*> m_mapInverterAlarm;

	//220529 YGY ä�� �� �˶��� �ƴ� ��� ���� �������� ���� �˶��� ��� �߰�
	tagVEALARM m_veEqAlarm;


	//lyw4_1 ---------------------------------------------

private:
	// Alarm List
	void	LoadAlarmList();
	void	LoadAlarmInverterList();

	void	InitSysAlarmList(); //lyw4_1 : �˶��ʱ�ȭ
	void	WriteAlarmHistory(int nChannelNumber, CAlarm* pAlarm);
	void	WriteStsAlarmHistory(int nChannelNumber, CSysAlarm pAlarm);
	void	WriteInverterAlarmHistory(int nStationNumber, CAlarmInverter* pAlarm);
	void	WriteEQAlarmHistory(int nCode, CString text);

	void	Add(int nChannelNumber, CAlarmMap* pNewElement);
	void	Add_Inverter(int nStationNo, CAlarmInverterMap* pNewElement);

	void	RemoveAll();
	void	RemoveAll_Inverter();

	static UINT OnDB2AlarmThread(LPVOID pParam);
};

