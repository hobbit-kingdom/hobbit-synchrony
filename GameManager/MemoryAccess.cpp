#include "MemoryAccess.h"

HANDLE MemoryAccess::process;
std::mutex MemoryAccess::guardWriteData;
std::mutex MemoryAccess::guardProcess;
//@return name of attached executable
std::string MemoryAccess::getExecutableName() {
	return executableName;
}

// set executable name
void MemoryAccess::setExecutableName(const std::string& newName) {
	executableName = newName;
}

//@return current process
HANDLE MemoryAccess::readProcess(const char* processName)
{
	DWORD pid = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return nullptr; // Failed to create snapshot
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &pe32)) {
		do {
			// Convert processName to wide character string
			WCHAR wProcessName[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, processName, -1, wProcessName, MAX_PATH);

			if (wcscmp(pe32.szExeFile, wProcessName) == 0) { // Use wcscmp for wide character strings
				pid = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &pe32)); // Continue until process is found or list ends
	}

	CloseHandle(snapshot);

	if (pid == 0) {
		return nullptr; // Process not found
	}

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	return processHandle; // This may return NULL if OpenProcess fails
}
//@return current process
HANDLE MemoryAccess::readProcess()
{
	if (executableName != "")
		return MemoryAccess::readProcess(executableName.c_str());
	return 0;
}

//@return processID by name of the process
DWORD MemoryAccess::readProcessID(const char* name)
{
	HANDLE h;
	PROCESSENTRY32 singleProcess;
	h = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,
		0);

	singleProcess.dwSize = sizeof(PROCESSENTRY32);

	DWORD pid = 0;
	do
	{
		// Convert WCHAR* to const char*
		char processName[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, singleProcess.szExeFile, -1, processName, MAX_PATH, NULL, NULL);

		if (strcmp(processName, name) == 0)
		{
			pid = singleProcess.th32ProcessID;
			break;
		}

	} while (Process32Next(h, &singleProcess));

	CloseHandle(h);

	return pid;
}
//@return processID of attached process
DWORD MemoryAccess::readProcessID()
{
	if (executableName != "")
		return MemoryAccess::readProcessID(executableName.c_str());
	return 0;
}

//FindBytePatternInMemory

OppenedQuery initVirtualQuery(HANDLE process)
{
	OppenedQuery q = {};

	q.queriedProcess = process;
	q.baseQueriedPtr = 0;
	return q;
}

std::vector<void*> MemoryAccess::findBytePatternInProcessMemory(void* pattern, size_t patternLen)
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

			SIZE_T readSize = 0;
			if(ReadProcessMemory(process, low, localCopyContents, size, &readSize))
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
std::vector<uint32_t> MemoryAccess::findBytePatternInProcessMemory(const std::vector<uint32_t>& pattern)
{
	// If the pattern is empty, return an empty vector
	if (pattern.empty()) { return {}; }

	// Convert the vector of uint32_t to a byte array
	size_t patternLen = pattern.size() * sizeof(uint32_t);
	std::vector<uint8_t> bytePattern(patternLen);
	memcpy(bytePattern.data(), pattern.data(), patternLen);

	// Call the original function
	std::vector<void*> addresses = findBytePatternInProcessMemory(bytePattern.data(), patternLen);

	// Convert the vector of void* to a vector of uint32_t
	std::vector<uint32_t> result;
	result.reserve(addresses.size());
	for (void* addr : addresses)
	{
		result.push_back(reinterpret_cast<uint32_t>(addr));
	}

	return result;
}

bool MemoryAccess::getNextQuery(OppenedQuery& query, void*& low, void*& hi, int& flags)
{
	if (query.queriedProcess == 0) { return false; }

	low = nullptr;
	hi = nullptr;
	flags = memQueryFlags_None;

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
	return false;
}
OppenedQuery MemoryAccess::initVirtualQuery(PROCESS process)
{
	OppenedQuery q = {};

	q.queriedProcess = process;
	q.baseQueriedPtr = 0;
	return q;
}