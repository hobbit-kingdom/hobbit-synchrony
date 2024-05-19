#pragma once
#include <PNet/IncludeMe.h>

class MyServer : public Server
{
public:
	MyServer();
	void SendPackets();
private:
	void OnConnect(TCPConnection& newConnection) override;
	void OnDisconnect(TCPConnection& lostConnection, std::string reason) override;
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
};