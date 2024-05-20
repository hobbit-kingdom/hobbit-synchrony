#pragma once
#include <vector>
#include <cstdint>
#include "MemoryAccess.h"
#include "ClientEntity.h"
#include <iostream>


class MainPlayer : public ClientEntity
{
private:
    static uint32_t bilboPosXPTR;
    static uint32_t bilboAnimPTR;
    static const uint32_t X_POSITION_PTR;

public:
    MainPlayer()
    {
        std::cout << "MainPlayer Constructor" << std::endl;
    }
    void setPackets(std::vector<uint32_t>& packets) override {
        uint32_t uintPosX = MemoryAccess::readData(0x7C4 + bilboPosXPTR);
        uint32_t uintPosY = MemoryAccess::readData(0x7C8 + bilboPosXPTR);
        uint32_t uintPosZ = MemoryAccess::readData(0x7CC + bilboPosXPTR);
        uint32_t uintRotY = MemoryAccess::readData(0x7AC + bilboPosXPTR);
        uint32_t animBilbo = MemoryAccess::readData(bilboAnimPTR);

        //display the packets
        std::cout << "\033[32m";
        std::cout << "Packet Send: " << std::endl;
        std::cout << "X: " << MemoryAccess::uint32ToFloat(uintPosX) << " || ";
        std::cout << "Y: " << MemoryAccess::uint32ToFloat(uintPosY) << " || ";
        std::cout << "Z: " << MemoryAccess::uint32ToFloat(uintPosZ) << " || ";
        std::cout << "R: " << MemoryAccess::uint32ToFloat(uintRotY) << " || ";
        std::cout << "A: " << animBilbo << std::endl << std::endl;
        std::cout << "\033[0m";

        // set the packet
        packets.push_back(0x1); // reader 0x1: OtherPlayers
        packets.push_back(0x1); // type 0x1: position, rotation and animation
        packets.push_back(0x5); // number of elements pushed


        packets.push_back(uintPosX);
        packets.push_back(uintPosY);
        packets.push_back(uintPosZ);
        packets.push_back(uintRotY);
        packets.push_back(animBilbo);
    }

    void setPtrs() override {
        bilboPosXPTR = MemoryAccess::readData(X_POSITION_PTR);
        bilboAnimPTR = 0x8 + MemoryAccess::readData(0x560 + MemoryAccess::readData(X_POSITION_PTR));
    }
};
