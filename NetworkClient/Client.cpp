#pragma once
#include <iostream>
#include <unordered_map>
#include <thread>
#include <iostream>

#include "../NetworkManager/MMO_Common.h"
#include "../GameManager/GameManager.h"

#define OLC_PGEX_TRANSFORMEDVIEW

#include <unordered_map>
class Client : public olc::net::client_interface<GameMsg>
{
private:
	uint32_t clientID = 0;
	std::vector<uint32_t> clientsIDs;

	bool bWaitingForConnection = true;

	std::thread processThread;
	std::thread readThread;
	std::thread sendThread;

	std::atomic<bool> stopThreads{ false };
	std::vector<std::vector<uint32_t>> gameMsg;

	std::mutex guardGameMsg;

public:
	Client()
	{
		std::cout << "Client Opened" << std::endl;

		OnUserCreate();
		// Create thread for reading and sending messages
		processThread = std::thread(&Client::ProcessMessages, this);
		readThread = std::thread(&Client::ReadMessages, this);
		sendThread = std::thread(&Client::SendMessages, this);
	}
	~Client() {
		// Signal threads to stop and wait for them to finish
		stopThreads = true;
		processThread.join();
		readThread.join();
		sendThread.join();
	}
	bool OnUserCreate()
	{
		if (Connect("26.142.171.132", 60000))
		{
			return true;
		}
		return false;
	}

	std::vector<uint32_t> PopGameMsg()
	{
		std::lock_guard<std::mutex> guard(guardGameMsg);

		if (gameMsg.empty())
			return std::vector<uint32_t>();

		std::vector<uint32_t> firstElement = gameMsg.front();
		gameMsg.erase(gameMsg.begin());
		return firstElement;
	}
	void PushGameMsg(std::vector<uint32_t> pushMsg)
	{
		std::lock_guard<std::mutex> guard(guardGameMsg);
		std::cout << "Number of game msg: " <<  gameMsg.size() << std::endl;
		gameMsg.push_back(pushMsg);
	}
	std::vector<uint32_t> BackGameMsg()
	{
		std::lock_guard<std::mutex> guard(guardGameMsg);
		if (gameMsg.empty())
			return std::vector<uint32_t>();

		std::vector<uint32_t> lastElement = gameMsg.back();
		gameMsg.pop_back();
		return lastElement;
	}
	void ClearGameMsg()
	{
		std::lock_guard<std::mutex> guard(guardGameMsg);
		gameMsg.clear();
	}

	void ProcessMessages()
	{
		while (!stopThreads)
		{
			if (bWaitingForConnection)
				continue;
			//update the game
			GameManager::Update();
			std::vector<uint32_t> gameMsg = BackGameMsg();
			ClearGameMsg();

			if (gameMsg.empty())
				continue;

			uint32_t indexClient = gameMsg.back();
			gameMsg.pop_back();

			// read packets
			GameManager::readPackets(gameMsg, indexClient);
			// Sleep for 200 milliseconds
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
	// Function for reading messages
	void ReadMessages()
	{
		while (!stopThreads)
		{
			if (!IsConnected())
				break;

			while (!Incoming().empty())
			{
				auto msg = Incoming().pop_front().msg;
				std::cout << "Buffer Size: " << Incoming().count() << std::endl;

				switch (msg.header.id)
				{
				case(GameMsg::Client_Accepted):
				{
					std::cout << "Server accepted client - you're in!\n";
					olc::net::message<GameMsg> msg;
					msg.header.id = GameMsg::Client_RegisterWithServer;
					Send(msg);
					break;
				}

				case(GameMsg::Client_AssignID):
				{
					// Server is assigning us OUR id
					msg >> clientID;
					std::cout << "Assigned Client ID = " << clientID << "\n";
					break;
				}

				case(GameMsg::Game_AddPlayer):
				{
					uint32_t desc;
					msg >> desc;
					clientsIDs.push_back(desc);

					// Connected to Server
					if (desc == clientID)
					{
						bWaitingForConnection = false;
					}
					break;
				}

				case(GameMsg::Game_RemovePlayer):
				{
					uint32_t removeClientID = 0;
					msg >> removeClientID;

					//remove the clients id
					auto it = std::find(clientsIDs.begin(), clientsIDs.end(), removeClientID);
					if (it != clientsIDs.end()) {
						clientsIDs.erase(it);
					}

					break;
				}

				case(GameMsg::Game_UpdatePlayer):
				{
					// connectedID
					uint32_t connectedID;
					msg >> connectedID;

					// find the index of id
					uint32_t indexClient;
					auto it = std::find(clientsIDs.begin(), clientsIDs.end(), connectedID);
					if (it != clientsIDs.end()) {
						indexClient = std::distance(clientsIDs.begin(), it) - 1;
					}

					// get message size
					uint32_t gameMsgSize;
					msg >> gameMsgSize;

					// get message game data 
					std::vector<uint32_t> gameMsgs(gameMsgSize, 0);
					for (uint32_t& gameMsg : gameMsgs)
					{
						msg >> gameMsg;
					}
					gameMsgs.push_back(indexClient);
					PushGameMsg(gameMsgs);
					break;
				}
				}
			}
			// Sleep for 200 milliseconds
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
	// Function for sending messages
	void SendMessages()
	{
		while (!stopThreads)
		{
			if (!bWaitingForConnection)
			{
				GameManager::Update();
				// set the packets
				std::vector<uint32_t> packets = GameManager::setPackets();

				// send message
				olc::net::message<GameMsg> msg;
				msg.header.id = GameMsg::Game_UpdatePlayer;
				// Send player data
				for (uint32_t i = packets.size(); i-- > 0;)
				{
					msg << packets[i];
				}

				msg << packets.size();
				msg << clientID;
				Send(msg);
			}
			// Sleep for 200 milliseconds
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
};

int main()
{
    Client client;
    char a;
    do
    {
        std::cin >> a;
        std::cout << std::endl;
    } while (a != 'e');
    return 0;
}
