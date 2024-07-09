#pragma once
#include "../GameManager/HobbitMemoryAccess.h"
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
        GamePacket gamePacket1(ReadType::Game_Snapshot, 0x3, 0x1);
        if (animBilbo == 84)
        {
            uint32_t activatedAddress = HobbitMemoryAccess::memoryAccess.readData(0x007735B0);
            uint32_t foundObject = HobbitMemoryAccess::findObjectAddressByGUID(activatedAddress);
            uint32_t PositionPredmetaX = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + foundObject);
            uint32_t PositionPredmetaY = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x4 + foundObject);
            uint32_t PositionPredmetaZ = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x8 + foundObject);
            uint32_t BBOXPredmetaX = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x30 + foundObject);
            uint32_t BBOXPredmetaY = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x34 + foundObject);
            uint32_t BBOXPredmetaZ = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x38 + foundObject);
            uint32_t BBOXPredmetaX1 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x3C + foundObject);
            uint32_t BBOXPredmetaY1 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x40 + foundObject);
            uint32_t BBOXPredmetaZ1 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0x44 + foundObject);
            uint32_t BBOXPredmetaX2 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0xEC + foundObject);
            uint32_t BBOXPredmetaY2 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0xF0 + foundObject);
            uint32_t BBOXPredmetaZ2 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0xF0 + 0x4 + foundObject);
            uint32_t BBOXPredmetaX3 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0xF0 + 0x8 + foundObject);
            uint32_t BBOXPredmetaY3 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0xF0 + 0xC + foundObject);
            uint32_t BBOXPredmetaZ3 = HobbitMemoryAccess::memoryAccess.readData(0xC + 0x8 + 0xF0 + 0x10 + foundObject);
            uint32_t XZCHTO = HobbitMemoryAccess::memoryAccess.readData(0x78 + foundObject);
            std::cout << "FOUND OBJECT: " << foundObject << std::endl;
            gamePacket1.pushBackGamePacket(activatedAddress);
            gamePacket1.pushBackGamePacket(PositionPredmetaX);
            gamePacket1.pushBackGamePacket(PositionPredmetaY);
            gamePacket1.pushBackGamePacket(PositionPredmetaZ);
            gamePacket1.pushBackGamePacket(BBOXPredmetaX);
            gamePacket1.pushBackGamePacket(BBOXPredmetaY);
            gamePacket1.pushBackGamePacket(BBOXPredmetaZ);
            gamePacket1.pushBackGamePacket(BBOXPredmetaX1);
            gamePacket1.pushBackGamePacket(BBOXPredmetaY1);
            gamePacket1.pushBackGamePacket(BBOXPredmetaZ1);
            gamePacket1.pushBackGamePacket(BBOXPredmetaX2);
            gamePacket1.pushBackGamePacket(BBOXPredmetaY2);
            gamePacket1.pushBackGamePacket(BBOXPredmetaZ2);
            gamePacket1.pushBackGamePacket(BBOXPredmetaX3);
            gamePacket1.pushBackGamePacket(BBOXPredmetaY3);
            gamePacket1.pushBackGamePacket(BBOXPredmetaZ3);
            gamePacket1.pushBackGamePacket(XZCHTO);
        }
        gamePackets.push_back(gamePacket1);
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
