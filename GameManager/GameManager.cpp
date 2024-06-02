#include "GameManager.h"

using Listener = std::function<void()>;

std::thread GameManager::updateThread;
std::atomic<bool> GameManager::stopThread = false;
// All derived classes
std::vector<ClientEntity*> GameManager::clientEntities;

// events
std::vector<Listener> GameManager::listenersEnterNewLevel;
std::vector<Listener> GameManager::listenersExitLevel;
std::vector<Listener> GameManager::listenersOpenGame;
std::vector<Listener> GameManager::listenersCloseGame;