#pragma once
#include "NPC.h"
#include <vector>
class PlayerCharacter : public NPC
{
public:
	// stores all NPCrgeom for players
	static std::vector<PlayerCharacter> playerCharacters;
	static const size_t MAX_PLAYERS = 3;	// number of players on one server

	//constants 

	static const std::vector<uint32_t> fakeGuids;

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
	}
	static void checkUpdateLevel()
	{
		static uint32_t isLoadingLayers;
		static uint32_t readLoadLayers;
		readLoadLayers = MemoryAccess::readData(LPVOID(0x00760864));
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

private:
	
	bool isUsed = false;
	int id = 0;
};


const std::vector<uint32_t> PlayerCharacter::fakeGuids = {3887403015, 3887403009, 3887403010};
std::vector<PlayerCharacter> PlayerCharacter::playerCharacters;

