#pragma once
#include <vector>
#include <windows.h>
#include "MemoryAccess.h"
#include "../PNet/ByteFunctions.h"


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

		setPositionAddress();
		setRotationAddress();
		setAnimationAddress();

		std::cout << std::endl;
	}
	// Object Address
	uint32_t getObjAddress() { return objAddress; }
	void setObjAddress(uint32_t newObjAddress) 
	{
		objAddress = newObjAddress; 
		std::cout << std::hex;
		std::cout << "newObjAddress: " << newObjAddress;
		std::cout << std::endl << std::dec;
	}

	// GUID
	void setGUID(uint32_t newGUID) 
	{ 
		guid = newGUID; 
	}

	// Position
	void setPositionX(uint32_t newPosition)
	{
		for (uint32_t posXadd : posxAddress)
		{
			MemoryAccess::writeData(posXadd, newPosition);
		}
	}
	void setPositionY(uint32_t newPosition)
	{
		for (uint32_t posXadd : posxAddress)
		{
			MemoryAccess::writeData(0x4 + posXadd, newPosition);
		}
	}
	void setPositionZ(uint32_t newPosition)
	{
		for (uint32_t posXadd : posxAddress)
		{
			MemoryAccess::writeData(0x8 + posXadd, newPosition);
		}
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
	uint32_t objAddress;
	std::vector<uint32_t> posxAddress;
	uint32_t rotyAddress;

	uint32_t animAddress;

	uint32_t guid;

	void setPositionAddress()
	{
		posxAddress.clear();

		uint32_t GUIDaddrs = getObjAddress();

		posxAddress.push_back(0xC + GUIDaddrs);
		//posxAddress.push_back(0xC + 0x4*3 + GUIDaddrs);
		posxAddress.push_back(0x18 + GUIDaddrs);

		uint32_t animAdd1 = getObjAddress();
		uint32_t animAdd2 = MemoryAccess::readData(0x304 - 0x8 + animAdd1);
		uint32_t animAdd3 = MemoryAccess::readData(0x50 + animAdd2);
		uint32_t animAdd4 = MemoryAccess::readData(0x10C + animAdd3);
		animAddress = 0x8 + animAdd4;
		posxAddress.push_back(-0xC4 + animAddress);
		//posxAddress.push_back(-0xC4 + 0x9*0x4 + animAddress);

		
		std::cout << std::hex;
		for (uint32_t posxAdd : posxAddress)
		{
			//dispplay the poistion Data
			std::cout << "Position Data:" << std::endl;
			std::cout << "posX: " << MemoryAccess::uint32ToFloat(MemoryAccess::readData(posxAdd)) << std::endl;
			std::cout << "posXAddress: " << posxAdd << std::endl;
		}
		std::cout << std::dec;
		std::cout << std::endl;
	}
	void setRotationAddress()
	{

		// general position X
		uint32_t animAdd1 = getObjAddress();
		rotyAddress = uint32_t(0x64 + uint32_t(animAdd1));

		//dispplay the poistion Data
		std::cout << std::hex;
		std::cout << "Rotation Data:" << std::endl;
		std::cout << "rotY: " << MemoryAccess::uint32ToFloat(MemoryAccess::readData(rotyAddress)) << std::endl;
		std::cout << "rotYAddress: " << rotyAddress << std::endl;
		std::cout << std::endl;
		std::cout << std::dec;

	}
	void setAnimationAddress()
	{

		// animation
		uint32_t animAdd1 = getObjAddress();
		uint32_t animAdd2 = MemoryAccess::readData(0x304 - 0x8 + animAdd1);
		uint32_t animAdd3 = MemoryAccess::readData(0x50 + animAdd2);
		uint32_t animAdd4 = MemoryAccess::readData(0x10C + animAdd3);
		animAddress = 0x8 + animAdd4;
		std::cout << std::hex;
		std::cout << "anim: " << MemoryAccess::readData(animAddress) << std::endl;
		std::cout << "animAddress: " << animAddress << std::endl;
		std::cout << std::endl;

	}
};

const uint32_t OBJECT_ARRAY_PTR = uint32_t(0x0076F648);
