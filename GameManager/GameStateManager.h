#pragma once
#include "MemoryAccess.h"

//0x00760864: loading layers

class GameStateManager
{
private:
    static uint32_t gameState;
    static uint32_t currentLevel;

    static void readGameState()
    {
        // when [0x00762B58] == 10 level on
        gameState = MemoryAccess::readData(0x00762B58); // 0x00762B58: game state
    }
    static void readGameLevel()
    {
        currentLevel = MemoryAccess::readData(0x00762B5C);  // 00762B5C: current level
    }

public:
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

