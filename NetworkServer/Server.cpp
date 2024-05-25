#include <iostream>
#include <unordered_map>

#include "../NetworkManager/MMO_Common.h"

class Server : public olc::net::server_interface<PacketType>
{
public:
	Server(uint16_t nPort) : olc::net::server_interface<PacketType>(nPort)
	{

	}
	std::vector<uint32_t> m_clientsIDs;
	std::vector<uint32_t> m_garbageIDs;
	const uint32_t m_MAX_CLIENTS = 4;

protected:
	bool OnClientConnect(std::shared_ptr<olc::net::connection<PacketType>> client) override
	{
		// check if to many users
		if (m_clientsIDs.size() >= m_MAX_CLIENTS)
			return false;
		
		return true;
	}

	void OnClientValidated(std::shared_ptr<olc::net::connection<PacketType>> client) override
	{
		// Client passed validation check, so send them a message informing
		// them they can continue to communicate
		olc::net::message<PacketType> msg;
		msg.header.id = PacketType::Client_Accepted;
		client->Send(msg);
	}

	void OnClientDisconnect(std::shared_ptr<olc::net::connection<PacketType>> client) override
	{
		if (client)
		{
			auto it = std::find(m_clientsIDs.begin(), m_clientsIDs.end(), client->GetID());
			if (it != m_clientsIDs.end())
			{
				std::cout << "[UNGRACEFUL REMOVAL]:" <<  std::to_string(*it) << std::endl;
				m_clientsIDs.erase(it);
				m_garbageIDs.push_back(client->GetID());
			}
		}
	}

	void OnMessage(std::shared_ptr<olc::net::connection<PacketType>> client, olc::net::message<PacketType>& msg) override
	{
		// notify garbage they are garbage and remove them from the list
		if (!m_garbageIDs.empty())
		{
			for (auto pid : m_garbageIDs)
			{
				olc::net::message<PacketType> m;
				m.header.id = PacketType::Game_RemoveClient;
				m << pid;
				std::cout << "Removing " << pid << "\n";
				MessageAllClients(m);
			}
			m_garbageIDs.clear();
		}

		// type of message
		switch (msg.header.id)
		{
		case PacketType::Client_RegisterWithServer:
		{

			uint32_t msgClientID;
			msgClientID = client->GetID();

			// message assing id
			olc::net::message<PacketType> msgSendID;
			msgSendID.header.id = PacketType::Client_AssignID;
			msgSendID << msgClientID;
			MessageClient(client, msgSendID);

			m_clientsIDs.push_back(msgClientID);

			// message add player
			olc::net::message<PacketType> msgAddPlayer;
			msgAddPlayer.header.id = PacketType::Game_AddClient;
			msgAddPlayer << msgClientID;
			MessageAllClients(msgAddPlayer);

			// message everyone added player
			for (const auto& m_clientID : m_clientsIDs)
			{
				olc::net::message<PacketType> msgAddOtherPlayers;
				msgAddOtherPlayers.header.id = PacketType::Game_AddClient;
				msgAddOtherPlayers << m_clientID;
				MessageClient(client, msgAddOtherPlayers);
			}

			break;
		}

		case PacketType::Client_UnregisterWithServer:
		{
			break;
		}

		case PacketType::Game_UpdateClient:
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
	server.start();
	while (1)
	{
		server.update(-1, true);
	}

	return 0;
}