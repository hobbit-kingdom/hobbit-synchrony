#pragma once

// Standard library headers
#include <vector>
#include <mutex>
#include <stack>

// The ClientConnection class represents a connection from a client
class ClientConnection
{
private:
    // Private member variables
    uint32_t id;  // Unique identifier for the client
    std::stack<std::vector<uint32_t>> gameStatePackets;  // Stack to hold game state packets
    std::mutex packetMutex;  // Mutex for thread-safe access to the stack

  

public:
    // Public member variables
    static std::vector<ClientConnection> connections;  // Vector to hold all client connections
    static std::mutex connectionsMutex;  // Mutex for thread-safe access to the vector

    // Constructors
    ClientConnection(uint32_t newID) : id(newID) {}  // Constructor with client ID as parameter
    ClientConnection(ClientConnection&& other) noexcept : id(other.id), gameStatePackets(std::move(other.gameStatePackets)) {}  // Move constructor

    // Assignment operator
    ClientConnection& operator=(const ClientConnection& other);


    // Public static methods
    static uint32_t getClientIndex(uint32_t clientID);  // Static method to get the index of a client in the vector
    static bool addClient(uint32_t clientID);  // Static method to add a client to the vector
    static bool removeClient(uint32_t clientID);  // Static method to remove a client from the vector
    static std::vector<std::vector<uint32_t>> getAllStatePackets();  // Static method to get all state packets from all clients

    // Private methods 
    void pushPacket(std::vector<uint32_t> packet);  // Method to push a packet into the stack, protected by packetMutex
    std::vector<uint32_t> getPacket();  // Method to get a packet from the stack, protected by packetMutex
    void clearPackets();  // Method to clear all packets in the stack, protected by packetMutex
};
