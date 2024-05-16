#include "MyServer.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>
#include <unordered_map>

//types of data 
//  1 - new player Joined 
//  0 - send positon rotation and animation
//  2 - add newplayer to hashmaps
//  3 - remove player from hashmaps

vector<float> fakePositions = { -2631.110107, -2831.110107, -2431.110107 };
vector<float> fakeRotations = { -2.796018124, -2.621485233, -2.446952105 };
vector<long long> fakeGuids = { 3887403015, 3887403009 , 3887403010 };


unordered_map<int, int> idToIndex = { {1111, 0} };

unordered_map<string, int> ipToId = {  };

union Type
{
	int8_t signed8;
	uint8_t unsigned8;
	int16_t signed16;
	uint16_t unsigned16;
	int32_t signed32;
	uint32_t unsigned32;
	int64_t signed64;
	uint64_t unsigned64;
	float real32;
	double real64;
};

enum Types
{
	t_signed8,
	t_unsigned8,
	t_signed16,
	t_unsigned16,
	t_signed32,
	t_unsigned32,
	t_signed64,
	t_unsigned64,
	t_real32,
	t_real64,
	t_string,
	typesCount
};

struct GenericType
{
	Type data = {};
	int type = 0;

	void* ptr() { return &data; }
	int getBytesSize()
	{
		if (type == t_signed8 ||
			type == t_unsigned8)
		{
			return 1;
		}
		if (type == t_signed16 ||
			type == t_unsigned16)
		{
			return 2;
		}
		if (type == t_signed32 ||
			type == t_unsigned32 ||
			type == t_real32)
		{
			return 4;
		}
		if (type == t_signed64 ||
			type == t_unsigned64 ||
			type == t_real64)
		{
			return 8;
		}

		return 0;
	}
};

struct FakeBilbo
{
	vector<void*> posx;
	vector<void*> roty;
	void* guid;

	bool used = false;
	vector<void*> anim;
	int id;
};


GenericType data1;
std::vector<void*> foundValues;
std::vector<void*> foundValuesPosX;
std::vector<void*> foundValuesRotY;
LPDWORD xPointer = 0x00;
float yRot = 0;

vector<FakeBilbo> fakeBilbos;


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

			std::shared_ptr<Packet> positonPacket = std::make_shared<Packet>(PacketType::PT_IntegerArray);

			*positonPacket << arraySize << type << id;

			uint32_t element = 0;

			*packet >> element;

			float convertedEl = uint32ToFloat(element);
			*positonPacket << element;
			cout << convertedEl << " ";
			for (int i = 0; i < fakeBilbos[idToIndex[id]].posx.size(); i++)
			{
				change_float_hobbit((LPVOID)(fakeBilbos[idToIndex[id]].posx[i]), convertedEl);
			}

			*packet >> element;
			convertedEl = uint32ToFloat(element);
			*positonPacket << element;
			cout << convertedEl << " ";

			for (int i = 0; i < fakeBilbos[idToIndex[id]].posx.size(); i++)
			{
				change_float_hobbit((LPVOID)((char*)fakeBilbos[idToIndex[id]].posx[i] + 0x4), convertedEl);
			}

			*packet >> element;
			convertedEl = uint32ToFloat(element);
			*positonPacket << element;
			cout << convertedEl << " ";

			for (int i = 0; i < fakeBilbos[idToIndex[id]].posx.size(); i++)
			{
				change_float_hobbit((LPVOID)((char*)fakeBilbos[idToIndex[id]].posx[i] + 0x8), convertedEl);
			}

			*packet >> element;
			convertedEl = uint32ToFloat(element);
			*positonPacket << element;
			cout << convertedEl << " ";

			for (int i = 0; i < fakeBilbos[idToIndex[id]].roty.size(); i++)
			{
				change_float_hobbit((LPVOID)(fakeBilbos[idToIndex[id]].roty[i]), convertedEl);
			}

			*packet >> element;

			*positonPacket << element;
			int convInt = element;

			cout << convInt << "\n";

			for (int i = 0; i < fakeBilbos[idToIndex[id]].anim.size(); i++)
			{
				change_1Byte_hobbit_no_switch((LPVOID)(fakeBilbos[idToIndex[id]].anim[i]), convInt);
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


void MyServer::OnConnect(TCPConnection& newConnection)
{
	std::cout << newConnection.ToString() << " - New connection accepted." << std::endl;

	if (connections.size() > 2)
	{
		std::cout << "New player has connected but we kick him since the server is full\n";
		newConnection.Close();
		return;
	}

	std::cout << "Total Fake Bilbos - " << fakeBilbos.size() << '\n';

	for (int i = 1; i < fakeBilbos.size(); i++)
	{
		if (!fakeBilbos[i].used)
		{
			srand(time(NULL));
			fakeBilbos[i].used = true;
			fakeBilbos[i].id = (rand() % 100000) + 1112;

			std::cout << "Assigned " << fakeBilbos[i].id << " " << "index " << i << '\n';

			//sending to a new player a packet with his id
			std::shared_ptr<Packet> assignId = std::make_shared<Packet>(PacketType::PT_IntegerArray);
			*assignId << 3 << 4 << fakeBilbos[i].id << i;
			newConnection.pm_outgoing.Append(assignId);

			//creating a packet to map a new player to existing player
			std::shared_ptr<Packet> mapNewPlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);

			idToIndex[fakeBilbos[i].id] = i;
			ipToId[newConnection.ToString()] = fakeBilbos[i].id;

			*mapNewPlayer << 3 << 2 << fakeBilbos[i].id << i;

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

	*removeDisconnectedPlayer << 2 << 3 << playerId << ind;

	//this fake bilbo is now free and available to use for someone else
	fakeBilbos[ind].used = false;
	fakeBilbos[ind].id = 0;

	for (auto& connection : connections)
	{
		if (&connection == &lostConnection)
			continue;

		connection.pm_outgoing.Append(removeDisconnectedPlayer);
	}
}

LPDWORD pointerToAnimationOfBilbo;

void MyServer::FindHobbits()
{
	DWORD processId = findPidByName("Meridian.exe");
	cout << "PROCESS ID " << processId << " \n";

	xPointer = ukazatel_hobbit((LPVOID)0x0075BA3C);

	for (int i = 0; i < fakePositions.size(); i++)
	{
		FakeBilbo fakeBilbo;
		vector<void*> guid;

		float fakePos = 0;
		float fakeRot = 0;

		//guid

		data1 = {};
		data1.type = 4;
		data1.data.unsigned32 = fakeGuids[i];

		guid = findBytePatternInProcessMemory(read_process_hobbit(), data1.ptr(), data1.getBytesSize());

		for (auto g : guid)
		{
			LPVOID lp = (LPVOID)((char*)g + 0xC);

			if (read_float_value(lp) > 1 || read_float_value(lp) < -1)
			{
				LPVOID lp2 = (LPVOID)((char*)g + 0x64);

				fakeBilbo.guid = g;
				fakePos = read_float_value(lp);
				fakeRot = read_float_value(lp2);

				cout << "X pos by GUID " << fakePos << '\n';
				cout << "Y rot by GUID " << fakeRot << '\n';

				string s;
				std::cout << "Input y and press ENTER if X looks like simillar to xxxx.xx ( 12.23, 155.22, -2456.02) \n";
				std::cout << "Otherwise press n\n";
				std::cin >> s;

				if (s == "y")
					break;


			}
		}


		//positions
		data1 = {};
		data1.type = 4;
		data1.data.real32 = fakePos;// fakePositions[i];

		fakeBilbo.posx = findBytePatternInProcessMemory(read_process_hobbit(), data1.ptr(), data1.getBytesSize());

		//rotation
		data1 = {};
		data1.type = 4;
		data1.data.real32 = fakeRot; // fakeRotations[i];

		fakeBilbo.roty = findBytePatternInProcessMemory(read_process_hobbit(), data1.ptr(), data1.getBytesSize());
		cout << "Rotations: ";
		for (auto it : fakeBilbo.roty) cout << it << " ";
		cout << "\n";
		//animation
		for (auto ittt : fakeBilbo.posx)
		{
			LPVOID lp = (LPVOID)((char*)ittt + 0xC4);
			if (read_int_value(lp) == 0) fakeBilbo.anim.push_back(lp);
		}
		if (i == 0) fakeBilbo.used = true;
		fakeBilbos.push_back(fakeBilbo);
	}
	pointerToAnimationOfBilbo = ukazatel_hobbit((LPVOID)0x0075BA3C);

	cout << "Finished finding all the Fake Biblo's on the level\n";
	cout << "Total amount of fake npc's found - " << fakeBilbos.size() << '\n';
	cout << "If none or not all are found - please restart the level from the beginning\n";
}

void MyServer::SendPackets()
{
	float xPos = read_float_value((LPVOID)(xPointer + 497));
	float yPos = read_float_value((LPVOID)(xPointer + 498));
	float zPos = read_float_value((LPVOID)(xPointer + 499));

	uint32_t uintPosX;
	floatToBytes(xPos, uintPosX);

	uint32_t uintPosY;
	floatToBytes(yPos, uintPosY);

	uint32_t uintPosZ;
	floatToBytes(zPos, uintPosZ);

	yRot = read_float_value((LPVOID)(xPointer + 491));

	uint32_t uintRotY;
	floatToBytes(yRot, uintRotY);

	int animBilbo = (read_int_value(ukazatel_hobbit(pointerToAnimationOfBilbo + (344)) + 2));

	std::shared_ptr<Packet> myLocation = std::make_shared<Packet>(PacketType::PT_IntegerArray);

	*myLocation << 7 << 0 << 1111 << uintPosX << uintPosY << uintPosZ << uintRotY << animBilbo;

	for (auto& connection : connections)
	{
		connection.pm_outgoing.Append(myLocation);
	}
}