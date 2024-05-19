#pragma once
#include "NPC.h"
#include <vector>
class PlayerCharacter : public NPC
{
public:
	// stores all NPCrgeom for players
	static std::vector<PlayerCharacter> playerCharacters;

	//constants 
	static const std::vector<float> fakePositions;
	static const std::vector<float> fakeRotations;
	static const std::vector<long long> fakeGuids;

	// Constructors
	//PlayerCharacter(UInt32Wrapper GUIDToFind) : NPC(UInt32Wrapper(GUIDToFind)) {}
	PlayerCharacter(LPVOID addresOfNPC) : NPC(addresOfNPC) {}

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
		LPVOID arrayStartAddress = MemoryAccess::readData(LPVOID(0x0076F648));
		for (int i = 0; i < fakePositions.size(); i++)
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

private:
	bool isUsed = false;
	int id = 0;
};


const std::vector<float> PlayerCharacter::fakePositions = { -2631.110107, -2831.110107, -2431.110107 };
const std::vector<float> PlayerCharacter::fakeRotations = { -2.796018124, -2.621485233, -2.446952105 };
const std::vector<long long> PlayerCharacter::fakeGuids = { 3887403015, 3887403009 , 3887403010 };
std::vector<PlayerCharacter> PlayerCharacter::playerCharacters;