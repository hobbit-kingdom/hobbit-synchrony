#pragma once
#include <iostream>
#include <unordered_map>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <stack>

#include "../NetworkManager/PacketType.h"
#include "../GameManager/GameManager.h"

#define CLIENT


class otherClient
{
public:
	uint32_t id;

	// stack:	packets
	// vector:	game packet
	std::stack<std::vector<uint32_t>> gameStatePacket;
	std::mutex guardPackets;

	static std::vector<otherClient> otherClients;
	static std::mutex guardOtherClients;
	
	otherClient(uint32_t newID) : id(newID) {}
	otherClient(otherClient&& other) noexcept : id(other.id), gameStatePacket(std::move(other.gameStatePacket)) {}


	// Custom assignment operator to properly handle the queue member
	otherClient& operator=(const otherClient& other)
	{
		if (this != &other)
		{
			id = other.id;
			// Copy queue contents
			gameStatePacket = other.gameStatePacket;
		}
		return *this;
	}

	void pushPacket(std::vector<uint32_t> pushPacket)
	{
		std::lock_guard<std::mutex> guard(guardPackets);
		gameStatePacket.push(pushPacket);
	}
	std::vector<uint32_t> getPacket()
	{
		std::lock_guard<std::mutex> guard(guardPackets);
		if (gameStatePacket.empty())
			return std::vector<uint32_t>();
		return gameStatePacket.top();
	}
	void clearPacket()
	{
		std::lock_guard<std::mutex> guard(guardPackets);
		std::stack<std::vector<uint32_t>>().swap(gameStatePacket);
	}

	static uint32_t getIndex(uint32_t clientID)
	{
		std::lock_guard<std::mutex> guard(guardOtherClients);
		for (uint32_t i = 0; i < otherClients.size(); ++i)
		{
			if (otherClients[i].id == clientID)
				return i;
		}
		return 0xFFFFFFFF; // Not found
	}
	static bool addClient(uint32_t addID)
	{
		std::lock_guard<std::mutex> guard(guardOtherClients);
		// Use emplace_back to construct the OtherClient object in-place
		otherClients.emplace_back(addID);
		return true;
	}
	static bool removeClient(uint32_t removeID)
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

	static std::vector<std::vector<uint32_t>> getStatePackets()
	{
		std::vector<std::vector<uint32_t>> statePackets;
		for (otherClient& e : otherClients)
		{
			statePackets.push_back(e.getPacket());
			e.clearPacket();
		}
		return statePackets;
	}
};
std::mutex otherClient::guardOtherClients;
std::vector<otherClient> otherClient::otherClients;


class Client : public net::client_interface<PacketType>
{
private:
	uint32_t m_myID = 0;

	// Threads
	std::thread m_updateThread;
	std::thread m_processThread;
	std::thread m_readThread;
	std::thread m_sendThread;

	// Threads Conditions
	std::atomic<bool> m_waitingForConnection = true;
	std::atomic<bool> m_stopThreads = true;

	// Loop Speeds
	const std::atomic<uint32_t> m_UPDATE_SPEED = 200;
	const std::atomic<uint32_t> m_PROCESS_SPEED = 200;
	const std::atomic<uint32_t> m_READ_SPEED = 200;
	const std::atomic<uint32_t> m_SEND_SPEED = 200;


	//Packet of Events
	std::queue<std::vector<uint32_t>> m_gameEventPacket;
	std::mutex m_guardGameEventPacket;

public:
	Client()
	{
		if (!onUserCreate())
		{
			return;
		}

		m_stopThreads = false;
		// Create thread for reading and sending messages
		m_updateThread = std::thread(&Client::updateGame, this);
		m_processThread = std::thread(&Client::processMessages, this);
		m_readThread = std::thread(&Client::readMessages, this);
		m_sendThread = std::thread(&Client::sendMessages, this);
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
	bool onUserCreate()
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
		std::lock_guard<std::mutex> guard(m_guardGameEventPacket);
		std::vector<std::vector<uint32_t>> finalPacket;
		while (!m_gameEventPacket.empty())
		{
			finalPacket.push_back(m_gameEventPacket.front());
			m_gameEventPacket.pop();
		}
		return finalPacket;
	}
	void pushEventPacket(std::vector<uint32_t> packet)
	{

	}

	// Update Game
	void updateGame()
	{
		while (!m_stopThreads && IsConnected())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(m_UPDATE_SPEED));
			if (m_waitingForConnection)
				continue;

			GameManager::update();

		}
	}
	// Process Message into the game
	void processMessages()
	{
		while (!m_stopThreads && IsConnected())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(m_PROCESS_SPEED));
			if (m_waitingForConnection)
				continue;
			
			//pakcets of gamepackets
			std::vector<std::vector<uint32_t>> gamePackets;
			std::vector<std::vector<uint32_t>> gameEvents = GetGameEventsPacket();
			std::vector<std::vector<uint32_t>> gameStates = otherClient::getStatePackets();

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
				GameManager::readPacket(e, indexClient);
			}
		}
	}
	// Function for reading messages
	void readMessages()
	{
		while (!m_stopThreads && IsConnected())
		{
				std::this_thread::sleep_for(std::chrono::milliseconds(m_READ_SPEED));
			if (!IsConnected())
				break;

			while (!Incoming().empty())
			{

				auto pkt = Incoming().pop_front().pkt;
				std::cout << "Buffer Size: " << Incoming().count() << std::endl;

				switch (pkt.header.id)
				{
				case(PacketType::Client_Accepted):
				{
					std::cout << "Server accepted client - you're in!\n";
					net::packet<PacketType> pkt;
					pkt.header.id = PacketType::Client_RegisterWithServer;
					Send(pkt);
					break;
				}

				case(PacketType::Client_AssignID):
				{
					// Server is assigning us OUR id
					pkt >> m_myID;
					std::cout << "Assigned Client ID = " << m_myID << "\n";
					break;
				}

				case(PacketType::Game_AddClient):
				{
					uint32_t addedClientID;
					pkt >> addedClientID;

					// Connected to Server
					if (addedClientID == m_myID)
					{
						m_waitingForConnection = false;
					}
					else
					{
						otherClient::addClient(addedClientID);
					}
					break;
				}

				case(PacketType::Game_RemoveClient):
				{
					uint32_t removeClientID = 0;
					pkt >> removeClientID;
					if (removeClientID == m_myID)
					{
						std::cout << "You are disconected from the server" << std::endl;
						m_stopThreads = true;
						break;
					}

					otherClient::removeClient(removeClientID);

					break;
				}

				case(PacketType::Game_Snapshot):
				{
					// connectedID
					uint32_t connectedID;
					pkt >> connectedID;

					// find the index of id
					uint32_t indexClient = otherClient::getIndex(connectedID);
					if (indexClient == 0xFFFFFFFF)
						break;
					// get message size
					uint32_t gameMsgSize;
					pkt >> gameMsgSize;

					// get message game data 
					std::vector<uint32_t> gameMsgs(gameMsgSize, 0);
					for (uint32_t& m_gamePackets : gameMsgs)
					{
						pkt >> m_gamePackets;
					}
					gameMsgs.push_back(indexClient);
					otherClient::otherClients[indexClient].pushPacket(gameMsgs);
					break;
				}
				case(PacketType::Game_EventClient):
				{
					continue;
					// connectedID
					uint32_t connectedID;
					pkt >> connectedID;

					// find the index of id
					uint32_t indexClient = otherClient::getIndex(connectedID);

					// get message size
					uint32_t gameMsgSize;
					pkt >> gameMsgSize;

					// get message game data 
					std::vector<uint32_t> gameMsgs(gameMsgSize, 0);
					for (uint32_t& m_gamePackets : gameMsgs)
					{
						pkt >> m_gamePackets;
					}
					gameMsgs.push_back(indexClient);
					pushEventPacket(gameMsgs);
					break;
				}
				}
			}
		}
	}
	// Function for sending messages
	void sendMessages()
	{
		while (!m_stopThreads && IsConnected())
		{
			// Capture the start time
			if (!m_waitingForConnection)
			{
				// set the packets
				std::vector<uint32_t> packets = GameManager::writePacket();

				// send message
				net::packet<PacketType> pkt;
				pkt.header.id = PacketType::Game_Snapshot;
				// Send player data
				for (uint32_t i = packets.size(); i-- > 0;)
				{
					pkt << packets[i];
				}

				pkt << packets.size();
				pkt << m_myID;
				Send(pkt);
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
		std::cout << "Enter [q] to close program: ";
		std::cin >> a;
		std::cout << std::endl;
	} while (a != 'q');
	return 0;
}
