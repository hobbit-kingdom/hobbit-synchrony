#pragma once
#include "ClientEntity.h"
#include "MemoryAccess.h"
#include "HobbitMemoryAccess.h"
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
		uint32_t LBOXPredmetX = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t LBOXPredmetY = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t LBOXPredmetZ = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t LBOXPredmetX1 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t LBOXPredmetY1 = gameData.front();
		gameData.erase(gameData.begin());
		uint32_t LBOXPredmetZ1 = gameData.front();
		gameData.erase(gameData.begin());

		uint32_t foundObject = HobbitMemoryAccess::findObjectAddressByGUID(PredmetGUID);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + foundObject, PredmetX);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x4 + foundObject, PredmetY);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x8 + foundObject, PredmetZ);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x30 + foundObject, LBOXPredmetX);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x34 + foundObject, LBOXPredmetY);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x38 + foundObject, LBOXPredmetZ);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x3C + foundObject, LBOXPredmetX1);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x40 + foundObject, LBOXPredmetY1);
		HobbitMemoryAccess::memoryAccess.writeData(0xC + 0x8 + 0x44 + foundObject, LBOXPredmetZ1);
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

