#include "stdafx.h"
#include "CPidModbus.h"

CPidModbus::CPidModbus()
{
}

CPidModbus::~CPidModbus()
{
	Dispose();
}

bool CPidModbus::Initialize()
{
	InitializeVariables();

	return true;
}

bool CPidModbus::Dispose()
{
	DisconnectServer();

	return true;
}

bool CPidModbus::InitializeVariables()
{
	return true;
}

// -----------------------------------------------------------------------------

/**
 * .
 * 
 * METHOD_NAME : CreateModbus
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
bool CPidModbus::CreateModbus(CString strIpAddRess, int nPort, int nSlaveId)
{
	//CT2CA pszConvertedAnsiString(strIpAddRess);
	//std::string tempIpAddress(pszConvertedAnsiString);
	std::string tempIpAddress = std::string(CT2CA(strIpAddRess.operator LPCSTR()));

	// create a modbus object
	m_Modbus = new modbus(tempIpAddress, nPort);

	// set slave id
	m_Modbus->modbus_set_slave_id(nSlaveId);

	return true;
}

bool CPidModbus::ConnectServer()
{
	//connect with the server
	bool resultValue = m_Modbus->modbus_connect();
	if (resultValue == false)
	{
		AfxMessageBox(_T("[MODBUS] PID CONTROLLER CONNECTION FAIL. (AUTO-CALIBRATION)"));
	}
	return resultValue;
}

bool CPidModbus::DisconnectServer()
{
	// close connection and free the memory
	if (m_Modbus != nullptr)
	{

	}
	m_Modbus->modbus_close();
	return true;
}

/**
 * .
 * 
 * METHOD_NAME : ReadCoil
 * METHOD_TYPE : -
 * DESCRIPTION : ReadCoil - MODBUS FUNCTION 0x01
 * @param address     Reference Address			: 0
 * @param amount      Amount of Coils to Read	: 1
 * @param buffer      Buffer to Store Data Read from Coils
 * \return 
 */
int CPidModbus::ReadCoil(uint16_t address, uint16_t amount, bool *buffer)
{
	//bool read_coil;
	//int resultValue = m_Modbus->modbus_read_coils(address, amount, &read_coil);
	return m_Modbus->modbus_read_coils(address, amount, buffer);
}

/**
 * .
 * 
 * METHOD_NAME : ReadInputBits
 * METHOD_TYPE : -
 * DESCRIPTION : Read Input Bits(Discrete Data) - MODBUS FUNCITON 0x02
 * @param address   Reference Address
 * @param amount    Amount of Bits to Read
 * @param buffer    Buffer to store Data Read from Input Bits
 * \return 
 */
int CPidModbus::ReadInputBits(uint16_t address, uint16_t amount, bool *buffer)
{
	//bool read_bits;
	//m_Modbus->modbus_read_input_bits(address, amount, &read_bits);
	return m_Modbus->modbus_read_input_bits(address, amount, buffer);
}

/**
 * .
 * 
 * METHOD_NAME : ReadHoldingRegisters
 * METHOD_TYPE : -
 * DESCRIPTION : Read Holding Registers - MODBUS FUNCTION 0x03
 * @param address    Reference Address
 * @param amount     Amount of Registers to Read
 * @param buffer     Buffer to Store Data Read from Registers
 * \return 
 */
int CPidModbus::ReadHoldingRegisters(uint16_t address, uint16_t amount, uint16_t *buffer)
{
	// uint16_t read_holding_regs[1];
	// return m_Modbus->modbus_read_holding_registers(address, amount, read_holding_regs);
	return m_Modbus->modbus_read_holding_registers(address, amount, buffer);
}

/**
 * .
 * 
 * METHOD_NAME : ReadInputRegisters
 * METHOD_TYPE : -
 * DESCRIPTION : Read Input Registers - MODBUS FUNCTION 0x04
 * @param address     Reference Address
 * @param amount      Amount of Registers to Read
 * @param buffer      Buffer to Store Data Read from Registers
 * \return 
 */
int CPidModbus::ReadInputRegisters(uint16_t address, uint16_t amount, uint16_t *buffer)
{
	//uint16_t read_input_regs[1];
	//m_Modbus->modbus_read_input_registers(address, amount, read_input_regs);
	return m_Modbus->modbus_read_input_registers(address, amount, buffer);
}

/**
 * .
 * 
 * METHOD_NAME : WriteCoil
 * METHOD_TYPE : -
 * DESCRIPTION : Write Single Coils - MODBUS FUNCTION 0x05
 * @param address    Reference Address
 * @param to_write   Value to be Written to Coil
 * \return 
 */
int CPidModbus::WriteSingleCoil(uint16_t address, const bool &to_write)
{
	//m_Modbus->modbus_write_coil(0, true);
	return m_Modbus->modbus_write_coil(address, to_write);
}

/**
 * .
 * 
 * METHOD_NAME : WriteSingleRegister
 * METHOD_TYPE : -
 * DESCRIPTION : Write Single Register - MODBUS FUCTION 0x06
 * @param address   Reference Address
 * @param value     Value to Be Written to Register
 * \return 
 */
int CPidModbus::WriteSingleRegister(uint16_t address, const uint16_t &value)
{
	//m_Modbus->modbus_write_register(0, 123);
	return m_Modbus->modbus_write_register(address, value);
}

/**
 * .
 * 
 * METHOD_NAME : WriteMultiCoils
 * METHOD_TYPE : -
 * DESCRIPTION : Write Multiple Coils - MODBUS FUNCTION 0x0F
 * @param address  Reference Address
 * @param amount   Amount of Coils to Write
 * @param value    Values to Be Written to Coils
 * \return 
 */
int CPidModbus::WriteMultipleCoils(uint16_t address, uint16_t amount, const bool *value)
{
	//bool write_cols[4] = { true, true, true, true };
	//m_Modbus->modbus_write_coils(0, 4, write_cols);
	return m_Modbus->modbus_write_coils(address, amount, value);
}

/**
 * .
 * 
 * METHOD_NAME : WriteMultipleRegisters
 * METHOD_TYPE : -
 * DESCRIPTION : Write Multiple Registers - MODBUS FUNCION 0x10
 * @param address Reference Address
 * @param amount  Amount of Value to Write
 * @param value   Values to Be Written to the Registers
 * \return 
 */
int CPidModbus::WriteMultipleRegisters(uint16_t address, uint16_t amount, const uint16_t *value)
{
	//uint16_t write_regs[4] = { 123, 123, 123 };
	//m_Modbus->modbus_write_registers(0, 4, write_regs);
	return m_Modbus->modbus_write_registers(address, amount, value);
}
