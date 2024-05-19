#include "MyClient.h"
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

int myID = 0;

unordered_map<string, int> ipToId = {  };


MyClient::MyClient()
{
	string s;
	cout << "Input y and press ENTER after you've launched the level\n";
	cin >> s;

	//setup the MemoryAccess
	MemoryAccess::setExecutableName("Meridian.exe");
	PlayerCharacter::OpenNewLevel();
}
void MyClient::SendPacket() {
	if (myID == 0) return;

	std::shared_ptr<Packet> myLocation = std::make_shared<Packet>(PacketType::PT_IntegerArray);	

	vector<uint32_t> packets = PlayerCharacter::setPacket();

	*myLocation << 2 + uint32_t(packets.size());
	*myLocation << 0 << myID;

	for (uint32_t i = 0; i < uint32_t(packets.size()); ++i)
	{
		*myLocation << packets[i];
	}
	connection.pm_outgoing.Append(myLocation);
}
void MyClient::Update()
{
	PlayerCharacter::checkUpdateLevel();
}
void MyClient::OnConnect()
{
	std::cout << "Successfully connected to the server!" << std::endl;
}

bool MyClient::ProcessPacket(std::shared_ptr<Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		*packet >> chatmessage;
		std::cout << "Chat Message: " << chatmessage << std::endl;
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

		if (type == 4)
		{
			// set id
			if (myID == 0)
			{
				myID = id;
				cout << "changed id" << endl;
			}
			cout << "Client ID: " << myID << endl;

			//store the players
			uint32_t numOnlineClients;
			uint32_t clientId = 0;
			*packet >> numOnlineClients;
			for (uint32_t i = 0; i < numOnlineClients; ++i)
			{
				*packet >> clientId;
				NetworkClient::networkClients.push_back(NetworkClient());
				NetworkClient::networkClients.back().id = clientId;
			}
			return true;
		}

		if (id == myID) return true;

		if (type == 2)
		{
			cout << "Mapped " << id << endl;
			NetworkClient::networkClients.push_back(NetworkClient());
			NetworkClient::networkClients.back().id = id;

		}

		if (type == 3)
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
			NetworkClient::networkClients.erase(NetworkClient::networkClients.begin() + index);
			PlayerCharacter::playerCharacters[index].setIsUsed(false);
			PlayerCharacter::playerCharacters[index].setId(0);

			cout << "UnMapped " << id << " TO " << index << "\n";
		}


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
			vector<uint32_t> pakcets;
			uint32_t packetUInt;

			for(uint32_t i = 0; i < arraySize - 2; ++i)
			{
				*packet >> packetUInt;
				pakcets.push_back(packetUInt);
			}
			PlayerCharacter::readPacket(pakcets, index);
		}
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}

