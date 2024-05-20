#pragma once
#include "NPC.h"
#include <vector>
#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
class OtherPlayer : public NPC
{
private:
	bool isUsed = false;
	static const std::vector<uint32_t> GUIDs ;//[initialize]

public:

	OtherPlayer(uint32_t addresOfNPC) : NPC(addresOfNPC){	}

	// isUsed
	bool getIsUsed() { return isUsed; }
	void setIsUsed(bool newIsUsed) { isUsed = newIsUsed; }

	static std::vector<OtherPlayer> setPtrs()
	{
		std::vector<OtherPlayer> otherPlayers;
		uint32_t arrayStartAddress = MemoryAccess::readData(0x0076F648);//0x0076F648 array address
		for (uint32_t i = 0; i < GUIDs.size(); i++)
		{
			uint32_t addressFBilbo = MemoryAccess::findDataInStackHobbit(arrayStartAddress, 0xEFEC, 0x14, GUIDs[i]);
			otherPlayers.push_back(OtherPlayer(addressFBilbo));
		}
		return otherPlayers;
	}

	std::vector<uint32_t> readPackets(std::vector<uint32_t>& packets)
	{
		if (packets.empty())
			return packets;

		// check size
		if (packets.front() == 0)
		{
			packets.erase(packets.begin());
			return packets;
		}

		// set x position
		setPositionX(packets.front());
		packets.erase(packets.begin());

		// set y position
		setPositionY(packets.front());
		packets.erase(packets.begin());

		// set z position
		setPositionZ(packets.front());
		packets.erase(packets.begin());

		// set y rotation
		setRotationY(packets.front());
		packets.erase(packets.begin());

		// set animation
		setAnimation(packets.front());
		packets.erase(packets.begin());
	}
	std::vector<uint32_t> setPacket(std::vector<uint32_t>& packets)
	{
		packets.push_back(0); // number of elements pushed
		return packets;
	}
};

