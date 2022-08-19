#pragma once

#include "Singleton.h"
#include "CommVector.h"
#include "PCANBasic.h"

#include <map>

enum VectorCommType
{
	VectorCommTypeCan = 0,
	VectorCommTypeLin
};

class CMgrVector : public CSingleton< CMgrVector >
{
public:
	CMgrVector();
	virtual ~CMgrVector();

	void	StartThread();
	void	StopThread();

	void	AddQueue(int nCommType, int nChannelNumber, CString strTime, int nBmsID, int nByteOrder, int nStartBit, int nBitLength, float fFactor, float fOffset, TPCANMsg& msg);

	CCommVector*	GetAt(int nChannelNumber);
private:
	std::map< int, CCommVector* > m_mapVector;
private:
	void	RemoveAll();
};

