#pragma once
#pragma warning(push)
#pragma warning(disable : 4312)
#pragma warning(disable : 4267)

#include <Windows.h>

#include<unordered_map>

#include"ProcessAnalyzerTypeWrapped.h"
class HobbitProcessAnalyzer : public ProcessAnalyzerTypeWrapped
{
public:
	HobbitProcessAnalyzer()
	{
		hobbitProcess = getProcess("Meridian.exe");
		objectStackAddress = 0;
		if (hobbitProcess != nullptr)
		{
			startAnalyzingProcess();
		}
	}
	using ProcessAnalyzer::readData;
	using ProcessAnalyzerTypeWrapped::readData;
	using ProcessAnalyzerTypeWrapped::searchProcessMemory;

	
	std::vector<uint8_t> readData(uint32_t address, size_t byesSize)
	{
		return ProcessAnalyzer::readData(hobbitProcess, (LPVOID)address, byesSize);
	}

	template <typename T>
	T readData(uint32_t address, size_t byesSize)
	{
		return convertToType<T>(ProcessAnalyzer::readData(hobbitProcess, (LPVOID)address, byesSize));
	}

	void startAnalyzingProcess()
	{
		try {																								 
			objectStackAddress = readData<uint32_t>(hobbitProcess, reinterpret_cast<LPVOID>(0x0076F648), 4);
		}																									 
		catch (const std::runtime_error& e) {																 
			std::cerr << "Error: " << e.what() << std::endl;												 
		}
	}
	uint32_t findGameObjByGUID(uint64_t guid)
	{
		if (hobbitProcess == 0)
		{
			std::cout << "ERROR: Hobbit Process is NOT set" << std::endl;
			return 0;
		}

		static const size_t stackSize = 0xEFEC;
		static const size_t jumpSize = 0x14;

		for (size_t offset = stackSize; offset > 0; offset -= jumpSize)
		{
			uint32_t objStackAddress = objectStackAddress + offset;

			uint32_t objAddrs = readData<uint32_t>(hobbitProcess, reinterpret_cast<LPVOID>(objStackAddress), 4);
			if (objAddrs != 0)
			{
				uint32_t guidAddrs = objAddrs + 0x8;
				uint64_t objGUID = readData<uint64_t>(hobbitProcess, reinterpret_cast<LPVOID>(guidAddrs), 8);
				if (objGUID == guid)
				{
					return objAddrs;
				}
			}
		}

		std::cout << "WARNING: Couldn't find " << guid << " GUID in the Game Object Stack" << std::endl;
		return 0;
	}

	uint32_t findGameObjByPattern(const std::vector<uint8_t>& data, size_t dataSize, uint32_t shift)
	{
		if (hobbitProcess == 0)
		{
			std::cout << "ERROR: Hobbit Process is NOT set" << std::endl;
			return 0;
		}

		static const size_t stackSize = 0xEFEC;
		static const size_t jumpSize = 0x14;

		for (size_t offset = stackSize; offset > 0; offset -= jumpSize)
		{
			uint32_t objStackAddress = objectStackAddress + offset;
			uint32_t objAddrs = readData<uint32_t>(hobbitProcess, reinterpret_cast<LPVOID>(objStackAddress), 4);
			if (objAddrs != 0)
			{
				uint32_t patternAddrs = objAddrs + shift;
				std::vector<uint8_t> objPattern = readData(hobbitProcess, reinterpret_cast<LPVOID>(patternAddrs), dataSize);
				if (memcmp(objPattern.data(), data.data(), dataSize) == 0)
				{
					return objAddrs;
				}
			}
		}

		std::cout << "WARNING: Couldn't find " << data[0] << " GUID in the Game Object Stack" << std::endl;
		return 0;
	}

	std::vector<uint32_t> findAllGameObjByPattern(const std::vector<uint8_t>& data, size_t dataSize, uint32_t shift)
	{
		if (hobbitProcess == nullptr)
		{
			std::cout << "ERROR: Hobbit Process is NOT set" << std::endl;
			return std::vector<uint32_t>();
		}

		std::vector<uint32_t> gameObjs;

		static const size_t stackSize = 0xEFEC;
		static const size_t jumpSize = 0x14;

		for (size_t offset = stackSize; offset > 0; offset -= jumpSize)
		{
			uint32_t objStackAddress = objectStackAddress + offset;
			uint32_t objAddrs = readData<uint32_t>(hobbitProcess, reinterpret_cast<LPVOID>(objStackAddress), 4);
			if (objAddrs != 0)
			{
				uint32_t patternAddrs = objAddrs + shift;
				std::vector<uint8_t> objPattern = readData(hobbitProcess, reinterpret_cast<LPVOID>(patternAddrs), dataSize);
				if (memcmp(objPattern.data(), data.data(), dataSize) == 0)
				{
					gameObjs.push_back(objStackAddress);
				}
			}
		}

		return gameObjs;
	}
	std::unordered_map<uint32_t, std::vector<uint8_t>> readAllGameObjByPattern(size_t dataSize, uint32_t shift)
	{
		if (hobbitProcess == nullptr)
		{
			std::cout << "ERROR: Hobbit Process is NOT set" << std::endl;
			return std::unordered_map<uint32_t, std::vector<uint8_t>>();
		}
		std::unordered_map<uint32_t, std::vector<uint8_t>>  gameObjs(0);

		static const size_t stackSize = 0xEFEC;
		static const size_t jumpSize = 0x14;

		for (size_t offset = stackSize; offset > 0; offset -= jumpSize)
		{
			uint32_t objStackAddress = objectStackAddress + offset;
			uint32_t objAddrs = readData<uint32_t>(hobbitProcess, reinterpret_cast<LPVOID>(objStackAddress), 4);
			if (objAddrs != 0)
			{
				uint32_t patternAddrs = objAddrs + shift;
				gameObjs[objStackAddress] = readData(hobbitProcess, reinterpret_cast<LPVOID>(patternAddrs), dataSize);
			}
		}

		return gameObjs;
	}

	std::vector<uint32_t> getAllObjects() {
		if (hobbitProcess == nullptr)
		{
			std::cout << "ERROR: Hobbit Process is NOT set" << std::endl;
			return std::vector<uint32_t>();
		}

		std::vector<uint32_t> foundObjects;

		static const size_t stackSize = 0xEFEC;
		static const size_t jumpSize = 0x14;

		for (size_t offset = stackSize; offset > 0; offset -= jumpSize) {
			uint32_t objStackAddress = objectStackAddress + offset;
			uint32_t objAddrs = readData<uint32_t>(hobbitProcess, reinterpret_cast<LPVOID>(objStackAddress), 4);
			if (objAddrs != 0)
			{
				foundObjects.push_back(objAddrs);
			}
		}

		return foundObjects;
	}

	bool isGameRunning()
	{
		hobbitProcess = getProcess("Meridian.exe");
		if (hobbitProcess != nullptr)
		{
			startAnalyzingProcess();
		}
		return  (hobbitProcess != nullptr);
	}
	void refreshObjectStackAddress()
	{
		if (hobbitProcess != nullptr)
		{
			startAnalyzingProcess();
		}
	}

	template <typename T>
	std::vector<uint32_t> searchProcessMemory(T pattern)
	{
		return ProcessAnalyzerTypeWrapped::searchProcessMemory(hobbitProcess, convertToUint8Vector(pattern));
	}
	template <typename T>
	std::vector<uint32_t> searchProcessMemory(const std::vector<T>& pattern)
	{
		return ProcessAnalyzerTypeWrapped::searchProcessMemory(hobbitProcess, convertToUint8Vector(pattern));
	}

private:
	HANDLE hobbitProcess;
	uint32_t objectStackAddress;
};