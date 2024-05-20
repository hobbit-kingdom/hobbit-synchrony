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
	static uint32_t GetIndexByID(uint32_t playerId)
	{
		for (int i = 0; i < networkClients.size(); ++i)
		{
			if (networkClients[i].id == playerId)
			{
				return playerId;
			}
		}
		return 0;
	}
	uint32_t id;

	static const int MAX_PLAYERS;
	static uint32_t idCounter;
	static std::vector<NetworkClient> networkClients;
};


