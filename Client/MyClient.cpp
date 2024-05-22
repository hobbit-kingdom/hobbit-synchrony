#include "MyClient.h"


using namespace std;

enum PackT : uint32_t {
	SEND_PLAYER_DATA = 0,
	ADD_PLAYER = 2,
	REMOVE_PLAYER = 3,
	CREATE_PLAYER = 4
};


unordered_map<string, int> ipToId = {  };

MyClient::MyClient()
{
	GameManager::Start();
}
void MyClient::SendPacket() {

	// when it is not initialized
	if (clientID == 0) return;

	std::shared_ptr<Packet> packet = std::make_shared<Packet>(PacketType::PT_IntegerArray);
	// set the packets
	vector<uint32_t> packets = GameManager::setPackets();
	// calculate the size, type of the packet, id
	*packet << 2 + uint32_t(packets.size()) << SEND_PLAYER_DATA << clientID;

	// pack all the packages
	for (uint32_t i = 0; i < uint32_t(packets.size()); ++i)
	{
		*packet << packets[i];
	}

	connection.pm_outgoing.Append(packet);
}
void MyClient::Update()
{
	GameManager::Update();
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


		if (type == SEND_PLAYER_DATA)
		{
			if (id == clientID) break;

			uint32_t index = NetworkClient::GetIndexByID(id);

			// get all packets to the vector packets
			vector<uint32_t> pakcets;
			uint32_t packetUInt;

			// subtract from size two elements (id, type)
			for (uint32_t i = 0; i < arraySize - 2; ++i)
			{
				*packet >> packetUInt;
				pakcets.push_back(packetUInt);
			}
			// apply the pakets to the game
			GameManager::readPackets(pakcets, index);
		}
		else if (type == ADD_PLAYER)
		{
			if (id == clientID) break;
			cout << "New Player ID: " << id << endl;
			NetworkClient::networkClients.push_back(NetworkClient());
			NetworkClient::networkClients.back().id = id;

		}
		else if (type == REMOVE_PLAYER)
		{
			if (id == clientID) break;
			uint32_t index = NetworkClient::GetIndexByID(id);

			NetworkClient::networkClients.erase(NetworkClient::networkClients.begin() + index);

			cout << "UnMapped " << id << " TO " << index << "\n";
		}
		else if (type == CREATE_PLAYER)
		{
			// set id
			clientID = id;
			cout << "Client ID: " << clientID << endl;

			//store the players connected to server
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
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}