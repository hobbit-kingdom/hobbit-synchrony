#include "PlayerCharacter.h"

std::vector<PlayerCharacter> PlayerCharacter::playerCharacters;
const std::vector<uint32_t> PlayerCharacter::fakeGuids = { 3887403015, 3887403009, 3887403010 };
const LPVOID PlayerCharacter::X_POSITION_PTR = LPVOID(0x0075BA3C);
LPVOID PlayerCharacter::bilboPosXPTR = 0;
LPVOID PlayerCharacter::bilboAnimPTR = 0;
bool PlayerCharacter::isLoaded = false;
const LPVOID test =  0x0;