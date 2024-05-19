#include "MyClient.h"
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

int myID = 0;

unordered_map<int, int> idToIndex = { {1111, 0} };
unordered_map<string, int> ipToId = {  };

LPVOID xPointer;
LPVOID pointerToAnimationOfBilbo;


MyClient::MyClient()
{
	string s;
	cout << "Input y and press ENTER after you've launched the level\n";
	cin >> s;

	//setup the MemoryAccess
	MemoryAccess::setExecutableName("Meridian.exe");

	//setup player
	xPointer = MemoryAccess::readData(LPVOID(0x0075BA3C));

	//calculating the animation address
	pointerToAnimationOfBilbo = LPVOID(0x560 + uint32_t(MemoryAccess::readData(LPVOID(0x0075BA3C))));
	pointerToAnimationOfBilbo = LPVOID(0x8 + uint32_t(MemoryAccess::readData(pointerToAnimationOfBilbo)));
	//find all playerCharacters in level

	PlayerCharacter::OpenNewLevel();
}
void MyClient::SendPacket() {
	if (myID == 0) return;

	uint32_t uintPosX = MemoryAccess::readData(LPVOID(0x7C4 + uint32_t(xPointer)));
	uint32_t uintPosY = MemoryAccess::readData(LPVOID(0x7C8 + uint32_t(xPointer)));
	uint32_t uintPosZ = MemoryAccess::readData(LPVOID(0x7CC + uint32_t(xPointer)));
	uint32_t uintRotY = MemoryAccess::readData(LPVOID(0x7AC + uint32_t(xPointer)));
	uint32_t animBilbo = MemoryAccess::readData(pointerToAnimationOfBilbo);

	std::shared_ptr<Packet> myLocation = std::make_shared<Packet>(PacketType::PT_IntegerArray);

	*myLocation << 7 << 0 << myID << uintPosX << uintPosY << uintPosZ << uintRotY << animBilbo;

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

		if (type == 4) cout << "Just " << myID << '\n';

		if (type == 4 && myID == 0)
		{
			myID = id;
			cout << "Setted my id to " << myID << '\n';
			*packet >> id;
			cout << "Index don't care " << id << '\n';

			return true;
		}

		if (id == myID) return true;

		if (type == 2)
		{
			uint32_t index = 0;
			*packet >> index;
			idToIndex[(id)] = (index);

			cout << "Mapped " << id << " TO " << index << "\n";
		}

		if (type == 3)
		{
			uint32_t index = 0;
			*packet >> index;

			idToIndex.erase(id);

			PlayerCharacter::playerCharacters[index].setIsUsed(false);
			PlayerCharacter::playerCharacters[index].setId(0);

			cout << "UnMapped " << id << " TO " << index << "\n";
		}


		if (type == 0)
		{
			cout << "Received\n";
			cout << arraySize << " " << type << " " << id << " ";
			cout << endl;

			cout << "\033[93m";
			cout << "Recieve Packet" << endl;
			cout << string(20, '~') << endl;


			// set x position
			uint32_t element = 0;
			*packet >> element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setPositionX(UInt32Wrapper(element));
			cout << "Xpos: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			// set y position
			*packet >> element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setPositionY(UInt32Wrapper(element));
			cout << "Ypos: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			// set z position
			*packet >> element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setPositionZ(UInt32Wrapper(element));
			cout << "Zpos: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			cout << endl;

			// set y rotation
			*packet >> element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setRotationY(UInt32Wrapper(element));
			cout << "Yrot: " << setw(10) << float(UInt32Wrapper(element)) << "| ";

			// set animation
			*packet >> element;
			PlayerCharacter::playerCharacters[idToIndex[id]].setAnimation(UInt32Wrapper(element));
			cout << "Anim: " << setw(10) << int(UInt32Wrapper(element)) << "| ";

			cout << endl;
			cout << string(20, '~');
			cout << "\033[0m";
			cout << endl;
		}
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}

