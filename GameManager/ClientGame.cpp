#include "ClientGame.h"
bool ClientGame::isLoaded = false;
std::vector<ClientEntity*> ClientGame::entities{ new MainPlayer(), new OtherPlayer() };
