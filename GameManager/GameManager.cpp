#include "GameManager.h"
// stores all classes
std::vector<ClientEntity*> GameManager::clientEntities{ new MainPlayer(), new OtherPlayer()};

// state of the game
uint32_t GameManager::gameState;
uint32_t GameManager::currentLevel;
