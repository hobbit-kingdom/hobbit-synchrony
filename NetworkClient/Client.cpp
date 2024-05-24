#pragma once
#include <iostream>
#include <unordered_map>
#include <thread>
#include <iostream>

#include "../NetworkManager/MMO_Common.h"
#include "../GameManager/GameManager.h"

#define OLC_PGEX_TRANSFORMEDVIEW

#include <unordered_map>
class Client : public olc::net::client_interface<PacketType>
{
private:
	uint32_t m_myID = 0;
	std::vector<uint32_t> m_connectedIDs;

	std::thread m_updateThread;
	std::thread m_processThread;
	std::thread m_readThread;
	std::thread m_sendThread;

	std::atomic<bool> m_waitingForConnection = true;
	std::atomic<bool> m_stopThreads = false ;

	const std::atomic<uint32_t> m_UPDATE_SPEED = 200;
	const std::atomic<uint32_t> m_PROCESS_SPEED = 200;
	const std::atomic<uint32_t> m_READ_SPEED = 200;
	const std::atomic<uint32_t> m_SEND_SPEED = 200;

	std::vector<std::vector<uint32_t>> m_gamePackets;

	std::mutex m_guardGameMsg;

public:
	Client()
	{
		std::cout << "Client Created" << std::endl;

		if (!OnUserCreate())
		{
			return;
		}

		// Create thread for reading and sending messages
		m_updateThread = std::thread(&Client::UpdateGame, this);
		m_processThread = std::thread(&Client::ProcessMessages, this);
		m_readThread = std::thread(&Client::ReadMessages, this);
		m_sendThread = std::thread(&Client::SendMessages, this);
	}
	~Client() {
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
		do
		{
			std::cout << "To exit press [q]" << std::endl;
			std::cout << "Enter IP of server: ";
			std::getline(std::cin, ip);
			std::cout << std::endl;

			if (ip.at(0) == 'q')
				return false;

		} while (!Connect(ip, 60000));
		return true;
	}

	std::vector<uint32_t> PopGamePacket()
	{
		std::lock_guard<std::mutex> guard(m_guardGameMsg);

		if (m_gamePackets.empty())
			return std::vector<uint32_t>();

		std::vector<uint32_t> firstElement = m_gamePackets.front();
		m_gamePackets.erase(m_gamePackets.begin());
		return firstElement;
	}
	void PushGamePacket(std::vector<uint32_t> pushMsg)
	{
		std::lock_guard<std::mutex> guard(m_guardGameMsg);
		std::cout << "~Unprocessed Packets Size: " <<  m_gamePackets.size() << std::endl;
		m_gamePackets.push_back(pushMsg);
	}
	std::vector<uint32_t> BackGameMsg()
	{
		std::lock_guard<std::mutex> guard(m_guardGameMsg);
		if (m_gamePackets.empty())
			return std::vector<uint32_t>();

		std::vector<uint32_t> lastElement = m_gamePackets.back();
		m_gamePackets.pop_back();

		return lastElement;
	}
	void ClearGameMsg()
	{
		std::lock_guard<std::mutex> guard(m_guardGameMsg);
		m_gamePackets.clear();
	}

	// Update Game
	void UpdateGame()
	{
		while (!m_stopThreads)
		{
			if (m_waitingForConnection)
				continue; 

			GameManager::Update();

			std::this_thread::sleep_for(std::chrono::milliseconds(m_UPDATE_SPEED));
		}
	}
	// Process Message into the game
	void ProcessMessages()
	{
		while (!m_stopThreads)
		{
			if (m_waitingForConnection)
				continue;

			std::vector<uint32_t> m_gamePackets = BackGameMsg();
			ClearGameMsg();

			if (m_gamePackets.empty())
				continue;

			uint32_t indexClient = m_gamePackets.back();
			m_gamePackets.pop_back();

			// read packets
			GameManager::ReadPacket(m_gamePackets, indexClient);

			std::this_thread::sleep_for(std::chrono::milliseconds(m_PROCESS_SPEED));
		}
	}
	// Function for reading messages
	void ReadMessages()
	{
		while (!m_stopThreads)
		{
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
					uint32_t desc;
					msg >> desc;
					m_connectedIDs.push_back(desc);

					// Connected to Server
					if (desc == m_myID)
					{
						m_waitingForConnection = false;
					}
					break;
				}

				case(PacketType::Game_RemoveClient):
				{
					uint32_t removeClientID = 0;
					msg >> removeClientID;

					//remove the clients id
					auto it = std::find(m_connectedIDs.begin(), m_connectedIDs.end(), removeClientID);
					if (it != m_connectedIDs.end()) {
						m_connectedIDs.erase(it);
					}

					break;
				}

				case(PacketType::Game_UpdateClient):
				{
					// connectedID
					uint32_t connectedID;
					msg >> connectedID;

					// find the index of id
					uint32_t indexClient;
					auto it = std::find(m_connectedIDs.begin(), m_connectedIDs.end(), connectedID);
					if (it != m_connectedIDs.end()) {
						indexClient = std::distance(m_connectedIDs.begin(), it) - 1;
					}

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
					PushGamePacket(gameMsgs);
					break;
				}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(m_READ_SPEED));
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
