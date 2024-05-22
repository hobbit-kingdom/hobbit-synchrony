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
    // virtual events 

    // packages
    virtual void ReadPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) {}
    virtual void SetPackets(std::vector<uint32_t>& packets) {}

    // game events
    virtual void Update() {}
    virtual void EnterNewLevel() {};

    // reads pointers data (usually when enter new level)
    virtual void ReadPtrs() {}
};
