#pragma once
//include headers from derived classes

#include <iostream>
#include <vector>


class ClientEntity
{
private:
protected:
    static const uint32_t PACKAGE_FLAG = 0xffffffff;

public:
    // packages
    virtual void ReadPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) = 0;
    virtual std::vector<uint32_t> SetPackets() = 0;
    // game events
    virtual void Update() = 0;
    virtual void EnterNewLevel() = 0;
    virtual void ExitLevel() = 0;
    // reads pointers data (usually when enter new level)
    virtual void ReadPtrs() = 0;
};
