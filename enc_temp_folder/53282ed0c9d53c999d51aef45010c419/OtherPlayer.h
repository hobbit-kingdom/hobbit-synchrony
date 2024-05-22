#pragma once
#include "NPC.h"
#include "ClientEntity.h"
#include <vector>
#include <cstdint>
class OtherPlayer : public ClientEntity
{
private:
	static const std::vector<uint32_t> GUIDs;//[initialize]
	static std::vector<NPC> otherPlayers;
public:
	OtherPlayer():ClientEntity()
	{
		std::cout << "~OtherPlayer Constructor" << std::endl;
	}
	void ReadPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) override
	{
		NPC& otherPlayer = otherPlayers[playerIndex];
		if (packets.empty())
			return;

		// check type
		if (packets.front() != 0x1)
		{
			packets.erase(packets.begin());
			return;
		}
		packets.erase(packets.begin());

		// check size
		if (packets.front() == 0)
		{
			packets.erase(packets.begin());
			return;
		}
		packets.erase(packets.begin());


		uint32_t uintPosX = packets.front();
		packets.erase(packets.begin());
		uint32_t uintPosY = packets.front();
		packets.erase(packets.begin());
		uint32_t uintPosZ = packets.front();
		packets.erase(packets.begin());
		uint32_t uintRotY = packets.front();
		packets.erase(packets.begin());
		uint32_t animBilbo = packets.front();
		packets.erase(packets.begin());

		std::cout << "\033[33m";
		std::cout << "Recieve the packet Send: " << std::endl;
		std::cout << "X: " << MemoryAccess::uint32ToFloat(uintPosX) << " || ";
		std::cout << "Y: " << MemoryAccess::uint32ToFloat(uintPosY) << " || ";
		std::cout << "Z: " << MemoryAccess::uint32ToFloat(uintPosZ) << " || ";
		std::cout << "R: " << MemoryAccess::uint32ToFloat(uintRotY) << " || ";
		std::cout << "A: " << animBilbo << std::endl << std::endl;
		std::cout << "\033[0m";


		// set x position
		otherPlayer.setPositionX(uintPosX);

		// set y position
		otherPlayer.setPositionY(uintPosY);

		// set z position
		otherPlayer.setPositionZ(uintPosZ);

		// set y rotation
		otherPlayer.setRotationY(uintRotY);

		// set animation
		otherPlayer.setAnimation(animBilbo);
	}

	void ReadPtrs() override {
		uint32_t arrayStartAddress = MemoryAccess::readData(0x0076F648);//0x0076F648 array address
		otherPlayers.clear();

		for (uint32_t i = 0; i < GUIDs.size(); i++)
		{
			uint32_t addressFBilbo = (uint32_t)MemoryAccess::findDataInStackHobbit(LPVOID(arrayStartAddress), 0xEFEC, 0x14, GUIDs[i]);
			otherPlayers.push_back(NPC(addressFBilbo));
		}
	};

	virtual void EnterNewLevel() { ReadPtrs(); }
};
