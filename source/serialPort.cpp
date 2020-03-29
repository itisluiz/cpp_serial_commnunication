#include "serialPort.h"
#include <string>

namespace serial
{
	#pragma region SerialConfig class implementation

	// Constructors and destructor
	SerialConfig::SerialConfig(const BYTE byteSize, const BYTE parity, const BYTE stopBits): 
		m_byteSize{ (byteSize >= 4 && byteSize <= 8) ? byteSize : static_cast<BYTE>(8)  },
		m_parity{ (parity <= SPACEPARITY) ? parity : static_cast<BYTE>(NOPARITY) },
		m_stopBits{ (stopBits <= TWOSTOPBITS) ? stopBits : static_cast<BYTE>(ONESTOPBIT) } {}

	SerialConfig::SerialConfig(const char* convNotation)
	{
		int byteSize{ convNotation[0] - '0' };
		this->m_byteSize = byteSize >= 4 && byteSize <= 8 ? byteSize : 8;
		
		switch (tolower(convNotation[1]))
		{
		case 'o':
			this->m_parity = ODDPARITY;
			break;
		case 'e':
			this->m_parity = EVENPARITY;
			break;
		case 'm':
			this->m_parity = MARKPARITY;
			break;
		case 's':
			this->m_parity = SPACEPARITY;
			break;
		default:
			this->m_parity = NOPARITY;
		}

		switch (convNotation[2])
		{
		case '2':
			this->m_stopBits = TWOSTOPBITS;
			break;
		case '5':
			this->m_stopBits = ONE5STOPBITS;
			break;
		default:
			this->m_stopBits = ONESTOPBIT;
		}

	}

	SerialConfig::SerialConfig(const BYTE arduinoPreset)
	{
		this->m_byteSize = 5 + (arduinoPreset % 0x08) / 2;
		this->m_stopBits = (arduinoPreset % 0x10 <= 0x6) ? ONESTOPBIT : TWOSTOPBITS;

		if (arduinoPreset <= 0x0E)
			this->m_parity = NOPARITY;
		else if (arduinoPreset <= 0x2E)
			this->m_parity = EVENPARITY;
		else
			this->m_parity = ODDPARITY;
	}

	SerialConfig::~SerialConfig() = default;

	// Methods: Utilities
	bool const SerialConfig::applyTo(const HANDLE hSerial)
	{
		DCB serialParams;
		if (!GetCommState(hSerial, &serialParams))
			return false;

		serialParams.ByteSize = this->m_byteSize;
		serialParams.Parity = this->m_parity;
		serialParams.StopBits = this->m_stopBits;

		if (!SetCommState(hSerial, &serialParams))
			return false;

		return true;
	}
	#pragma endregion

	#pragma region SerialPort class implementation

	// Constructors and destructor
	SerialPort::SerialPort(const BYTE portNumber, const DWORD baudRate, const SerialConfig configuration)
		: m_portNumber { portNumber }, m_baudRate{ baudRate }, m_configuration{ configuration } {};

	SerialPort::~SerialPort()
	{
		this->close();
	}

	// Methods: Getters
	bool const SerialPort::isOpen()
	{
		return this->m_open;
	}

	// Methods: Handle managing
	bool SerialPort::open()
	{
		if (this->m_open)
			return false;

		std::string portName{ "\\\\.\\COM" };
		portName.append(std::to_string(this->m_portNumber));

		this->m_hSerial = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (this->m_hSerial == INVALID_HANDLE_VALUE)
			return false;

		if (!this->m_configuration.applyTo(this->m_hSerial))
			return false;

		this->m_open = true;
		this->purge();

		return true;
	}

	bool SerialPort::close()
	{
		if (!this->m_open)
			return false;

		if (!CloseHandle(this->m_hSerial))
			return false;

		this->m_open = false;
		return true;
	}

	// Methods: IO
	size_t const SerialPort::available()
	{
		if (!this->m_open)
			return 0;

		COMSTAT portStatus;
		if (!ClearCommError(this->m_hSerial, NULL, &portStatus))
			return 0;

		return portStatus.cbInQue;
	}

	bool SerialPort::purge()
	{
		if (!this->m_open)
			return false;

		return PurgeComm(this->m_hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
	}

	bool SerialPort::read(const LPVOID outBuffer, const size_t byteCount)
	{
		if (!this->m_open)
			return false;
		
		DWORD availableBytes{ this->available() };

		if (byteCount > availableBytes)
			return false;

		DWORD readBytes{ 0 };
		ReadFile(this->m_hSerial, outBuffer, byteCount, &readBytes, NULL);

		return readBytes == byteCount;
	}

	bool SerialPort::write(const LPVOID inBuffer, const size_t byteCount)
	{
		if (!this->m_open)
			return false;

		DWORD writtenBytes{ 0 };
		WriteFile(this->m_hSerial, inBuffer, byteCount, &writtenBytes, NULL);

		return writtenBytes == byteCount;
	}
	#pragma endregion

}