#pragma once
#include <PNet/IncludeMe.h>
#include "../PNet/MemoryAccess.h"
#include "../HobbitMemory/NPC.h"
#include "../PNet/NetworkClient.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>
#include <unordered_map>
#include <iomanip>

class MyServer : public Server
{
public:
private:
	void OnConnect(TCPConnection& newConnection) override;
	void OnDisconnect(TCPConnection& lostConnection, std::string reason) override;
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
};