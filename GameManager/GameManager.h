#pragma once
#include "MemoryAccess.h"
#include "GamePacket.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"

#include <vector>
#include <mutex>
//0x00760864: loading layers

#include <iomanip>
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
    static std::mutex guardReadPacket;
    static std::mutex guardWritePacket;
public:
    static void start()
    {
        //when started the server
    }
    static void update()
    {
        std::lock_guard<std::mutex> guard(guardUpdate);

        if (!checkGameOpen())
            return;
        MemoryAccess::udpateProcess();

        readInstanices();

      /*  uint32_t OBJECT_STACK_ADDRESS = MemoryAccess::readData(0x0076F648);
        std::cout << "The interactive thing Address:" << MemoryAccess::findObjectAddressByGUID(OBJECT_STACK_ADDRESS, 0x41F77800) << std::endl;*/


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

            // new level
            if (previousLevelLoaded != currentLevelLoaded && currentLevelLoaded)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->enterNewLevel();
                }
            }

            // exit level, 0xA: in a loaded level
            if (previousState == 0xA && currentState != 0xA)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->exitLevel();
                }
            }
            previousState = currentState;
            previousLevelLoaded = currentLevelLoaded;
        }

        // handle Update event
        {
            for (ClientEntity* e : clientEntities)
            {
                e->update();
            }
        }
    }
    
    static void readPacket(std::vector<uint32_t>& packets, uint32_t playerIndex) 
    {
        std::lock_guard<std::mutex> guard(guardUpdate);
        if (!checkGameOpen())
            return;

        // convert packets into GamePackets
        std::vector<GamePacket> gamePackets;
        gamePackets = GamePacket::packetsToGamePackets(packets);
        
        // read the GamePackets
        for (GamePacket gamePacket : gamePackets)
        {
            for (uint32_t reader : gamePacket.getReadersIndexes())
            {
                clientEntities[reader]->readPacket(gamePacket, playerIndex);
            }
        }
    }
    static std::vector<uint32_t> writePacket()
    {
        std::lock_guard<std::mutex> guard(guardUpdate);
      

        std::vector<uint32_t> packets;      // packets to send
        std::vector<uint32_t> packetsEvents;      // packets to send
        std::vector<uint32_t> entityPackets;// entity packets
        std::vector<GamePacket> gamePackets;// packeof the game
        std::vector<uint32_t> processedGamePacket;

        if (!checkGameOpen())
        {
            //indicate the end of packet
            processedGamePacket = GamePacket::lastPacket();
            packets.insert(packets.end(), processedGamePacket.begin(), processedGamePacket.end());
            return packets;

        }

        // get game packet from all entities
        for (ClientEntity* e : clientEntities)
        {
            gamePackets.push_back(e->writePacket());
            e->finishedWritePacket();
        }

        // get packet from game packet
        for (GamePacket gamePacket : gamePackets)
        {
            processedGamePacket = gamePacket.getPacket();
            if (processedGamePacket.front() == 0x0)
            {
                packets.insert(packets.end(), processedGamePacket.begin() + 1, processedGamePacket.end());
            }
            else
            {
                packetsEvents.insert(packetsEvents.end(), processedGamePacket.begin() + 1, processedGamePacket.end());
            }
        }

        //indicate the end of packet
        processedGamePacket = GamePacket::lastPacket();
        packets.insert(packets.end(), processedGamePacket.begin(), processedGamePacket.end());
        packetsEvents.insert(packetsEvents.end(), processedGamePacket.begin(), processedGamePacket.end());

        return packets;
    }
};

