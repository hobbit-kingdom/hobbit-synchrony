#pragma once
#include <queue> //for std::queue
#include <memory> //for std::shared_ptr
#include "Packet.h"

namespace PNet
{
	enum PacketManagerTask
	{
		ProcessPacketSize,
		ProcessPacketContents
	};

	class PacketManager
	{
		//add constructor to see who inherits it
	private:
		std::queue<std::shared_ptr<Packet>> packets;
	public:
		void Clear();
		bool HasPendingPackets();
		void Append(std::shared_ptr<Packet> p);
		std::shared_ptr<Packet> Retrieve();
		std::shared_ptr<Packet> RetrieveLast();

		void Pop();

		uint16_t currentPacketSize = 0;
		int currentPacketExtractionOffset = 0;
		PacketManagerTask currentTask = PacketManagerTask::ProcessPacketSize;

	};
}