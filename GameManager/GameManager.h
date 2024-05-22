#pragma once
#include "MemoryAccess.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"

#include <vector>
//0x00760864: loading layers

class GameManager : protected ClientEntity
{
private:
    // All derived classes
    static std::vector<ClientEntity*> clientEntities;
    
    
    static uint32_t gameState;
    static uint32_t currentLevel;

    static void readGameState()
    {
        gameState = MemoryAccess::readData(0x00762B58); // 0x00762B58: game state
    }
    static void readGameLevel()
    {
        currentLevel = MemoryAccess::readData(0x00762B5C);  // 00762B5C: current level
    }
public:
    static void Start()
    {
        // check if the game is open
        std::string s;
        while (!checkGameOpen())
        {
            std::cout << "Press [y] when the Hobbit_2003 is open: ";
            std::cin >> s;
        }
        //
        readInstanices();
    }
    static void Update()
    {
        readInstanices();

        // handle game states
        {
            static uint32_t lastState;
            static uint32_t currentState;

            currentState = getGameState();

            // 0xA: open level
            if (lastState != currentState && currentState == 0xA)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->EnterNewLevel();
                }
            }
            // exit level
            if (lastState == 0xA && currentState != 0xA)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->ExitLevel();
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
        std::vector<uint32_t> packets;      // packets to send
        std::vector<uint32_t> entityPackets;// entity packets

        // get packets from all entities
        for (ClientEntity* e : clientEntities)
        {
            entityPackets = e->SetPackets();
            // Add all elements from `entityPackets` to `packets` at the end of packets
            packets.insert(packets.end(), entityPackets.begin(), entityPackets.end());
        }

        // end of packets
        packets.push_back(ClientEntity::PACKAGE_FLAG);
        packets.push_back(ClientEntity::PACKAGE_FLAG);

        return packets;
    }


    static bool checkGameOpen()
    {
        MemoryAccess::setExecutableName("Meridian.exe");
        return MemoryAccess::readProcess();
    }
    static void readInstanices()
    {
        readGameState();
        readGameLevel();
    }
    

    static uint32_t getGameState()
    {
        return gameState;
    }
    static uint32_t getGameLevel()
    {
        return currentLevel;
    }
};
