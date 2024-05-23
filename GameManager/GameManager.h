#pragma once
#include "MemoryAccess.h"
#include "GamePacket.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"

#include <vector>
#include <mutex>
//0x00760864: loading layers

class GameManager
{
private:
    // All derived classes
    static std::vector<ClientEntity*> clientEntities;
    

    static uint32_t gameState;
    static bool levelLoaded;
    static uint32_t currentLevel;

    static void readGameState()
    {
        gameState = MemoryAccess::readData(0x00762B58); // 0x00762B58: game state address
    }
    static void readGameLevel()
    {
        currentLevel = MemoryAccess::readData(0x00762B5C);  // 00762B5C: current level address
    }
    static void readLevelLoaded()
    {
        levelLoaded = !MemoryAccess::readData(0x0072C7D4);  //0x0072C7D4: is loaded level address
    }


    static uint32_t getGameState()
    {
        return gameState;
    }
    static bool getLevelLoaded()
    {
        return levelLoaded;
    }
    static uint32_t getGameLevel()
    {
        return currentLevel;
    }


    static bool checkGameOpen()
    {
        MemoryAccess::setExecutableName("Meridian.exe");
        return MemoryAccess::readProcess();
    }
    static void readInstanices()
    {
        readGameState();
        readLevelLoaded();
        readGameLevel();
    }
    static std::mutex guardUpdate;
public:
    static void Start()
    {
        //when started the server
    }
    static void Update()
    {
        std::lock_guard<std::mutex> guard(guardUpdate);
        //temporary
       // uint32_t OBJECT_STACK_ADDRESS = MemoryAccess::readData(0x0076F648);
        //std::cout << "The interactive thing Address:" << MemoryAccess::findObjectAddressByGUID(OBJECT_STACK_ADDRESS, 0x41F77800) << std::endl;

        if (!checkGameOpen())
        {
            return;
        }

        readInstanices();

        // handle game states
        {
            // game state
            static uint32_t previousState;
            static uint32_t currentState;
            currentState = getGameState();

            // level loaded
            static bool previousLevelLoaded;
            static bool currentLevelLoaded;
            currentLevelLoaded = getLevelLoaded();

            if (previousLevelLoaded != currentLevelLoaded && currentLevelLoaded)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->EnterNewLevel();
                }
            }

            // exit level, 0xA: in a loaded level
            if (previousState == 0xA && currentState != 0xA)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->ExitLevel();
                }
            }
            previousState = currentState;
            previousLevelLoaded = currentLevelLoaded;
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
        // convert packets into GamePackets
        std::vector<GamePacket> gamePackets;
        gamePackets = GamePacket::packetsToGamePackets(packets);
        
        // read the GamePackets
        for (GamePacket gamePacket : gamePackets)
        {
            for (uint32_t reader : gamePacket.getReadersIndexes())
            {
                clientEntities[reader]->ReadPackets(gamePacket, playerIndex);
            }
        }
    }
    static std::vector<uint32_t> setPackets()
    {
        std::vector<uint32_t> packets;      // packets to send
        std::vector<uint32_t> entityPackets;// entity packets
        std::vector<GamePacket> gamePackets;// packeof the game

        // get packets from all entities
        for (ClientEntity* e : clientEntities)
        {
            gamePackets.push_back(e->SetPackets());
        }

        // end of packets
        std::vector<uint32_t> processedGamePacket;
        for (GamePacket gamePacket : gamePackets)
        {
            processedGamePacket = gamePacket.getPacket();
            packets.insert(packets.end(), processedGamePacket.begin(), processedGamePacket.end());
        }

        //indicate the end of packet
        processedGamePacket = GamePacket::lastPacket();
        packets.insert(packets.end(), processedGamePacket.begin(), processedGamePacket.end());

        return packets;
    }
};
