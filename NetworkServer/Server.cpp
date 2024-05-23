#include <iostream>
#include <unordered_map>

#include "../NetworkManager/MMO_Common.h"

class Server : public olc::net::server_interface<GameMsg>
{
public:
	Server(uint16_t nPort) : olc::net::server_interface<GameMsg>(nPort)
	{
	}
	std::vector<uint32_t> clientsIDs;
	std::vector<uint32_t> m_vGarbageIDs;

protected:
	bool OnClientConnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		// For now we will allow all 
		return true;
	}

	void OnClientValidated(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		// Client passed validation check, so send them a message informing
		// them they can continue to communicate
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Client_Accepted;
		client->Send(msg);
	}

	void OnClientDisconnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		if (client)
		{
			auto it = std::find(clientsIDs.begin(), clientsIDs.end(), client->GetID());
			if (it != clientsIDs.end()) 
			{
				std::cout << "[UNGRACEFUL REMOVAL]:" <<  std::to_string(*it) << std::endl;
				clientsIDs.erase(it);
				m_vGarbageIDs.push_back(client->GetID());
			}
		}
	}

	void OnMessage(std::shared_ptr<olc::net::connection<GameMsg>> client, olc::net::message<GameMsg>& msg) override
	{
		if (!m_vGarbageIDs.empty())
		{
			for (auto pid : m_vGarbageIDs)
			{
				olc::net::message<GameMsg> m;
				m.header.id = GameMsg::Game_RemovePlayer;
				m << pid;
				std::cout << "Removing " << pid << "\n";
				MessageAllClients(m);
			}
			m_vGarbageIDs.clear();
		}



		switch (msg.header.id)
		{
		case GameMsg::Client_RegisterWithServer:
		{
			uint32_t msgClientID;
			msgClientID = client->GetID();
			// message assing id
			olc::net::message<GameMsg> msgSendID;
			msgSendID.header.id = GameMsg::Client_AssignID;
			msgSendID << msgClientID;
			MessageClient(client, msgSendID);

			clientsIDs.push_back(msgClientID);

			// message add player
			olc::net::message<GameMsg> msgAddPlayer;
			msgAddPlayer.header.id = GameMsg::Game_AddPlayer;
			msgAddPlayer << msgClientID;
			MessageAllClients(msgAddPlayer);

			// message everyone added player
			for (const auto& clientID : clientsIDs)
			{
				olc::net::message<GameMsg> msgAddOtherPlayers;
				msgAddOtherPlayers.header.id = GameMsg::Game_AddPlayer;
				msgAddOtherPlayers << clientID;
				MessageClient(client, msgAddOtherPlayers);
			}

			break;
		}

		case GameMsg::Client_UnregisterWithServer:
		{
			break;
		}

		case GameMsg::Game_UpdatePlayer:
		{
			// Simply bounce update to everyone except incoming client
			MessageAllClients(msg, client);
			break;
		}

		}

	}

};

int main()
{
	std::cout << "Server is Running" << std::endl;
	Server server(60000);
	server.Start();
	while (1)
	{
		server.Update(-1, true);
	}

	return 0;
}