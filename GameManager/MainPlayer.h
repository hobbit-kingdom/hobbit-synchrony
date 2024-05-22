#pragma once
#include "MemoryAccess.h"
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
    bool processPackets = false;
public:

    // packages
    void ReadPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) override
    {
        if (!processPackets)
            return;
        // read packets
    }
    std::vector<uint32_t> SetPackets() override
    {
        if (!processPackets)
            return std::vector<uint32_t>();
        std::vector<uint32_t> packets;
        // Prepares packets to send
        uint32_t uintPosX = MemoryAccess::readData(0x7C4 + bilboPosXPTR);
        uint32_t uintPosY = MemoryAccess::readData(0x7C8 + bilboPosXPTR);
        uint32_t uintPosZ = MemoryAccess::readData(0x7CC + bilboPosXPTR);
        uint32_t uintRotY = MemoryAccess::readData(0x7AC + bilboPosXPTR);
        uint32_t animBilbo = MemoryAccess::readData(bilboAnimPTR);

        // Set the packet
        packets.push_back(0x1); // reader: OtherPlayers 0x0

        packets.push_back(0x1); // type 0x1: position, rotation and animation
        packets.push_back(0x5); // number of elements pushed

        packets.push_back(uintPosX);
        packets.push_back(uintPosY);
        packets.push_back(uintPosZ);
        packets.push_back(uintRotY);
        packets.push_back(animBilbo);

        // Display the packets
        std::cout << "\033[32m";
        std::cout << "Packet Send: " << std::endl;
        std::cout << "X: " << MemoryAccess::uint32ToFloat(uintPosX) << " || ";
        std::cout << "Y: " << MemoryAccess::uint32ToFloat(uintPosY) << " || ";
        std::cout << "Z: " << MemoryAccess::uint32ToFloat(uintPosZ) << " || ";
        std::cout << "R: " << MemoryAccess::uint32ToFloat(uintRotY) << " || ";
        std::cout << "A: " << animBilbo << std::endl << std::endl;
        std::cout << "\033[0m";

        return packets;
    }

    // game events
    void Update() override 
    {
        // calls each frame
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
    // reads pointers data (usually when enter new level)
    void ReadPtrs() override {
        bilboPosXPTR = MemoryAccess::readData(X_POSITION_PTR);
        bilboAnimPTR = 0x8 + MemoryAccess::readData(0x560 + MemoryAccess::readData(X_POSITION_PTR));
    }
};
