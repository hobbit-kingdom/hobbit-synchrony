#pragma once
#include "MemoryAccess.h"
#include "ClientEntity.h"
#include "GamePacket.h"

#include <vector>
#include <cstdint>
#include <iostream>

class MainPlayer : public ClientEntity
{

private:
    static uint32_t bilboPosXPTR;
    static uint32_t bilboAnimPTR;
    static const uint32_t X_POSITION_PTR;
    bool processPackets = false;
public:

    // packages
    void readPacket(GamePacket gamePacket, uint32_t playerIndex) override
    {
        if (!processPackets)
            return;
        // read packets
    }
    GamePacket writePacket() const override
    {
        if (!processPackets)
            return GamePacket();

        // Prepares packets to send
        uint32_t uintPosX = MemoryAccess::readData(0x7C4 + bilboPosXPTR);
        uint32_t uintPosY = MemoryAccess::readData(0x7C8 + bilboPosXPTR);
        uint32_t uintPosZ = MemoryAccess::readData(0x7CC + bilboPosXPTR);
        uint32_t uintRotY = MemoryAccess::readData(0x7AC + bilboPosXPTR);
        uint32_t animBilbo = MemoryAccess::readData(bilboAnimPTR);

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
        std::cout << "X: " << MemoryAccess::uint32ToFloat(uintPosX) << " || ";
        std::cout << "Y: " << MemoryAccess::uint32ToFloat(uintPosY) << " || ";
        std::cout << "Z: " << MemoryAccess::uint32ToFloat(uintPosZ) << " || ";
        std::cout << "R: " << MemoryAccess::uint32ToFloat(uintRotY) << " || ";
        std::cout << "A: " << animBilbo << std::endl << std::endl;
        std::cout << "\033[0m";

        return gamePacket;
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
        bilboPosXPTR = MemoryAccess::readData(X_POSITION_PTR);
        bilboAnimPTR = 0x8 + MemoryAccess::readData(0x560 + MemoryAccess::readData(X_POSITION_PTR));
    }
};
