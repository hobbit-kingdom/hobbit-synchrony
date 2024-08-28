#include "Client.h"


Client::Client()
{
    // Call the onUserCreate method. If it returns false, exit the constructor.
    if (!onUserCreate())
    {
        return;
    }

    // Set stopThreads to false, indicating that the threads should not be stopped
    stopThreads = false;

    // Start the various threads, passing them the appropriate member function pointers and the current object
    processThread = std::thread(&Client::processPacket, this);
    readThread = std::thread(&Client::readPacket, this);
    sendThread = std::thread(&Client::sendPacket, this);
}
Client::~Client()
{
    // If stopThreads is already true, exit the destructor
    if (stopThreads) return;

    // Set stopThreads to true, indicating that the threads should be stopped
    stopThreads = true;

    // Wait for all the threads to finish executing
    processThread.join();
    readThread.join();
    sendThread.join();
}

bool Client::onUserCreate()
{
    std::string ip;  // Variable to store the IP address

    do
    {
        // Prompt the user to enter the IP address of the server
        std::cout << "To exit press [q]" << std::endl;
        std::cout << "Enter IP of server: ";
        std::getline(std::cin, ip);
        std::cout << std::endl;

        // If the user enters 'q', return false to exit
        if (ip.at(0) == 'q')
            return false;

        // Attempt to connect to the server at the given IP address and port 60000
        Connect(ip, 60000);

        // Continue this loop until a connection is established
    } while (!IsConnected());

    // If a connection is established, return true
    return true;
}


void Client::processPacket()
{
    // Continue processing packets as long as the client is connected and not stopping
    while (!stopThreads && IsConnected())
    {
        // Sleep for a specified interval
        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_INTERVAL));

        // If the client is waiting for connection, skip this iteration
        if (waitingForConnection)
            continue;
        // Get all packets
        std::vector<std::vector<uint32_t>> allPackets = getAllPackets();

        // If there are no packets, skip this iteration
        if (allPackets.empty())
            continue;

        // Process each packet
        processEachPacket(allPackets);
    }
}
void Client::sendPacket()
{
    while (!stopThreads && IsConnected())
    {
        if (!waitingForConnection)
        {
            // Prepare the packets
            std::vector<uint32_t> snapshotPackets;
            std::vector<uint32_t> eventPackets;
            //GameManager::writePacket(snapshotPackets, eventPackets);

            // Send snapshot packets if any
            if (!snapshotPackets.empty())
            {
                sendSpecificPacket(snapshotPackets, PacketType::Game_Snapshot);
            }

            // Send event packets if any
            if (!eventPackets.empty())
            {
                sendSpecificPacket(eventPackets, PacketType::Game_EventClient);
            }
        }

        // Wait for the specified interval before sending the next packet
        std::this_thread::sleep_for(std::chrono::milliseconds(SEND_INTERVAL));
    }
}
void Client::readPacket()
{
    // Continue reading packets as long as the client is connected and not stopping
    while (!stopThreads && IsConnected())
    {
        // Sleep for a specified interval
        std::this_thread::sleep_for(std::chrono::milliseconds(READ_INTERVAL));

        // Break the loop if the client is no longer connected
        if (!IsConnected())
            break;

        // Process all incoming packets
        processIncomingPackets();
    }
}



std::vector<std::vector<uint32_t>> Client::getGameEventPackets()
{
    std::lock_guard<std::mutex> guard(gameEventPacketMutex);  // Ensure thread-safe access
    std::vector<std::vector<uint32_t>> packets;

    // Transfer packets from queue to vector
    while (!gameEventPackets.empty())
    {
        packets.push_back(gameEventPackets.front());
        gameEventPackets.pop();
    }

    return packets;
}
void Client::pushEventPacket(std::vector<uint32_t> packet)
{
    std::lock_guard<std::mutex> guard(gameEventPacketMutex);
    gameEventPackets.push(packet);
    std::cout << "EventPackets: " << gameEventPackets.size() << std::endl;
}


std::vector<std::vector<uint32_t>> Client::getAllPackets()
{
    // Get event and state packets
    std::vector<std::vector<uint32_t>> eventPackets = getGameEventPackets();
    std::vector<std::vector<uint32_t>> statePackets = ClientConnection::getAllStatePackets();

    // Combine event and state packets into all packets
    std::vector<std::vector<uint32_t>> allPackets;
    allPackets.insert(allPackets.end(), eventPackets.begin(), eventPackets.end());
    allPackets.insert(allPackets.end(), statePackets.begin(), statePackets.end());

    return allPackets;
}
void Client::processEachPacket(const std::vector<std::vector<uint32_t>>& allPackets)
{
    // Process each packet
    for (std::vector<uint32_t> packet : allPackets)
    {
        // If the packet is empty, skip this iteration
        if (packet.empty())
            continue;

        // Get the client index and remove it from the packet
        uint32_t clientIndex = packet.back();
        packet.pop_back();

        // Read the packet
       // GameManager::readPacket(packet, clientIndex);
    }
}


std::vector<uint32_t> Client::retrieveGameMsgs(net::packet<PacketType>& pkt)
{
    uint32_t connectedID;
    pkt >> connectedID;

    uint32_t clientIndex = ClientConnection::getClientIndex(connectedID);
    if (clientIndex == 0xFFFFFFFF)
        return {};

    uint32_t gameMsgSize;
    pkt >> gameMsgSize;

    std::vector<uint32_t> gameMsgs(gameMsgSize, 0);
    for (uint32_t& msg : gameMsgs)
    {
        pkt >> msg;
    }
    gameMsgs.push_back(clientIndex);

    return gameMsgs;
}
void Client::sendSpecificPacket(const std::vector<uint32_t>& packets, PacketType packetType)
{
    net::packet<PacketType> pkt;
    pkt.header.id = packetType;

    // Add all packets to the packet
    for (uint32_t i = packets.size(); i-- > 0;)
    {
        pkt << packets[i];
    }
    // Add the size of the packets and the client ID to the packet
    pkt << packets.size();
    pkt << clientID;

    // Send the packet
    Send(pkt);
}

void Client::processIncomingPackets()
{
    while (!Incoming().empty())
    {
        // Get the next packet
        auto pkt = Incoming().pop_front().pkt;

        // Print the current buffer size
        std::cout << "Buffer Size: " << Incoming().count() << std::endl;

        // Handle the packet based on its type
        handlePacket(pkt);
    }
}

void Client::handlePacket(net::packet<PacketType>& pkt)
{
    switch (pkt.header.id)
    {
    case(PacketType::Client_Accepted):
        handleClientAccepted(pkt);
        break;
    case(PacketType::Client_AssignID):
        handleClientAssignID(pkt);
        break;
    case(PacketType::Game_AddClient):
        handleGameAddClient(pkt);
        break;
    case(PacketType::Game_RemoveClient):
        handleGameRemoveClient(pkt);
        break;
    case(PacketType::Game_Snapshot):
        handleGameSnapshot(pkt);
        break;
    case(PacketType::Game_EventClient):
        handleGameEventClient(pkt);
        break;
    }
}
void Client::handleClientAccepted(net::packet<PacketType>& pkt)
{
    // The server has accepted the client
    std::cout << "Server accepted client - you're in!\n";

    // Register with the server
    net::packet<PacketType> registerPkt;
    registerPkt.header.id = PacketType::Client_RegisterWithServer;
    Send(registerPkt);
}
void Client::handleClientAssignID(net::packet<PacketType>& pkt)
{
    // The server has assigned an ID to the client
    pkt >> clientID;
    std::cout << "Assigned Client ID = " << clientID << "\n";
}
void Client::handleGameAddClient(net::packet<PacketType>& pkt)
{
    // A client has been added to the game
    uint32_t addedClientID;
    pkt >> addedClientID;

    // If the added client is this client, stop waiting for connection
    if (addedClientID == clientID)
    {
        waitingForConnection = false;
    }
    else
    {
        // Otherwise, add the client to the list of connections
        ClientConnection::addClient(addedClientID);
    }
}
void Client::handleGameRemoveClient(net::packet<PacketType>& pkt)
{
    // A client has been removed from the game
    uint32_t removeClientID = 0;
    pkt >> removeClientID;

    // If the removed client is this client, stop the threads
    if (removeClientID == clientID)
    {
        std::cout << "You are disconnected from the server" << std::endl;
        stopThreads = true;
    }
    else
    {
        // Otherwise, remove the client from the list of connections
        ClientConnection::removeClient(removeClientID);
    }
}
void Client::handleGameSnapshot(net::packet<PacketType>& pkt)
{
    auto gameMsgs = retrieveGameMsgs(pkt);
    if (!gameMsgs.empty())
        ClientConnection::connections[gameMsgs.back()].pushPacket(gameMsgs);
}
void Client::handleGameEventClient(net::packet<PacketType>& pkt)
{
    auto gameMsgs = retrieveGameMsgs(pkt);
    if (!gameMsgs.empty())
        pushEventPacket(gameMsgs);
}
