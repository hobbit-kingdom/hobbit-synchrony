#include "MyServer.h"
#include "../PNet/MemoryAccess.h"
#include "../HobbitMemory/NPC.h"
#include "../HobbitMemory/PlayerCharacter.h"
#include "../PNet/NetworkClient.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>
#include <unordered_map>
#include <iomanip>
using namespace std;

//types of data 
//  1 - new player Joined 
//  0 - send positon rotation and animation
//  2 - add newplayer to hashmaps
//  3 - remove player from hashmaps

// id , position in array
unordered_map<string, int> ipToId = {  };



void MyServer::OnConnect(TCPConnection& newConnection)
{
	std::cout << newConnection.ToString() << " - New connection accepted." << std::endl;

	if (connections.size() > NetworkClient::MAX_PLAYERS - 1)
	{
		std::cout << "New player has connected but we kick him since the server is full\n";
		newConnection.Close();
		return;
	}

	NetworkClient::networkClients.push_back(NetworkClient());

	std::cout << "Assigned " << NetworkClient::networkClients.back().id << " " << "index " << NetworkClient::networkClients.size() << '\n';

	// assign id to client
	std::shared_ptr<Packet> assignId = std::make_shared<Packet>(PacketType::PT_IntegerArray);
	*assignId << 3 << 4 << NetworkClient::networkClients.back().id;
	newConnection.pm_outgoing.Append(assignId);

	// send data about other clients
	*assignId << NetworkClient::networkClients.size() - 1;
	for (int i = 0; i < NetworkClient::networkClients.size() - 1; ++i)
	{
		*assignId << NetworkClient::networkClients[i].id;
	}

	// notify other clients about new client
	std::shared_ptr<Packet> mapNewPlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);
	ipToId[newConnection.ToString()] = NetworkClient::networkClients.back().id;
	*mapNewPlayer << 3 << 2 << NetworkClient::networkClients.back().id;

	for (auto& connection : connections)
	{
		if (&connection == &newConnection)
			continue;

		connection.pm_outgoing.Append(mapNewPlayer);
	}
}
void MyServer::OnDisconnect(TCPConnection& lostConnection, std::string reason)
{
	std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << "." << std::endl;

	std::shared_ptr<Packet> removeDisconnectedPlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);
	uint32_t playerId = ipToId[lostConnection.ToString()];

	*removeDisconnectedPlayer << 2 << 3 << playerId;

	for (int i = 0; i < NetworkClient::networkClients.size(); ++i)
	{
		if (NetworkClient::networkClients[i].id == playerId)
		{
			NetworkClient::networkClients.erase(NetworkClient::networkClients.begin() + i);
			break;
		}
	}
	for (auto& connection : connections)
	{
		if (&connection == &lostConnection)
			continue;

		connection.pm_outgoing.Append(removeDisconnectedPlayer);
	}
}

bool MyServer::ProcessPacket(std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		*packet >> chatmessage;
		std::cout << chatmessage << std::endl;
		break;
	}
	case PacketType::PT_IntegerArray:
	{
		uint32_t arraySize = 0;
		uint32_t type = 0;
		uint32_t id = 0;

		*packet >> arraySize;
		*packet >> type;
		*packet >> id;

		if (type == 0)
		{
			uint32_t index = 0;
			for (int i = 0; i < NetworkClient::networkClients.size(); ++i)
			{
				if (id == NetworkClient::networkClients[i].id)
				{
					index = i;
					break;
				}
			}

			std::shared_ptr<Packet> positonPacket = std::make_shared<Packet>(PacketType::PT_IntegerArray);
			*positonPacket << arraySize << type << id;
			uint32_t packetUInt;

			for (uint32_t i = 0; i < arraySize - 2; ++i)
			{
				*packet >> packetUInt;
				*positonPacket << packetUInt;
			}

			for (auto& connection : connections)
			{
				connection.pm_outgoing.Append(positonPacket);
			}
		}
		break;
	}
	case PacketType::PT_Test:
	{
		std::cout << "Received test packet." << std::endl;
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}

