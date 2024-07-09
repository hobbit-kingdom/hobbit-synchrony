#pragma once
#include "HobbitMemoryAccess.h"

#include <vector>
#include <mutex>
#include <iomanip>
#include <functional>

class GameManager
{
protected:

    using Listener = std::function<void()>;

    static std::thread updateThread;
    static std::atomic<bool> stopThread;
    // All derived classes

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

                //update Event
                //[TO DO]
                previousState = currentState;
                previousLevelLoaded = currentLevelLoaded;
                previousLevelFullyLoaded = currentLevelFullyLoaded;
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

