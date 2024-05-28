#include "OtherPlayer.h"
std::vector<uint32_t> OtherPlayer::GUIDs{};
std::vector<NPC> OtherPlayer::otherPlayers;
std::atomic<bool> OtherPlayer::processPackets = false;
