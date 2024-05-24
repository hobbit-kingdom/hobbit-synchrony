#pragma once
#include <cstdint>

#define OLC_PGEX_NETWORK
#include "olcPGEX_Network.h"

enum class PacketType : uint32_t
{
	Server_GetStatus,
	Server_GetPing,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	Game_AddClient,
	Game_RemoveClient,
	Game_UpdateClient,
};