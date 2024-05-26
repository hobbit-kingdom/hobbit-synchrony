#include <iostream>
#include <unordered_map>
#include <thread>
#include <atomic>
#include "../NetworkManager/server_interface.h"

class Server : public net::server_interface<PacketType>
{
public:
	Server(uint16_t nPort) : net::server_interface<PacketType>(nPort)
	{

	}
	std::vector<uint32_t> m_clientsIDs;
	std::vector<uint32_t> m_garbageIDs;
	const uint32_t m_MAX_CLIENTS = 8;

protected:
	bool OnClientConnect(std::shared_ptr<net::connection<PacketType>> client) override
	{
		// check if to many users
		if (m_clientsIDs.size() >= m_MAX_CLIENTS)
			return false;
		
		return true;
	}

	void OnClientValidated(std::shared_ptr<net::connection<PacketType>> client) override
	{
		// Client passed validation check, so send them a message informing
		// them they can continue to communicate
		net::packet<PacketType> pkt;
		pkt.header.id = PacketType::Client_Accepted;
		client->Send(pkt);
	}

	void OnClientDisconnect(std::shared_ptr<net::connection<PacketType>> client) override
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

	void OnRecievePacket(std::shared_ptr<net::connection<PacketType>> client, net::packet<PacketType>& pkt) override
	{
		// notify garbage they are garbage and remove them from the list
		if (!m_garbageIDs.empty())
		{
			for (auto pid : m_garbageIDs)
			{
				net::packet<PacketType> m;
				m.header.id = PacketType::Game_RemoveClient;
				m << pid;
				std::cout << "Removing " << pid << "\n";
				SendPacketAllClients(m);
			}
			m_garbageIDs.clear();
		}

		// type of message
		switch (pkt.header.id)
		{
		case PacketType::Client_RegisterWithServer:
		{

			uint32_t msgClientID;
			msgClientID = client->GetID();

			// message assing id
			net::packet<PacketType> msgSendID;
			msgSendID.header.id = PacketType::Client_AssignID;
			msgSendID << msgClientID;
			SendPacketClient(client, msgSendID);

			m_clientsIDs.push_back(msgClientID);

			// message add player
			net::packet<PacketType> msgAddPlayer;
			msgAddPlayer.header.id = PacketType::Game_AddClient;
			msgAddPlayer << msgClientID;
			SendPacketAllClients(msgAddPlayer);

			// message everyone added player
			for (const auto& m_clientID : m_clientsIDs)
			{
				net::packet<PacketType> msgAddOtherPlayers;
				msgAddOtherPlayers.header.id = PacketType::Game_AddClient;
				msgAddOtherPlayers << m_clientID;
				SendPacketClient(client, msgAddOtherPlayers);
			}

			break;
		}

		case PacketType::Client_UnregisterWithServer:
		{
			break;
		}

		case PacketType::Game_Snapshot:
		{
			// Simply bounce update to everyone except incoming client
			SendPacketAllClients(pkt, client);
			break;
		}
		case PacketType::Game_EventClient:
		{
			// Simply bounce update to everyone except incoming client
			SendPacketAllClients(pkt, client);
			break;
		}

		}

	}

};


std::atomic<bool> m_stopThreads = false;
void UpdateServer(Server& server)
{
	while (m_stopThreads)
	{
		server.Update(-1, true);
	}
}

int main()
{
	std::cout << "Server is Running" << std::endl;

	// start server
	Server server(60000);
	server.Start();

	//updateserver
	m_stopThreads = true;
	std::thread serverThread(UpdateServer, std::ref(server)); // run it on a different thread

	// input to stop server
	char input;
	do
	{
		std::cin >> input;
	} while (input != 'q');


	// stop server update
	m_stopThreads = false;
	serverThread.join(); // join the thread


	return 0;
}