#pragma once
#include "NPC.h"
#include <vector>
#include <iomanip>
#include <iostream>
class PlayerCharacter : public NPC
{
public:
	// stores all NPCrgeom for players
	static std::vector<PlayerCharacter> playerCharacters;
	static const size_t MAX_PLAYERS = 3;	// number of players on one server
	static const std::vector<uint32_t> fakeGuids;
	static const LPVOID X_POSITION_PTR;


	static LPVOID bilboPosXPTR;
	static LPVOID bilboAnimPTR;

	// Constructors
	//PlayerCharacter(UInt32Wrapper GUIDToFind) : NPC(UInt32Wrapper(GUIDToFind)) {}
	PlayerCharacter(LPVOID addresOfNPC) : NPC(addresOfNPC) 
	{
		 
	}

	// isUsed
	bool getIsUsed() { return isUsed; }
	void setIsUsed(bool newIsUsed) { isUsed = newIsUsed; }

	// Id
	int getId() { return id; };
	void setId(int newId) { id = newId; }
	void setRandId() { id = (rand() % 100000) + 1112; }

	// find all playerCharacters
	static void findHobbits()
	{
		LPVOID arrayStartAddress = MemoryAccess::readData(LPVOID(LPVOID(0x0076F648)));
		for (int i = 0; i < fakeGuids.size(); i++)
		{
			LPVOID addressFBilbo = MemoryAccess::findDataInStackHobbit(arrayStartAddress, 0xEFEC, 0x14, fakeGuids[i]);
			PlayerCharacter fakeBilbo = PlayerCharacter(addressFBilbo);

			// first character reserved for server
			if (playerCharacters.size() == 0)
			{
				fakeBilbo.setIsUsed(true);
				fakeBilbo.setId(1111);
			}

			playerCharacters.push_back(fakeBilbo);
		}
	}

	static void OpenNewLevel()
	{
		playerCharacters.clear();
		findHobbits();
		setPtrs();
	}
	static void setPtrs()
	{
		bilboPosXPTR = MemoryAccess::readData(LPVOID(X_POSITION_PTR));
		bilboAnimPTR = LPVOID(0x8 + uint32_t(MemoryAccess::readData(LPVOID(0x560 + uint32_t(MemoryAccess::readData(X_POSITION_PTR))))));
	}
	static void checkUpdateLevel()
	{
		static uint32_t isLoadingLayers;
		static uint32_t readLoadLayers;
		readLoadLayers = MemoryAccess::readData(LPVOID(0x00760864));
		isLoaded = !readLoadLayers;
		if (isLoadingLayers != readLoadLayers)
		{
			if (!readLoadLayers)
			{
				OpenNewLevel();
			}

			isLoadingLayers = readLoadLayers;

		}
	}
	static int getLevel()
	{
		return MemoryAccess::readData(LPVOID(0x762b5c));
	}

	static std::vector<uint32_t> setPacket()
	{
		if (!isLoaded)
			return std::vector<uint32_t>();

		uint32_t uintPosX = MemoryAccess::readData(LPVOID(0x7C4 + uint32_t(bilboPosXPTR)));
		uint32_t uintPosY = MemoryAccess::readData(LPVOID(0x7C8 + uint32_t(bilboPosXPTR)));
		uint32_t uintPosZ = MemoryAccess::readData(LPVOID(0x7CC + uint32_t(bilboPosXPTR)));
		uint32_t uintRotY = MemoryAccess::readData(LPVOID(0x7AC + uint32_t(bilboPosXPTR)));
		uint32_t animBilbo = MemoryAccess::readData(bilboAnimPTR);

		std::vector<uint32_t> packets;

		packets.push_back(uintPosX);
		packets.push_back(uintPosY);
		packets.push_back(uintPosZ);
		packets.push_back(uintRotY);
		packets.push_back(animBilbo);

		return packets;
	}
	
	static void readPacket(std::vector<uint32_t> packets, uint32_t playerIdx)
	{
		if (!isLoaded)
			return;
		if (packets.empty())
			return;

		std::cout << "\033[93m";
		std::cout << "Recieve Packet" << std::endl;
		std::cout << std::string(20, '~') << std::endl;

		// set x position
		int packInd = 0;
		playerCharacters[playerIdx].setPositionX(UInt32Wrapper(packets[packInd]));
		std::cout << "Xpos: " << std::setw(10) << float(UInt32Wrapper(packets[packInd])) << "| ";
		++packInd;

		// set y position
		playerCharacters[playerIdx].setPositionY(UInt32Wrapper(packets[packInd]));
		std::cout << "Ypos: " << std::setw(10) << float(UInt32Wrapper(packets[packInd])) << "| ";
		++packInd;

		// set z position
		playerCharacters[playerIdx].setPositionZ(UInt32Wrapper(packets[packInd]));
		std::cout << "Zpos: " << std::setw(10) << float(UInt32Wrapper(packets[packInd])) << "| ";
		++packInd;

		std::cout << std::endl;

		// set y rotation
		playerCharacters[playerIdx].setRotationY(UInt32Wrapper(packets[packInd]));
		std::cout << "Yrot: " << std::setw(10) << float(UInt32Wrapper(packets[packInd])) << "| ";
		++packInd;

		// set animation
		playerCharacters[playerIdx].setAnimation(UInt32Wrapper(packets[packInd]));
		std::cout << "Anim: " << std::setw(10) << int(UInt32Wrapper(packets[packInd])) << "| ";
		++packInd;

		std::cout << std::endl;
		std::cout << std::string(20, '~');
		std::cout << "\033[0m";
		std::cout << std::endl;
	}
private:
	static bool isLoaded;
	bool isUsed = false;
	int id = 0;
};

