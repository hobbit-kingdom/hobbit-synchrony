#pragma once
#include "ClientEntity.h"
#include "MemoryAccess.h"
#include "HobbitMemoryAccess.h"
#include "GamePacket.h"

#include <vector>
#include <cstdint>
#include <iostream>

class AllNPC : public ClientEntity
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

		while ((!gameData.empty()))
		{
			uint32_t PredmetGUID = gameData.front();
			gameData.erase(gameData.begin());
			uint32_t HpNPC = gameData.front();
			gameData.erase(gameData.begin());
			uint32_t foundObject = HobbitMemoryAccess::findObjectAddressByGUID(PredmetGUID);
			HobbitMemoryAccess::memoryAccess.writeData(0x28C + foundObject, HpNPC);
		}

		
	}
	/*std::vector<GamePacket> writePacket() const override
	{
		if (!processPackets)
			return std::vector<GamePacket>();

		// sets packets to send
		return std::vector<GamePacket>();
	}
	*/
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
