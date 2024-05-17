#include "MyClient.h"
#include "../PNet/MemoryAccess.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>
#include <unordered_map>

using namespace memoryAccess;
using namespace std;

struct FakeBilbo
{
	uint32_t guid;
	vector<void*> posx;
	vector<void*> roty;

	bool used = false;
	vector<void*> anim;
	int id;
};

vector<float> fakePositions = { -2631.110107, -2831.110107, -2431.110107 };
vector<float> fakeRotations = { -2.796018124, -2.621485233, -2.446952105 };
vector<long long> fakeGuids = { 3887403015, 3887403009 , 3887403010 };

unordered_map<int, int> idToIndex = { {1111, 0} };

int myID = 0;

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

GenericType data1;
std::vector<void*> foundValues;
std::vector<void*> foundValuesPosX;
std::vector<void*> foundValuesRotY;
LPDWORD xPointer = 0x00;
float yRot = 0;

vector<FakeBilbo> fakeBilbos;

LPDWORD pointerToAnimationOfBilbo = MemoryAccess::ReadData((LPVOID)0x0075BA3C);
const UInt32Wrapper OBJECT_ARRAY_PTR = UInt32Wrapper(0x0076F648);

MyClient::MyClient()
{
	MemoryAccess::setExecutableName("Meridian.exe");
}

void SetFakeBilbo(uint32_t addressFBilbo)
{
	FakeBilbo fakeBilbo;

	float fakePos = 0;
	float fakeRot = 0;

	data1 = {};
	data1.type = 4;
	data1.data.unsigned32 = MemoryAccess::ReadData(LPVOID(addressFBilbo + 0x8));


	fakeBilbo.guid = addressFBilbo + 0x8;

	LPVOID lp = (LPVOID)(fakeBilbo.guid + 0xC);

	if (read_float_value(lp) > 1 || read_float_value(lp) < -1)
	{
		LPVOID lp2 = (LPVOID)(fakeBilbo.guid + 0x64);

		fakePos = read_float_value(lp);
		fakeRot = read_float_value(lp2);
	}




	//positions
	data1 = {};
	data1.type = 4;
	data1.data.real32 = fakePos; //fakePositions[i];

	fakeBilbo.posx = findBytePatternInProcessMemory(read_process_hobbit(), data1.ptr(), data1.getBytesSize());

	//rotation
	data1 = {};
	data1.type = 4;
	data1.data.real32 = fakeRot; //fakeRotations[i];

	fakeBilbo.roty = findBytePatternInProcessMemory(read_process_hobbit(), data1.ptr(), data1.getBytesSize());

	//animation
	for (auto ittt : fakeBilbo.posx)
	{
		LPVOID lp = (LPVOID)((char*)ittt + 0xC4);

		if (read_int_value(lp) == 0)
			fakeBilbo.anim.push_back(lp);
	}

	//set Server's fake Bilbo
	if (fakeBilbos.size() == 0)
	{
		fakeBilbo.used = true;
		fakeBilbo.id = 1111;
	}
	fakeBilbos.push_back(fakeBilbo);
}
void MyClient::FindHobbits()
{
	DWORD processId = findPidByName("Meridian.exe");
	cout << "PROCESS ID " << processId << " \n";

	xPointer = ukazatel_hobbit((LPVOID)0x0075BA3C);

	uint32_t arrayStartAddress = MemoryAccess::ReadData((LPVOID)OBJECT_ARRAY_PTR);
	for (size_t i = 0xEFEC; i > 0; i -= 0x14)
	{
		uint32_t objectAddress = MemoryAccess::ReadData((LPVOID)(arrayStartAddress + i));
		uint32_t objectGUID = MemoryAccess::ReadData((LPVOID)(objectAddress + 0x8));
		//cout << objectGUID << endl;

		for (int j = 0; j < fakePositions.size(); j++)
		{
			if (objectGUID == (uint32_t)fakeGuids[j])
			{
				cout << "FOUND MODEL" << endl;
				SetFakeBilbo(objectAddress);
				break;
			}
			if (objectGUID == 0)
			{
				break;
			}
		}
	}

	cout << "OK, finished founding objects. This process must've been run after a level is loaded.\n";
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
			fakeBilbos[index].used = false;
			fakeBilbos[index].id = 0;

			cout << "UnMapped " << id << " TO " << index << "\n";

		}


		if (type == 0)
		{
			cout << "received\n";

			cout << arraySize << " " << type << " " << id << " ";

			uint32_t element = 0;

			*packet >> element;

			float convertedEl = uint32ToFloat(element);
			cout << convertedEl << " ";

			for (int i = 0; i < fakeBilbos[idToIndex[id]].posx.size(); i++)
			{
				change_float_hobbit((LPVOID)(fakeBilbos[idToIndex[id]].posx[i]), convertedEl);
			}

			*packet >> element;
			convertedEl = uint32ToFloat(element);
			cout << convertedEl << " ";
			for (int i = 0; i < fakeBilbos[idToIndex[id]].posx.size(); i++)
			{
				change_float_hobbit((LPVOID)((char*)fakeBilbos[idToIndex[id]].posx[i] + 0x4), convertedEl);
			}

			*packet >> element;
			convertedEl = uint32ToFloat(element);
			cout << convertedEl << " ";
			for (int i = 0; i < fakeBilbos[idToIndex[id]].posx.size(); i++)
			{
				change_float_hobbit((LPVOID)((char*)fakeBilbos[idToIndex[id]].posx[i] + 0x8), convertedEl);
			}

			*packet >> element;
			convertedEl = uint32ToFloat(element);
			cout << convertedEl << " ";
			for (int i = 0; i < fakeBilbos[idToIndex[id]].roty.size(); i++)
			{
				change_float_hobbit((LPVOID)(fakeBilbos[idToIndex[id]].roty[i]), convertedEl);
			}

			*packet >> element;
			int convInt = (element);
			cout << convInt << "\n";
			for (int i = 0; i < fakeBilbos[idToIndex[id]].anim.size(); i++)
			{
				change_1Byte_hobbit_no_switch((fakeBilbos[idToIndex[id]].anim[i]), convInt);
			}
		}
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}
void MyClient::OnConnect()
{
	std::cout << "Successfully connected to the server!" << std::endl;
}
void MyClient::SendPacket() {
	if (myID == 0) return;

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

	*myLocation << 7 << 0 << myID << uintPosX << uintPosY << uintPosZ << uintRotY << animBilbo;

	connection.pm_outgoing.Append(myLocation);
}