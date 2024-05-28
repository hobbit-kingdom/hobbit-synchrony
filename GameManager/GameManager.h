#pragma once
#include "HobbitMemoryAccess.h"
#include "GamePacket.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"
#include "LevelEntity.h"

#include <vector>
#include <mutex>
//0x00760864: loading layers

#include <iomanip>
#include <functional>

class GameManager
{
private:

    using Listener = std::function<void()>;

    static std::thread updateThread;
    static std::atomic<bool> stopThread;
    // All derived classes
    static std::vector<ClientEntity*> clientEntities;

    // in game states
    static uint32_t gameState;
    static bool levelLoaded;
    static bool levelFullyLoaded;
    static uint32_t currentLevel;

    // events
    static std::vector<Listener> listenersEnterNewLevel;
    static std::vector<Listener> listenersExitLevel;
    static std::vector<Listener> listenersOpenGame;
    static std::vector<Listener> listenersCloseGame;

    // guards for multithread
    static std::mutex guardUpdate;
    static std::mutex guardReadPacket;
    static std::mutex guardWritePacket;

    // event functions
    static void eventEnterNewLevel() {

        HobbitMemoryAccess::setHobbitMemoryAccess();

        for (const auto& listener : listenersEnterNewLevel) 
        {
            listener();
        }
        for (ClientEntity* e : clientEntities)
        {
            e->enterNewLevel();
        }
    }
    static void eventExitLevel() {

        HobbitMemoryAccess::setHobbitMemoryAccess();

        for (const auto& listener : listenersExitLevel) 
        {
            listener();
        }
        for (ClientEntity* e : clientEntities)
        {
            e->exitLevel();
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


    // game states functions
    static void readGameState()
    {
        gameState = HobbitMemoryAccess::memoryAccess.readData(0x00762B58); // 0x00762B58: game state address
    }
    static void readGameLevel()
    {
        currentLevel = HobbitMemoryAccess::memoryAccess.readData(0x00762B5C);  // 00762B5C: current level address
    }
    static void readLevelLoaded()
    {
        levelLoaded = HobbitMemoryAccess::memoryAccess.readData(0x00760354);  //0x0072C7D4: is loaded level address
    }
    static void readLevelFullyLoaded()
    {
        levelFullyLoaded = !HobbitMemoryAccess::memoryAccess.readData(0x0076035C);  //0x0072C7D4: is loaded level address
    }

    static uint32_t getGameState()
    {
        return gameState;
    }
    static bool getLevelLoaded()
    {
        return levelLoaded;
    }
    static bool getLevelFullyLoaded()
    {
        return levelFullyLoaded;
    }
    static uint32_t getGameLevel()
    {
        return currentLevel;
    }

    static void readInstanices()
    {
        readGameState();
        readLevelLoaded();
        readLevelFullyLoaded();
        readGameLevel();
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

            // read instances of game (current level, etc.)
            readInstanices();

            // handle game states
            // game state
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
        if (stopThread)
        {
            std::cout << "OPPS" << std::endl;
        }

    }
public:
    
    void addListenerEnterNewLevel(const Listener& listener) {
        listenersEnterNewLevel.push_back(listener);
    }
    void addListenerExitLevel(const Listener& listener) {
        listenersExitLevel.push_back(listener);
    }
    void addListenerOpenGame(const Listener& listener) {
        listenersOpenGame.push_back(listener);
    }
    void addListenerCloseGame(const Listener& listener) {
        listenersCloseGame.push_back(listener);
    }

    GameManager()
    {
        GameManager::clientEntities.push_back(new MainPlayer());
        GameManager::clientEntities.push_back(new OtherPlayer());
        GameManager::clientEntities.push_back(new LevelEntity());
        start();
        updateThread = std::thread(update);
    }
    ~GameManager()
    {
        stopThread = true;
        updateThread.join();
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
        if(!snapshotPackets.empty())
            snapshotPackets.insert(snapshotPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
        if (!eventPackets.empty())
            eventPackets.insert(eventPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
    }
};

