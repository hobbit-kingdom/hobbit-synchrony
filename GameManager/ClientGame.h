#pragma once
#include "MainPlayer.h"
#include "OtherPlayer.h"
#include "MemoryAccess.h"
#include "ClientEntity.h"
#include <vector>

class ClientGame
{
private:
    // is loaded to a level
    static bool isLoaded;

    // the vector of all entities
    static std::vector<ClientEntity*> entities;

public:
    ClientGame()
    {
        std::cout << "ClientGame Constructor" << std::endl;
    }
    // manage packets
    void readPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) {
        if (!isLoaded)
            return;

        uint32_t currentFlag = 0;
        uint32_t previousFlag = 0;

        std::vector<uint32_t> storePackets;
        // 0xffffffff is a flag for manipulating data
        while (currentFlag != 0xffffffff && previousFlag != 0xffffffff) {
            previousFlag = currentFlag;
            currentFlag = packets.front();

            // st current flag
            if (currentFlag == 0xffffffff) {
                packets.erase(packets.begin());
                continue;
            }

            // multiple readers from packet
            if (previousFlag == 0xffffffff && currentFlag != 0xffffffff) {
                storePackets = packets;
                // find all readers indexes
                std::vector<uint32_t> readers;
                while (currentFlag != 0xffffffff) {
                    readers.push_back(packets.front());
                    packets.erase(packets.begin());
                    currentFlag = packets.front();
                }
                // remove the end flag from packet
                packets.erase(packets.begin());

                // read packet 
                // update the original packet
                entities[readers[0]]->readPackets(packets, playerIndex);
                // other read the packet
                for (uint32_t i = 1; i < readers.size(); ++i) {
                    std::vector<uint32_t> tempPackets(storePackets);
                    entities[readers[i]]->readPackets(tempPackets, playerIndex);
                }

                // set current flag after the reading
                currentFlag = packets.front();
                if (currentFlag == 0xffffffff) {
                    packets.erase(packets.begin());
                }
                continue;
            }

            // one reader
            uint32_t reader = packets.front();
            packets.erase(packets.begin());
            entities[reader]->readPackets(packets, playerIndex);
            // no need to set current flag as it was not the special flag
        }
    }
    std::vector<uint32_t> setPackets() {
        if (!isLoaded) {
            return std::vector<uint32_t>(2, 0xffffffff);
        }
        std::vector<uint32_t> packets;
        // get packets from all entities
        for (ClientEntity* entity : entities) {
            entity->setPackets(packets);
        }

        // end of packets
        packets.push_back(0xffffffff);
        packets.push_back(0xffffffff);
        return packets;
    }

    void openNewLevel() {
        setPtrs();
    }
    void setPtrs() {
        for (ClientEntity* entity : entities)
        {
            entity->setPtrs();
        }
    }
    void checkUpdateLevel() {
        static uint32_t isLoadingLayers;
        static uint32_t readLoadLayers;
        readLoadLayers = MemoryAccess::readData(uint32_t(0x00760864)); // 0x00760864 the loading state (1 - between lvl, 0 - in the level)
        isLoaded = !readLoadLayers;
        if (isLoadingLayers != readLoadLayers) {
            if (!readLoadLayers) {
                openNewLevel();
            }

            isLoadingLayers = readLoadLayers;
        }
    }
    bool checkInGame() {
        MemoryAccess::setExecutableName("Meridian.exe");
        return MemoryAccess::readProcess();
    }

    int getLevel() {
        return MemoryAccess::readData(0x762b5c);
    }
    // destructor
    ~ClientGame() {
        // clear the memory
        for (auto entity : entities) {
            delete entity;
        }
    }
};
