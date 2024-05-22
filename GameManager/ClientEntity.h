#pragma once
#include "GamePacket.h"
#include <iostream>
#include <vector>


class ClientEntity
{
private:
protected:

public:
    // packages
    virtual void ReadPackets(GamePacket gamePacket, uint32_t playerIndex) = 0;
    virtual GamePacket SetPackets() = 0;
    // game events
    virtual void Update() = 0;
    virtual void EnterNewLevel() = 0;
    virtual void ExitLevel() = 0;
    // reads pointers data (usually when enter new level)
    virtual void ReadPtrs() = 0;
};
