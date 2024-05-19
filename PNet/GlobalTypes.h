#pragma once
#include "../PNet/MemoryAccess.h"
#include "../PNet/ByteFunctions.h"
union Type
{
	int8_t signed8;
	uint8_t unsigned8;
	int16_t signed16;
	uint16_t unsigned16;
	int32_t signed32;
	uint32_t unsigned32;
	int64_t signed64;
	uint64_t unsigned64;
	float real32;
	double real64;
};
enum Types
{
	t_signed8,
	t_unsigned8,
	t_signed16,
	t_unsigned16,
	t_signed32,
	t_unsigned32,
	t_signed64,
	t_unsigned64,
	t_real32,
	t_real64,
	t_string,
	typesCount
};
struct GenericType
{
	Type data = {};
	int type = 0;

	void* ptr() { return &data; }
	int getBytesSize()
	{
		if (type == t_signed8 ||
			type == t_unsigned8)
		{
			return 1;
		}
		if (type == t_signed16 ||
			type == t_unsigned16)
		{
			return 2;
		}
		if (type == t_signed32 ||
			type == t_unsigned32 ||
			type == t_real32)
		{
			return 4;
		}
		if (type == t_signed64 ||
			type == t_unsigned64 ||
			type == t_real64)
		{
			return 8;
		}

		return 0;
	}
};
