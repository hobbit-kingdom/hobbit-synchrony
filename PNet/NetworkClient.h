#pragma once
#include <cstdint>
#include <vector>

class NetworkClient
{
public:
	NetworkClient() {
		id = idCounter;
		++idCounter;
	}
	static const int MAX_PLAYERS;
	static uint32_t idCounter;
	static std::vector<NetworkClient> networkClients;
	uint32_t id;
};
const int NetworkClient::MAX_PLAYERS = 4;
uint32_t NetworkClient::idCounter = 0x0001;

std::vector<NetworkClient> NetworkClient::networkClients;

