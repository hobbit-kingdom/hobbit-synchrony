#pragma once
#include "ClientEntity.h"
#include "../GameManager/MemoryAccess.h"
#include "../GameManager/HobbitMemoryAccess.h"
#include "GamePacket.h"

#include <vector>
#include <cstdint>
#include <iostream>

class PodnitiiPredmet : public ClientEntity
{

private:
    static std::atomic<bool> processPackets;

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

		std::vector<uint32_t> gameData = gamePacket.getGameData();

		// Read Packet Game Data
		uint32_t PredmetGUID = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t PredmetX = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t PredmetY = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t PredmetZ = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetX = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetY = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetZ = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetX1 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetY1 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetZ1 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetX2 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetY2 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetZ2 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetX3 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetY3 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t BBOXPredmetZ3 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t XZCHTO = gameData.front();
		gameData.erase(gameData.begin());

		uint32_t foundObject = HobbitMemoryAccess::findObjectAddressByGUID(PredmetGUID);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + foundObject, PredmetX);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x4 + foundObject, PredmetY);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x8 + foundObject, PredmetZ);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x30 + foundObject, BBOXPredmetX);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x34 + foundObject, BBOXPredmetY);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x38 + foundObject, BBOXPredmetZ);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x3C + foundObject, BBOXPredmetX1);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x40 + foundObject, BBOXPredmetY1);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x44 + foundObject, BBOXPredmetZ1);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0xEC + foundObject, BBOXPredmetX2);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0xF0 + foundObject, BBOXPredmetY2);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0xF0 + 0x4 + foundObject, BBOXPredmetZ2);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0xF0 + 0x8 + foundObject, BBOXPredmetX3);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0xF0 + 0xC + foundObject, BBOXPredmetY3);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0xF0 + 0x10 + foundObject, BBOXPredmetZ3);
		HobbitMemoryAccess::memoryAccess.writeData(0x78 + foundObject, XZCHTO);
		// Display the data
		std::cout << "\033[33m";
		std::cout << "Recieve the packet Send: " << std::endl;
		std::cout << foundObject << std::endl;
		std::cout << "PredmetGuid: " << PredmetGUID << std::endl;
		std::cout << "PredmetX: " << PredmetX << std::endl;
		std::cout << "PredmetY: " << PredmetY << std::endl;
		std::cout << "PredmetZ: " << PredmetZ << std::endl;
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
        // calls each frame
    }
    void enterNewLevel() override
    {
        processPackets = true;
        readPtrs();
    }
    void exitLevel() override
    {
        processPackets = false;
    }
    // reads pointers data (usually when enter new level)
    void readPtrs() override {
    }
};

