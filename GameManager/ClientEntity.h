#pragma once
#include "GamePacket.h"
#include <iostream>
#include <vector>


class ClientEntity
{
private:
protected:
    std::atomic<bool> processPackets{ false };
public:
    // packages
    virtual void ReadPacket(GamePacket gamePacket, uint32_t playerIndex) = 0;
    virtual GamePacket WritePacket() const = 0;
    virtual void FinishedWritePacket() {}

    // game events
    virtual void Update() = 0;
    virtual void EnterNewLevel() = 0;
    virtual void ExitLevel() = 0;

    // reads pointers data (usually when enter new level)
    virtual void ReadPtrs() = 0;
};
