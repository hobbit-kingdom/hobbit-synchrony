#include "MyServer.h"

using namespace std;

enum PackageType : uint32_t {
	SEND_PLAYER_DATA = 0,
	ADD_PLAYER = 1,
	REMOVE_PLAYER = 2,
	CREATE_PLAYER = 3
};

unordered_map<string, int> ipToId = {  };


void MyServer::OnConnect(TCPConnection& newConnection)
{
	std::cout << newConnection.ToString() << " - New connection accepted." << std::endl;

	// check if overloaded
	if (connections.size() > NetworkClient::MAX_PLAYERS)
	{
		std::cout << "New player has connected but we kick him since the server is full\n";
		newConnection.Close();
		return;
	}

	// add client to the list
	NetworkClient::networkClients.push_back(NetworkClient());
	std::cout << "Assigned " << NetworkClient::networkClients.back().id << " " << "index " << NetworkClient::networkClients.size() << '\n';


	// assign ID to client
	std::shared_ptr<Packet> createPlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);
	*createPlayer << 2 << PackageType::CREATE_PLAYER << NetworkClient::networkClients.back().id;
	// send data about other clients, not including itself
	*createPlayer << NetworkClient::networkClients.size() - 1;
	for (int i = 0; i < NetworkClient::networkClients.size() - 1; ++i)
	{
		*createPlayer << NetworkClient::networkClients[i].id;
	}
	//send only to new conection
	newConnection.pm_outgoing.Append(createPlayer);


	// notify other clients about new client
	std::shared_ptr<Packet> addPlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);
	ipToId[newConnection.ToString()] = NetworkClient::networkClients.back().id;
	*addPlayer << 2 << PackageType::ADD_PLAYER << NetworkClient::networkClients.back().id;
	// send to everyone
	for (auto& connection : connections)
	{
		if (&connection == &newConnection)
			continue;

		connection.pm_outgoing.Append(addPlayer);
	}
}
void MyServer::OnDisconnect(TCPConnection& lostConnection, std::string reason)
{
	std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << "." << std::endl;
	
	// gets id of the player to remove
	uint32_t playerId = ipToId[lostConnection.ToString()];

	// find index
	uint32_t index = NetworkClient::GetIndexByID(playerId);
	// remove client
	NetworkClient::networkClients.erase(NetworkClient::networkClients.begin() + index);

	// sned remove player message
	std::shared_ptr<Packet> removePlayer = std::make_shared<Packet>(PacketType::PT_IntegerArray);
	*removePlayer << 2 << PackageType::REMOVE_PLAYER << playerId;
	for (auto& connection : connections)
	{
		if (&connection == &lostConnection)
			continue;

		connection.pm_outgoing.Append(removePlayer);
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

		if (type == SEND_PLAYER_DATA)
		{
			std::shared_ptr<Packet> positonPacket = std::make_shared<Packet>(PacketType::PT_IntegerArray);
			uint32_t packetUInt;


			*positonPacket << arraySize << type << id;
			// subtract from size two elements (id, type)
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