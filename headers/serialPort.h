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
		bool const applyTo(const HANDLE hSerial);
	public:

		friend class SerialPort;
		SerialConfig(const BYTE byteSize, const BYTE parity, const BYTE stopBits);
		SerialConfig(const char* convNotation);
		SerialConfig(const BYTE arduinoPreset);
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
		SerialPort(const BYTE portNumber, const DWORD baudRate = CBR_9600, const SerialConfig configuration = SerialConfig(8, NOPARITY, ONESTOPBIT));
		~SerialPort();
		bool open();
		bool close();
		bool const isOpen();
		
		// IO: Misc
		size_t const available();
		bool purge();

		// IO: Read
		bool read(const LPVOID outBuffer, const size_t byteCount);
		template<typename dataType>
		bool read(dataType &dataOut) { return this->read(reinterpret_cast<LPVOID>(&dataOut), sizeof(dataType)); }

		// IO: Write
		bool write(const LPVOID inBuffer, const size_t byteCount);
		template<typename dataType>
		bool write(dataType& dataIn) { return this->write(reinterpret_cast<LPVOID>(&dataIn), sizeof(dataType)); }
	};


}


#endif
