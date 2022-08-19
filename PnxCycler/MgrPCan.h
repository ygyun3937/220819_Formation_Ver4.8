#pragma once

#include "PCanCom.h"

class CMgrPCan : public CSingleton<CMgrPCan>
{
public:
	CMgrPCan();
	virtual~CMgrPCan();

	void	Initialize();
	void	Dispose();

	void	StartThread();
	void	StopThread();
	
	CPCanCom*	GetAt(int nCh);							//시리얼 얻어오기
	CPCanCom*	GetStation(int nStation);

	CPCanCom*	GetAtComm(int nPrimaryKey);
	//int			GetSize();

	CList< CPCanCom*, CPCanCom* >&	GetComList();		

private:
	bool			m_bAlarm;
	CList< CPCanCom*, CPCanCom* > m_listCom;

private:
	void	RemoveAll();
	void	RemoveElement(int nPrimaryKey);
};

