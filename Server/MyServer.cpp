#include "MyServer.h"
#include "../PNet/MemoryAccess.h"
#include "../HobbitMemory/NPC.h"
#include "../HobbitMemory/PlayerCharacter.h"
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


unordered_map<int, int> idToIndex = { {1111, 0} };
unordered_map<string, int> ipToId = {  };


LPVOID xPointer = MemoryAccess::readData(LPVOID(0x0075BA3C));
LPVOID pointerToAnimationOfBilbo = MemoryAccess::readData((LPVOID)0x0075BA3C);

MyServer::MyServer()
{
	string s;
	cout << "Input y and press ENTER after you've launched the level\n";
	cin >> s;

	//setup the MemoryAccess
	MemoryAccess::setExecutableName("Meridian.exe");

	xPointer = MemoryAccess::readData(LPVOID(0x0075BA3C));

	//calculating the animation address
	pointerToAnimationOfBilbo = LPVOID(0x560 + uint32_t(MemoryAccess::readData(LPVOID(0x0075BA3C))));
	pointerToAnimationOfBilbo = LPVOID(0x8 + uint32_t(MemoryAccess::readData(pointerToAnimationOfBilbo)));
	PlayerCharacter::OpenNewLevel();
}
void MyServer::SendPackets()
{
	uint32_t uintPosX = MemoryAccess::readData(LPVOID(0x7C4 + uint32_t(xPointer)));
	uint32_t uintPosY = MemoryAccess::readData(LPVOID(0x7C8 + uint32_t(xPointer)));
	uint32_t uintPosZ = MemoryAccess::readData(LPVOID(0x7CC + uint32_t(xPointer)));
	uint32_t uintRotY = MemoryAccess::readData(LPVOID(0x7AC + uint32_t(xPointer)));
	uint32_t animBilbo = MemoryAccess::readData(pointerToAnimationOfBilbo);

	std::shared_ptr<Packet> myLocation = std::make_shared<Packet>(PacketType::PT_IntegerArray);

	*myLocation << 7 << 0 << 1111 << uintPosX << uintPosY << uintPosZ << uintRotY << animBilbo;

	for (auto& connection : connections)
	{
		connection.pm_outgoing.Append(myLocation);
	}
}
void MyServer::Update()
{
	PlayerCharacter::checkUpdateLevel();
}
void MyServer::OnConnect(TCPConnection& newConnection)
{
	std::cout << newConnection.ToString() << " - New connection accepted." << std::endl;

	//number of connections one less then max players
	if (connections.size() > PlayerCharacter::MAX_PLAYERS - 1)
	{
		std::cout << "New player has connected but we kick him since the server is full\n";
		newConnection.Close();
		return;
	}

	for (int i = 0; i < PlayerCharacter::playerCharacters.size(); i++)
	{
		if (!PlayerCharacter::playerCharacters[i].getIsUsed())
		{
			srand(time(NULL));
			PlayerCharacter::playerCharacters[i].setIsUsed(true);
			PlayerCharacter::playerCharacters[i].setRandId();

			std::cout << "Assigned " << PlayerCharacter::playerCharacters[i].getId() << " " << "index " << i << '\n';

			//sending to a new player a packet with his id
			std::shared_ptr<Packet> assignId = std::make_shared<Packet>(PacketType::PT_IntegerArray);
			*assignId << 3 << 4 << PlayerCharacter::playerCharacters[i].getId() << i;
			newConnection.pm_outgoing.Append(assignId);

			//creating a packet to map a new player to existing player
			std::shared_ptr<Packet> mapNewPlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);

			idToIndex[PlayerCharacter::playerCharacters[i].getId()] = i;
			ipToId[newConnection.ToString()] = PlayerCharacter::playerCharacters[i].getId();

			//[explain]
			// 3? 2? playerID playerNum
			*mapNewPlayer << 3 << 2 << PlayerCharacter::playerCharacters[i].getId() << i;

			//[explain]
			for (auto& connection : connections)
			{
				if (&connection == &newConnection)
					continue;

				connection.pm_outgoing.Append(mapNewPlayer);
			}

			return;
		}
	}

}
void MyServer::OnDisconnect(TCPConnection& lostConnection, std::string reason)
{
	std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << "." << std::endl;

	//if someone is running 2 clients from 1 pc ( basically ip adresses is the same ) some troubles may occure
	//since we delete the ip adress from hastables of everyone else as well
	std::shared_ptr<Packet> removeDisconnectedPlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);

	int playerId = ipToId[lostConnection.ToString()];
	int ind = idToIndex[playerId];

	//[explain]
	*removeDisconnectedPlayer << 2 << 3 << playerId << ind;

	//this fake bilbo is now free and available to use for someone else
	PlayerCharacter::playerCharacters[ind].setIsUsed(false);
	PlayerCharacter::playerCharacters[ind].setId(0);

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
			cout << "Received\n";
			cout << arraySize << " " << type << " " << id << " ";
			cout << endl;

			cout << "\033[93m";
			cout << "Recieve Packet" << endl;
			cout << string(20, '~') << endl;

			// [explain]
			std::shared_ptr<Packet> positonPacket = std::make_shared<Packet>(PacketType::PT_IntegerArray);
			*positonPacket << arraySize << type << id;

			uint32_t element = 0;

			// set x position
			*packet >> element;
			*positonPacket << element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setPositionX(UInt32Wrapper(element));
			cout << "Xpos: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			// set y position
			*packet >> element;
			cout << float(UInt32Wrapper(element));
			*positonPacket << element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setPositionY(UInt32Wrapper(element));
			cout << "Ypos: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			// set z position
			*packet >> element;
			cout << float(UInt32Wrapper(element));
			*positonPacket << element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setPositionZ(UInt32Wrapper(element));
			cout << "YZos: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			cout << endl;

			// set y rotation
			*packet >> element;
			cout << float(UInt32Wrapper(element));
			*positonPacket << element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setRotationY(UInt32Wrapper(element));
			cout << "Yrot: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			// set animation
			*packet >> element;
			cout << int(UInt32Wrapper(element));
			*positonPacket << element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setAnimation(UInt32Wrapper(element));
			cout << "Anim: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			cout << endl;
			cout << string(20, '~');
			cout << "\033[0m";
			cout << endl;

			// [explain]
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

