#pragma once

#include "./include/modbus.h"

class CPidModbus
{
public:
	CPidModbus();
	virtual ~CPidModbus();

public:
	bool	Initialize();
	bool	Dispose();

private:
	bool	InitializeVariables();

	// -------------------------------------------------------------------------

public:
	bool	CreateModbus(CString strIpAddRess, int nPort, int nSlaveId);

	bool	ConnectServer();
	bool	DisconnectServer();

	int		ReadCoil(uint16_t address, uint16_t amount, bool *buffer);
	int		ReadInputBits(uint16_t address, uint16_t amount, bool *buffer);
	int		ReadHoldingRegisters(uint16_t address, uint16_t amount, uint16_t *buffer);
	int		ReadInputRegisters(uint16_t address, uint16_t amount, uint16_t *buffer);

	int		WriteSingleCoil(uint16_t address, const bool &to_write);
	int		WriteSingleRegister(uint16_t address, const uint16_t &value);
	int		WriteMultipleCoils(uint16_t address, uint16_t amount, const bool *value);
	int		WriteMultipleRegisters(uint16_t address, uint16_t amount, const uint16_t *value);

	// -------------------------------------------------------------------------

private:
	modbus*	m_Modbus;
};

