#pragma once
#include "HobbitMemoryAccess.h"
#include "GamePacket.h"
#include "ClientEntity.h"

#include <vector>
#include <cstdint>
#include <iostream>

class MainPlayer : public ClientEntity
{

private:
    static uint32_t bilboPosXPTR;
    static uint32_t bilboAnimPTR;
    static const uint32_t X_POSITION_PTR;
    static std::atomic<bool> processPackets;
public:

    // packages
    void readPacket(GamePacket gamePacket, uint32_t playerIndex) override
    {
        if (!processPackets)
            return;
        // read packets
    }
    std::vector<GamePacket>  writePacket() const override
    {
        if (!processPackets)
            return std::vector<GamePacket>();
        std::vector<GamePacket> gamePackets;

        // Prepares packets to send
        uint32_t uintPosX = HobbitMemoryAccess::memoryAccess.readData(0x7C4 + bilboPosXPTR);
        uint32_t uintPosY = HobbitMemoryAccess::memoryAccess.readData(0x7C8 + bilboPosXPTR);
        uint32_t uintPosZ = HobbitMemoryAccess::memoryAccess.readData(0x7CC + bilboPosXPTR);
        uint32_t uintRotY = HobbitMemoryAccess::memoryAccess.readData(0x7AC + bilboPosXPTR);
        uint32_t animBilbo = HobbitMemoryAccess::memoryAccess.readData(bilboAnimPTR);

        // Set the packet
        GamePacket gamePacket(ReadType::Game_Snapshot, 0x1, 0x1);// first read Type, second is reader, third is type 

        gamePacket.pushBackGamePacket(uintPosX);
        gamePacket.pushBackGamePacket(uintPosY);
        gamePacket.pushBackGamePacket(uintPosZ);
        gamePacket.pushBackGamePacket(uintRotY);
        gamePacket.pushBackGamePacket(animBilbo);

        // Display the packets
        std::cout << "\033[32m";
        std::cout << "Packet Send: " << std::endl;
        std::cout << "X: " << HobbitMemoryAccess::memoryAccess.uint32ToFloat(uintPosX) << " || ";
        std::cout << "Y: " << HobbitMemoryAccess::memoryAccess.uint32ToFloat(uintPosY) << " || ";
        std::cout << "Z: " << HobbitMemoryAccess::memoryAccess.uint32ToFloat(uintPosZ) << " || ";
        std::cout << "R: " << HobbitMemoryAccess::memoryAccess.uint32ToFloat(uintRotY) << " || ";
        std::cout << "A: " << animBilbo << std::endl << std::endl;
        std::cout << "\033[0m";
        
        gamePackets.push_back(gamePacket);
        return gamePackets;
    }

    // game events
    void update() override 
    {
        // calls each frame
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
    // reads pointers data (usually when enter new level)
    void readPtrs() override {
        bilboPosXPTR = HobbitMemoryAccess::memoryAccess.readData(X_POSITION_PTR);
        bilboAnimPTR = 0x8 + HobbitMemoryAccess::memoryAccess.readData(0x560 + HobbitMemoryAccess::memoryAccess.readData(X_POSITION_PTR));
    }
};
