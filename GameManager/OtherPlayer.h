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
public:
	
	// packages
	void readPacket(GamePacket gamePacket, uint32_t playerIndex) override
	{
		// Check type
		if (gamePacket.getPacketType() != 0x1)
			return;
		// Check size
		if (gamePacket.getGameDataSize() == 0)
			return;
		// Not processing packets state
		if (!processPackets)
			return;

		// Get Players characters
		NPC& otherPlayer = otherPlayers[playerIndex];

		std::vector<uint32_t> gameData = gamePacket.getGameData();
		
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
	std::vector<GamePacket> writePacket() const override
	{
		if (!processPackets)
			return std::vector<GamePacket>();

		// sets packets to send
		return std::vector<GamePacket>();
	}

	// game events
	void update() override
	{
		// updates every frame
	}
	void enterNewLevel() override
	{
		readPtrs();
		processPackets = true;
	}
	void exitLevel() override
	{
		processPackets = false;
	}

	void readPtrs() override 
	{
		otherPlayers.clear();
		for (uint32_t i = 0; i < GUIDs.size(); i++)
		{
			otherPlayers.push_back(NPC(GUIDs[i]));
		}
	};
};