#pragma once
#include <vector>
#include <windows.h>
#include "UInt32Wrapper.h"
#include "../PNet/MemoryAccess.h"
#include "../PNet/ByteFunctions.h"

class NPC
{

public:
	// Data members
	static const LPVOID OBJECT_ARRAY_PTR; //equals to 0x0076F648

	// Constructors
	NPC(LPVOID addressOfNPC)
	{
		std::cout << "CreateNPC" << std::endl;
		std::cout << "GUID Address: " << addressOfNPC << std::endl;
		setObjAddress(addressOfNPC);
		setGUID(MemoryAccess::readData(LPVOID(0x8 + uint32_t(addressOfNPC))));

		updatePositionAddress();
		updateRotationAddress();
		updateAnimationAddress();

		std::cout << std::endl;
	}
	// Object Address
	LPVOID getObjAddress() { return objAddress; }
	void setObjAddress(LPVOID newObjAddress) { objAddress = newObjAddress; }

	// GUID
	void setGUID(UInt32Wrapper newGUID) { guid = newGUID; }

	// Position
	void setPositionX(UInt32Wrapper newPosition)
	{
		MemoryAccess::writeData(posxAddress, newPosition);
	}
	void setPositionY(UInt32Wrapper newPosition)
	{
		MemoryAccess::writeData(LPVOID(0x4 + uint32_t(posxAddress)), newPosition);
	}
	void setPositionZ(UInt32Wrapper newPosition)
	{
		MemoryAccess::writeData(LPVOID(0x8 + uint32_t(posxAddress)), newPosition);
	}


	void setPosition(UInt32Wrapper newPositionX, UInt32Wrapper newPositionY, UInt32Wrapper newPositionZ)
	{
		setPositionX(newPositionX);
		setPositionY(newPositionY);
		setPositionZ(newPositionZ);
	}

	// Rotation
	void setRotationY(UInt32Wrapper newRotation)
	{
		MemoryAccess::writeData(rotyAddress, newRotation);
	}

	// Animation
	void setAnimation(UInt32Wrapper newAnimation)
	{
		MemoryAccess::writeData(animAddress, newAnimation);
	}

private:
	//temporary
	LPVOID objAddress;

	LPVOID posxAddress;
	LPVOID rotyAddress;

	LPVOID animAddress;

	UInt32Wrapper guid;

	void updatePositionAddress()
	{
		// general position X
		LPVOID animAdd1 = getObjAddress();
		posxAddress = LPVOID(0xC + uint32_t(animAdd1));


		//dispplay the poistion Data
		std::cout << "Position Data:" << std::endl;
		std::cout << "posX: " << float(MemoryAccess::readData(posxAddress)) << std::endl;
		std::cout << "posXAddress: " << posxAddress << std::endl;
		
		std::cout << std::endl;
	}
	void updateRotationAddress()
	{
		
		// general position X
		LPVOID animAdd1 = getObjAddress();
		rotyAddress = LPVOID(0x64 + uint32_t(animAdd1));


		//dispplay the poistion Data
		std::cout << "Rotation Data:" << std::endl;
		std::cout << "rotY: " << float(MemoryAccess::readData(rotyAddress)) << std::endl;
		std::cout << "rotYAddress: " << rotyAddress << std::endl;
		std::cout << std::endl;
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

const LPVOID OBJECT_ARRAY_PTR = LPVOID(0x0076F648);
