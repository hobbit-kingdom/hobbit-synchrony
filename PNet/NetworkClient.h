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
		for (uint32_t i = 0; i < otherClients.size(); ++i)
		{
			if (otherClients[i].id == playerId)
			{
				return i;
			}
		}
		return 0;
	}
	uint32_t id;

	static const uint32_t MAX_PLAYERS;
	static uint32_t idCounter;
	static std::vector<NetworkClient> otherClients;
};


