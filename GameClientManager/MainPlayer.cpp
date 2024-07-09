#include "MainPlayer.h"
uint32_t MainPlayer::bilboPosXPTR;
uint32_t MainPlayer::bilboAnimPTR;
const uint32_t MainPlayer::X_POSITION_PTR = 0x0075BA3C;
std::atomic<bool> MainPlayer::processPackets = false;