#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>
#include <unordered_map>


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
	HANDLE queriedProcess = 0;
	char* baseQueriedPtr = (char*)0x0;
	bool oppened()
	{
		return queriedProcess != 0;
	}
};


void floatToBytes(float floatValue, uint32_t& uintValue) {
	std::memcpy(&uintValue, &floatValue, sizeof(uintValue));
}

float uint32ToFloat(uint32_t uintValue) {
	float floatValue;
	std::memcpy(&floatValue, &uintValue, sizeof(floatValue));
	return floatValue;
}

HANDLE read_process_hobbit()
{
	DWORD pid = 0;     //переменна€ айди процесса
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(snapshot, &pe32)) {
		while (Process32Next(snapshot, &pe32)) { //ищет айди процесса
			if (strcmp(pe32.szExeFile, "Meridian.exe") == 0) {
				pid = pe32.th32ProcessID; //переменна€ айди процесса
				break;
			}
		}
	}
	CloseHandle(snapshot);
	DWORD Prava = PROCESS_ALL_ACCESS; //это права доступа
	HANDLE Process = OpenProcess(Prava, FALSE, pid); //числовое значение - это айди процесса в диспетчере задач
	return Process;
}


int change_float_hobbit(LPVOID Address, float a)
{
	HANDLE Process;
	Process = read_process_hobbit();
	float value;  //переменна€ значени€ байта по адресу
	if (!ReadProcessMemory(Process, Address, &value, sizeof(value), NULL)) { //„тение значени€ байта
		CloseHandle(Process);
		return 1;
	}
	DWORD oldProtect;
	float Znachenie;
	Znachenie = a;
	SIZE_T dwSize = sizeof(Znachenie);
	VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &Znachenie, dwSize, NULL);
	VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
	if (bWriteSuccess) return 0;
}


int change_1Byte_hobbit_no_switch(LPVOID Address, BYTE Znachenie)
{
	HANDLE Process;
	Process = read_process_hobbit();
	BYTE value;  //переменна€ значени€ байта по адресу
	if (!ReadProcessMemory(Process, Address, &value, sizeof(value), NULL)) { //„тение значени€ байта
		CloseHandle(Process);
		return 1;
	}
	DWORD oldProtect;

	SIZE_T dwSize = sizeof(Znachenie);
	VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &Znachenie, dwSize, NULL);
	VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
	if (bWriteSuccess) return 0;

}

DWORD findPidByName(const char* name)
{
	HANDLE h;
	PROCESSENTRY32 singleProcess;
	h = CreateToolhelp32Snapshot( //takes a snapshot of specified processes
		TH32CS_SNAPPROCESS, //get all processes
		0); //ignored for SNAPPROCESS

	singleProcess.dwSize = sizeof(PROCESSENTRY32);

	do
	{
		if (strcmp(singleProcess.szExeFile, name) == 0)
		{
			DWORD pid = singleProcess.th32ProcessID;
			CloseHandle(h);
			return pid;
		}

	} while (Process32Next(h, &singleProcess));

	CloseHandle(h);

	return 0;
}


bool getNextQuery(OppenedQuery& query, void*& low, void*& hi, int& flags)
{

	if (query.queriedProcess == 0) { return false; }

	flags = memQueryFlags_None;
	low = nullptr;
	hi = nullptr;

	MEMORY_BASIC_INFORMATION memInfo;

	bool rez = 0;
	while (true)
	{
		rez = VirtualQueryEx(query.queriedProcess, (void*)query.baseQueriedPtr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		if (!rez)
		{
			query = {};
			return false;
		}

		query.baseQueriedPtr = (char*)memInfo.BaseAddress + memInfo.RegionSize;

		if (memInfo.State == MEM_COMMIT)
		{
			if (memInfo.Protect & PAGE_READONLY)
			{
				flags |= memQueryFlags_Read;
			}

			if (memInfo.Protect & PAGE_READWRITE)
			{
				flags |= (memQueryFlags_Read | memQueryFlags_Write);
			}

			if (memInfo.Protect & PAGE_EXECUTE)
			{
				flags |= memQueryFlags_Execute;
			}

			if (memInfo.Protect & PAGE_EXECUTE_READ)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read);
			}

			if (memInfo.Protect & PAGE_EXECUTE_READWRITE)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read | memQueryFlags_Write);
			}

			if (memInfo.Protect & PAGE_EXECUTE_WRITECOPY)
			{
				flags |= (memQueryFlags_Execute | memQueryFlags_Read);
			}

			if (memInfo.Protect & PAGE_WRITECOPY)
			{
				flags |= memQueryFlags_Read;
			}

			low = memInfo.BaseAddress;
			hi = (char*)memInfo.BaseAddress + memInfo.RegionSize;
			return true;
		}

	}
}

OppenedQuery initVirtualQuery(HANDLE process)
{
	OppenedQuery q = {};

	q.queriedProcess = process;
	q.baseQueriedPtr = 0;
	return q;
}

bool readMemory(HANDLE process, void* start, size_t size, void* buff)
{
	SIZE_T readSize = 0;
	return ReadProcessMemory(process, start, buff, size, &readSize);
}

std::vector<void*> findBytePatternInProcessMemory(HANDLE process, void* pattern, size_t patternLen)
{
	if (patternLen == 0) { return {}; }

	std::vector<void*> returnVec;
	returnVec.reserve(1000);

	auto query = initVirtualQuery(process);

	if (!query.oppened())
		return {};

	void* low = nullptr;
	void* hi = nullptr;
	int flags = memQueryFlags_None;

	while (getNextQuery(query, low, hi, flags))
	{
		if ((flags | memQueryFlags_Read) && (flags | memQueryFlags_Write))
		{
			//search for our byte patern
			size_t size = (char*)hi - (char*)low;
			char* localCopyContents = new char[size];
			if (
				readMemory(process, low, size, localCopyContents)
				)
			{
				char* cur = localCopyContents;
				size_t curPos = 0;
				while (curPos < size - patternLen + 1)
				{
					if (memcmp(cur, pattern, patternLen) == 0)
					{
						returnVec.push_back((char*)low + curPos);
					}
					curPos++;
					cur++;
				}
			}
			delete[] localCopyContents;
		}
	}

	return returnVec;
}

int read_int_value(LPVOID Address)
{
	HANDLE Process;
	Process = read_process_hobbit();
	int value;  //переменна€ значени€ байта по адресу
	if (!ReadProcessMemory(Process, Address, &value, sizeof(value), NULL)) { //„тение значени€ байта
		CloseHandle(Process);
		return 1;
	}
	return value;
}

LPDWORD ukazatel_hobbit(LPVOID Address) {
	HANDLE Process;
	Process = read_process_hobbit();
	LPDWORD value, ukazatel = 0x00;  //переменна€ значени€ байта по адресу
	if (!ReadProcessMemory(Process, Address, &value, sizeof(value), NULL)) { //„тение значени€ байта
		CloseHandle(Process);
		return ukazatel;
	}

	ukazatel = value;
	return ukazatel;
}

float read_float_value(LPVOID Address)
{
	HANDLE Process;
	Process = read_process_hobbit();
	float value;  //переменна€ значени€ байта по адресу
	if (!ReadProcessMemory(Process, Address, &value, sizeof(value), NULL)) { //„тение значени€ байта
		CloseHandle(Process);
		return 1.0;
	}
	return value;
}
