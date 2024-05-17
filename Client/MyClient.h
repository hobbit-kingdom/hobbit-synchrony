#pragma once
#include <PNet/IncludeMe.h>

class MyClient : public Client
{
	

public:
	MyClient();
	void SendPacket();
	void FindHobbits();
	//void OnConnectFail() override;
	//void OnDisconnect(std::string reason) override;
private:
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
	void OnConnect() override;
};