#pragma once
#include <vector>
#include <iostream>
class ClientEntity
{
public:
    ClientEntity()
    {
        std::cout << "ClientEntity Constructor" << std::endl;
    }
    virtual void readPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) {}

    virtual void setPackets(std::vector<uint32_t>& packets) {}

    virtual void setPtrs() {}
};
