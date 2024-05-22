#pragma once
#include "Socket.h"
#include "PacketManager.h"

namespace PNet
{
	class TCPConnection
	{
	public:

		TCPConnection(Socket socket, IPEndpoint endpoint)
			: socket(socket), endpoint(endpoint)
		{
			stringRepresentation = endpoint.GetIPString();
			std::fill(std::begin(buffer), std::end(buffer), 0);
		}
		TCPConnection(): socket(Socket())
		{
			std::fill(std::begin(buffer), std::end(buffer), 0);
		}

		void Close();
		std::string ToString();
		Socket socket;

		PacketManager pm_incoming;
		PacketManager pm_outgoing;
		char buffer[PNet::g_MaxPacketSize];
		bool simpleTransmissionMode = false; //simple transmission = text only (for dcs)
	private:
		IPEndpoint endpoint;
		std::string stringRepresentation = "";
	};
}