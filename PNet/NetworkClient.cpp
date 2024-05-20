#include "NetworkClient.h"

const int NetworkClient::MAX_PLAYERS = 4;
uint32_t NetworkClient::idCounter = 0x0001;

std::vector<NetworkClient> NetworkClient::networkClients;
