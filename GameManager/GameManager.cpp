#include "GameManager.h"
// stores all classes
std::vector<ClientEntity*> GameManager::clientEntities{ new MainPlayer(), new OtherPlayer()};

// state of the game
uint32_t GameManager::gameState;
bool GameManager::levelLoaded;
uint32_t GameManager::currentLevel;
std::mutex GameManager::guardUpdate;
std::mutex GameManager::guardReadPacket;
std::mutex GameManager::guardWritePacket;