#pragma once
#include "MainPlayer.h"
#include "OtherPlayer.h"
#include <vector>
#include "../PNet/IncludeMe.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
class ClientGame
{
private:
	MainPlayer mainPlayer;
	std::vector<OtherPlayer> otherPlayers;
	static bool isLoaded;

public:
	void readPackets(std::vector<uint32_t> packets, uint32_t playerIdx)
	{
		if (!isLoaded)
			return;
		
		packets = mainPlayer.readPackets(packets);

		for (OtherPlayer& otherPlayer : otherPlayers)
		{
			otherPlayer.readPackets(packets);
		}
		
		// you can add more clases to read packets
	}
	std::vector<uint32_t> setPackets()
	{
		if (!isLoaded)
			return std::vector<uint32_t>();

		std::vector<uint32_t> packets;

		packets = mainPlayer.readPackets(packets);

		for (OtherPlayer& otherPlayer : otherPlayers)
		{
			otherPlayer.setPacket(packets);
		}

		// you can add more clases to set packets

		return packets;
	}

	void openNewLevel()
	{
		setPtrs();
	}
	void setPtrs()
	{
		MainPlayer::setPtrs();
		OtherPlayer::setPtrs();
	}

	void checkUpdateLevel()
	{
		static uint32_t isLoadingLayers;
		static uint32_t readLoadLayers;
		readLoadLayers = MemoryAccess::readData(0x00760864);//0x00760864 the loading state (1 - between lvl, 0 - in the level)
		isLoaded = !readLoadLayers;
		if (isLoadingLayers != readLoadLayers)
		{
			if (!readLoadLayers)
			{
				openNewLevel();
			}

			isLoadingLayers = readLoadLayers;
		}
	}
	int getLevel()
	{
		return MemoryAccess::readData(0x762b5c);
	}
};

