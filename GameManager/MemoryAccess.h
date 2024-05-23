#pragma once
#include <vector>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>

#include <cstring> // for std::memcpy
#include <type_traits> // for std::enable_if and std::is_arithmetic


using PROCESS = HANDLE;

enum
{
	memQueryFlags_ = 0,
	memQueryFlags_None = 0,
	memQueryFlags_Read = 0b0001,
	memQueryFlags_Write = 0b0010,
	memQueryFlags_Execute = 0b0100,

};
struct OppenedQuery
{
	PROCESS queriedProcess = 0;
	char* baseQueriedPtr = (char*)0x0;
	bool oppened()
	{
		return queriedProcess != 0;
	}
};

static std::string executableName = "";

class MemoryAccess
{
public:

	static std::string getExecutableName();
	static void setExecutableName(const std::string& newName);

	static HANDLE readProcess(const char* processName);
	static HANDLE readProcess();
	static DWORD readProcessID(const char* name);
	static DWORD readProcessID();

	static uint32_t readData(LPVOID Address)
	{
		uint32_t data = 0;
		HANDLE Process = readProcess();
		if (!ReadProcessMemory(Process, Address, &data, sizeof(data), NULL)) { // Reading the data from memory

			data = 0;
		}
		CloseHandle(Process);
		return uint32_t(data);
	}
	static uint32_t readData(uint32_t Address)
	{
		return readData(LPVOID(Address));
	}


	static uint32_t findObjectAddressByGUID(uint32_t beginStackAddress, uint32_t guid) {
		
		const size_t stackSize = 0xEFEC;
		const size_t jumpSize = 0x14;
		HANDLE Process = readProcess();
		if (!Process) {
			return 0;
		}

		// Loop through the stack memory
		for (size_t offset = stackSize; offset > 0; offset -= jumpSize) {
			uint32_t objectAddress = 0;
			uint32_t objectGUID = 0;
			LPVOID objectPtrPtr = LPVOID(beginStackAddress + offset);

			//read the pointer of an object
			if (ReadProcessMemory(Process, objectPtrPtr, &objectAddress, sizeof(objectAddress), NULL)) {
				LPVOID guidAddress = LPVOID(objectAddress + 0x8);
				//read the guid
				if (ReadProcessMemory(Process, guidAddress, &objectGUID, sizeof(objectGUID), NULL) && objectGUID == guid) {
					CloseHandle(Process);
					return objectAddress;
				}
			}

		}

		CloseHandle(Process);
		return 0; // Return nullptr if dataToFind is not found in the stack range
	}

	// templates
	template<typename T>
	static T writeData(LPVOID Address, T data)
	{
		HANDLE Process = readProcess();
		T temporary = data;
		if (!ReadProcessMemory(Process, Address, &temporary, sizeof(temporary), NULL)) { // Reading the value from memory
			CloseHandle(Process);
			return T(); // Return default value of type T if reading fails
		}
		DWORD oldProtect;

		SIZE_T dwSize = sizeof(data);
		VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
		BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &data, dwSize, NULL);
		VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
		if (bWriteSuccess) return T(); // Return default value of type T if writing fails
		return 0;
	}
	static void writeData(uint32_t Address, uint32_t data)
	{
		writeData(LPVOID(Address), data);
	}

	// converts uint32_t to float
	static float uint32ToFloat(uint32_t value) {
		float result;
		std::memcpy(&result, &value, sizeof(result));
		return result;
	}
	// converts float to uint32_t
	static uint32_t floatToUint32(float value) {
		uint32_t result;
		std::memcpy(&result, &value, sizeof(result));
		return result;
	}


	// additional funcitons Find by Pattern
	static std::vector<void*> findBytePatternInProcessMemory(void* pattern, size_t patternLen);
	static std::vector<uint32_t> findBytePatternInProcessMemory(const std::vector<uint32_t>& pattern);
	static std::vector<uint32_t> findBytePatternInProcessMemory(uint32_t pattern) 
	{
		std::vector<uint32_t> tempPattern;
		tempPattern.push_back(pattern);
		return findBytePatternInProcessMemory(tempPattern);
	};
	static bool getNextQuery(OppenedQuery& query, void*& low, void*& hi, int& flags);
	static OppenedQuery initVirtualQuery(PROCESS process);
private:
};
