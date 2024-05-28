#include "GameManager.h"

using Listener = std::function<void()>;

std::thread GameManager::updateThread;
std::atomic<bool> GameManager::stopThread = false;
// All derived classes
std::vector<ClientEntity*> GameManager::clientEntities;

// in game states
uint32_t GameManager::gameState;
bool GameManager::levelLoaded;
bool GameManager::levelFullyLoaded;
uint32_t GameManager::currentLevel;
// events
std::vector<Listener> GameManager::listenersEnterNewLevel;
std::vector<Listener> GameManager::listenersExitLevel;
std::vector<Listener> GameManager::listenersOpenGame;
std::vector<Listener> GameManager::listenersCloseGame;

// guards for multithread
std::mutex GameManager::guardUpdate;
std::mutex GameManager::guardReadPacket;
std::mutex GameManager::guardWritePacket;