#pragma once
#include <iostream>
#include <unordered_map>
#include <thread>
#include <iostream>

#include "../NetworkManager/MMO_Common.h"
#include "../GameManager/GameManager.h"

#define OLC_PGEX_TRANSFORMEDVIEW

#include <unordered_map>
#include <queue>
#include <stack>
class OtherClient
{
public:
	OtherClient(uint32_t newID) : id(newID) {}
	uint32_t id;
	// atomic:    thread protection
	// queue:    packets
	// vector:    game packet
	std::stack<std::vector<uint32_t>> gameStatePacket;
	std::mutex guardPackets;

	static std::vector<OtherClient> otherClients;
	static std::mutex guardOtherClients;

	// Custom assignment operator to properly handle the queue member
	OtherClient& operator=(const OtherClient& other)
	{
		if (this != &other)
		{
			id = other.id;
			// Copy queue contents
			gameStatePacket = other.gameStatePacket;
		}
		return *this;
	}

	void PushPacket(std::vector<uint32_t> pushPacket)
	{
		std::lock_guard<std::mutex> guard(guardPackets);
		gameStatePacket.push(pushPacket);
	}
	std::vector<uint32_t> GetPacket()
	{
		std::lock_guard<std::mutex> guard(guardPackets);
		if (gameStatePacket.empty())
			return std::vector<uint32_t>();
		return gameStatePacket.top();
	}
	void ClearPacket()
	{
		std::lock_guard<std::mutex> guard(guardPackets);
		std::stack<std::vector<uint32_t>>().swap(gameStatePacket);
	}


	static uint32_t GetIndex(uint32_t clientID)
	{
		std::lock_guard<std::mutex> guard(guardOtherClients);
		for (uint32_t i = 0; i < otherClients.size(); ++i)
		{
			if (otherClients[i].id == clientID)
				return i;
		}
		return 0xFFFFFFFF; // Not found
	}
	// Move constructor
	OtherClient(OtherClient&& other) noexcept : id(other.id), gameStatePacket(std::move(other.gameStatePacket)) {}
	static bool AddClient(uint32_t addID)
	{
		std::lock_guard<std::mutex> guard(guardOtherClients);
		// Use emplace_back to construct the OtherClient object in-place
		otherClients.emplace_back(addID);
		return true;
	}


	static bool RemoveClient(uint32_t removeID)
	{
		std::lock_guard<std::mutex> guard(guardOtherClients);
		for (auto it = otherClients.begin(); it != otherClients.end(); ++it)
		{
			if (it->id == removeID)
			{
				otherClients.erase(it);
				return true;
			}
		}
		return false;
	}

	static std::vector<std::vector<uint32_t>> GetStatePackets()
	{
		std::vector<std::vector<uint32_t>> statePackets;
		for (OtherClient& e : otherClients)
		{
			statePackets.push_back(e.GetPacket());
			e.ClearPacket();
		}
		return statePackets;
	}

};
std::mutex OtherClient::guardOtherClients;
std::vector<OtherClient> OtherClient::otherClients;


class Client : public olc::net::client_interface<PacketType>
{
private:
	uint32_t m_myID = 0;

	std::thread m_updateThread;
	std::thread m_processThread;
	std::thread m_readThread;
	std::thread m_sendThread;

	std::atomic<bool> m_waitingForConnection = true;
	std::atomic<bool> m_stopThreads = true;

	const std::atomic<uint32_t> m_UPDATE_SPEED = 200;
	const std::atomic<uint32_t> m_PROCESS_SPEED = 200;
	const std::atomic<uint32_t> m_READ_SPEED = 200;
	const std::atomic<uint32_t> m_SEND_SPEED = 200;

	std::vector<uint32_t> m_currentPacket;

	//q_Packets, v_GamePacket
	std::queue<std::vector<uint32_t>> m_gameEventPacket;
	std::mutex guardGameEventPacket;

public:
	Client()
	{
		if (!OnUserCreate())
		{
			return;
		}

		m_stopThreads = false;
		// Create thread for reading and sending messages
		m_updateThread = std::thread(&Client::UpdateGame, this);
		m_processThread = std::thread(&Client::ProcessMessages, this);
		m_readThread = std::thread(&Client::ReadMessages, this);
		m_sendThread = std::thread(&Client::SendMessages, this);
	}
	~Client() {
		if (m_stopThreads) return;
		// Signal threads to stop and wait for them to finish
		m_stopThreads = true;
		m_updateThread.join();
		m_processThread.join();
		m_readThread.join();
		m_sendThread.join();
	}
	bool OnUserCreate()
	{
		std::string ip;

		// connect to server
		do
		{
			std::cout << "To exit press [q]" << std::endl;
			std::cout << "Enter IP of server: ";
			std::getline(std::cin, ip);
			std::cout << std::endl;

			if (ip.at(0) == 'q')
				return false;
			Connect(ip, 60000);
		} while (!IsConnected());
		return true;
	}

	std::vector<std::vector<uint32_t>> GetGameEventsPacket()
	{
		std::lock_guard<std::mutex> guard(guardGameEventPacket);
		std::vector<std::vector<uint32_t>> finalPacket;
		while (!m_gameEventPacket.empty())
		{
			finalPacket.push_back(m_gameEventPacket.front());
			m_gameEventPacket.pop();
		}
		return finalPacket;
	}
	void PushEventPacket(std::vector<uint32_t> packet)
	{

	}

	// Update Game
	void UpdateGame()
	{
		while (!m_stopThreads)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(m_UPDATE_SPEED));
			if (m_waitingForConnection)
				continue;

			GameManager::Update();

		}
	}
	// Process Message into the game
	void ProcessMessages()
	{
		while (!m_stopThreads)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(m_PROCESS_SPEED));
			if (m_waitingForConnection)
				continue;
			
			//pakcets of gamepackets
			std::vector<std::vector<uint32_t>> gamePackets;
			std::vector<std::vector<uint32_t>> gameEvents = GetGameEventsPacket();
			std::vector<std::vector<uint32_t>> gameStates = OtherClient::GetStatePackets();

			// Copy packets
			gamePackets.insert(gamePackets.end(), gameEvents.begin(), gameEvents.end());
			gamePackets.insert(gamePackets.end(), gameStates.begin(), gameStates.end());
	
			// check if empty
			if (gamePackets.empty())
				continue;
			// process each packet
			for (std::vector<uint32_t> e : gamePackets)
			{
				if (e.empty())
					continue;

				uint32_t indexClient = e.back();
				e.pop_back();

				// read packets
				GameManager::ReadPacket(e, indexClient);
			}
		}
	}
	// Function for reading messages
	void ReadMessages()
	{
		while (!m_stopThreads)
		{
				std::this_thread::sleep_for(std::chrono::milliseconds(m_READ_SPEED));
			if (!IsConnected())
				break;

			while (!Incoming().empty())
			{

				auto msg = Incoming().pop_front().msg;
				std::cout << "Buffer Size: " << Incoming().count() << std::endl;

				switch (msg.header.id)
				{
				case(PacketType::Client_Accepted):
				{
					std::cout << "Server accepted client - you're in!\n";
					olc::net::message<PacketType> msg;
					msg.header.id = PacketType::Client_RegisterWithServer;
					Send(msg);
					break;
				}

				case(PacketType::Client_AssignID):
				{
					// Server is assigning us OUR id
					msg >> m_myID;
					std::cout << "Assigned Client ID = " << m_myID << "\n";
					break;
				}

				case(PacketType::Game_AddClient):
				{
					uint32_t addedClientID;
					msg >> addedClientID;

					// Connected to Server
					if (addedClientID == m_myID)
					{
						m_waitingForConnection = false;
					}
					else
					{
						OtherClient::AddClient(addedClientID);
					}
					break;
				}

				case(PacketType::Game_RemoveClient):
				{
					uint32_t removeClientID = 0;
					msg >> removeClientID;


					OtherClient::RemoveClient(removeClientID);

					break;
				}

				case(PacketType::Game_UpdateClient):
				{
					// connectedID
					uint32_t connectedID;
					msg >> connectedID;

					// find the index of id
					uint32_t indexClient = OtherClient::GetIndex(connectedID);
					if (indexClient == 0xFFFFFFFF)
						break;
					// get message size
					uint32_t gameMsgSize;
					msg >> gameMsgSize;

					// get message game data 
					std::vector<uint32_t> gameMsgs(gameMsgSize, 0);
					for (uint32_t& m_gamePackets : gameMsgs)
					{
						msg >> m_gamePackets;
					}
					gameMsgs.push_back(indexClient);
					OtherClient::otherClients[indexClient].PushPacket(gameMsgs);
					break;
				}
				case(PacketType::Game_EventClient):
				{
					continue;
					// connectedID
					uint32_t connectedID;
					msg >> connectedID;

					// find the index of id
					uint32_t indexClient = OtherClient::GetIndex(connectedID);

					// get message size
					uint32_t gameMsgSize;
					msg >> gameMsgSize;

					// get message game data 
					std::vector<uint32_t> gameMsgs(gameMsgSize, 0);
					for (uint32_t& m_gamePackets : gameMsgs)
					{
						msg >> m_gamePackets;
					}
					gameMsgs.push_back(indexClient);
					PushEventPacket(gameMsgs);
					break;
				}
				}
			}
		}
	}
	// Function for sending messages
	void SendMessages()
	{
		while (!m_stopThreads)
		{
			// Capture the start time
			if (!m_waitingForConnection)
			{
				// set the packets
				std::vector<uint32_t> packets = GameManager::WritePacket();

				// send message
				olc::net::message<PacketType> msg;
				msg.header.id = PacketType::Game_UpdateClient;
				// Send player data
				for (uint32_t i = packets.size(); i-- > 0;)
				{
					msg << packets[i];
				}

				msg << packets.size();
				msg << m_myID;
				Send(msg);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(m_SEND_SPEED));
		}
	}
};

int main()
{
	Client client;
	char a;
	do
	{
		std::cout << "To exit press [q]: ";
		std::cin >> a;
		std::cout << std::endl;
	} while (a != 'q');
	return 0;
}
