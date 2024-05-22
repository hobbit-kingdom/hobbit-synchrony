#pragma once
#include <PNet/IncludeMe.h>

#include "../GameManager/GameManager.h"

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

class MyClient : public Client
{
public:
	MyClient();
	void SendPacket();
	void Update();
	//void OnConnectFail() override;
	//void OnDisconnect(std::string reason) override;
private:
	uint32_t clientID;
	void OnConnect() override;
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
};