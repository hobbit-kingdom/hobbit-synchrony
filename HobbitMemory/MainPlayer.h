#pragma once
#include <vector>
#include <cstdint>
#include "../PNet/MemoryAccess.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
class MainPlayer
{
private:
	static uint32_t bilboPosXPTR;
	static uint32_t bilboAnimPTR;
	static const uint32_t X_POSITION_PTR;
public:
	std::vector<uint32_t> readPackets(std::vector<uint32_t>& packets)
	{ return packets; }

	std::vector<uint32_t> setPacket(std::vector<uint32_t>& packets)
	{
		uint32_t uintPosX = MemoryAccess::readData(0x7C4 + bilboPosXPTR);
		uint32_t uintPosY = MemoryAccess::readData(0x7C8 + bilboPosXPTR);
		uint32_t uintPosZ = MemoryAccess::readData(0x7CC + bilboPosXPTR);
		uint32_t uintRotY = MemoryAccess::readData(0x7AC + bilboPosXPTR);
		uint32_t animBilbo = MemoryAccess::readData(bilboAnimPTR);

		packets.push_back(0x5);		 // number of elements pushed

		packets.push_back(uintPosX);
		packets.push_back(uintPosY);
		packets.push_back(uintPosZ);
		packets.push_back(uintRotY);
		packets.push_back(animBilbo);

		return packets;
	}

	static void setPtrs()
	{
		bilboPosXPTR = MemoryAccess::readData(X_POSITION_PTR);
		bilboAnimPTR = 0x8 + MemoryAccess::readData(0x560 + MemoryAccess::readData(X_POSITION_PTR));
	}
};

