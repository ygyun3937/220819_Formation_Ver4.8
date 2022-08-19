#include "stdafx.h"
#include "CMgrMaterial.h"

CMgrMaterial::CMgrMaterial()
{
	LoadUsingEquipTime();
}

CMgrMaterial::~CMgrMaterial()
{

}

void	CMgrMaterial::IncreaseEquipTime()
{
	m_lUsingEquipTime++;

	SaveUsingEquipTime();
}

CString	CMgrMaterial::GetUsingEquipTime()
{
	long lDay		= (m_lUsingEquipTime / 60 / 60 / 60 / 24);
	long lHour		= (m_lUsingEquipTime / 60 / 60) % 24;
	long lMinute	= (m_lUsingEquipTime / 60) % 60;
	long lSecond	= m_lUsingEquipTime % 60;

	CString strUsingTime;
	strUsingTime.Format("%03dD %02dH %02dM %02dS", lDay, lHour, lMinute, lSecond);

	return strUsingTime;
}

void	CMgrMaterial::SaveUsingEquipTime()
{
	CString strFilePath;
	strFilePath.Format("%s\\Material\\UsageEquip.ini", (LPCSTR)GetExePath());

	INIWriteInt("Base", "UsageTime", m_lUsingEquipTime, strFilePath);
}

void	CMgrMaterial::LoadUsingEquipTime()
{
	CString strFilePath;
	strFilePath.Format("%s\\Material\\UsageEquip.ini", (LPCSTR)GetExePath());

	m_lUsingEquipTime = INIReadInt("Base", "UsageTime", strFilePath);
}
