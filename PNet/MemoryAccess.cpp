#include "MemoryAccess.h"


namespace memoryAccess {

	std::string MemoryAccess::getExecutableName() {
		return executableName;
	}
	void MemoryAccess::setExecutableName(const std::string& newName) {
		executableName = newName;
	}

	HANDLE MemoryAccess::readProcess(const char* processName)
	{
		DWORD pid = 0;     //переменная айди процесса
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(snapshot, &pe32)) {
			while (Process32Next(snapshot, &pe32)) { //ищет айди процесса
				if (strcmp(pe32.szExeFile, processName) == 0) {
					pid = pe32.th32ProcessID; //переменная айди процесса
					break;
				}
			}
		}
		CloseHandle(snapshot);
		DWORD Prava = PROCESS_ALL_ACCESS; //это права доступа
		HANDLE Process = OpenProcess(Prava, FALSE, pid); //числовое значение - это айди процесса в диспетчере задач
		return Process;
	}
	HANDLE MemoryAccess::readProcess()
	{
		if (executableName != "")
			return MemoryAccess::readProcess(executableName.c_str());
		return 0;
	}
	DWORD MemoryAccess::readProcessID(const char* name)
	{
		HANDLE h;
		PROCESSENTRY32 singleProcess;
		h = CreateToolhelp32Snapshot( //takes a snapshot of specified processes
			TH32CS_SNAPPROCESS, //get all processes
			0); //ignored for SNAPPROCESS

		singleProcess.dwSize = sizeof(PROCESSENTRY32);

		DWORD pid = 0;
		do
		{
			if (strcmp(singleProcess.szExeFile, name) == 0)
			{
				pid = singleProcess.th32ProcessID;
				break;
			}

		} while (Process32Next(h, &singleProcess));

		CloseHandle(h);

		return pid;
	}
	DWORD MemoryAccess::readProcessID()
	{
		if (executableName != "")
			return MemoryAccess::readProcessID(executableName.c_str());
		return 0;
	}

	std::vector<void*> MemoryAccess::findBytePatternInProcessMemory(void* pattern, size_t patternLen)
	{

		if (patternLen == 0) { return {}; }

			HANDLE Process = MemoryAccess::readProcess();
			std::vector<void*> returnVec;
			returnVec.reserve(1000);
		
			auto query = initVirtualQuery(Process);
		
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
						ReadProcessMemory(Process, low, &size, sizeof(size), NULL)
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
		
		CloseHandle(Process);
		return returnVec;
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
}