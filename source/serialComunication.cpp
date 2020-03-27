#include "serialComunication.h"

namespace serial
{
	std::vector<BYTE> availablePorts(const BYTE fromPort, const BYTE toPort)
	{
		std::vector<BYTE> portVector;

		for (BYTE curPort{ fromPort }; curPort < toPort; ++curPort)
		{
			SerialPort probingPort(curPort);
			probingPort.open();

			if (probingPort.isOpen())
				portVector.push_back(curPort);

			probingPort.close();
		}

		return portVector;
	}


}