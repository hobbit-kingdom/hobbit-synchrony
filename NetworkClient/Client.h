#pragma once

// Standard library headers
#include <vector>
#include <mutex>
#include <queue>

// Project-specific headers
#include "../NetworkManager/PacketType.h"  // Packet types for network communication
#include "../GameManager/GameManager.h"    // Game management functionality
#include "ClientConnection.h"              // Client connection handling

// The Client class inherits from the client_interface class template
class Client : public net::client_interface<PacketType>
{
private:
    uint32_t clientID = 0;  // Unique identifier for the client

    // Threads for various operations
    std::thread updateThread;    // Thread for updating the game
    std::thread processThread;   // Thread for processing packets
    std::thread readThread;      // Thread for reading packets
    std::thread sendThread;      // Thread for sending packets

    // Atomic variables for thread control
    std::atomic<bool> waitingForConnection = true;  // Flag for connection status
    std::atomic<bool> stopThreads = true;           // Flag to control thread execution

    // Constants for interval timings
    const std::atomic<uint32_t> UPDATE_INTERVAL = 200;     // Update interval in milliseconds
    const std::atomic<uint32_t> PROCESS_INTERVAL = 200;    // Packet processing interval in milliseconds
    const std::atomic<uint32_t> READ_INTERVAL = 200;       // Packet reading interval in milliseconds
    const std::atomic<uint32_t> SEND_INTERVAL = 200;       // Packet sending interval in milliseconds

    // Queue for game event packets
    std::queue<std::vector<uint32_t>> gameEventPackets;  // Queue to hold game event packets
    std::mutex gameEventPacketMutex;  // Mutex for thread-safe access to the queue

public:
    Client();  // Constructor

    ~Client();  // Destructor


private:
    bool onUserCreate();  // Method called upon user creation  


    // Methods for various operations
    void updateGame();       // Method to update the game state
    void processPacket();    // Method to process packets
    void readPacket();       // Method to read packets
    void sendPacket();       // Method to send packets

    // Method to retrieve game event packets
    // Mutex: gameEventPacketMutex
    std::vector<std::vector<uint32_t>> getGameEventPackets();
    // Method to push an event packet into the queue
    // Mutex: gameEventPacketMutex
    void pushEventPacket(std::vector<uint32_t> packet);

    // Helper methods for packet processing
    std::vector<std::vector<uint32_t>> getAllPackets();  // Method to get all packets
    void processEachPacket(const std::vector<std::vector<uint32_t>>& allPackets);  // Method to process each packet
    std::vector<uint32_t> retrieveGameMsgs(net::packet<PacketType>& pkt);  // Method to retrieve game messages from a packet
    void sendSpecificPacket(const std::vector<uint32_t>& packets, PacketType packetType);  // Method to send a specific type of packet

    void processIncomingPackets();  // Method to process incoming packets

    // Packet handling methods
    void handlePacket(net::packet<PacketType>& pkt);  // Method to handle a packet based on its type
    void handleClientAccepted(net::packet<PacketType>& pkt);  // Method to handle client accepted packet
    void handleClientAssignID(net::packet<PacketType>& pkt);  // Method to handle client ID assignment packet
    void handleGameAddClient(net::packet<PacketType>& pkt);  // Method to handle game add client packet
    void handleGameRemoveClient(net::packet<PacketType>& pkt);  // Method to handle game remove client packet
    void handleGameSnapshot(net::packet<PacketType>& pkt);  // Method to handle game snapshot packet
    void handleGameEventClient(net::packet<PacketType>& pkt);  // Method to handle game event client packet
};
