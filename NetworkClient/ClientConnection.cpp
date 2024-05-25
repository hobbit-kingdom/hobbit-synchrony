#include "ClientConnection.h"

std::mutex ClientConnection::connectionsMutex;
std::vector<ClientConnection> ClientConnection::connections;


ClientConnection& ClientConnection::operator=(const ClientConnection& other)
{
    if (this != &other)
    {
        id = other.id;
        gameStatePackets = other.gameStatePackets;
    }
    return *this;
}

uint32_t ClientConnection::getClientIndex(uint32_t clientID)
{
    std::lock_guard<std::mutex> guard(connectionsMutex);
    for (uint32_t i = 0; i < connections.size(); ++i)
    {
        if (connections[i].id == clientID)
            return i;
    }
    return 0xFFFFFFFF; // Not found
}
bool ClientConnection::addClient(uint32_t clientID)
{
    std::lock_guard<std::mutex> guard(connectionsMutex);
    connections.emplace_back(clientID);
    return true;
}
bool ClientConnection::removeClient(uint32_t clientID)
{
    std::lock_guard<std::mutex> guard(connectionsMutex);
    for (auto it = connections.begin(); it != connections.end(); ++it)
    {
        if (it->id == clientID)
        {
            connections.erase(it);
            return true;
        }
    }
    return false;
}

void ClientConnection::pushPacket(std::vector<uint32_t> packet)
{
    std::lock_guard<std::mutex> guard(packetMutex);
    gameStatePackets.push(packet);
}
std::vector<uint32_t> ClientConnection::getPacket()
{
    std::lock_guard<std::mutex> guard(packetMutex);
    if (gameStatePackets.empty())
        return {};
    return gameStatePackets.top();
}
void ClientConnection::clearPackets()
{
    std::lock_guard<std::mutex> guard(packetMutex);
    std::stack<std::vector<uint32_t>>().swap(gameStatePackets);
}

std::vector<std::vector<uint32_t>> ClientConnection::getAllStatePackets()
{
    std::vector<std::vector<uint32_t>> statePackets;
    for (ClientConnection& conn : connections)
    {
        statePackets.push_back(conn.getPacket());
        conn.clearPackets();
    }
    return statePackets;
}