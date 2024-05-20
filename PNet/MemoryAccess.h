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
			return uint32_t(readData(LPVOID(Address)));
		}
		static LPVOID findDataInStackHobbit(LPVOID beginStackAddress, size_t stackSize, uint32_t jumpSize, uint32_t dataToFind) {
			HANDLE Process = readProcess();
			if (!Process) {
				return nullptr;
			}

			// Loop through the stack memory
			for (size_t offset = stackSize; offset > 0; offset -= jumpSize) {
				uint32_t currentData = 0;
				LPVOID currentAddress = LPVOID(uint32_t(beginStackAddress) + offset);

				if (ReadProcessMemory(Process, currentAddress, &currentData, sizeof(currentData), NULL)) {
					LPVOID guidAddress = LPVOID(uint32_t(currentData) + 0x8);
					if (ReadProcessMemory(Process, guidAddress, &currentData, sizeof(currentData), NULL) && currentData == dataToFind) {
						CloseHandle(Process);
						return guidAddress;
					}
				}
				
			}

			CloseHandle(Process);
			return nullptr; // Return nullptr if dataToFind is not found in the stack range
		}

		// templates
		template<typename T>
		static T writeDataSwitcher(LPVOID Address, T newData, T initialData)
		{
			T data;  // Variable to store the data read from memory
			HANDLE Process = readProcess();
			if (!ReadProcessMemory(Process, Address, &data, sizeof(data), NULL)) { // Reading the data from memory
				CloseHandle(Process);
				return 1;
			}

			DWORD oldProtect;
			if (data == initialData) {
				SIZE_T dwSize = sizeof(newData);
				VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &newData, dwSize, NULL);
				VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
				CloseHandle(Process);
				if (bWriteSuccess) return 0;
			}
			else {
				SIZE_T dwSize = sizeof(initialData);
				VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &initialData, dwSize, NULL);
				VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
				CloseHandle(Process);
				if (bWriteSuccess) return 0;
			}
		}
		template<typename T>
		static uint32_t writeDataSwitcher(uint32_t Address, uint32_t newData, uint32_t initialData)
		{
			writeDataSwitcher(LPVOID(Address), newData, initialData);
		}
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
		}
		static void writeData(uint32_t Address, uint32_t data)
		{
			writeData(LPVOID(Address), data);
		}

		// additional funcitons
		static std::vector<void*> findBytePatternInProcessMemory(void* pattern, size_t patternLen);
		
		static bool getNextQuery(OppenedQuery& query, void*& low, void*& hi, int& flags);
		static OppenedQuery initVirtualQuery(PROCESS process);
		
		private:
	};
