#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include "serialPort.h"
#include <vector>

namespace serial
{
	std::vector<BYTE> availablePorts(const BYTE fromPort = 0, const BYTE toPort = 255);

}

#endif
