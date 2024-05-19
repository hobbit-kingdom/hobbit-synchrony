#pragma once
#include <PNet/IncludeMe.h>

class MyClient : public Client
{
public:
	MyClient();
	void SendPacket();
	void Update();
	//void OnConnectFail() override;
	//void OnDisconnect(std::string reason) override;
private:
	void OnConnect() override;
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
};