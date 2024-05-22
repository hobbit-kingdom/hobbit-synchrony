#pragma once
#include "MemoryAccess.h"
#include "ClientEntity.h"
#include "GamePacket.h"

#include "NPC.h"

#include <vector>
#include <cstdint>

class OtherPlayer : public ClientEntity
{
private:
	static const std::vector<uint32_t> GUIDs;
	static std::vector<NPC> otherPlayers;
	bool processPackets;
public:
	
	// packages
	void ReadPackets(GamePacket gamePaket, uint32_t playerIndex) override
	{
		// Get Players characters
		NPC& otherPlayer = otherPlayers[playerIndex];

		// Check type
		if (gamePaket.getPacketType() != 0x1)
		{
			return;
		}
		// Check size
		if (gamePaket.getGameDataSize() == 0)
		{
			return;
		}

		// Not processing packets state
		if (!processPackets)
		{
			return;
		}

		std::vector<uint32_t> gameData = gamePaket.getGameData();
		
		// Read Packet Game Data
		uint32_t uintPosX = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t uintPosY = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t uintPosZ = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t uintRotY = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t animBilbo = gameData.front();
		gameData.erase(gameData.begin());


		// Apply data from Packets
		
		//set position, rotation, and animation
		otherPlayer.setPositionX(uintPosX);
		otherPlayer.setPositionY(uintPosY);
		otherPlayer.setPositionZ(uintPosZ);
		otherPlayer.setRotationY(uintRotY);
		otherPlayer.setAnimation(animBilbo);

		// Display the data
		std::cout << "\033[33m";
		std::cout << "Recieve the packet Send: " << std::endl;
		std::cout << "X: " << MemoryAccess::uint32ToFloat(uintPosX) << " || ";
		std::cout << "Y: " << MemoryAccess::uint32ToFloat(uintPosY) << " || ";
		std::cout << "Z: " << MemoryAccess::uint32ToFloat(uintPosZ) << " || ";
		std::cout << "R: " << MemoryAccess::uint32ToFloat(uintRotY) << " || ";
		std::cout << "A: " << animBilbo << std::endl << std::endl;
		std::cout << "\033[0m";
	}
	GamePacket SetPackets() override
	{
		if (!processPackets)
			return GamePacket();

		// sets packets to send
		return GamePacket();
	}

	// game events
	void Update() override
	{
		// updates every frame
	}
	void EnterNewLevel() override
	{
		processPackets = true;
		ReadPtrs();
	}
	void ExitLevel() override
	{
		processPackets = false;
	}

	// reads pointers data (you call it when enter new level)
	void ReadPtrs() override 
	{
		uint32_t arrayStartAddress = MemoryAccess::readData(0x0076F648);//0x0076F648 array address
		otherPlayers.clear();

		for (uint32_t i = 0; i < GUIDs.size(); i++)
		{
			uint32_t addressFBilbo = (uint32_t)MemoryAccess::findDataInStackHobbit(LPVOID(arrayStartAddress), 0xEFEC, 0x14, GUIDs[i]);
			otherPlayers.push_back(NPC(addressFBilbo));
		}
	};
};