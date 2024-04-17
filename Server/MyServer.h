#pragma once
#include <PNet/IncludeMe.h>

using namespace std;

class MyServer : public Server
{
public:
	void SendPackets();
	void FindHobbits();
private:
	void OnConnect(TCPConnection& newConnection) override;
	void OnDisconnect(TCPConnection& lostConnection, std::string reason) override;
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
};