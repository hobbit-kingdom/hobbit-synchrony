#pragma once
#include <vector>
#include <windows.h>
#include "UInt32Wrapper.h"
#include "../PNet/MemoryAccess.h"
#include "../PNet/ByteFunctions.h"
#include "../PNet/GlobalTypes.h"

class NPC
{

public:
	// Data members
	static const UInt32Wrapper OBJECT_ARRAY_PTR; //equals to 0x0076F648

	// Constructors
	NPC(LPVOID addressOfNPC)
	{
		setObjAddress(addressOfNPC);
		setGUID(MemoryAccess::readData(LPVOID(0x8 + uint32_t(addressOfNPC))));

		updatePositionAddress();
		updateRotationAddress();
		updateAnimationAddress();
	}
	// Object Address
	LPVOID getObjAddress() { return objAddress; }
	void setObjAddress(LPVOID newObjAddress) { objAddress = newObjAddress; }

	// GUID
	void setGUID(UInt32Wrapper newGUID) { guid = newGUID; }

	// Position
	void setPositionX(UInt32Wrapper newPosition)
	{
		for (int i = 0; i < posxAddress.size(); i++)
		{
			MemoryAccess::writeData(posxAddress[i], newPosition);
		}
	}
	void setPositionY(UInt32Wrapper newPosition)
	{
		for (int i = 0; i < posxAddress.size(); i++)
		{
			MemoryAccess::writeData(LPVOID(0x4 + uint32_t(posxAddress[i])), newPosition);
		}
	}
	void setPositionZ(UInt32Wrapper newPosition)
	{
		for (int i = 0; i < posxAddress.size(); i++)
		{
			MemoryAccess::writeData(LPVOID(0x8 + uint32_t(posxAddress[i])), newPosition);
		}
	}
	

	//std::vector<UInt32Wrapper> getPoistion();
	void setPosition(UInt32Wrapper newPositionX, UInt32Wrapper newPositionY, UInt32Wrapper newPositionZ)
	{
		setPositionX(newPositionX);
		setPositionY(newPositionY);
		setPositionZ(newPositionZ);
	}

	// Rotation
	void setRotationY(UInt32Wrapper newRotation)
	{
		for (int i = 0; i < rotyAddress.size(); i++)
		{
			MemoryAccess::writeData(rotyAddress[i], newRotation);
		}
	}

	// Animation
	void setAnimation(UInt32Wrapper newAnimation)
	{
		MemoryAccess::writeData(animAddress, newAnimation);
	}

private:
	//temporary
	GenericType data1;

	
	LPVOID objAddress;

	std::vector<LPVOID> posxAddress;
	std::vector<LPVOID> rotyAddress;

	LPVOID animAddress;

	UInt32Wrapper guid;

	void updatePositionAddress()
	{
		float fakePos = 0;
		LPVOID lp = LPVOID((0xC + uint32_t(getObjAddress())));
		fakePos = MemoryAccess::readData(lp);
		//positions
		data1 = {};
		data1.type = 4;
		data1.data.real32 = fakePos; //fakePositions[i];
		posxAddress = findBytePatternInProcessMemory(read_process_hobbit(), data1.ptr(), data1.getBytesSize());
		for (int i = 0; i < posxAddress.size(); i++)
		{
			std::cout << "pX: " << float(MemoryAccess::readData(posxAddress[i])) << std::endl;
		}
	}
	void updateRotationAddress()
	{
		float fakeRot = 0;
		LPVOID lp2 = LPVOID((0x64 + uint32_t(getObjAddress())));
		fakeRot = MemoryAccess::readData(lp2);
		//rotation
		data1 = {};
		data1.type = 4;
		data1.data.real32 = fakeRot; //fakeRotations[i];
		rotyAddress = findBytePatternInProcessMemory(read_process_hobbit(), data1.ptr(), data1.getBytesSize());
		std::cout << (rotyAddress.size()) << std::endl;

		for (int i = 0; i < rotyAddress.size(); i++)
		{
			std::cout << "rY: " << float(MemoryAccess::readData(rotyAddress[i])) << std::endl;
		}
	}
	void updateAnimationAddress()
	{
		// animation
		LPVOID animAdd1 = getObjAddress();
		LPVOID animAdd2 = MemoryAccess::readData(LPVOID(0x304 - 0x8 + uint32_t(animAdd1)));
		LPVOID animAdd3 = MemoryAccess::readData(LPVOID(0x50 + uint32_t(animAdd2)));
		LPVOID animAdd4 = MemoryAccess::readData(LPVOID(0x10C + uint32_t(animAdd3)));
		animAddress = LPVOID(0x8 + uint32_t(animAdd4));
		std::cout << "anim: " << MemoryAccess::readData(animAddress) << std::endl;
		std::cout << "animAddress: " << animAddress << std::endl;
	}
};

const UInt32Wrapper OBJECT_ARRAY_PTR = UInt32Wrapper(0x0076F648);
