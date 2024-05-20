#pragma once
#include <vector>
#include <windows.h>
#include "UInt32Wrapper.h"
#include "../PNet/MemoryAccess.h"
#include "../PNet/ByteFunctions.h"
#include "../PNet/GlobalTypes.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
class NPC
{

public:
	// Data members
	static const uint32_t OBJECT_ARRAY_PTR; //equals to 0x0076F648

	// Constructors
	NPC(uint32_t addressOfNPC)
	{
		std::cout << "CreateNPC" << std::endl;
		std::cout << "GUID Address: " << addressOfNPC << std::endl;
		setObjAddress(addressOfNPC);
		setGUID(MemoryAccess::readData(0x8 + addressOfNPC));

		updatePositionAddress();
		updateRotationAddress();
		updateAnimationAddress();

		std::cout << std::endl;
	}
	// Object Address
	uint32_t getObjAddress() { return objAddress; }
	void setObjAddress(uint32_t newObjAddress) { objAddress = newObjAddress; }

	// GUID
	void setGUID(uint32_t newGUID) { guid = newGUID; }

	// Position
	void setPositionX(uint32_t newPosition)
	{
		MemoryAccess::writeData(posxAddress, newPosition);
	}
	void setPositionY(uint32_t newPosition)
	{
		MemoryAccess::writeData(0x4 + posxAddress, newPosition);
	}
	void setPositionZ(uint32_t newPosition)
	{
		MemoryAccess::writeData(0x8 + posxAddress, newPosition);
	}


	void setPosition(uint32_t newPositionX, uint32_t newPositionY, uint32_t newPositionZ)
	{
		setPositionX(newPositionX);
		setPositionY(newPositionY);
		setPositionZ(newPositionZ);
	}

	// Rotation
	void setRotationY(uint32_t newRotation)
	{
		MemoryAccess::writeData(rotyAddress, newRotation);
	}

	// Animation
	void setAnimation(uint32_t newAnimation)
	{
		MemoryAccess::writeData(animAddress, newAnimation);
	}

private:
	//temporary
	GenericType data1;


	uint32_t objAddress;

	uint32_t posxAddress;
	uint32_t rotyAddress;

	uint32_t animAddress;

	uint32_t guid;

	void updatePositionAddress()
	{
		// general position X
		uint32_t animAdd1 = getObjAddress();
		posxAddress = 0xC + animAdd1;


		//dispplay the poistion Data
		std::cout << "Position Data:" << std::endl;
		std::cout << "posX: " << float(MemoryAccess::readData(posxAddress)) << std::endl;
		std::cout << "posXAddress: " << posxAddress << std::endl;
		
		std::cout << std::endl;
	}
	void updateRotationAddress()
	{
		
		// general position X
		uint32_t animAdd1 = getObjAddress();
		rotyAddress = 0x64 + animAdd1;


		//dispplay the poistion Data
		std::cout << "Rotation Data:" << std::endl;
		std::cout << "rotY: " << float(MemoryAccess::readData(rotyAddress)) << std::endl;
		std::cout << "rotYAddress: " << rotyAddress << std::endl;
		std::cout << std::endl;
	}
	void updateAnimationAddress()
	{
		// animation
		uint32_t animAdd1 = getObjAddress();
		uint32_t animAdd2 = MemoryAccess::readData(0x304 - 0x8 + animAdd1);
		uint32_t animAdd3 = MemoryAccess::readData(0x50 + animAdd2);
		uint32_t animAdd4 = MemoryAccess::readData(0x10C + animAdd3);
		animAddress = 0x8 + animAdd4;
		std::cout << "anim: " << MemoryAccess::readData(animAddress) << std::endl;
		std::cout << "animAddress: " << animAddress << std::endl;
	}
};

const uint32_t OBJECT_ARRAY_PTR = 0x0076F648;
