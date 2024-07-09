#pragma once
#include "HobbitMemoryAccess.h"
#include "GamePacket.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"
#include "LevelEntity.h"
#include "PodnitiiPredmet.h"
#include <vector>
#include <mutex>
//0x00760864: loading layers

#include <iomanip>
#include <functional>

class GameManager
{
protected:

    using Listener = std::function<void()>;

    static std::thread updateThread;
    static std::atomic<bool> stopThread;
    static std::mutex guardUpdate;
    // All derived classes
    static std::vector<ClientEntity*> clientEntities;

    // events
    static std::vector<Listener> listenersEnterNewLevel;
    static std::vector<Listener> listenersExitLevel;
    static std::vector<Listener> listenersUpdate;
    static std::vector<Listener> listenersOpenGame;
    static std::vector<Listener> listenersCloseGame;

    // event functions
    static void eventEnterNewLevel() {

        HobbitMemoryAccess::setHobbitMemoryAccess();

        for (const auto& listener : listenersEnterNewLevel) 
        {
            listener();
        }
    }
    static void eventExitLevel() {

        HobbitMemoryAccess::setHobbitMemoryAccess();

        for (const auto& listener : listenersExitLevel) 
        {
            listener();
        }
    }

    static void eventOpenGame()
    {
        HobbitMemoryAccess::setHobbitMemoryAccess();
        for (const auto& listener : listenersOpenGame)
        {
            listener();
        }
    }
    static void eventCloseGame()
    {
        for (const auto& listener : listenersCloseGame)
        {
            listener();
        }
    }

    static uint32_t getGameState()
    {
        return HobbitMemoryAccess::memoryAccess.readData(0x00762B58);        
    }
    static bool getLevelLoaded()
    {
        return HobbitMemoryAccess::memoryAccess.readData(0x00760354);        
    }
    static bool getLevelFullyLoaded()
    {
        return !HobbitMemoryAccess::memoryAccess.readData(0x0076035C);       
    }
    static uint32_t getGameLevel()
    {
        return HobbitMemoryAccess::memoryAccess.readData(0x00762B5C);        
    }


    static void start()
    {
        HobbitMemoryAccess::setHobbitMemoryAccess();
    }
    static void update()
    {
        bool wasHobbitOpen = false;
        bool isHobbitOpen = false;

        uint32_t previousState = -1;
        uint32_t currentState = -1;

        bool previousLevelLoaded = false;
        bool currentLevelLoaded = false;

        bool previousLevelFullyLoaded = false;
        bool currentLevelFullyLoaded = false;
        while (!stopThread)
        {
            // update speed
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            {
                std::lock_guard<std::mutex> guard(guardUpdate);
                // check if game open
                isHobbitOpen = HobbitMemoryAccess::isGameOpen();
                if (!isHobbitOpen)
                {
                    if (isHobbitOpen != wasHobbitOpen)
                        eventCloseGame();
                    std::cout << "Waiting for HobbitTM" << std::endl;
                    wasHobbitOpen = isHobbitOpen;
                    continue;
                }
                if (wasHobbitOpen)
                {
                    eventOpenGame();
                    wasHobbitOpen = isHobbitOpen;
                }


                // handle game states
                currentState = getGameState();

                // level loaded
                currentLevelLoaded = getLevelLoaded();
                currentLevelFullyLoaded = getLevelFullyLoaded();

                // new level
                if (previousLevelFullyLoaded != currentLevelFullyLoaded && currentLevelFullyLoaded)
                {
                    eventEnterNewLevel();
                }
                if (previousLevelLoaded != currentLevelLoaded && !currentLevelLoaded)
                {
                    eventExitLevel();
                }

                previousState = currentState;
                previousLevelLoaded = currentLevelLoaded;
                previousLevelFullyLoaded = currentLevelFullyLoaded;


                // handle update event
                for (ClientEntity* e : clientEntities)
                {
                    e->update();
                }
            }
        }
    }

public:
    static void addListenerEnterNewLevel(const Listener& listener) {
        listenersEnterNewLevel.push_back(listener);
    }
    static void addListenerExitLevel(const Listener& listener) {
        listenersExitLevel.push_back(listener);
    }
    static void addListenerUpdate(const Listener& listener) {
        listenersUpdate.push_back(listener);
    }
    static void addListenerOpenGame(const Listener& listener) {
        listenersOpenGame.push_back(listener);
    }
    static void addListenerCloseGame(const Listener& listener) {
        listenersCloseGame.push_back(listener);
    }


    static void readPacket(std::vector<uint32_t>& packets, uint32_t playerIndex)
    {
        std::lock_guard<std::mutex> guard(guardUpdate);
        if (!HobbitMemoryAccess::isGameOpen())
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
    static void writePacket(std::vector<uint32_t>& snapshotPackets, std::vector<uint32_t>& eventPackets)
    {
        std::lock_guard<std::mutex> guard(guardUpdate);


        std::vector<uint32_t> entityPackets;// entity packets
        std::vector<GamePacket> gamePackets;// packeof the game
        std::vector<uint32_t> processedGamePacket;

        if (!HobbitMemoryAccess::isGameOpen())
        {
            if (snapshotPackets.empty())
                return;

            //indicate the end of packet
            processedGamePacket = GamePacket::lastPacket();
            snapshotPackets.insert(snapshotPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
            return;
        }

        // get game packet from all entities
        GamePacket pkt;
        std::vector<GamePacket> pkts;
        for (ClientEntity* e : clientEntities)
        {
            pkts = e->writePacket();
            for (GamePacket pkt : pkts)
            {
                if (pkt.getGameDataSize() != 0)
                    gamePackets.push_back(pkt);
            }
            e->finishedWritePacket();
        }

        // get packet from game packet
        for (GamePacket gamePacket : gamePackets)
        {
            processedGamePacket = gamePacket.getPacket();

            if (processedGamePacket.front() == (uint32_t)ReadType::Game_Snapshot)
            {
                snapshotPackets.insert(snapshotPackets.end(), processedGamePacket.begin() + 1, processedGamePacket.end());
            }
            else if (processedGamePacket.front() == (uint32_t)ReadType::Game_EventClient)
            {
                eventPackets.insert(eventPackets.end(), processedGamePacket.begin() + 1, processedGamePacket.end());
            }
        }

        //indicate the end of packet
        processedGamePacket = GamePacket::lastPacket();
        if (!snapshotPackets.empty())
            snapshotPackets.insert(snapshotPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
        if (!eventPackets.empty())
            eventPackets.insert(eventPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
    }


    GameManager()
    {
        start();
        updateThread = std::thread(update);
    }
    ~GameManager()
    {
        stopThread = true;
        updateThread.join();
    }
};

