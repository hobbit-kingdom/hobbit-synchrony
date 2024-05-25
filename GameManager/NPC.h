#pragma once
#include <vector>
#include <windows.h>
#include "MemoryAccess.h"

class NPC
{
private:
	// Address of Stack of Objects
	static uint32_t OBJECT_STACK_ADDRESS; //equals to 0x0076F648
	// Pointers
	uint32_t objectAddress;					// Object pointer
	std::vector<uint32_t> positionXAddress;	// Position X pointer
	uint32_t rotationYAddress;				// Rotation Y pointer
	uint32_t animationAddress;				// Animation pointer

	// GUID of object
	uint32_t guid;

	// Sets objects pointer of the NPC
	void setObjectPtrByGUID(uint32_t guid)
	{
		objectAddress = MemoryAccess::findObjectAddressByGUID(OBJECT_STACK_ADDRESS, guid);

		// Display new ObjAddress
		std::cout << std::hex;
		std::cout << "~ObjectPtr: " << objectAddress;
		std::cout << std::dec << std::endl;
	}
	// Sets position X pointers of the NPC 
	void setPositionXPtr()
	{
		// remove all stored pointers
		positionXAddress.clear();

		// store object pointer
		uint32_t ObjectPtr = getObjectPtr();


		// set current position pointer
		positionXAddress.push_back(0xC + 0x8 + ObjectPtr);

		// set root position X pointer
		positionXAddress.push_back(0x18 + 0x8 + ObjectPtr);

		// set the animation position X pointer
		uint32_t animAdd1 = getObjectPtr();
		uint32_t animAdd2 = MemoryAccess::readData(0x304 + animAdd1);
		uint32_t animAdd3 = MemoryAccess::readData(0x50 + animAdd2);
		uint32_t animAdd4 = MemoryAccess::readData(0x10C + animAdd3);
		animationAddress = 0x8 + animAdd4;
		positionXAddress.push_back(-0xC4 + animationAddress);

		// Display the position X pointers Data
		std::cout << std::hex;
		for (uint32_t posxAdd : positionXAddress)
		{
			//dispplay the poistion Data
			std::cout << "~Position Data:" << std::endl;
			std::cout << "~posX: " << MemoryAccess::uint32ToFloat(MemoryAccess::readData(posxAdd)) << std::endl;
			std::cout << "~posXAddress: " << posxAdd << std::endl;
		}
		std::cout << std::dec;
		std::cout << std::endl;
	}
	// Sets rotation Y pointer of the NPC 
	void setRotationYPtr()
	{
		// store object pointer
		uint32_t ObjectPtr = getObjectPtr();

		// set rotation Y pointer
		rotationYAddress = 0x64 + 0x8 + ObjectPtr;

		// Display the rotation Y pointer Data
		std::cout << std::hex;
		std::cout << "~Rotation Data:" << std::endl;
		std::cout << "~rotY: " << MemoryAccess::uint32ToFloat(MemoryAccess::readData(rotationYAddress)) << std::endl;
		std::cout << "~rotYAddress: " << rotationYAddress << std::endl;
		std::cout << std::endl;
		std::cout << std::dec;
	}
	// Sets animation pointer of the NPC
	void setAnimationPtr()
	{
		// store object pointer
		uint32_t ObjectPtr = getObjectPtr();

		// set animation pointer
		uint32_t animAdd1 = MemoryAccess::readData(0x304 + ObjectPtr);
		uint32_t animAdd2 = MemoryAccess::readData(0x50 + animAdd1);
		uint32_t animAdd3 = MemoryAccess::readData(0x10C + animAdd2);
		animationAddress = 0x8 + animAdd3;

		// Display the animation pointer Data
		std::cout << std::hex;
		std::cout << "anim: " << MemoryAccess::readData(animationAddress) << std::endl;
		std::cout << "animAddress: " << animationAddress << std::endl;
		std::cout << std::endl;
	}
	

public:
	// Constructors
	NPC(uint32_t GUID) :guid(GUID)
	{
		// set the Object Array Address
		OBJECT_STACK_ADDRESS = MemoryAccess::readData(0x0076F648);

		// Constructor message
		std::cout << "~CreateNPC" << std::endl;

		// read the pointers of instances
		setObjectPtrByGUID(GUID);
		setPositionXPtr();
		setRotationYPtr();
		setAnimationPtr();

		// end Constructor message
		std::cout << std::endl;
	}

	// Returns object pointer
	uint32_t getObjectPtr() { 
		return objectAddress; 
	}

	// writes new GUID 
	// modifies game file
	void setGUID(uint32_t newGUID) 
	{ 
		MemoryAccess::writeData(objectAddress, newGUID);
	}

	// writes new positionX 
	// modifies game file
	void setPositionX(uint32_t newPosition)
	{
		for (uint32_t posXadd : positionXAddress)
		{
			MemoryAccess::writeData(posXadd, newPosition);
		}
	}
	// writes new positionY
	// modifies game file
	void setPositionY(uint32_t newPosition)
	{
		for (uint32_t posXadd : positionXAddress)
		{
			MemoryAccess::writeData(0x4 + posXadd, newPosition);
		}
	}
	// writes new positionZ 
	// modifies game file
	void setPositionZ(uint32_t newPosition)
	{
		for (uint32_t posXadd : positionXAddress)
		{
			MemoryAccess::writeData(0x8 + posXadd, newPosition);
		}
	}
	// writes new Position 
	// modifies game file
	void setPosition(uint32_t newPositionX, uint32_t newPositionY, uint32_t newPositionZ)
	{
		setPositionX(newPositionX);
		setPositionY(newPositionY);
		setPositionZ(newPositionZ);
	}

	// writes new GUID 
	// modifies game filen
	void setRotationY(uint32_t newRotation)
	{
		MemoryAccess::writeData(rotationYAddress, newRotation);
	}
	// writes new GUID 
	// modifies game file
	void setAnimation(uint32_t newAnimation)
	{
		MemoryAccess::writeData(animationAddress, newAnimation);
	}
};

