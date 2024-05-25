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
    virtual void readPacket(GamePacket gamePacket, uint32_t playerIndex) = 0;
    virtual GamePacket writePacket() const = 0;
    virtual void finishedWritePacket() {}

    // game events
    virtual void update() = 0;
    virtual void enterNewLevel() = 0;
    virtual void exitLevel() = 0;

    // reads pointers data (usually when enter new level)
    virtual void readPtrs() = 0;
};
