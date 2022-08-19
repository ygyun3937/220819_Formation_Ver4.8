#pragma once

#include "Singleton.h"
#include <vector>

enum eChamberStatus
{
	eChamberStatusOff = 0,
	eChamberStatusOn,
	eChamberStatusToHigh,	// �¿�
	eChamberStatusToLow		// �ð�
};

struct SChamber
{
	int		nStatus = 0;	// ���� ����
	int		nNumber = 0;

	float	fCurrentValue = 0;
	float	fSettingValue = 0;	
	float	fManualSettingValue = 0;

	bool	bSendOnCommand = false;
	bool	bSendOffCommand = false;
	bool	bSendChangeValue = false;
	bool	bChamberExec = false;	// è�� On / Off Flag
	bool	bChamberAlarm = false;
	int		nAlarmCode = 0;

	CString	strName;

	std::vector<CString> vecCyclerChannel;
	//bViewFlag : View �������θ� Chamber ������ �������� ���
	void	GetChamberInfo(std::vector<CString>& vecValue, bool bViewFlag = false);
	
};

class CMgrChamber : public CSingleton< CMgrChamber >
{
public:
	CMgrChamber();
	virtual ~CMgrChamber();

	SChamber*	GetDevice(int nChamberNumber);
	SChamber*	GetDeivceCyclerChannel(int nCyclerChannelNumber);

	std::vector<SChamber*>&	GetDeviceChamber() { return m_vecDeviceChamber; }
private:
	std::vector<SChamber*>	m_vecDeviceChamber;
private:
	void	LoadChamber();
};

