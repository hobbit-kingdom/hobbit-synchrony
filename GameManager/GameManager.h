#pragma once
#include "MemoryAccess.h"
#include "GameStateManager.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"
#include <vector>

class GameManager : protected GameStateManager, protected ClientEntity
{
private:
    static std::vector<ClientEntity*> clientEntities;
    // add more as new classes made

public:
    
    static void Update()
    {
        GameStateManager::readInstanices();

        // handle new level event
        {
            static uint32_t lastState;
            static uint32_t currentState;

            currentState = GameStateManager::getGameState();

            // 0xE: open level
            if (lastState != currentState && currentState == 0xA)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->EnterNewLevel();
                }
            }
            lastState = currentState;
        }
        // handle Update event
        {
            for (ClientEntity* e : clientEntities)
            {
                e->Update();
            }
        }
    }
    static void Start()
    {
        std::string s;
        while (!checkGameOpen())
        {
            std::cout << "Press [y] when the Hobbit_2003 is open: ";
            std::cin >> s;
        }
        readInstanices();
    }

    static void readPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) 
    {
        // flags that are used for 
        uint32_t currentFlag = 0;
        uint32_t previousFlag = 0;

        // read packages loop
        std::vector<uint32_t> storePackets;
        while (currentFlag != ClientEntity::PACKAGE_FLAG && previousFlag != ClientEntity::PACKAGE_FLAG)
        {
            previousFlag = currentFlag;
            currentFlag = packets.front();

            // set current flag
            if (currentFlag == ClientEntity::PACKAGE_FLAG)
            {
                packets.erase(packets.begin());
                continue;
            }

            // multiple readers from packet
            if (previousFlag == ClientEntity::PACKAGE_FLAG && currentFlag != ClientEntity::PACKAGE_FLAG)
            {
                storePackets = packets;
                // find all readers indexes
                std::vector<uint32_t> readers;
                while (currentFlag != ClientEntity::PACKAGE_FLAG)
                {
                    readers.push_back(packets.front());
                    packets.erase(packets.begin());
                    currentFlag = packets.front();
                }
                // remove the end flag from packet
                packets.erase(packets.begin());

                // read packet 
                // update the original packet
                clientEntities[readers[0]]->ReadPackets(packets, playerIndex);
                // other read the packet
                for (uint32_t i = 1; i < readers.size(); ++i) 
                {
                    std::vector<uint32_t> tempPackets(storePackets);
                    clientEntities[readers[i]]->ReadPackets(tempPackets, playerIndex);
                }

                // set current flag after the reading
                currentFlag = packets.front();
                if (currentFlag == ClientEntity::PACKAGE_FLAG)
                {
                    packets.erase(packets.begin());
                }
                continue;
            }

            // one reader
            uint32_t reader = packets.front();
            packets.erase(packets.begin());
            clientEntities[reader]->ReadPackets(packets, playerIndex);
            // no need to set current flag as it was not the special flag
        }
    }
    static std::vector<uint32_t> setPackets()
    {

        std::vector<uint32_t> packets;

        // get packets from all entities
        for (ClientEntity* e : clientEntities)
        {
            e->SetPackets(packets);
        }

        // end of packets
        packets.push_back(ClientEntity::PACKAGE_FLAG);
        packets.push_back(ClientEntity::PACKAGE_FLAG);
        return packets;
    }
};
