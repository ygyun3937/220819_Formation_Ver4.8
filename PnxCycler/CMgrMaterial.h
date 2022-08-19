#pragma once

#include "Singleton.h"

class CMgrMaterial : public CSingleton< CMgrMaterial >
{
public:
	CMgrMaterial();
	virtual ~CMgrMaterial();

	void	IncreaseEquipTime();
	CString	GetUsingEquipTime();
private:
	unsigned long	m_lUsingEquipTime;
private:
	void	SaveUsingEquipTime();
	void	LoadUsingEquipTime();
};

