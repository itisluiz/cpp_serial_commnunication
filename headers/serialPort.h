#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <Windows.h>

namespace serial
{

	class SerialConfig
	{
		BYTE m_byteSize;
		BYTE m_parity;
		BYTE m_stopBits;
		bool const applyTo(HANDLE hSerial);
	public:

		friend class SerialPort;
		SerialConfig(BYTE byteSize = 8, BYTE parity = NOPARITY, BYTE stopBits = ONESTOPBIT);
		SerialConfig(const char* convNotation);
		~SerialConfig();
	};

	class SerialPort
	{
		HANDLE m_hSerial{ nullptr };
		bool m_open{ false };
		BYTE m_portNumber;
		DWORD m_baudRate;
		SerialConfig m_configuration;
		
	public:
		SerialPort(BYTE portNumber, DWORD baudRate = CBR_9600, SerialConfig configuration = SerialConfig());
		~SerialPort();
		bool const isOpen();
		bool open();
		bool close();
		DWORD const available();	
		// IO: Read
		bool const read(LPVOID outBuffer, size_t byteCount);
		template<typename dataType>
		bool const read(dataType &dataOut) { return this->read(reinterpret_cast<LPVOID>(&dataOut), sizeof(dataType)); }
		
		// IO: Write
		bool const write(LPVOID inBuffer, size_t byteCount);
		template<typename dataType>
		bool const write(dataType& dataIn) { return this->write(reinterpret_cast<LPVOID>(&dataIn), sizeof(dataType)); }
	};


}


#endif
