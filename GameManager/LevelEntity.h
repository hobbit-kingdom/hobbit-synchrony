#pragma once
#include "ClientEntity.h"
#include "MemoryAccess.h"
#include "HobbitMemoryAccess.h"
#include "GamePacket.h"

#include <vector>
#include <cstdint>
#include <iostream>

class LevelEntity : public ClientEntity
{

private:
    static uint32_t bilboPosXPTR;
    static uint32_t bilboAnimPTR;
    static const uint32_t X_POSITION_PTR;
    bool processPackets = false;
    static bool isSentExitLevel;

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
        //NPC& otherPlayer = otherPlayers[playerIndex];

        std::vector<uint32_t> gameData = gamePacket.getGameData();

        // Read Packet Game Data
        uint32_t enteredLevel = gameData.front();
        gameData.erase(gameData.begin());

        std::cout << "NOTIFY NEW LEVEL" << std::endl;


        // Apply data from Packets

        //IF level greated then mine
        //  go to next level
        //  update my current level
        //  go back if the level is not the same as reciever
     
    }
    std::vector<GamePacket> writePacket() const override
    {
        if (!processPackets)
            return std::vector<GamePacket>();
        std::vector<GamePacket> gamePackets;
        if (!isSentExitLevel)
        {
            GamePacket gamePacket(ReadType::Game_EventClient, 0x2, 0x1);// first read Type, second is reader, third is type 

            uint32_t currentLevel = HobbitMemoryAccess::memoryAccess.readData(0x7C4);
            gamePacket.pushBackGamePacket(currentLevel);

            gamePackets.push_back(gamePacket);
            std::cout << "ENTERD NEW LEVEL" << std::endl;
            isSentExitLevel = true;
        }

        return gamePackets;
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
        isSentExitLevel = false;
        processPackets = false;
    }
    // reads pointers data (usually when enter new level)
    void readPtrs() override {
    }
};

